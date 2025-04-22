#include <functional>
#include <cstddef>
#include <optional>
#include <memory>
#include <any>
#include <list>
#include <stdexcept>


class TTaskScheduler {
    struct TaskBase;

    template <class T>
    struct TaskIterator {
        std::list<std::unique_ptr<TaskBase>>::iterator it_;

        std::unique_ptr<TaskBase>& operator*() { return *it_; }
        std::unique_ptr<TaskBase>& operator->() { return *it_; }
    };

    template <class T>
    struct GetType { using type = T; };
    template <class T>
    struct GetType<TaskIterator<T>> { using type = T; };
    template <class T>
    using GetType_t = GetType<T>::type;


    class TaskBase {
    public:
        virtual ~TaskBase() = default;
        virtual void Execute() = 0;
        virtual std::any GetResult() const = 0;
    };

    template <class Ret, class... Args>
    class Task : public TaskBase {
        std::optional<Ret> result_;
        std::function<Ret(typename GetType<std::decay_t<Args>>::type...)> function_;
        std::tuple<Args...> args_;

    public:
        template <class Func>
        Task(Func&& func, Args&&... args)
            : function_(std::forward<Func>(func))
            , args_(std::forward<Args>(args)...) {
        }

        void Execute() override {
            if (result_) {
                return;
            }
            result_ = std::apply(function_, GetArgs(args_));
        }

        std::any GetResult() const override {
            return *result_;
        }

    private:
        template <class T>
        static T GetArg(T&& arg) {
            return std::forward<T>(arg);
        }

        template <class T>
        static T GetArg(TaskIterator<T> iter) {
            iter->Execute();
            return std::any_cast<T>(iter->GetResult());
        }

        struct GetArgsHelper {
            template <class... Args>
            auto operator()(Args&&... args) {
                return std::make_tuple(GetArg(std::forward<Args>(args))...);
            }
        };

        template <class... Args>
        static auto GetArgs(std::tuple<Args...>& t) {
            return std::apply(GetArgsHelper(), t);
        }
    };

public:
    template <class Func, class... Args>
    auto add(Func&& func, Args&&... args) {
        static_assert(std::is_invocable_v<Func, GetType_t<Args>...>,
            "Function cannot be called with given arguments");

        using Ret = std::invoke_result_t<Func, typename GetType<std::decay_t<Args>>::type...>;
        tasks_.push_back(std::make_unique<Task<Ret, Args...>>(
            std::forward<Func>(func), std::forward<Args>(args)...));
        return TaskIterator<Ret>(std::prev(tasks_.end()));
    }

    template <class T>
    TaskIterator<T> getFutureResult(TaskIterator<T> id) const {
        return id;
    }

    template <class Res>
    Res getResult(TaskIterator<Res> id) {
        id->Execute();
        try {
            return std::any_cast<Res>(id->GetResult());
        } catch (const std::bad_any_cast&) {
            throw std::runtime_error("Wrong result type requested");
        }
    }

    void executeAll() {
        for (auto& i : tasks_) {
            i->Execute();
        }
    }

private:
    std::list<std::unique_ptr<TaskBase>> tasks_;
};


#include <cassert>
#include <cmath>
#include <string>
#include <iostream>

void TestBasicOperations() {
    TTaskScheduler scheduler;

    // Простая задача без зависимостей
    auto id1 = scheduler.add([](int a, int b) { return a + b; }, 2, 3);
    scheduler.executeAll();
    assert(scheduler.getResult<int>(id1) == 5);

    // Проверка, что задача выполняется только один раз
    static int counter = 0;
    auto id2 = scheduler.add([]() { counter++; return 10; });
    scheduler.executeAll();
    scheduler.executeAll();  // Должен быть выполнен только один раз
    assert(counter == 1);
    assert(scheduler.getResult<int>(id2) == 10);
}

void TestDependentTasks() {
    TTaskScheduler scheduler;



    

    // Граф зависимых задач
    auto id1 = scheduler.add([](int a) { return a * 2; }, 5);
    auto id2 = scheduler.add([](int b) { return b + 3; }, scheduler.getFutureResult<int>(id1));
    auto id3 = scheduler.add([](int c, int d) { return c * d; },
        scheduler.getFutureResult<int>(id1),
        scheduler.getFutureResult<int>(id2));

    scheduler.executeAll();
    assert(scheduler.getResult<int>(id1) == 10);
    assert(scheduler.getResult<int>(id2) == 13);
    assert(scheduler.getResult<int>(id3) == 130);
}

void TestLazyEvaluation() {
    TTaskScheduler scheduler;

    bool executed1 = false, executed2 = false;

    auto id1 = scheduler.add([&executed1]() { executed1 = true; return 1; });
    auto id2 = scheduler.add([&executed2](int x) { executed2 = true; return x * 2; },
        scheduler.getFutureResult<int>(id1));

    // Проверяем, что задачи не выполнялись до вызова executeAll
    assert(!executed1);
    assert(!executed2);

    // Получаем результат только второй задачи - должны выполниться обе
    assert(scheduler.getResult<int>(id2) == 2);
    assert(executed1);
    assert(executed2);
}

