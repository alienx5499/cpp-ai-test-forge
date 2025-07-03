#!/usr/bin/env python3
"""
GitHub Models integration for AI-Powered C++ Unit Test Generator
This module provides integration with GitHub Models API
"""

import os
import json
import requests
from typing import Optional
import logging

logger = logging.getLogger(__name__)

class GitHubModelsClient:
    """Client for interacting with GitHub Models API"""
    
    def __init__(self, api_token: Optional[str] = None, model_name: str = "gpt-4"):
        """
        Initialize GitHub Models client
        
        Args:
            api_token: GitHub API token (if not provided, will look for GITHUB_TOKEN env var)
            model_name: Name of the model to use
        """
        self.api_token = api_token or os.getenv('GITHUB_TOKEN')
        self.model_name = model_name
        self.base_url = "https://models.inference.ai.azure.com"
        
        if not self.api_token:
            raise ValueError("GitHub API token is required. Set GITHUB_TOKEN environment variable or provide api_token parameter.")
    
    def generate_response(self, prompt: str, context: str = "", max_tokens: int = 4000) -> str:
        """
        Generate response using GitHub Models API
        
        Args:
            prompt: The prompt to send to the model
            context: Additional context (will be prepended to prompt)
            max_tokens: Maximum number of tokens in response
            
        Returns:
            Generated response text
        """
        try:
            full_prompt = f"{context}\n\n{prompt}" if context else prompt
            
            headers = {
                "Authorization": f"Bearer {self.api_token}",
                "Content-Type": "application/json"
            }
            
            payload = {
                "model": self.model_name,
                "messages": [
                    {
                        "role": "system",
                        "content": "You are an expert C++ unit test generator specializing in the Drogon framework."
                    },
                    {
                        "role": "user", 
                        "content": full_prompt
                    }
                ],
                "max_tokens": max_tokens,
                "temperature": 0.3  # Lower temperature for more consistent code generation
            }
            
            response = requests.post(
                f"{self.base_url}/chat/completions",
                headers=headers,
                json=payload,
                timeout=120
            )
            
            if response.status_code == 200:
                result = response.json()
                if 'choices' in result and len(result['choices']) > 0:
                    return result['choices'][0]['message']['content']
                else:
                    logger.error("No choices in GitHub Models response")
                    return ""
            else:
                logger.error(f"GitHub Models API error: {response.status_code} - {response.text}")
                return ""
                
        except requests.exceptions.Timeout:
            logger.error("GitHub Models API request timed out")
            return ""
        except requests.exceptions.RequestException as e:
            logger.error(f"GitHub Models API request failed: {e}")
            return ""
        except Exception as e:
            logger.error(f"Unexpected error calling GitHub Models: {e}")
            return ""
    
    def list_available_models(self) -> list:
        """
        List available models in GitHub Models
        
        Returns:
            List of available model names
        """
        try:
            headers = {
                "Authorization": f"Bearer {self.api_token}",
                "Content-Type": "application/json"
            }
            
            response = requests.get(
                f"{self.base_url}/models",
                headers=headers,
                timeout=30
            )
            
            if response.status_code == 200:
                models = response.json()
                return [model['id'] for model in models.get('data', [])]
            else:
                logger.error(f"Failed to list models: {response.status_code}")
                return []
                
        except Exception as e:
            logger.error(f"Error listing models: {e}")
            return []
    
    def check_model_availability(self, model_name: str) -> bool:
        """
        Check if a specific model is available
        
        Args:
            model_name: Name of the model to check
            
        Returns:
            True if model is available, False otherwise
        """
        available_models = self.list_available_models()
        return model_name in available_models


def create_github_client(model_name: str = "gpt-4") -> GitHubModelsClient:
    """
    Create and return a GitHub Models client
    
    Args:
        model_name: Name of the model to use
        
    Returns:
        Configured GitHubModelsClient instance
    """
    try:
        client = GitHubModelsClient(model_name=model_name)
        
        # Test the connection
        available_models = client.list_available_models()
        if available_models:
            logger.info(f"GitHub Models client created successfully. Available models: {available_models}")
            
            if model_name not in available_models:
                logger.warning(f"Model '{model_name}' not in available models. Using first available model.")
                if available_models:
                    client.model_name = available_models[0]
                    logger.info(f"Using model: {client.model_name}")
        else:
            logger.warning("Could not retrieve available models, but client created successfully")
        
        return client
        
    except Exception as e:
        logger.error(f"Failed to create GitHub Models client: {e}")
        raise


# Example usage and testing
if __name__ == "__main__":
    # Test the GitHub Models integration
    logging.basicConfig(level=logging.INFO)
    
    try:
        # Create client
        client = create_github_client("gpt-4")
        
        # Test with a simple C++ code generation prompt
        test_prompt = """
Generate a simple C++ unit test using Google Test for the following function:

```cpp
int add(int a, int b) {
    return a + b;
}
```

Please generate a complete test file with proper includes.
"""
        
        print("Testing GitHub Models integration...")
        response = client.generate_response(test_prompt)
        
        if response:
            print("✅ GitHub Models integration working!")
            print("Sample response:")
            print(response[:200] + "..." if len(response) > 200 else response)
        else:
            print("❌ GitHub Models integration failed")
            
    except Exception as e:
        print(f"❌ Error testing GitHub Models: {e}")
        print("Make sure to set GITHUB_TOKEN environment variable") 