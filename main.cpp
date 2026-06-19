#include <iostream>
#include "engine/order_book.h"

void runScenario1() {
    std::cout << "--- Running Scenario 1 ---\n";
    OrderBook book;

    // 1. Build the resting book
    Order resting_sell = {1, Side::SELL, OrderType::LIMIT, 100.50, 10};
    book.addOrder(resting_sell);

    // 2. Fire the aggressive order that should trigger a match
    Order incoming_buy = {2, Side::BUY, OrderType::LIMIT, 101.00, 5};
    std::vector<Trade> trades = book.addOrder(incoming_buy);

    // 3. Print the output to verify
    for (const auto& trd : trades) {
        std::cout << "Match! ID: " << trd.trade_id 
                  << " | Qty: " << trd.quantity 
                  << " @ Price: " << trd.price 
                  << " | Latency(ns): " << trd.timestamp_ns << "\n";
    }
}

int main() {
    runScenario1();
    // runScenario2();
    // runScenario3();
    return 0;
}