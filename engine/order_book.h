#pragma once
#include "../shared/order.h"
#include <map>
#include <deque>
#include <unordered_map>
#include <vector>
#include <iostream>

struct OrderLocation {
    Side side;
    double price;
};

class OrderBook {
public:
    void printBook();
    std::vector<Trade> addOrder(const Order& o);
    bool cancelOrder(uint64_t order_id);

private:
    template <typename MapType>
        bool performCancel(MapType& book_side, double price, uint64_t order_id);
    template <typename MatchMap, typename RestMap>
        std::vector<Trade> matchLimit(Order ord, MatchMap& match_map, RestMap& rest_map, bool is_buy);
    template <typename MatchMap>
        std::vector<Trade> matchMarket(Order ord, MatchMap& match_map, bool is_buy);
    std::map<double, std::deque<Order>, std::greater<double>> bids_;
    std::map<double, std::deque<Order>> asks_;
    std::unordered_map<uint64_t, OrderLocation> index_;
    uint64_t next_trade_id_ = 1;
};