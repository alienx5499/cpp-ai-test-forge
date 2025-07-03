#!/bin/bash
# Setup script for AI-Powered C++ Unit Test Generator

set -e

echo "🚀 Setting up AI-Powered C++ Unit Test Generator Environment"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if running on Linux/macOS
if [[ "$OSTYPE" != "linux-gnu"* ]] && [[ "$OSTYPE" != "darwin"* ]]; then
    print_error "This script is designed for Linux/macOS. For Windows, please use WSL."
    exit 1
fi

print_status "Checking system requirements..."

# Check for required system packages
check_command() {
    if command -v "$1" &> /dev/null; then
        print_status "$1 is installed ✓"
        return 0
    else
        print_warning "$1 is not installed"
        return 1
    fi
}

# System dependencies
MISSING_DEPS=()

if ! check_command "cmake"; then
    MISSING_DEPS+=("cmake")
fi

if ! check_command "g++"; then
    MISSING_DEPS+=("g++")
fi

if ! check_command "python3"; then
    MISSING_DEPS+=("python3")
fi

if ! check_command "pip3"; then
    MISSING_DEPS+=("python3-pip")
fi

if ! check_command "git"; then
    MISSING_DEPS+=("git")
fi

# Install missing dependencies based on OS
if [ ${#MISSING_DEPS[@]} -ne 0 ]; then
    print_warning "Installing missing dependencies: ${MISSING_DEPS[*]}"
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        # Ubuntu/Debian
        if command -v apt-get &> /dev/null; then
            sudo apt-get update
            sudo apt-get install -y "${MISSING_DEPS[@]}"
        # CentOS/RHEL/Fedora
        elif command -v yum &> /dev/null; then
            sudo yum install -y "${MISSING_DEPS[@]}"
        elif command -v dnf &> /dev/null; then
            sudo dnf install -y "${MISSING_DEPS[@]}"
        else
            print_error "Unsupported Linux distribution. Please install dependencies manually."
            exit 1
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS
        if command -v brew &> /dev/null; then
            brew install "${MISSING_DEPS[@]}"
        else
            print_error "Homebrew not found. Please install Homebrew first: https://brew.sh/"
            exit 1
        fi
    fi
fi

# Install Python dependencies
print_status "Installing Python dependencies..."
pip3 install --user -r requirements.txt

# Install C++ testing frameworks
print_status "Installing C++ testing frameworks..."

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Google Test
    if ! pkg-config --exists gtest; then
        print_status "Installing Google Test..."
        sudo apt-get install -y libgtest-dev libgmock-dev
        
        # Build and install if not available as package
        if [ ! -f "/usr/lib/libgtest.a" ]; then
            cd /usr/src/gtest
            sudo cmake CMakeLists.txt
            sudo make
            sudo cp lib/*.a /usr/lib
        fi
    fi
    
    # Coverage tools
    sudo apt-get install -y lcov gcov
    
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS with Homebrew
    brew install googletest lcov
fi

# Install Drogon framework
print_status "Installing Drogon framework..."

if ! pkg-config --exists drogon; then
    print_status "Drogon not found. Installing from source..."
    
    # Install Drogon dependencies
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        sudo apt-get install -y libjsoncpp-dev uuid-dev zlib1g-dev openssl libssl-dev libpq-dev
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        brew install jsoncpp ossp-uuid zlib openssl postgresql
    fi
    
    # Clone and build Drogon
    TEMP_DIR=$(mktemp -d)
    cd "$TEMP_DIR"
    git clone https://github.com/drogonframework/drogon
    cd drogon
    git submodule update --init
    mkdir build && cd build
    cmake ..
    make -j$(nproc)
    sudo make install
    
    # Clean up
    cd /
    rm -rf "$TEMP_DIR"
    
    print_status "Drogon framework installed successfully"
else
    print_status "Drogon framework is already installed ✓"
fi

# Setup Ollama (optional)
print_status "Checking for Ollama..."
if ! command -v ollama &> /dev/null; then
    print_warning "Ollama not found. You can install it from: https://ollama.ai/"
    print_warning "Or use GitHub Models instead with --ai-model github"
else
    print_status "Ollama is installed ✓"
    
    # Pull recommended model
    print_status "Pulling recommended AI model (codellama)..."
    ollama pull codellama
fi

# Make scripts executable
chmod +x scripts/*.sh
chmod +x src/*.py

print_status "Environment setup completed successfully! 🎉"
print_status ""
print_status "Next steps:"
print_status "1. Run the test generator: python3 src/test_generator.py orgChartApi/"
print_status "2. Or use the convenience script: ./scripts/run_test_generator.sh"
print_status ""
print_status "For help: python3 src/test_generator.py --help" 