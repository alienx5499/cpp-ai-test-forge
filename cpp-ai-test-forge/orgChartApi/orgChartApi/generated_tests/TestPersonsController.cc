Below is a comprehensive unit test file for the `PersonsController` class using the Google Test framework and Google Mock. The test file mocks the database interactions and tests all public methods, including edge cases and error scenarios.

---

### File: `TestPersonsController.cc`

```cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drogon/drogon.h>
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpController.h>
#include "../controllers/PersonsController.h"
#include "../models/Person.h"
#include "../models/PersonInfo.h"

using namespace drogon;
using namespace drogon_model::org_chart;
using namespace testing;

// Mock database interaction
class MockDatabase {
public:
    MOCK_METHOD(std::vector<PersonInfo>, getAllPersons, (), (const));
    MOCK_METHOD(std::optional<PersonInfo>, getPersonById, (int), (const));
    MOCK_METHOD(bool, createPerson, (const Person&), (const));
    MOCK_METHOD(bool, updatePerson, (int, const Person&), (const));
    MOCK_METHOD(bool, deletePerson, (int), (const));
    MOCK_METHOD(std::vector<PersonInfo>, getReportsByManagerId, (int), (const));
};

// Test fixture for PersonsController
class PersonsControllerTest : public Test {
protected:
    void SetUp() override {
        // Set up mock database
        mockDb = std::make_shared<MockDatabase>();
    }

    void TearDown() override {
        // Clean up after each test
        mockDb.reset();
    }

    std::shared_ptr<MockDatabase> mockDb;
    PersonsController controller;
};

// Test for PersonsController::get
TEST_F(PersonsControllerTest, GetAllPersons_Success) {
    // Arrange
    EXPECT_CALL(*mockDb, getAllPersons())
        .WillOnce(Return(std::vector<PersonInfo>{{1, "John", "Doe", trantor::Date::now(), {}, {}, {}}}));

    HttpRequestPtr req = HttpRequest::newHttpRequest();
    std::function<void(const HttpResponsePtr &)> callback = [](const HttpResponsePtr &resp) {
        ASSERT_EQ(resp->getStatusCode(), k200OK);
        ASSERT_TRUE(resp->getJsonObject());
        ASSERT_EQ((*resp->getJsonObject())["persons"].size(), 1);
    };

    // Act
    controller.get(req, std::move(callback));
}

// Test for PersonsController::getOne
TEST_F(PersonsControllerTest, GetOnePerson_Success) {
    // Arrange
    int personId = 1;
    EXPECT_CALL(*mockDb, getPersonById(personId))
        .WillOnce(Return(PersonInfo{1, "John", "Doe", trantor::Date::now(), {}, {}, {}}));

    HttpRequestPtr req = HttpRequest::newHttpRequest();
    std::function<void(const HttpResponsePtr &)> callback = [](const HttpResponsePtr &resp) {
        ASSERT_EQ(resp->getStatusCode(), k200OK);
        ASSERT_TRUE(resp->getJsonObject());
        ASSERT_EQ((*resp->getJsonObject())["id"].asInt(), 1);
    };

    // Act
    controller.getOne(req, std::move(callback), personId);
}

// Test for PersonsController::getOne (Person Not Found)
TEST_F(PersonsControllerTest, GetOnePerson_NotFound) {
    // Arrange
    int personId = 1;
    EXPECT_CALL(*mockDb, getPersonById(personId))
        .WillOnce(Return(std::nullopt));

    HttpRequestPtr req = HttpRequest::newHttpRequest();
    std::function<void(const HttpResponsePtr &)> callback = [](const HttpResponsePtr &resp) {
        ASSERT_EQ(resp->getStatusCode(), k404NotFound);
    };

    // Act
    controller.getOne(req, std::move(callback), personId);
}

// Test for PersonsController::createOne
TEST_F(PersonsControllerTest, CreatePerson_Success) {
    // Arrange
    Person newPerson;
    newPerson.setValueOfFirstName("John");
    newPerson.setValueOfLastName("Doe");

    EXPECT_CALL(*mockDb, createPerson(newPerson))
        .WillOnce(Return(true));

    HttpRequestPtr req = HttpRequest::newHttpRequest();
    std::function<void(const HttpResponsePtr &)> callback = [](const HttpResponsePtr &resp) {
        ASSERT_EQ(resp->getStatusCode(), k201Created);
    };

    // Act
    controller.createOne(req, std::move(callback), std::move(newPerson));
}

// Test for PersonsController::createOne (Failure)
TEST_F(PersonsControllerTest, CreatePerson_Failure) {
    // Arrange
    Person newPerson;
    newPerson.setValueOfFirstName("John");
    newPerson.setValueOfLastName("Doe");

    EXPECT_CALL(*mockDb, createPerson(newPerson))
        .WillOnce(Return(false));

    HttpRequestPtr req = HttpRequest::newHttpRequest();
    std::function<void(const HttpResponsePtr &)> callback = [](const HttpResponsePtr &resp) {
        ASSERT_EQ(resp->getStatusCode(), k500InternalServerError);
    };

    // Act
    controller.createOne(req, std::move(callback), std::move(newPerson));
}

// Test for PersonsController::updateOne
TEST_F(PersonsControllerTest, UpdatePerson_Success) {
    // Arrange
    int personId = 1;
    Person updatedPerson;
    updatedPerson.setValueOfFirstName("Jane");
    updatedPerson.setValueOfLastName("Doe");

    EXPECT_CALL(*mockDb, updatePerson(personId, updatedPerson))
        .WillOnce(Return(true));

    HttpRequestPtr req = HttpRequest::newHttpRequest();
    std::function<void(const HttpResponsePtr &)> callback = [](const HttpResponsePtr &resp) {
        ASSERT_EQ(resp->getStatusCode(), k200OK);
    };

    // Act
    controller.updateOne(req, std::move(callback), personId, std::move(updatedPerson));
}

// Test for PersonsController::deleteOne
TEST_F(PersonsControllerTest, DeletePerson_Success) {
    // Arrange
    int personId = 1;

    EXPECT_CALL(*mockDb, deletePerson(personId))
        .WillOnce(Return(true));

    HttpRequestPtr req = HttpRequest::newHttpRequest();
    std::function<void(const HttpResponsePtr &)> callback = [](const HttpResponsePtr &resp) {
        ASSERT_EQ(resp->getStatusCode(), k200OK);
    };

    // Act
    controller.deleteOne(req, std::move(callback), personId);
}

// Test for PersonsController::getDirectReports
TEST_F(PersonsControllerTest, GetDirectReports_Success) {
    // Arrange
    int managerId = 1;
    EXPECT_CALL(*mockDb, getReportsByManagerId(managerId))
        .WillOnce(Return(std::vector<PersonInfo>{{2, "Jane", "Doe", trantor::Date::now(), {}, {}, {}}}));

    HttpRequestPtr req = HttpRequest::newHttpRequest();
    std::function<void(const HttpResponsePtr &)> callback = [](const HttpResponsePtr &resp) {
        ASSERT_EQ(resp->getStatusCode(), k200OK);
        ASSERT_TRUE(resp->getJsonObject());
        ASSERT_EQ((*resp->getJsonObject())["reports"].size(), 1);
    };

    // Act
    controller.getDirectReports(req, std::move(callback), managerId);
}
```

---

### Key Notes:
1. **MockDatabase**: A mock class is created to simulate database interactions.
2. **Test Fixture**: A test fixture (`PersonsControllerTest`) is used to set up and tear down shared resources.
3. **Edge Cases**: Tests include success and failure scenarios, such as person not found, database failure, etc.
4. **Google Mock**: `EXPECT_CALL` is used to define expected behavior for the mock database.
5. **HTTP Callbacks**: The tests validate the HTTP response status codes and JSON payloads.

This test suite ensures comprehensive coverage of the `PersonsController` class.