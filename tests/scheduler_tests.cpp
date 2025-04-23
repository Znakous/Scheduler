#include <scheduler.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>


TEST(SchedulerTests, SimpleAddTest) {
    TTaskScheduler scheduler;
    auto T1 = scheduler.add([](const int& i, const int& j) {return i + j;}, 67, 78);
    auto el = scheduler.getResult<int>(T1);
    ASSERT_EQ(el, 67+78);
}

TEST(SchedulerTests, NoArgAddTest) {
    TTaskScheduler scheduler;
    auto T1 = scheduler.add([]() {return "aboba";});
    scheduler.executeAll();
    auto el = scheduler.getResult<const char*>(T1);
    ASSERT_EQ(el, "aboba");
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
    auto res1 = scheduler.getFutureResult<int>(T1);
    auto T2 = scheduler.add([](int a, int b, int c) {return a+b+c;}, res1, 3, 4);
    scheduler.executeAll();
    auto el = scheduler.getResult<int>(T2);
    ASSERT_EQ(el, 1+2+3+4);
}

TEST(SchedulerTests, MixedTypesTest) {
    TTaskScheduler scheduler;
    
    auto T1 = scheduler.add([]() { return std::string("Hello"); });
    auto T2 = scheduler.add([](const std::string& s) { return s + " World"; }, 
                          scheduler.getFutureResult<std::string>(T1));
    auto T3 = scheduler.add([](const std::string& s) { return s.size(); }, 
                          scheduler.getFutureResult<std::string>(T2));
    
    scheduler.executeAll();
    
    ASSERT_EQ(scheduler.getResult<std::string>(T2), "Hello World");
    ASSERT_EQ(scheduler.getResult<size_t>(T3), 11);
}

TEST(SchedulerTests, VoidTest) {
    TTaskScheduler scheduler;
    int x=0;
    
    auto T1 = scheduler.add([&x]() { x++;return; });
    auto T2 = scheduler.add([&x]() { x++;return; });
    auto T3 = scheduler.add([&x]() { x++;return; });

    scheduler.executeAll();
    ASSERT_EQ(x, 3);
}

TEST(SchedulerTests, VoidTest2) {
    TTaskScheduler scheduler;
    int x=0;

    auto T1 = scheduler.add([&x]() { x++;return; });
    auto T2 = scheduler.add([](int x) { return x + 1;}, x);

    x = scheduler.getResult<int> (T2);
    scheduler.executeAll();
    ASSERT_EQ(x, 2);
}

struct Voider {
    int* changeable;
    void VoidMethod1() {
        (*changeable)++;
    }
    void VoidMethod2() {
        (*changeable)*=2;
    }
    void VoidMethod3(int a) {
        (*changeable)+=a;
    }
    void VoidWrite(int a) {
        (*changeable)=a;
    }
};

TEST(SchedulerTests, VoidClassMethodTest) {
    TTaskScheduler scheduler;
    int x=0;
    Voider DarthVoider(&x);

    auto T1 = scheduler.add(&Voider::VoidMethod1, DarthVoider);
    auto T2 = scheduler.add(&Voider::VoidMethod2, DarthVoider);
    auto T3 = scheduler.add(&Voider::VoidMethod3, DarthVoider, 5);

    scheduler.executeAll();
    ASSERT_EQ(x, 7);
}
TEST(SchedulerTests, VoidMixTest) {
    TTaskScheduler scheduler;
    int x=0;
    Voider DarthVoider(&x);

    auto T1 = scheduler.add(&Voider::VoidMethod1, DarthVoider);
    auto T2 = scheduler.add(&Voider::VoidMethod2, DarthVoider);
    scheduler.executeAll();

    auto T3 = scheduler.add([](int x) {return x * 3;}, x);
    auto T3_result = scheduler.getFutureResult<int>(T3);
    
    auto T4 = scheduler.add(&Voider::VoidWrite, DarthVoider, T3_result);

    scheduler.executeAll();
    ASSERT_EQ(x, 6);
}

TEST(SchedulerTests, PerformanceTest) {
    TTaskScheduler scheduler;
    constexpr int n = 30;
    
    std::vector<SchedulerIterator> iters;
    iters.push_back(scheduler.add([](){return 1;}));

    for (int i = 0; i < n; ++i) {
        iters.push_back(scheduler.add([](int e) { return e + 1; }, scheduler.getFutureResult<int>(iters.back())));
    }
    
    scheduler.executeAll();
    
    for (int i = 0; i < n; ++i) {
        ASSERT_EQ(scheduler.getResult<int>(iters[i]), i+1);
    }
}

struct CustVect {
    std::vector<std::string> v;
    size_t Push(const std::string& s) {
        v.push_back(s);
        return v.size();
    }
};

TEST(SchedulerTests, PushTest) {
    TTaskScheduler scheduler;
    int x=0;
    CustVect vector;
    auto T1 = scheduler.add(&CustVect::Push, vector, "i");
    auto T2 = scheduler.add(&CustVect::Push, vector, "love");
    auto T3 = scheduler.add(&CustVect::Push, vector, "c++");

    scheduler.executeAll();
    ASSERT_EQ(scheduler.getResult<size_t>(T1), 1);
    ASSERT_EQ(scheduler.getResult<size_t>(T2), 2);
    ASSERT_EQ(scheduler.getResult<size_t>(T3), 3);
    ASSERT_THAT(vector.v, ::testing::ElementsAre("i", "love", "c++"));
}