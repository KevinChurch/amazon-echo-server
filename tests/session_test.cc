#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "session.h"

// class MockSession : public Session {
// public:
//     MockSession(boost::asio::io_service& io_service) : Session(io_service) {}
// };

// class MockIOService : public boost::asio::io_service {
// public:
//     MOCK_METHOD1(Equals, bool(const MockIOService &));
//     virtual bool operator==(const MockIOService & rhs) { return Equals(rhs); }
// };

// class SessionTest : public ::testing::Test {
// protected:
//     MockIOService io_service_;
//     Session* new_session = new Session(io_service_);
// };

// TEST_F(SessionTest, Socket) {
//     EXPECT_EQ(new_session->socket(), io_service_);
// }

// TEST_F(SessionTest, Start) {
//     std::string reply = sess->get_reply();
//     EXPECT_FALSE(reply.empty());
//     EXPECT_EQ(reply.substr(0, 17), "HTTP/1.1 200 OK\r\n");
// }

// TEST_F(SessionTest, HandleRequest) {
//     std::string reply = sess->get_reply();
//     EXPECT_FALSE(reply.empty());
//     EXPECT_EQ(reply.substr(0, 17), "HTTP/1.1 200 OK\r\n");
// }

// TEST_F(SessionTest, WriteString) {
//     std::string reply = sess->get_reply();
//     EXPECT_FALSE(reply.empty());
//     EXPECT_EQ(reply.substr(0, 17), "HTTP/1.1 200 OK\r\n");
// }