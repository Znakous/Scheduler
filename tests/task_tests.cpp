#include <scheduler.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>


TEST(TaskTests, LambdaTest) {
    auto T1 = Task([](const int& i, const int& j) {return i + j;}, 67, 78);
}

int _TestFuncSum(int a, int b) {
    return a + b;
}
TEST(TaskTests, FunctionTest) {
    auto T1 = Task(_TestFuncSum, 67, 78);
    auto el = T1.Execute();
    ASSERT_EQ(el, 67+78);
}


struct Muler{
    auto operator()(int a) const {
        return a * 2;
    }
};

TEST(TaskTests, FunctorTest) {
    auto T1 = Task(Muler(), 67);
    ASSERT_EQ(T1.template GetArg<0>(), 67);
    auto el = T1.Execute();
    ASSERT_EQ(el, 67*2);
    // ASSERT_EQ(T1.template GetArg<1>(), 78);
}

TEST(TaskTests, GetArgTest) {
    auto T1 = Task([](const int& i, const int& j) {return i + j;}, 67, 78);
    ASSERT_EQ(T1.template GetArg<0>(), 67);
    ASSERT_EQ(T1.template GetArg<1>(), 78);
}