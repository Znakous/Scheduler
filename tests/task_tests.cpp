#include <scheduler.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>


TEST(TaskTests, LambdaTest) {
    auto T1 = Task([](const int& i, const int& j) {return i + j;}, 67, 78);
    auto el = T1.Execute();
    ASSERT_EQ(el, 67+78);
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

struct MulerLhs{
    int lhs;
    int Res(int rhs) {
        return lhs = lhs * rhs;
    }
};

TEST(TaskTests, MethodTest) {
    MulerLhs abobus;
    abobus.lhs = 3;
    bool a = is_class_member_v<decltype(&MulerLhs::Res)>;
    ASSERT_EQ(a, true);
    ASSERT_EQ(is_class_member_v<decltype(_TestFuncSum)>, false);

    auto T1 = Task(&MulerLhs::Res, abobus, 67);
    auto el = T1.Execute();
    ASSERT_EQ(el, 67*3);
    ASSERT_TRUE(T1.Calculated());
    ASSERT_EQ(el, 67*3);
    ASSERT_EQ(abobus.lhs, 67*3);
}


struct SimulatorMuler{
    int lhs;
    int Res(int rhs) const {
        return lhs * rhs;
    }
};


TEST(TaskTests, ConstMethodTest) {
    const SimulatorMuler abobus{3};
    bool a = is_class_member_v<decltype(&SimulatorMuler::Res)>;
    ASSERT_EQ(a, true);
    ASSERT_EQ(is_class_member_v<decltype(_TestFuncSum)>, false);

    auto T1 = Task(&SimulatorMuler::Res, abobus, 67);
    auto el = T1.Execute();
    ASSERT_EQ(el, 67*3);
    ASSERT_TRUE(T1.Calculated());
    ASSERT_EQ(el, 67*3);
}

TEST(TaskTests, GetArgTest) {
    auto T1 = Task([](const int& i, const int& j) {return i + j;}, 67, 78);
    ASSERT_EQ(T1.template GetArg<0>(), 67);
    ASSERT_EQ(T1.template GetArg<1>(), 78);
}


TEST(TaskTests, CalcTimeTest) {
    auto T1 = Task([](const int& i, const int& j) {return i + j;}, 67, 78);
    ASSERT_FALSE(T1.Calculated());
    auto el = T1.Execute();
    ASSERT_EQ(el, 67+78);
    ASSERT_TRUE(T1.Calculated());
    ASSERT_EQ(el, 67+78);
}