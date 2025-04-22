#pragma once


template<typename T>
struct ListNode
{
    LisNode(T&& val)
     : value(std::forward<T> (val)), prev(nullptr), next(nullptr) 
    {}

    T value;
    ListNode<T>* prev;
    ListNode<T>* next;
};

template<typename... Args>
auto MakeNode(Args&&... args) {
    return (new ListNode(std::forward<Args> (args)...));
}

template<typename T>
struct List{

    List() 
     : tail_(nullptr), head_(nullptr) 
    {}

    template<typename T_T = T>
    void push_back(T_T&& val) {
        emplace_back(std::forward<T_T> (val));
    }

    template<typename... Args>
    void emplace_back(Args&&... args) {
        auto node_ptr = MakeNode(std::forward<Args> (args)...);
        if (!tail_) {
            tail_ = node_ptr;
            head_ = node_ptr;
            return;
        }
        node_ptr->prev = tail_;
        tail_->next = node_ptr;
        tail_ = node_ptr;
    }
    
private:
    ListNode<T>* head_;
    ListNode<T>* tail_;
};