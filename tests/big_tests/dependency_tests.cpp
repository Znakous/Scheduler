#include <scheduler.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>


int DoShit(int a, int b) {
    int* r = new int[90];
    r = nullptr;
    return (*r) + a + b;
}

int GoodBoys(int a, int b) {
    return a + b;
}

TEST(DependencyTests, CrashAvoidTest) {
    TTaskScheduler scheduler;
    int a = 10, b = 17;
    
    auto bad_boy_1 = scheduler.add(DoShit, 56, b);

    auto normal_boy_1 = scheduler.add(GoodBoys, a, b);

    auto normal_boy_2 = scheduler.add(GoodBoys, scheduler.getFutureResult<int>(normal_boy_1), b);

    auto bad_boy_3 = scheduler.add(DoShit, scheduler.getFutureResult<int>(normal_boy_2), b);

    auto normal_boy_3 = scheduler.add(GoodBoys, scheduler.getFutureResult<int>(normal_boy_2), b);

    scheduler.ExecuteDependent(normal_boy_3);

    // scheduler.executeAll(); // will cause seg fault :)
    
    ASSERT_EQ(scheduler.getResult<int>(normal_boy_3), a + b*3);
}
