#pragma once
#include <cstdint>

enum class OrderType {MARKET , LIMIT , CANCEL};
// OrderType::CANCEL flows through addOrder and is forwarded directly to cancelOrder.
enum class Side {BUY , SELL};

struct Order{
    uint64_t order_id;
    Side side;
    OrderType type;
    double price; // ignored when type == MARKET
    int quantity;
    uint64_t timestamp_ns;
};

struct Trade {
    uint64_t trade_id;
    uint64_t buy_order_id;
    uint64_t sell_order_id;
    double price;
    int quantity;
    uint64_t timestamp_ns;
};
