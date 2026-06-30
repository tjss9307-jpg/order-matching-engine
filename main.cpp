#include <iostream>
#include <vector>
#include "shared/order.h"
#include "engine/order_book.h"
#include "tests/test_batch_feed.h"
#include "shared/queue.h"
#include "engine/feed_generator.h"
#include <thread>
#include <algorithm>
#include <cstring>

void batch_mode(){
    std::vector<std::vector<Order>> feeds = get_batch_test_feeds();

    
    for (int i=0; i<feeds.size(); i++){
        std::cout << "Scenario " << i+1 << ": ------\n";
        auto& feed = feeds[i];

        OrderBook book;
        for (auto& ord : feed){
            
            
            if (ord.type == OrderType::CANCEL){
                bool cancelled = book.cancelOrder(ord.order_id);
                std::cout << "Action: CANCEL ID " << ord.order_id 
                          << " -> " << (cancelled ? "CANCELLED" : "FAILED (Order Not Found)") << "\n" ;
            } else{
                auto trades = book.addOrder(ord);

                for (auto& t : trades){
                std::cout << "TRADE: Buy=" << t.buy_order_id << " Sell=" << t.sell_order_id 
                << " Price=" << t.price << " Qty=" << t.quantity << "\n";
                }
            }

        }

        book.printBook();
    }
}

void consume(OrderQueue& ordQ, OrderBook& book, std::vector<uint64_t>& latency_samples, int& total_trades){
    
    while (true){
        Order ord = ordQ.pop();

        if (ord.order_id == 0){
            break;
        }

        auto timestamp0 = std::chrono::steady_clock::now();
        auto trds = book.addOrder(ord);
        auto timestamp1 = std::chrono::steady_clock::now();

        uint64_t latency = std::chrono::duration_cast<std::chrono::nanoseconds>
                                (timestamp1 - timestamp0).count();
        latency_samples.push_back(latency);

        total_trades += trds.size();


    }
}

void live_feed_mode(){
    OrderBook book;
    OrderQueue ordQ;
    feedGenerator feedGen;
    std::vector<uint64_t> latency_samples;
    latency_samples.reserve(100000);

    int total_trades = 0;

    auto start_time  = std::chrono::steady_clock::now();
    std::thread feed_generator_thread (&feedGenerator::runLiveFeed, &feedGen, std::ref(ordQ), 100000);
    std::thread consumer_thread (consume, std::ref(ordQ), std::ref(book), std::ref(latency_samples), std::ref(total_trades));
    

    feed_generator_thread.join();
    consumer_thread.join();

    auto end_time = std::chrono::steady_clock::now();

    int total_orders = latency_samples.size();

    std::sort(latency_samples.begin(), latency_samples.end());

    int p99_indx = 0.99 * (total_orders - 1);
    int p99_9_indx = 0.999 * (total_orders - 1);
    int p50_indx = 0.50 * (total_orders - 1);

    uint64_t p99 = latency_samples[p99_indx];
    uint64_t p99_9 = latency_samples[p99_9_indx];
    uint64_t p50 = latency_samples[p50_indx];

    uint64_t wallClock_runtime_ns = std::chrono::duration_cast<std::chrono::nanoseconds>
                                    (end_time - start_time).count();

    double wallClock_runtime_s = (wallClock_runtime_ns / 1e9);
    uint64_t wallClock_runtime_ms = (wallClock_runtime_ns / 1e6);

    double throughput = (double)(total_orders) / wallClock_runtime_s;

    std::cout << "\n-- PERFORMANCE SUMMARY " << std::string(43, '-') << "\n";
    std::cout << std::left << std::setw(17) << "Orders Processed" << " : " << total_orders << "\n";
    std::cout << std::left << std::setw(17) << "Trades Executed"  << " : " << total_trades << "\n";
    std::cout << std::left << std::setw(17) << "Wall Clock Time"  << " : " 
            << std::fixed << std::setprecision(2) << (wallClock_runtime_ns / 1e6) << " ms\n";
    std::cout << std::left << std::setw(17) << "Throughput"       << " : " 
            << std::fixed << std::setprecision(2) << throughput << " orders/sec\n\n";

    std::cout << "Latency Profile:\n";
    std::cout << "  p50   : " << p50 << " ns\n";
    std::cout << "  p99   : " << p99 << " ns\n";
    std::cout << "  p99.9 : " << p99_9 << " ns\n";
    std::cout << std::string(67, '-') << "\n";

}


int main(int argc, char* argv[]) {
    if (argc != 2){
        std::cout << "No of arguments mismatched" << "\n";
        return 0;
    }

    if (strcmp(argv[1], "--batch")==0){
        batch_mode();
    } else if (strcmp(argv[1], "--live")==0){
        live_feed_mode();
    } else{
        std::cout << "Invalid arguments" << "\n";
        std::cout << "Enter ./ome --batch for executing batch mode" << "\n";
        std::cout << "Enter ./ome --live for executing live feed mode" << "\n";
    }

    return 0;
}