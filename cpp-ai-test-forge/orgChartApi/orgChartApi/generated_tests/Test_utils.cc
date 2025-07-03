Below is the comprehensive unit test file for the provided `utils/utils.h` code using the Google Test framework, including Google Mock for mocking. Since the provided code does not include any database interactions, mocking database interactions is not applicable here. However, I will mock the callback function to ensure proper testing.

```cpp
// TestUtils.cc
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drogon/drogon.h>
#include <utils/utils.h>

// Mock callback function
class MockCallback {
public:
    MOCK_METHOD(void, callback, (const drogon::HttpResponsePtr &), ());
};

// Test suite for badRequest function
class TestUtils : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code, if needed
    }

    void TearDown() override {
        // Teardown code, if needed
    }
};

// Test: badRequest - Valid Input
TEST_F(TestUtils, BadRequest_ValidInput) {
    MockCallback mockCallback;

    // Create a mock callback function
    EXPECT_CALL(mockCallback, callback(::testing::_))
        .Times(1)
        .WillOnce([](const drogon::HttpResponsePtr &response) {
            // Verify the response
            EXPECT_EQ(response->getStatusCode(), drogon::k400BadRequest);
            auto json = response->getJsonObject();
            ASSERT_NE(json, nullptr);
            EXPECT_EQ((*json)["error"].asString(), "Invalid request");
        });

    // Call the badRequest function
    badRequest(
        [&mockCallback](const drogon::HttpResponsePtr &response) {
            mockCallback.callback(response);
        },
        "Invalid request"
    );
}

// Test: badRequest - Custom Status Code
TEST_F(TestUtils, BadRequest_CustomStatusCode) {
    MockCallback mockCallback;

    // Create a mock callback function
    EXPECT_CALL(mockCallback, callback(::testing::_))
        .Times(1)
        .WillOnce([](const drogon::HttpResponsePtr &response) {
            // Verify the response
            EXPECT_EQ(response->getStatusCode(), drogon::k403Forbidden);
            auto json = response->getJsonObject();
            ASSERT_NE(json, nullptr);
            EXPECT_EQ((*json)["error"].asString(), "Forbidden access");
        });

    // Call the badRequest function with a custom status code
    badRequest(
        [&mockCallback](const drogon::HttpResponsePtr &response) {
            mockCallback.callback(response);
        },
        "Forbidden access",
        drogon::k403Forbidden
    );
}

// Test: badRequest - Empty Error Message
TEST_F(TestUtils, BadRequest_EmptyErrorMessage) {
    MockCallback mockCallback;

    // Create a mock callback function
    EXPECT_CALL(mockCallback, callback(::testing::_))
        .Times(1)
        .WillOnce([](const drogon::HttpResponsePtr &response) {
            // Verify the response
            EXPECT_EQ(response->getStatusCode(), drogon::k400BadRequest);
            auto json = response->getJsonObject();
            ASSERT_NE(json, nullptr);
            EXPECT_EQ((*json)["error"].asString(), "");
        });

    // Call the badRequest function with an empty error message
    badRequest(
        [&mockCallback](const drogon::HttpResponsePtr &response) {
            mockCallback.callback(response);
        },
        ""
    );
}

// Test suite for makeErrResp function
// Test: makeErrResp - Valid Input
TEST_F(TestUtils, MakeErrResp_ValidInput) {
    std::string errorMessage = "Something went wrong";

    // Call the makeErrResp function
    Json::Value response = makeErrResp(errorMessage);

    // Verify the response
    EXPECT_EQ(response["error"].asString(), errorMessage);
}

// Test: makeErrResp - Empty Error Message
TEST_F(TestUtils, MakeErrResp_EmptyErrorMessage) {
    std::string errorMessage = "";

    // Call the makeErrResp function
    Json::Value response = makeErrResp(errorMessage);

    // Verify the response
    EXPECT_EQ(response["error"].asString(), errorMessage);
}

// Test: makeErrResp - Special Characters in Error Message
TEST_F(TestUtils, MakeErrResp_SpecialCharacters) {
    std::string errorMessage = "Error: Something went wrong! @#$%^&*()";

    // Call the makeErrResp function
    Json::Value response = makeErrResp(errorMessage);

    // Verify the response
    EXPECT_EQ(response["error"].asString(), errorMessage);
}
```

### Explanation of the Tests
1. **`badRequest` Tests**:
   - **Valid Input**: Verifies that the function correctly creates a response with the default `400 Bad Request` status and the given error message.
   - **Custom Status Code**: Verifies that the function correctly creates a response with a custom status code (e.g., `403 Forbidden`) and the given error message.
   - **Empty Error Message**: Verifies that the function handles an empty error message correctly.

2. **`makeErrResp` Tests**:
   - **Valid Input**: Verifies that the function correctly creates a JSON response with the given error message.
   - **Empty Error Message**: Verifies that the function handles an empty error message correctly.
   - **Special Characters**: Verifies that the function correctly handles error messages with special characters.

### Notes
- The `MockCallback` class is used to mock the callback function passed to `badRequest`.
- The `EXPECT_CALL` statements ensure that the mock callback is invoked and validate the response object.
- Edge cases such as empty error messages and special characters are included.
- Proper setup and teardown methods are provided, although they are not used in this specific case.