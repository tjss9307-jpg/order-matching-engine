#pragma once
#include "../shared/order.h"
#include <vector>

inline std::vector<std::vector<Order>> get_batch_test_feeds() {
    std::vector<std::vector<Order>> feeds;

    std::vector<Order> feed1 , feed2 , feed3 , feed4;

    // --- Scenario 1 ---
    feed1 = {
            {1, Side::SELL, OrderType::LIMIT, 100.00, 10, 0},
            {2, Side::SELL, OrderType::LIMIT, 101.00, 5, 0},
            {3, Side::BUY,  OrderType::LIMIT, 100.50, 8, 0},
            {4, Side::BUY,  OrderType::LIMIT, 100.00, 5, 0}
        };

    feeds.push_back(feed1);

    // --- Scenario 2 ---
    feed2 = {
            {10, Side::SELL, OrderType::LIMIT, 50.00, 10, 0},
            {11, Side::SELL, OrderType::LIMIT, 50.00, 20, 0},
            {12, Side::BUY,  OrderType::LIMIT, 50.00, 15, 0}
        };

    feeds.push_back(feed2);

    // --- Scenario 3  ---
    feed3 = {
            {1, Side::SELL, OrderType::LIMIT, 100.00, 5, 0},
            {2, Side::SELL, OrderType::LIMIT, 101.00, 10, 0},
            {20, Side::BUY, OrderType::MARKET, 0.00, 12, 0} 
        };
    
    feeds.push_back(feed3);

    // --- Scenario 4 ---
    feed4 = {
            {30, Side::BUY, OrderType::LIMIT, 98.00, 10, 0},
            {30, Side::BUY, OrderType::CANCEL, 98.00, 10, 0}
        };

    feeds.push_back(feed4);
    return feeds;
}