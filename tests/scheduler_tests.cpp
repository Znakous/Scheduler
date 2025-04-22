#include <scheduler.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>


TEST(SchedulerTests, SimpleAddTest) {
    TTaskScheduler scheduler;
    auto T1 = scheduler.add([](const int& i, const int& j) {return i + j;}, 67, 78);
    auto el = scheduler.getResult<int>(T1);
    ASSERT_EQ(el, 67+78);
}
TEST(SchedulerTests, MultipleArgAddTest) {
    TTaskScheduler scheduler;
    auto T1 = scheduler.add([](int a, int b, int c, int d) {return a+b+c+d;}, 1, 2, 3, 4);
    auto el = scheduler.getResult<int>(T1);
    ASSERT_EQ(el, 1+2+3+4);
}

TEST(SchedulerTests, FutureAddTest) {
    TTaskScheduler scheduler;
    auto T1 = scheduler.add([](int i, int j) {return i + j;}, 1, 2);
    std::cout << "added task\n";
    auto res1 = scheduler.getFutureResult<int>(T1);
    std::cout << "got fut res\n";
    auto T2 = scheduler.add([](int a, int b, int c) {return a+b+c;}, res1, 3, 4);
    scheduler.executeAll();
    auto el = scheduler.getResult<int>(T2);
    ASSERT_EQ(el, 1+2+3+4);
}

