Below is the comprehensive unit test file for the provided `DepartmentsController` class. The test file uses the Google Test framework and Google Mock to mock database interactions and test all public methods, including edge cases and error scenarios.

---

### File: `TestDepartmentsController.cc`

```cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drogon/HttpAppFramework.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpController.h>
#include "../controllers/DepartmentsController.h"
#include "../models/Department.h"

using namespace drogon;
using namespace drogon_model::org_chart;
using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::DoAll;
using ::testing::SetArgReferee;

// Mock database or service layer
class MockDatabase {
public:
    MOCK_METHOD(std::vector<Department>, getAllDepartments, (), (const));
    MOCK_METHOD(std::optional<Department>, getDepartmentById, (int), (const));
    MOCK_METHOD(bool, createDepartment, (const Department&), (const));
    MOCK_METHOD(bool, updateDepartment, (int, const Department&), (const));
    MOCK_METHOD(bool, deleteDepartment, (int), (const));
    MOCK_METHOD(std::vector<std::string>, getPersonsByDepartmentId, (int), (const));
};

// Test fixture for DepartmentsController
class DepartmentsControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up mock database
        mockDb = std::make_shared<MockDatabase>();
        controller = std::make_unique<DepartmentsController>();
    }

    void TearDown() override {
        // Clean up resources
        mockDb.reset();
        controller.reset();
    }

    std::shared_ptr<MockDatabase> mockDb;
    std::unique_ptr<DepartmentsController> controller;
};

// Test: Get all departments
TEST_F(DepartmentsControllerTest, GetAllDepartments_Success) {
    // Arrange
    std::vector<Department> mockDepartments = {
        Department(1, "HR"), Department(2, "Engineering")
    };
    EXPECT_CALL(*mockDb, getAllDepartments())
        .WillOnce(Return(mockDepartments));

    auto req = HttpRequest::newHttpRequest();
    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), k200OK);
        EXPECT_NE(resp->getBody(), "");
    };

    // Act
    controller->get(req, std::move(callback));
}

// Test: Get one department by ID (success)
TEST_F(DepartmentsControllerTest, GetOneDepartment_Success) {
    // Arrange
    int departmentId = 1;
    Department mockDepartment(departmentId, "HR");
    EXPECT_CALL(*mockDb, getDepartmentById(departmentId))
        .WillOnce(Return(mockDepartment));

    auto req = HttpRequest::newHttpRequest();
    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), k200OK);
        EXPECT_NE(resp->getBody(), "");
    };

    // Act
    controller->getOne(req, std::move(callback), departmentId);
}

// Test: Get one department by ID (not found)
TEST_F(DepartmentsControllerTest, GetOneDepartment_NotFound) {
    // Arrange
    int departmentId = 999;
    EXPECT_CALL(*mockDb, getDepartmentById(departmentId))
        .WillOnce(Return(std::nullopt));

    auto req = HttpRequest::newHttpRequest();
    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), k404NotFound);
    };

    // Act
    controller->getOne(req, std::move(callback), departmentId);
}

// Test: Create a new department (success)
TEST_F(DepartmentsControllerTest, CreateDepartment_Success) {
    // Arrange
    Department newDepartment(0, "Finance");
    EXPECT_CALL(*mockDb, createDepartment(newDepartment))
        .WillOnce(Return(true));

    auto req = HttpRequest::newHttpJsonRequest(newDepartment.toJson());
    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), k201Created);
    };

    // Act
    controller->createOne(req, std::move(callback), std::move(newDepartment));
}

// Test: Create a new department (failure)
TEST_F(DepartmentsControllerTest, CreateDepartment_Failure) {
    // Arrange
    Department newDepartment(0, "Finance");
    EXPECT_CALL(*mockDb, createDepartment(newDepartment))
        .WillOnce(Return(false));

    auto req = HttpRequest::newHttpJsonRequest(newDepartment.toJson());
    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), k500InternalServerError);
    };

    // Act
    controller->createOne(req, std::move(callback), std::move(newDepartment));
}

// Test: Update a department (success)
TEST_F(DepartmentsControllerTest, UpdateDepartment_Success) {
    // Arrange
    int departmentId = 1;
    Department updatedDepartment(departmentId, "Updated HR");
    EXPECT_CALL(*mockDb, updateDepartment(departmentId, updatedDepartment))
        .WillOnce(Return(true));

    auto req = HttpRequest::newHttpJsonRequest(updatedDepartment.toJson());
    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), k200OK);
    };

    // Act
    controller->updateOne(req, std::move(callback), departmentId, std::move(updatedDepartment));
}

// Test: Update a department (not found)
TEST_F(DepartmentsControllerTest, UpdateDepartment_NotFound) {
    // Arrange
    int departmentId = 999;
    Department updatedDepartment(departmentId, "Non-existent");
    EXPECT_CALL(*mockDb, updateDepartment(departmentId, updatedDepartment))
        .WillOnce(Return(false));

    auto req = HttpRequest::newHttpJsonRequest(updatedDepartment.toJson());
    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), k404NotFound);
    };

    // Act
    controller->updateOne(req, std::move(callback), departmentId, std::move(updatedDepartment));
}

// Test: Delete a department (success)
TEST_F(DepartmentsControllerTest, DeleteDepartment_Success) {
    // Arrange
    int departmentId = 1;
    EXPECT_CALL(*mockDb, deleteDepartment(departmentId))
        .WillOnce(Return(true));

    auto req = HttpRequest::newHttpRequest();
    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), k200OK);
    };

    // Act
    controller->deleteOne(req, std::move(callback), departmentId);
}

// Test: Delete a department (not found)
TEST_F(DepartmentsControllerTest, DeleteDepartment_NotFound) {
    // Arrange
    int departmentId = 999;
    EXPECT_CALL(*mockDb, deleteDepartment(departmentId))
        .WillOnce(Return(false));

    auto req = HttpRequest::newHttpRequest();
    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), k404NotFound);
    };

    // Act
    controller->deleteOne(req, std::move(callback), departmentId);
}

// Test: Get persons by department ID (success)
TEST_F(DepartmentsControllerTest, GetDepartmentPersons_Success) {
    // Arrange
    int departmentId = 1;
    std::vector<std::string> mockPersons = {"Alice", "Bob"};
    EXPECT_CALL(*mockDb, getPersonsByDepartmentId(departmentId))
        .WillOnce(Return(mockPersons));

    auto req = HttpRequest::newHttpRequest();
    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), k200OK);
        EXPECT_NE(resp->getBody(), "");
    };

    // Act
    controller->getDepartmentPersons(req, std::move(callback), departmentId);
}

// Test: Get persons by department ID (not found)
TEST_F(DepartmentsControllerTest, GetDepartmentPersons_NotFound) {
    // Arrange
    int departmentId = 999;
    EXPECT_CALL(*mockDb, getPersonsByDepartmentId(departmentId))
        .WillOnce(Return(std::vector<std::string>{}));

    auto req = HttpRequest::newHttpRequest();
    auto callback = [](const HttpResponsePtr &resp) {
        EXPECT_EQ(resp->getStatusCode(), k404NotFound);
    };

    // Act
    controller->getDepartmentPersons(req, std::move(callback), departmentId);
}
```

---

### Key Notes:
1. **Mock Database**: The `MockDatabase` class is used to simulate database interactions.
2. **Test Fixture**: The `DepartmentsControllerTest` fixture sets up the controller and mock database for each test.
3. **Edge Cases**: Tests include scenarios for success, failure, and not-found cases.
4. **Google Mock**: The `EXPECT_CALL` macros are used to define expectations for mock database methods.
5. **HTTP Responses**: Tests validate HTTP response status codes and bodies.

This test suite provides comprehensive coverage for the `DepartmentsController` class.