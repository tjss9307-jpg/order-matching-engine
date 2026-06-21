#include <iostream>
#include <vector>
#include "shared/order.h"
#include "engine/order_book.h"

void run_scenario_1() {
    std::cout << "--- Scenario 1: Basic Matching and Partial Fills ---\n";
    OrderBook book;
    book.addOrder(Order{1, Side::SELL, OrderType::LIMIT, 100.00, 10, 0});
    book.addOrder(Order{2, Side::SELL, OrderType::LIMIT, 101.00, 5, 0});
    
    auto t1 = book.addOrder(Order{3, Side::BUY,  OrderType::LIMIT, 100.50, 8, 0});
    for (auto& t : t1) std::cout << "TRADE: Buy=" << t.buy_order_id << " Sell=" << t.sell_order_id << " Price=" << t.price << " Qty=" << t.quantity << "\n";

    auto t2 = book.addOrder(Order{4, Side::BUY,  OrderType::LIMIT, 100.00, 5, 0});
    for (auto& t : t2) std::cout << "TRADE: Buy=" << t.buy_order_id << " Sell=" << t.sell_order_id << " Price=" << t.price << " Qty=" << t.quantity << "\n";
    
    book.printBook();
}

void run_scenario_2() {
    std::cout << "--- Scenario 2: Time Priority Within One Level ---\n";
    OrderBook book;
    book.addOrder(Order{10, Side::SELL, OrderType::LIMIT, 50.00, 10, 0});
    book.addOrder(Order{11, Side::SELL, OrderType::LIMIT, 50.00, 20, 0});
    
    auto trades = book.addOrder(Order{12, Side::BUY,  OrderType::LIMIT, 50.00, 15, 0});
    for (auto& t : trades) std::cout << "TRADE: Buy=" << t.buy_order_id << " Sell=" << t.sell_order_id << " Price=" << t.price << " Qty=" << t.quantity << "\n";
    
    book.printBook();
}

void run_scenario_3() {
    std::cout << "--- Scenario 3: Market Order Sweeping ---\n";
    OrderBook book;
    book.addOrder(Order{1, Side::SELL, OrderType::LIMIT, 100.00, 5, 0});
    book.addOrder(Order{2, Side::SELL, OrderType::LIMIT, 101.00, 10, 0});
    
    auto trades = book.addOrder(Order{20, Side::BUY, OrderType::MARKET, 0.0, 12, 0});
    for (auto& t : trades) std::cout << "TRADE: Buy=" << t.buy_order_id << " Sell=" << t.sell_order_id << " Price=" << t.price << " Qty=" << t.quantity << "\n";
    
    book.printBook();
}

void run_scenario_4() {
    std::cout << "--- Scenario 4: Cancel & Cleanup ---\n";
    OrderBook book;
    book.addOrder(Order{30, Side::BUY, OrderType::LIMIT, 98.00, 10, 0});
    
    bool cancelled = book.cancelOrder(30);
    std::cout << "Cancelled ID 30: " << (cancelled ? "TRUE" : "FALSE") << "\n";
    
    book.printBook();
}

int main() {
    run_scenario_1();
    run_scenario_2();
    run_scenario_3();
    run_scenario_4();
    return 0;
}