void TestMethodAsTask() {
    struct Multiplier {
        int multiply(int x) const { return x * factor; }
        int factor;
    };

    TTaskScheduler scheduler;
    Multiplier mult{ 5 };

    auto id1 = scheduler.add([]() { return 10; });
    auto id2 = scheduler.add(&Multiplier::multiply, mult, scheduler.getFutureResult<int>(id1));

    scheduler.executeAll();
    assert(scheduler.getResult<int>(id2) == 50);
}

void TestDifferentTypes() {
    TTaskScheduler scheduler;

    auto id1 = scheduler.add([]() { return std::string("Hello"); });
    auto id2 = scheduler.add([](const std::string& s) { return s + " World"; },
        scheduler.getFutureResult<std::string>(id1));
    auto id3 = scheduler.add([](const std::string& s) { return s.size(); },
        scheduler.getFutureResult<std::string>(id2));

    scheduler.executeAll();
    assert(scheduler.getResult<std::string>(id1) == "Hello");
    assert(scheduler.getResult<std::string>(id2) == "Hello World");
    assert(scheduler.getResult<size_t>(id3) == 11);
}

void TestComplexGraph() {
    TTaskScheduler scheduler;

    // Более сложный граф вычислений
    auto idA = scheduler.add([]() { return 2; });
    auto idB = scheduler.add([]() { return 3; });
    auto idC = scheduler.add([](int a, int b) { return a + b; },
        scheduler.getFutureResult<int>(idA),
        scheduler.getFutureResult<int>(idB));
    auto idD = scheduler.add([](int c) { return c * 2; },
        scheduler.getFutureResult<int>(idC));
    auto idE = scheduler.add([](int d, int a) { return d - a; },
        scheduler.getFutureResult<int>(idD),
        scheduler.getFutureResult<int>(idA));

    scheduler.executeAll();
    assert(scheduler.getResult<int>(idA) == 2);
    assert(scheduler.getResult<int>(idB) == 3);
    assert(scheduler.getResult<int>(idC) == 5);
    assert(scheduler.getResult<int>(idD) == 10);
    assert(scheduler.getResult<int>(idE) == 8);
}

void TestQuadraticEquationExample() {
    TTaskScheduler scheduler;
    float a = 1, b = -5, c = 6;  // x² -5x +6 = 0 → x1=2, x2=3

    auto id1 = scheduler.add([](float a, float c) { return -4 * a * c; }, a, c);
    auto id2 = scheduler.add([](float b, float v) { return b * b + v; }, b, scheduler.getFutureResult<float>(id1));
    auto id3 = scheduler.add([](float b, float d) { return -b + std::sqrt(d); }, b, scheduler.getFutureResult<float>(id2));
    auto id4 = scheduler.add([](float b, float d) { return -b - std::sqrt(d); }, b, scheduler.getFutureResult<float>(id2));
    auto id5 = scheduler.add([](float a, float v) { return v / (2 * a); }, a, scheduler.getFutureResult<float>(id3));
    auto id6 = scheduler.add([](float a, float v) { return v / (2 * a); }, a, scheduler.getFutureResult<float>(id4));

    scheduler.executeAll();
    assert(std::abs(scheduler.getResult<float>(id5) - 3.0f) < 1e-6);
    assert(std::abs(scheduler.getResult<float>(id6) - 2.0f) < 1e-6);
}





void StandartTest() {
    struct AddNumber {
        float add(float a) const {
            return a + number;
        }

        float number;
    };

    float a = 1;
    float b = -2;
    float c = 0;
    AddNumber add{
      .number = 3
    };

    TTaskScheduler scheduler;

    auto id1 = scheduler.add([](float a, float c) {return -4 * a * c; }, a, c);
    auto id2 = scheduler.add([](float b, float v) {return b * b + v; }, b, scheduler.getFutureResult<float>(id1));
    auto id3 = scheduler.add([](float b, float d) {return -b + std::sqrt(d); }, b, scheduler.getFutureResult<float>(id2));
    auto id4 = scheduler.add([](float b, float d) {return -b - std::sqrt(d); }, b, scheduler.getFutureResult<float>(id2));
    auto id5 = scheduler.add([](float a, float v) {return v / (2 * a); }, a, scheduler.getFutureResult<float>(id3));
    auto id6 = scheduler.add([](float a, float v) { return v / (2 * a); }, a, scheduler.getFutureResult<float>(id4));
    auto id7 = scheduler.add(&AddNumber::add, add, scheduler.getFutureResult<float>(id6));

    scheduler.executeAll();

    assert(scheduler.getResult<float>(id5) == 2);
    scheduler.getResult<float>(id6);
    scheduler.getResult<float>(id7);
}

int main() {
    TestBasicOperations();
    TestDependentTasks();
    TestLazyEvaluation();
    TestMethodAsTask();
    TestDifferentTypes();
    TestComplexGraph();
    TestQuadraticEquationExample();
    StandartTest();

    std::cout << "All tests passed successfully!" << std::endl;
    return 0;
}
2