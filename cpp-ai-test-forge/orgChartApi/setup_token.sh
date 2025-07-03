#!/bin/bash
# Simple script to set up GitHub token for AI Test Generator

echo "🔑 GitHub Token Setup for AI Test Generator"
echo "==========================================="
echo ""

# Check if token is provided as argument
if [ "$1" ]; then
    TOKEN="$1"
    echo "✓ Using provided token"
else
    echo "Please paste your GitHub token here:"
    read -s TOKEN  # -s hides the input for security
fi

if [ -z "$TOKEN" ]; then
    echo "❌ No token provided. Exiting."
    exit 1
fi

# Export the token for current session
export GITHUB_TOKEN="$TOKEN"

# Also save to a temporary file for the current session
echo "export GITHUB_TOKEN='$TOKEN'" > ~/.github_token_temp

echo ""
echo "✅ GitHub token set successfully!"
echo ""
echo "Now you can run the AI test generator:"
echo "  ./scripts/run_test_generator.sh -m github -n gpt-4"
echo ""
echo "Or activate the token in a new terminal with:"
echo "  source ~/.github_token_temp" 