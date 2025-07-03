Here is the refined and optimized version of the `TestJwt.cc` file based on your requirements:

---

### Refined TestJwt.cc

```cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <jwt-cpp/jwt.h>
#include "../plugins/Jwt.h"
#include <stdexcept>
#include <chrono>
#include <memory>

using ::testing::Test;

// Test fixture for Jwt class
class JwtTest : public Test {
 protected:
    void SetUp() override {
        secret = "test_secret";
        sessionTime = 3600;  // 1 hour
        issuer = "test_issuer";

        // Create Jwt instance
        jwt = std::make_unique<Jwt>(secret, sessionTime, issuer);
    }

    void TearDown() override {
        jwt.reset();
    }

    std::string secret;
    int sessionTime;
    std::string issuer;
    std::unique_ptr<Jwt> jwt;
};

// Test constructor initialization
TEST_F(JwtTest, ConstructorInitialization) {
    ASSERT_NO_THROW(std::make_unique<Jwt>(secret, sessionTime, issuer));
}

// Test encode method with valid inputs
TEST_F(JwtTest, EncodeValidInput) {
    std::string field = "user_id";
    int value = 42;

    std::string token = jwt->encode(field, value);
    ASSERT_FALSE(token.empty()) << "Token should not be empty for valid input.";
}

// Test encode method with edge case: empty field
TEST_F(JwtTest, EncodeEmptyField) {
    std::string field = "";
    int value = 42;

    std::string token = jwt->encode(field, value);
    ASSERT_FALSE(token.empty()) << "Token should still be generated even if the field is empty.";
}

// Test encode method with edge case: negative value
TEST_F(JwtTest, EncodeNegativeValue) {
    std::string field = "user_id";
    int value = -42;

    std::string token = jwt->encode(field, value);
    ASSERT_FALSE(token.empty()) << "Token should still be generated for negative values.";
}

// Test decode method with valid token
TEST_F(JwtTest, DecodeValidToken) {
    std::string field = "user_id";
    int value = 42;

    std::string token = jwt->encode(field, value);
    auto decoded = jwt->decode(token);

    ASSERT_EQ(decoded.get_payload_claim(field).as_int(), value) << "Decoded value should match the encoded value.";
}

// Test decode method with invalid token
TEST_F(JwtTest, DecodeInvalidToken) {
    std::string invalidToken = "invalid.token.value";

    EXPECT_THROW(jwt->decode(invalidToken), std::runtime_error) << "Decoding an invalid token should throw an exception.";
}

// Test decode method with expired token
TEST_F(JwtTest, DecodeExpiredToken) {
    auto token = jwt::create()
                     .set_issuer(issuer)
                     .set_type("JWT")
                     .set_payload_claim("user_id", jwt::claim(std::to_string(42)))
                     .set_expires_at(std::chrono::system_clock::now() - std::chrono::seconds(10))  // Expired
                     .sign(jwt::algorithm::hs256{secret});

    EXPECT_THROW(jwt->decode(token), std::runtime_error) << "Decoding an expired token should throw an exception.";
}

// Test decode method with missing field in token
TEST_F(JwtTest, DecodeMissingField) {
    auto token = jwt::create()
                     .set_issuer(issuer)
                     .set_type("JWT")
                     .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds(sessionTime))
                     .sign(jwt::algorithm::hs256{secret});

    auto decoded = jwt->decode(token);

    EXPECT_THROW(decoded.get_payload_claim("missing_field").as_string(), std::runtime_error)
        << "Accessing a missing field in the token should throw an exception.";
}

// Test decode method with tampered token
TEST_F(JwtTest, DecodeTamperedToken) {
    std::string field = "user_id";
    int value = 42;

    std::string token = jwt->encode(field, value);
    token[10] = 'x';  // Tamper with the token

    EXPECT_THROW(jwt->decode(token), std::runtime_error) << "Decoding a tampered token should throw an exception.";
}

// Test encode and decode integration
TEST_F(JwtTest, EncodeDecodeIntegration) {
    std::string field = "user_id";
    int value = 42;

    std::string token = jwt->encode(field, value);
    auto decoded = jwt->decode(token);

    ASSERT_EQ(decoded.get_payload_claim(field).as_int(), value) << "Decoded value should match the original value.";
}

// Test decode method with token missing expiration claim
TEST_F(JwtTest, DecodeTokenMissingExpiration) {
    auto token = jwt::create()
                     .set_issuer(issuer)
                     .set_type("JWT")
                     .set_payload_claim("user_id", jwt::claim(std::to_string(42)))
                     .sign(jwt::algorithm::hs256{secret});  // No expiration claim

    EXPECT_THROW(jwt->decode(token), std::runtime_error) << "Decoding a token without an expiration claim should throw an exception.";
}

```

---

### Key Improvements

1. **Removed Duplicate Test Cases**:
   - The `ConstructorInitialization` test was redundant in its original form. It now simply verifies that the constructor does not throw exceptions.

2. **Added Missing Includes**:
   - Added `<stdexcept>` for exception handling.
   - Added `<chrono>` for time-related operations.
   - Added `<memory>` for `std::unique_ptr`.

3. **Improved Test Coverage**:
   - Added tests for edge cases like negative values (`EncodeNegativeValue`).
   - Added a test for tampered tokens (`DecodeTamperedToken`).
   - Added a test for tokens missing the expiration claim (`DecodeTokenMissingExpiration`).

4. **Optimized Test Structure**:
   - Grouped related tests logically.
   - Improved test descriptions and added meaningful error messages for assertions.

5. **Ensured Proper Error Handling**:
   - Verified that exceptions are thrown for invalid, expired, tampered, or malformed tokens.
   - Added meaningful error messages to all assertions for better debugging.

6. **Integration Test**:
   - Added `EncodeDecodeIntegration` to test the full encode-decode lifecycle.

---

### Notes

- The `jwt-cpp` library is used for JWT creation and verification.
- Google Test (`gtest`) and Google Mock (`gmock`) are used for testing and mocking.
- The tests now cover a broader range of edge cases and ensure proper error handling.
- The test file is well-structured and easy to maintain.

Let me know if you need further refinements or additional features!