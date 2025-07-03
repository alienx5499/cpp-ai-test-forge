#!/bin/bash
# Demo script for AI-Powered C++ Unit Test Generator

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m'

print_header() {
    echo -e "${PURPLE}
╔══════════════════════════════════════════════════════════════╗
║                                                              ║
║        🤖 AI-Powered C++ Unit Test Generator Demo           ║
║                                                              ║
║           Day 5 Assignment - Keploy AI Test Forge           ║
║                                                              ║
╚══════════════════════════════════════════════════════════════╝${NC}"
}

print_step() {
    echo -e "${BLUE}
┌────────────────────────────────────────────────────────────┐
│  $1
└────────────────────────────────────────────────────────────┘${NC}"
}

print_status() {
    echo -e "${GREEN}✓${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}⚠${NC} $1"
}

print_error() {
    echo -e "${RED}✗${NC} $1"
}

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

print_header

# Step 1: Check environment
print_step "Step 1: Checking Environment"

cd "$PROJECT_ROOT"

if [ ! -d "orgChartApi" ]; then
    print_warning "orgChartApi not found. Cloning repository..."
    git clone https://github.com/keploy/orgChartApi.git
    print_status "Repository cloned successfully"
else
    print_status "orgChartApi repository found"
fi

# Check Python dependencies
if python3 -c "import yaml, requests" 2>/dev/null; then
    print_status "Python dependencies available"
else
    print_warning "Installing Python dependencies..."
    pip3 install -r requirements.txt
fi

# Step 2: Show project structure
print_step "Step 2: Analyzing orgChartApi Project Structure"

echo "📁 Project Structure:"
echo "   orgChartApi/"
tree orgChartApi/ -I 'build|.git|third_party' -L 3 2>/dev/null || find orgChartApi/ -type d -name "build" -prune -o -type d -name ".git" -prune -o -type d -name "third_party" -prune -o -type f -print | head -20

echo
echo "🔍 Key Components Found:"
echo "   • Controllers: $(find orgChartApi/controllers -name "*.h" | wc -l) files"
echo "   • Models: $(find orgChartApi/models -name "*.h" | wc -l) files"
echo "   • Utils: $(find orgChartApi/utils -name "*.h" | wc -l) files"

# Step 3: Show YAML configurations
print_step "Step 3: AI Configuration Templates"

echo "📋 YAML Configuration Files:"
for yaml_file in yaml_configs/*.yaml; do
    echo "   • $(basename "$yaml_file")"
done

echo
echo "📄 Sample Configuration (initial_test_generation.yaml):"
echo "   • Task: Generate comprehensive unit tests"
echo "   • Framework: Google Test + Google Mock"
echo "   • Coverage Target: 80%+ line coverage"
echo "   • Output: Complete .cc test files"

# Step 4: Check AI model availability
print_step "Step 4: Checking AI Model Availability"

# Check Ollama
if command -v ollama &> /dev/null; then
    if curl -s http://localhost:11434/api/version &> /dev/null; then
        print_status "Ollama server is running"
        echo "   Available models:"
        ollama list | grep -E "^[a-zA-Z]" | head -5
    else
        print_warning "Ollama is installed but not running"
        echo "   To start: ollama serve"
    fi
else
    print_warning "Ollama not found"
    echo "   Install from: https://ollama.ai/"
fi

# Check GitHub token
if [ -n "$GITHUB_TOKEN" ]; then
    print_status "GitHub token configured"
    echo "   Model: GitHub Models API ready"
else
    print_warning "GitHub token not set"
    echo "   Set with: export GITHUB_TOKEN='your_token'"
fi

# Step 5: Demonstrate workflow
print_step "Step 5: AI Test Generation Workflow Demonstration"

echo "🔄 Workflow Steps:"
echo "   1. 📖 Analyze C++ source code"
echo "   2. 🧠 Generate initial tests with AI"
echo "   3. ✨ Refine and optimize tests"
echo "   4. 🔨 Build and compile tests"
echo "   5. 🔧 Fix any build issues"
echo "   6. 📊 Calculate test coverage"
echo "   7. 🎯 Improve coverage iteratively"
echo "   8. 📋 Generate final report"

# Step 6: Show sample output
print_step "Step 6: Expected Output Structure"

echo "📂 Generated Output:"
echo "   orgChartApi/"
echo "   ├── generated_tests/"
echo "   │   ├── TestPerson.cc"
echo "   │   ├── TestDepartment.cc"
echo "   │   ├── TestPersonsController.cc"
echo "   │   ├── CMakeLists.txt"
echo "   │   └── build/"
echo "   └── coverage_reports/"
echo "       ├── index.html"
echo "       ├── coverage.info"
echo "       └── test_generation_report.md"

# Step 7: Demo execution options
print_step "Step 7: Execution Options"

echo "🚀 Run the Generator:"
echo
echo "   Option 1 - Using Ollama (Local):"
echo "   $ ./scripts/run_test_generator.sh"
echo
echo "   Option 2 - Using GitHub Models (Cloud):"
echo "   $ ./scripts/run_test_generator.sh -m github -n gpt-4"
echo
echo "   Option 3 - Custom project:"
echo "   $ ./scripts/run_test_generator.sh -p /path/to/project"
echo
echo "   Option 4 - Direct Python call:"
echo "   $ python3 src/test_generator.py orgChartApi/ --verbose"

# Step 8: Interactive demo choice
print_step "Step 8: Interactive Demo"

echo "Would you like to run a live demonstration? (y/n)"
read -r response

if [[ "$response" =~ ^[Yy]$ ]]; then
    print_status "Starting live demonstration..."
    
    # Check which AI model to use
    if command -v ollama &> /dev/null && curl -s http://localhost:11434/api/version &> /dev/null; then
        print_status "Using Ollama for demonstration"
        ./scripts/run_test_generator.sh -v
    elif [ -n "$GITHUB_TOKEN" ]; then
        print_status "Using GitHub Models for demonstration"
        ./scripts/run_test_generator.sh -m github -n gpt-4 -v
    else
        print_error "No AI model available. Please set up Ollama or GitHub token."
        echo
        echo "Setup instructions:"
        echo "  • Ollama: https://ollama.ai/"
        echo "  • GitHub: export GITHUB_TOKEN='your_token'"
        exit 1
    fi
else
    print_status "Demo completed. Ready to generate tests!"
fi

print_step "Demo Summary"

echo "✨ Key Features Demonstrated:"
echo "   • 🤖 AI-powered test generation"
echo "   • 📋 YAML-driven instructions"
echo "   • 🔄 Iterative refinement process"
echo "   • 📊 Coverage analysis integration"
echo "   • 🛠️ Automated build fixing"

echo
echo "🎯 Assignment Completed:"
echo "   ✓ Environment setup automation"
echo "   ✓ AI model integration (Ollama + GitHub)"
echo "   ✓ YAML instruction framework"
echo "   ✓ Complete test generation workflow"
echo "   ✓ Coverage analysis with GNU tools"
echo "   ✓ Iterative improvement process"

echo
print_status "Ready for production use! 🎉"
echo
echo "Next steps:"
echo "  1. Run: ./scripts/run_test_generator.sh"
echo "  2. Review: orgChartApi/generated_tests/"
echo "  3. Check: coverage_reports/index.html" 