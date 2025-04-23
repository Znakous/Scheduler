#include <scheduler.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>


struct AddNumber {
    float add(float a) const {
        return a + number;
    }

    float number;
};

TEST(SchedulerTests, TestFromMain) {
    float a = 1;
    float b = -6;
    float c = 8;
    AddNumber add{
        .number = 3.5
    };

    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](float a, float c) {return -4 * a * c;}, a, c);

    auto id2 = scheduler.add([](float b, float v) {return b * b + v;}, b, scheduler.getFutureResult<float>(id1));

    auto id3 = scheduler.add([](float b, float d) {return -b + std::sqrt(d);}, b, scheduler.getFutureResult<float>(id2));

    auto id4 = scheduler.add([](float b, float d) {return -b - std::sqrt(d);}, b, scheduler.getFutureResult<float>(id2));

    auto id5 = scheduler.add([](float a, float v) {return v / (2 * a);}, a, scheduler.getFutureResult<float>(id3));

    auto id6 = scheduler.add([](float a, float v) {return v / (2 * a);}, a, scheduler.getFutureResult<float>(id4));

    auto id7 = scheduler.add(&AddNumber::add, add, scheduler.getFutureResult<float>(id6));

    scheduler.executeAll();

    ASSERT_EQ(scheduler.getResult<float>(id5), 4);
    ASSERT_EQ(scheduler.getResult<float>(id6), 2);
    ASSERT_EQ(scheduler.getResult<float>(id7), 5.5);
}