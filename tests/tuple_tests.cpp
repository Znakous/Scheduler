#include <my_tuple.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>



TEST(TupleTest, SimpleTest) {
    Tuple<int, std::string> tuple(78, "chivapchichi");
    ASSERT_EQ(tuple.Get<0>(), 78);
    ASSERT_EQ(tuple.Get<1>(), "chivapchichi");
    std::string exc_status = "";
    try {
        auto got = tuple.Get<2>();
    } catch (const std::exception& exc) {
        exc_status = exc.what();
    }
    ASSERT_EQ(exc_status, "index outside of tuple bounds");
    // ASSERT_THAT(result, testing::ElementsAre("hello", "hi", "hippohondric", "hola mundo"));
}
