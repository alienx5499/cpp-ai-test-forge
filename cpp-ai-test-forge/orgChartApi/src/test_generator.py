#!/usr/bin/env python3
"""
AI-Powered C++ Unit Test Generator for orgChartApi
This tool generates comprehensive unit tests using AI models (Ollama, GitHub Models, etc.)
"""

import os
import sys
import json
import yaml
import subprocess
import argparse
import requests
from pathlib import Path
from typing import List, Dict, Tuple, Optional
import logging

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

class AITestGenerator:
    def __init__(self, project_path: str, ai_model: str = "ollama", model_name: str = "codellama"):
        self.project_path = Path(project_path)
        self.ai_model = ai_model
        self.model_name = model_name
        self.yaml_configs_path = Path(__file__).parent.parent / "yaml_configs"
        self.tests_output_path = self.project_path / "generated_tests"
        self.coverage_reports_path = Path(__file__).parent.parent / "coverage_reports"
        
        # Create output directories
        self.tests_output_path.mkdir(exist_ok=True)
        self.coverage_reports_path.mkdir(exist_ok=True)
        
        # Load YAML configurations
        self.configs = self._load_yaml_configs()
        
    def _load_yaml_configs(self) -> Dict:
        """Load all YAML configuration files"""
        configs = {}
        yaml_files = [
            "initial_test_generation.yaml",
            "test_refinement.yaml", 
            "build_fix.yaml",
            "coverage_improvement.yaml"
        ]
        
        for yaml_file in yaml_files:
            config_path = self.yaml_configs_path / yaml_file
            if config_path.exists():
                with open(config_path, 'r') as f:
                    configs[yaml_file.replace('.yaml', '')] = yaml.safe_load(f)
            else:
                logger.warning(f"Configuration file not found: {config_path}")
                
        return configs
    
    def analyze_source_code(self) -> List[Dict]:
        """Analyze C++ source files in the project"""
        source_files = []
        
        # Find C++ source and header files
        cpp_extensions = ['.cpp', '.cc', '.cxx', '.h', '.hpp']
        
        for root, dirs, files in os.walk(self.project_path):
            # Skip certain directories
            if any(skip_dir in root for skip_dir in ['build', '.git', 'third_party']):
                continue
                
            for file in files:
                if any(file.endswith(ext) for ext in cpp_extensions):
                    file_path = Path(root) / file
                    relative_path = file_path.relative_to(self.project_path)
                    
                    # Read file content
                    try:
                        with open(file_path, 'r', encoding='utf-8') as f:
                            content = f.read()
                        
                        source_files.append({
                            'path': str(relative_path),
                            'full_path': str(file_path),
                            'content': content,
                            'type': 'header' if file.endswith(('.h', '.hpp')) else 'source'
                        })
                    except Exception as e:
                        logger.warning(f"Could not read file {file_path}: {e}")
        
        logger.info(f"Found {len(source_files)} C++ files to analyze")
        return source_files
    
    def call_ai_model(self, prompt: str, context: str = "") -> str:
        """Call the specified AI model with the given prompt"""
        if self.ai_model == "ollama":
            return self._call_ollama(prompt, context)
        elif self.ai_model == "github":
            return self._call_github_models(prompt, context)
        else:
            raise ValueError(f"Unsupported AI model: {self.ai_model}")
    
    def _call_ollama(self, prompt: str, context: str = "") -> str:
        """Call Ollama API"""
        try:
            full_prompt = f"{context}\n\n{prompt}" if context else prompt
            
            response = requests.post(
                "http://localhost:11434/api/generate",
                json={
                    "model": self.model_name,
                    "prompt": full_prompt,
                    "stream": False
                },
                timeout=300
            )
            
            if response.status_code == 200:
                return response.json().get('response', '')
            else:
                logger.error(f"Ollama API error: {response.status_code}")
                return ""
                
        except Exception as e:
            logger.error(f"Error calling Ollama: {e}")
            return ""
    
    def _call_github_models(self, prompt: str, context: str = "") -> str:
        """Call GitHub Models API"""
        try:
            from github_models import create_github_client
            
            if not hasattr(self, '_github_client'):
                self._github_client = create_github_client(self.model_name)
            
            return self._github_client.generate_response(prompt, context)
            
        except ImportError:
            logger.error("GitHub Models module not found. Please ensure github_models.py is available.")
            return ""
        except Exception as e:
            logger.error(f"Error calling GitHub Models: {e}")
            return ""
    
    def generate_initial_tests(self, source_files: List[Dict]) -> List[Dict]:
        """Generate initial unit tests for all source files"""
        logger.info("Starting initial test generation...")
        
        config = self.configs.get('initial_test_generation', {})
        system_prompt = config.get('llm_instructions', {}).get('system_prompt', '')
        
        generated_tests = []
        
        for source_file in source_files:
            if source_file['type'] == 'header':  # Focus on header files for test generation
                logger.info(f"Generating tests for: {source_file['path']}")
                
                prompt = f"""
{system_prompt}

Please generate comprehensive unit tests for the following C++ code:

File: {source_file['path']}
Content:
{source_file['content']}

Requirements:
1. Use Google Test framework
2. Include Google Mock for mocking
3. Test all public methods
4. Include edge cases and error scenarios
5. Mock database interactions
6. Follow naming convention: Test{{ClassName}}.cc
7. Include proper setup and teardown

Generate complete test file with all necessary includes.
"""
                
                test_content = self.call_ai_model(prompt)
                
                if test_content:
                    # Extract class name for test file naming
                    class_name = self._extract_class_name(source_file['content'])
                    test_filename = f"Test{class_name}.cc" if class_name else f"Test_{Path(source_file['path']).stem}.cc"
                    
                    generated_tests.append({
                        'original_file': source_file['path'],
                        'test_file': test_filename,
                        'content': test_content
                    })
        
        return generated_tests
    
    def _extract_class_name(self, content: str) -> str:
        """Extract class name from C++ code"""
        import re
        
        # Look for class declarations
        class_pattern = r'class\s+(\w+)'
        matches = re.findall(class_pattern, content)
        
        if matches:
            # Return the first class found
            return matches[0]
        
        return ""
    
    def refine_tests(self, generated_tests: List[Dict]) -> List[Dict]:
        """Refine generated tests by removing duplicates and improving coverage"""
        logger.info("Refining generated tests...")
        
        config = self.configs.get('test_refinement', {})
        system_prompt = config.get('llm_instructions', {}).get('system_prompt', '')
        
        refined_tests = []
        
        for test in generated_tests:
            prompt = f"""
{system_prompt}

Please refine the following unit test file:

Original file: {test['original_file']}
Test file: {test['test_file']}
Current test content:
{test['content']}

Please:
1. Remove any duplicate test cases
2. Add missing includes and dependencies
3. Improve test coverage
4. Optimize test structure
5. Add proper error handling tests
6. Ensure all required libraries are included

Provide the refined test file.
"""
            
            refined_content = self.call_ai_model(prompt)
            
            if refined_content:
                refined_tests.append({
                    'original_file': test['original_file'],
                    'test_file': test['test_file'],
                    'content': refined_content
                })
            else:
                # Keep original if refinement fails
                refined_tests.append(test)
        
        return refined_tests
    
    def save_tests(self, tests: List[Dict]) -> None:
        """Save generated tests to files"""
        logger.info(f"Saving {len(tests)} test files...")
        
        for test in tests:
            test_file_path = self.tests_output_path / test['test_file']
            
            with open(test_file_path, 'w') as f:
                f.write(test['content'])
            
            logger.info(f"Saved: {test_file_path}")
    
    def build_tests(self) -> Tuple[bool, str]:
        """Build the generated tests"""
        logger.info("Building generated tests...")
        
        # Create CMakeLists.txt for tests
        self._create_test_cmake()
        
        # Create build directory
        build_dir = self.tests_output_path / "build"
        build_dir.mkdir(exist_ok=True)
        
        try:
            # Run cmake
            cmake_cmd = ["cmake", "..", "-DCMAKE_BUILD_TYPE=Debug", "-DCMAKE_CXX_FLAGS=--coverage"]
            result = subprocess.run(cmake_cmd, cwd=build_dir, capture_output=True, text=True)
            
            if result.returncode != 0:
                return False, f"CMake failed: {result.stderr}"
            
            # Run make
            make_cmd = ["make", "-j4"]
            result = subprocess.run(make_cmd, cwd=build_dir, capture_output=True, text=True)
            
            if result.returncode != 0:
                return False, f"Build failed: {result.stderr}"
            
            return True, "Build successful"
            
        except Exception as e:
            return False, f"Build error: {str(e)}"
    
    def _create_test_cmake(self) -> None:
        """Create CMakeLists.txt for the test directory"""
        cmake_content = """cmake_minimum_required(VERSION 3.16)
project(OrgChartApiTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(Drogon CONFIG REQUIRED)
find_package(GTest REQUIRED)
find_package(GMock REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../orgChartApi)

# Add test executable
file(GLOB_RECURSE TEST_SOURCES "*.cc" "*.cpp")
add_executable(unit_tests ${TEST_SOURCES})

# Link libraries
target_link_libraries(unit_tests 
    Drogon::Drogon
    GTest::GTest
    GTest::Main
    GMock::GMock
    GMock::Main
    gcov
)

# Enable testing
enable_testing()
add_test(NAME unit_tests COMMAND unit_tests)

# Coverage flags
target_compile_options(unit_tests PRIVATE --coverage)
target_link_options(unit_tests PRIVATE --coverage)
"""
        
        cmake_path = self.tests_output_path / "CMakeLists.txt"
        with open(cmake_path, 'w') as f:
            f.write(cmake_content)
    
    def fix_build_issues(self, build_error: str) -> List[Dict]:
        """Fix build issues using AI"""
        logger.info("Attempting to fix build issues...")
        
        config = self.configs.get('build_fix', {})
        system_prompt = config.get('llm_instructions', {}).get('system_prompt', '')
        
        # Read all test files
        test_files = []
        for test_file in self.tests_output_path.glob("*.cc"):
            with open(test_file, 'r') as f:
                test_files.append({
                    'file': test_file.name,
                    'content': f.read()
                })
        
        prompt = f"""
{system_prompt}

Build failed with the following error:
{build_error}

Test files:
"""
        
        for test_file in test_files:
            prompt += f"\n--- {test_file['file']} ---\n{test_file['content']}\n"
        
        prompt += """
Please fix the build issues and provide corrected test files and CMakeLists.txt if needed.
"""
        
        fixed_content = self.call_ai_model(prompt)
        
        # Parse and save fixed files (this would need more sophisticated parsing)
        return test_files  # Placeholder
    
    def calculate_coverage(self) -> Dict:
        """Calculate test coverage using gcov"""
        logger.info("Calculating test coverage...")
        
        build_dir = self.tests_output_path / "build"
        
        try:
            # Run tests first
            test_cmd = ["./unit_tests"]
            subprocess.run(test_cmd, cwd=build_dir, check=True)
            
            # Generate coverage data
            gcov_cmd = ["gcov", "*.gcno"]
            subprocess.run(gcov_cmd, cwd=build_dir, capture_output=True)
            
            # Generate lcov report
            lcov_cmd = ["lcov", "--capture", "--directory", ".", "--output-file", "coverage.info"]
            subprocess.run(lcov_cmd, cwd=build_dir, capture_output=True)
            
            # Generate HTML report
            genhtml_cmd = ["genhtml", "coverage.info", "--output-directory", str(self.coverage_reports_path)]
            subprocess.run(genhtml_cmd, cwd=build_dir, capture_output=True)
            
            # Parse coverage data
            return self._parse_coverage_data(build_dir / "coverage.info")
            
        except Exception as e:
            logger.error(f"Coverage calculation failed: {e}")
            return {}
    
    def _parse_coverage_data(self, coverage_file: Path) -> Dict:
        """Parse lcov coverage data"""
        coverage_data = {
            'line_coverage': 0,
            'function_coverage': 0,
            'branch_coverage': 0
        }
        
        if coverage_file.exists():
            with open(coverage_file, 'r') as f:
                content = f.read()
                
            # Parse lcov format (simplified)
            import re
            
            line_pattern = r'LF:(\d+).*?LH:(\d+)'
            func_pattern = r'FNF:(\d+).*?FNH:(\d+)'
            branch_pattern = r'BRF:(\d+).*?BRH:(\d+)'
            
            line_match = re.search(line_pattern, content, re.DOTALL)
            func_match = re.search(func_pattern, content, re.DOTALL)
            branch_match = re.search(branch_pattern, content, re.DOTALL)
            
            if line_match:
                lf, lh = int(line_match.group(1)), int(line_match.group(2))
                coverage_data['line_coverage'] = (lh / lf * 100) if lf > 0 else 0
                
            if func_match:
                fnf, fnh = int(func_match.group(1)), int(func_match.group(2))
                coverage_data['function_coverage'] = (fnh / fnf * 100) if fnf > 0 else 0
                
            if branch_match:
                brf, brh = int(branch_match.group(1)), int(branch_match.group(2))
                coverage_data['branch_coverage'] = (brh / brf * 100) if brf > 0 else 0
        
        return coverage_data
    
    def improve_coverage(self, coverage_data: Dict) -> List[Dict]:
        """Improve test coverage using AI"""
        logger.info("Improving test coverage...")
        
        config = self.configs.get('coverage_improvement', {})
        system_prompt = config.get('llm_instructions', {}).get('system_prompt', '')
        
        prompt = f"""
{system_prompt}

Current coverage data:
- Line coverage: {coverage_data.get('line_coverage', 0):.1f}%
- Function coverage: {coverage_data.get('function_coverage', 0):.1f}%
- Branch coverage: {coverage_data.get('branch_coverage', 0):.1f}%

Please analyze the coverage and suggest additional tests to improve coverage to 85%+ for all metrics.
"""
        
        improvements = self.call_ai_model(prompt)
        
        # This would parse the AI response and generate additional tests
        return []  # Placeholder
    
    def run_complete_workflow(self) -> None:
        """Run the complete test generation workflow"""
        logger.info("Starting complete AI-powered test generation workflow...")
        
        try:
            # Step 1: Analyze source code
            source_files = self.analyze_source_code()
            
            # Step 2: Generate initial tests
            initial_tests = self.generate_initial_tests(source_files)
            
            # Step 3: Refine tests
            refined_tests = self.refine_tests(initial_tests)
            
            # Step 4: Save tests
            self.save_tests(refined_tests)
            
            # Step 5: Build tests
            build_success, build_message = self.build_tests()
            
            if not build_success:
                logger.warning(f"Build failed: {build_message}")
                
                # Step 6: Fix build issues
                self.fix_build_issues(build_message)
                
                # Retry build
                build_success, build_message = self.build_tests()
            
            if build_success:
                logger.info("Build successful!")
                
                # Step 7: Calculate coverage
                coverage_data = self.calculate_coverage()
                
                logger.info(f"Coverage Results:")
                logger.info(f"  Line coverage: {coverage_data.get('line_coverage', 0):.1f}%")
                logger.info(f"  Function coverage: {coverage_data.get('function_coverage', 0):.1f}%")
                logger.info(f"  Branch coverage: {coverage_data.get('branch_coverage', 0):.1f}%")
                
                # Step 8: Improve coverage if needed
                if coverage_data.get('line_coverage', 0) < 85:
                    logger.info("Attempting to improve coverage...")
                    self.improve_coverage(coverage_data)
                
                logger.info("Test generation workflow completed successfully!")
                
                # Generate final report
                self._generate_final_report(coverage_data, len(refined_tests))
                
            else:
                logger.error(f"Build failed after fixes: {build_message}")
                
        except Exception as e:
            logger.error(f"Workflow failed: {e}")
            raise
    
    def _generate_final_report(self, coverage_data: Dict, num_tests: int) -> None:
        """Generate final test report"""
        report_path = self.coverage_reports_path / "test_generation_report.md"
        
        report_content = f"""# AI-Powered Unit Test Generation Report

## Project: orgChartApi

### Test Generation Summary
- **Total test files generated**: {num_tests}
- **Output directory**: {self.tests_output_path}
- **Coverage reports**: {self.coverage_reports_path}

### Coverage Results
- **Line coverage**: {coverage_data.get('line_coverage', 0):.1f}%
- **Function coverage**: {coverage_data.get('function_coverage', 0):.1f}%
- **Branch coverage**: {coverage_data.get('branch_coverage', 0):.1f}%

### AI Model Used
- **Model**: {self.ai_model}
- **Model name**: {self.model_name}

### Generated Test Files
"""
        
        # List all generated test files
        for test_file in self.tests_output_path.glob("*.cc"):
            report_content += f"- {test_file.name}\n"
        
        report_content += f"""
### Coverage Report
Detailed HTML coverage report available at: {self.coverage_reports_path}/index.html

### Recommendations
1. Review generated tests for accuracy
2. Add additional edge case tests if coverage is below 85%
3. Integrate tests into CI/CD pipeline
4. Regularly update tests when code changes

### Build Instructions
```bash
cd {self.tests_output_path}
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=--coverage
make -j4
./unit_tests
```

### Coverage Analysis
```bash
gcov *.gcno
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory ../../../coverage_reports
```
"""
        
        with open(report_path, 'w') as f:
            f.write(report_content)
        
        logger.info(f"Final report generated: {report_path}")


def main():
    parser = argparse.ArgumentParser(description="AI-Powered C++ Unit Test Generator")
    parser.add_argument("project_path", help="Path to the C++ project")
    parser.add_argument("--ai-model", choices=["ollama", "github"], default="ollama",
                       help="AI model to use (default: ollama)")
    parser.add_argument("--model-name", default="codellama",
                       help="Specific model name (default: codellama)")
    parser.add_argument("--verbose", "-v", action="store_true",
                       help="Enable verbose logging")
    
    args = parser.parse_args()
    
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
    
    # Create and run test generator
    generator = AITestGenerator(args.project_path, args.ai_model, args.model_name)
    generator.run_complete_workflow()


if __name__ == "__main__":
    main() 