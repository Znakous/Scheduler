#include <scheduler.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>


template<typename First>
auto Summ(First&& f) {
    return f;
}

template<typename First, typename... Args>
auto Summ(First&& first, Args&&... args) {
    return first + Summ(args...);
}



struct Adder {
    auto Add(int a, int b) { return a+b; }
};

TEST(SchedulerTests, BigTest) {
    TTaskScheduler scheduler;

    auto T1 = scheduler.add(Summ<int, int, int, int, int>, 10000, 2000, 300, 40, 5);
    auto res_sum = scheduler.getFutureResult<int>(T1);

    auto T2 = scheduler.add([](int a) {return a * 100000;}, res_sum);
    auto res_mult = scheduler.getFutureResult<int>(T2);

    Adder adder_entity;
    auto T3 = scheduler.add(&Adder::Add, adder_entity, res_mult, res_sum);

    scheduler.executeAll();
    auto el = scheduler.getResult<int>(T3);

    ASSERT_EQ(el, 1234512345);
}
