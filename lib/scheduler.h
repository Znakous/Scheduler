#pragma once


struct ret {};
// TODO: Implement scheduler
class TTaskScheduler {
public:
    template<typename T, typename A, typename B>
    ret add(T t, A a, B b){return ret();}


    template<typename T, typename A>
    ret getFutureResult(A a){return ret();}

    template<typename T, typename A>
    int getResult(A a){return int();}


    void executeAll(){}
};
