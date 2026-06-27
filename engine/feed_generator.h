#pragma once
#include <random>
#include <chrono>
#include "../shared/queue.h"

class feedGenerator {
    private:
        uint64_t next_order_id_;
    public:
        feedGenerator(){
            next_order_id_ = 1;
        }

        void runLiveFeed(OrderQueue& ordQ, int num_orders);
}
