#include <my_any.h>
#include <my_tuple.h>

#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

struct smth{
    bool operator==(const smth&) const {return true;}
};

TEST(AnyTests, SimpleTest) {
    Any any = 56;
    ASSERT_EQ(any.Get<int>().has_value(), true);
    ASSERT_EQ(any.Get<int>().value(), 56);
}

TEST(AnyTests, WrongTypeTest) {
    Any any = 56;
    ASSERT_EQ(any.Get<std::string>().has_value(), false);
    ASSERT_EQ(any.Get<smth>().has_value(), false);
    ASSERT_EQ(any.Get<int>().has_value(), true);
}

TEST(AnyTests, CustomTypeTest) {
    Any any = smth();
    ASSERT_EQ(any.Get<std::string>().has_value(), false);
    ASSERT_EQ(any.Get<smth>().has_value(), true);
    ASSERT_EQ(any.Get<int>().has_value(), false);
}

TEST(AnyTests, VectorTest) {
    Any any = std::vector<int> {45, 67, 78};
    ASSERT_EQ(any.Get<std::string>().has_value(), false);
    ASSERT_EQ(any.Get<smth>().has_value(), false);
    ASSERT_EQ(any.Get<int>().has_value(), false);
    ASSERT_EQ(any.Get<std::vector<int>>().has_value(), true);
    ASSERT_THAT(any.Get<std::vector<int>>().value(), ::testing::ElementsAre(45, 67, 78));
}

TEST(AnyTests, TupleTest) {
    using typee = Tuple<int, smth, std::string, bool>;
    using little_wrong = Tuple<int, smth, std::string, int>;

    Any any = typee(90, smth(), "aboba", true);
    ASSERT_EQ(any.Get<std::string>().has_value(), false);
    ASSERT_EQ(any.Get<smth>().has_value(), false);
    ASSERT_EQ(any.Get<int>().has_value(), false);
    ASSERT_EQ(any.Get<std::vector<int>>().has_value(), false);

    ASSERT_EQ(any.Get<typee>().has_value(), true);
    ASSERT_EQ(any.Get<little_wrong>().has_value(), false);

    ASSERT_EQ(any.Get<typee>().value().Get<0>(), 90);
    ASSERT_EQ(any.Get<typee>().value().Get<1>(), smth());
    ASSERT_EQ(any.Get<typee>().value().Get<2>(), "aboba");
    ASSERT_EQ(any.Get<typee>().value().Get<3>(), true);
}


