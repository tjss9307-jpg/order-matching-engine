#include <iostream>
#include <vector>
#include <iomanip>
#include "shared/order.h"
#include "engine/order_book.h"

void printTrade(const Trade& t) {
    std::cout << "  -> TRADE: buy_id=" << t.buy_order_id 
              << ", sell_id=" << t.sell_order_id 
              << ", price=" << std::fixed << std::setprecision(2) << t.price 
              << ", qty=" << t.quantity << "\n";
}

int main() {
    OrderBook book;
    int total_trades_executed = 0;

    // Combine all 4 scenarios into one sequential pipeline
    std::vector<std::pair<std::string, std::vector<Order>>> pipeline = {
        {
            "Scenario 1: Basic Matching & Partial Fills",
            {
                {1, Side::SELL, OrderType::LIMIT, 100.00, 10, 0},
                {2, Side::SELL, OrderType::LIMIT, 101.00,  5, 0},
                {3, Side::BUY,  OrderType::LIMIT, 100.50,  8, 0},
                {4, Side::BUY,  OrderType::LIMIT, 100.00,  5, 0}
            }
        },
        {
            "Scenario 2: Time Priority Within One Price Level",
            {
                {10, Side::SELL, OrderType::LIMIT, 50.00, 10, 0},
                {11, Side::SELL, OrderType::LIMIT, 50.00, 20, 0},
                {12, Side::BUY,  OrderType::LIMIT, 50.00, 15, 0}
            }
        },
        {
            "Scenario 3: Market Order Sweeping Multiple Levels",
            {
                {15, Side::SELL, OrderType::LIMIT, 100.00,  5, 0}, 
                {16, Side::SELL, OrderType::LIMIT, 101.00, 10, 0}, 
                {20, Side::BUY,  OrderType::MARKET,  0.00, 12, 0}  
            }
        },
        {
            "Scenario 4: Cancel & Empty-Level Cleanup",
            {
                {30, Side::BUY,  OrderType::LIMIT,  98.00, 10, 0},
                {30, Side::BUY,  OrderType::CANCEL, 98.00,  0, 0} 
            }
        }
    };

    // Execute the pipeline sequentially
    for (const auto& [scene_name, orders] : pipeline) {
        std::cout << "\n--- Running " << scene_name << " ---\n";
        for (const auto& order : orders) {
            if (order.type == OrderType::CANCEL) {
                bool success = book.cancelOrder(order.order_id);
                std::cout << "Executing Cancel for ID " << order.order_id 
                          << (success ? " -> Success\n" : " -> Failed\n");
            } else {
                std::cout << "Processing Order ID " << order.order_id << "...\n";
                std::vector<Trade> trades = book.addOrder(order);
                for (const auto& trade : trades) {
                    printTrade(trade);
                    total_trades_executed++;
                }
            }
        }
    }

    std::cout << "\n========================================\n";
    std::cout << "Pipeline Finished. Total Pipeline Trades: " << total_trades_executed << "\n";
    std::cout << "========================================\n";

    return 0;
}