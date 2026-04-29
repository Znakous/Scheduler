#include <my_optional.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>



TEST(OptionalTests, NoValueTest) {
    Optional<std::string> op_string;
    ASSERT_EQ(op_string.has_value(), false);
    std::string exc_status = "";
    try {
        auto l = op_string.value();
    } catch (const std::exception& exc) {
        exc_status = exc.what();
    }
    ASSERT_EQ(exc_status, "optional doesn't contain value");
}


TEST(OptionalTests, HasValueTest) {
    Optional<std::string> op_string("aboba");
    ASSERT_EQ(op_string.has_value(), true);
    ASSERT_EQ(op_string.value(), "aboba");
}

struct NoCopyable{
    NoCopyable(){}
    NoCopyable(const NoCopyable&) = delete;
    NoCopyable(NoCopyable&&) {}
    NoCopyable& operator=(const NoCopyable&) = delete;
    NoCopyable& operator=(NoCopyable&&) {return *this; };
};

TEST(OptionalTests, NoCopyableTest) {
    Optional<NoCopyable> op_string = Optional(NoCopyable()); // shouldn't throw
    ASSERT_EQ(op_string.has_value(), true);
}
