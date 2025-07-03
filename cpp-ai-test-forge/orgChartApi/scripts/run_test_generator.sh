#!/bin/bash
# Convenience script to run the AI-Powered C++ Unit Test Generator

set -e

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

print_status "🤖 AI-Powered C++ Unit Test Generator"
print_status "======================================"

# Default values
PROJECT_PATH="$PROJECT_ROOT/orgChartApi"
AI_MODEL="ollama"
MODEL_NAME="codellama"
VERBOSE=""

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -p|--project)
            PROJECT_PATH="$2"
            shift 2
            ;;
        -m|--ai-model)
            AI_MODEL="$2"
            shift 2
            ;;
        -n|--model-name)
            MODEL_NAME="$2"
            shift 2
            ;;
        -v|--verbose)
            VERBOSE="-v"
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  -p, --project PATH      Path to C++ project (default: $PROJECT_PATH)"
            echo "  -m, --ai-model MODEL    AI model to use: ollama, github (default: ollama)"
            echo "  -n, --model-name NAME   Specific model name (default: codellama)"
            echo "  -v, --verbose           Enable verbose logging"
            echo "  -h, --help              Show this help message"
            echo ""
            echo "Examples:"
            echo "  $0                                          # Use defaults"
            echo "  $0 -p ./my-cpp-project                     # Specify project path"
            echo "  $0 -m github -n gpt-4                      # Use GitHub Models"
            echo "  $0 -v                                       # Enable verbose output"
            exit 0
            ;;
        *)
            print_warning "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Validate project path
if [ ! -d "$PROJECT_PATH" ]; then
    print_warning "Project path does not exist: $PROJECT_PATH"
    exit 1
fi

print_status "Configuration:"
print_status "  Project Path: $PROJECT_PATH"
print_status "  AI Model: $AI_MODEL"
print_status "  Model Name: $MODEL_NAME"

# Check if Ollama is running (if using Ollama)
if [ "$AI_MODEL" = "ollama" ]; then
    if ! curl -s http://localhost:11434/api/version &> /dev/null; then
        print_warning "Ollama server is not running!"
        print_status "Please start Ollama: ollama serve"
        print_status "Then pull the model: ollama pull $MODEL_NAME"
        exit 1
    fi
    
    # Check if model is available
    if ! ollama list | grep -q "$MODEL_NAME"; then
        print_warning "Model '$MODEL_NAME' not found in Ollama"
        print_status "Available models:"
        ollama list
        print_status "To pull the model: ollama pull $MODEL_NAME"
        exit 1
    fi
fi

print_status "Starting test generation..."
print_status ""

# Run the test generator
cd "$PROJECT_ROOT"
python3 src/test_generator.py "$PROJECT_PATH" \
    --ai-model "$AI_MODEL" \
    --model-name "$MODEL_NAME" \
    $VERBOSE

print_status ""
print_status "Test generation completed! Check the generated_tests directory."
print_status "Coverage reports will be available in coverage_reports/" 