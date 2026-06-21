#pragma once
#include "order.h"
#include <mutex>
#include <condition_variable>
#include <deque>

class OrderQueue{
    public:
        void push(const Order& o){
            std::lock_guard<std::mutex> lock(mtx_);

            buffer_.push_back(o);

            cv_.notify_one();
        }
        Order pop(){
            std::unique_lock<std::mutex> lock(mtx_);

            cv_.wait(lock , [this] {return !buffer_.empty() || closed_; });

            if (buffer_.empty() && closed_){
                return Order{0 , Side::BUY, OrderType::CANCEL , 0.0 , 0 , 0};
            }

            Order ord = buffer_.front();
            buffer_.pop_front();
            return ord;

        } // blocks until order available

        bool try_pop(Order& out){
            std::lock_guard<std::mutex> lock(mtx_);

            if (buffer_.empty()){
                return false;
            }

            out = buffer_.front();
            buffer_.pop_front();

            return true;
        } // non-blocking : return false if empty

        void close(){
            std::lock_guard<std::mutex> lock(mtx_);

            closed_ = true;

            cv_.notify_all();
        } // signals : no more order coming
    private:
        std::deque<Order> buffer_;
        std::mutex mtx_;
        std::condition_variable cv_;
        bool closed_ = false; 
};