#pragma once
#include "../shared/order.h"
#include <vector>

inline std::vector<Order> get_batch_test_feed() {
    std::vector<Order> feed;

    // --- Scenario 1 ---
    feed.push_back(Order{1, Side::SELL, OrderType::LIMIT, 100.00, 10, 0});
    feed.push_back(Order{2, Side::SELL, OrderType::LIMIT, 101.00, 5, 0});
    feed.push_back(Order{3, Side::BUY,  OrderType::LIMIT, 100.50, 8, 0});
    feed.push_back(Order{4, Side::BUY,  OrderType::LIMIT, 100.00, 5, 0});

    // --- Scenario 2 ---
    feed.push_back(Order{10, Side::SELL, OrderType::LIMIT, 50.00, 10, 0});
    feed.push_back(Order{11, Side::SELL, OrderType::LIMIT, 50.00, 20, 0});
    feed.push_back(Order{12, Side::BUY,  OrderType::LIMIT, 50.00, 15, 0});

    // --- Scenario 3 & 4 ---
    feed.push_back(Order{20, Side::BUY,  OrderType::MARKET, 0.0,  12, 0});
    feed.push_back(Order{30, Side::BUY,  OrderType::LIMIT,  98.00, 10, 0});
    feed.push_back(Order{30, Side::BUY,  OrderType::CANCEL, 98.00, 0,  0});

    return feed;
}