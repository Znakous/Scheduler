#include <scheduler.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

struct Adder {
    consteval Adder(int ident)
     : identity(ident) {}

    Adder(const Adder&) = delete;
    Adder& operator=(const Adder&) = delete;

    int Add5(int a, int b, int c, int d, int e) const { return a+b+c+d+e; }
    int Add(int a, int b) const { return a+b; }
    int Mult100000(int a) const { return a * 100000; }
    
    int identity;
    int AddIdentity(int a) const { 
        std::cout << identity << "\n";
        return a + identity;
    }
};

template<bool is_const, int identity>
int WorkWithIdentity() {
    TTaskScheduler scheduler;
    conditional_t<is_const, Adder, const Adder> adder_entity(identity);

    auto T1 = scheduler.add(&Adder::Add5, adder_entity, 10000, 2000, 300, 40, 5);
    auto res_sum = scheduler.getFutureResult<int>(T1);

    auto T2 = scheduler.add(&Adder::Mult100000, adder_entity, res_sum);
    auto res_mult = scheduler.getFutureResult<int>(T2);
    
    auto T3 = scheduler.add(&Adder::Add, adder_entity, res_mult, res_sum);
    auto res_sum2 = scheduler.getFutureResult<int>(T3);

    auto T4 = scheduler.add(&Adder::AddIdentity, adder_entity, res_sum2);

    scheduler.executeAll();
    auto el = scheduler.getResult<int>(T4);

    return el;
}

TEST(SchedulerTests, ClassTest) {
    constexpr int x = 1;
    constexpr int y = 19;
    ASSERT_EQ((WorkWithIdentity<false, x>()), 1234512345 + x);
    ASSERT_EQ((WorkWithIdentity<true, x>()), 1234512345 + x);
    ASSERT_EQ((WorkWithIdentity<false, y>()), 1234512345 + y);
    ASSERT_EQ((WorkWithIdentity<true, y>()), 1234512345 + y);
}
