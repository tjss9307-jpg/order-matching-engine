#pragma once
#include "../shared/order.h"
#include <map>
#include <deque>
#include <unordered_map>
#include <vector>

struct OrderLocation {
    Side side;
    double price;
};

class OrderBook {
public:
    std::vector<Trade> addOrder(const Order& o);
    bool cancelOrder(uint64_t order_id);

private:
    std::map<double, std::deque<Order>, std::greater<double>> bids_;
    std::map<double, std::deque<Order>> asks_;
    std::unordered_map<uint64_t, OrderLocation> index_;
    uint64_t next_trade_id_ = 1;
};