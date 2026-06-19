# Order Matching Engine

A C++ project that simulates how stock exchanges match buy and sell orders in real time.

# What it does

Takes a stream of buy/sell orders and matches them based on price-time priority. The best price gets matched first, and if two orders are at the same price, the one that arrived earlier gets priority.

Supports two modes:

-> Batch mode – reads a fixed list of orders, processes them, and prints all trades along with the final order book state. Useful for testing.
-> Live mode – one thread continuously generates orders while another thread matches them. At the end, it prints throughput and latency statistics.

# Data structures used

Order book

Two sorted maps are maintained, one for bids and one for asks.

std::map<double, std::deque<Order>, std::greater<double>> bids;
std::map<double, std::deque<Order>> asks;

std::map automatically keeps prices sorted. Each price level stores orders in a std::deque, which allows O(1) removal from the front and preserves time priority.

Order index

std::unordered_map<uint64_t, OrderLocation> index;

This is used for fast order cancellation. Without it, cancelling an order would require scanning the entire book. With the index, cancellation can be performed in O(log P).

Queue

The order queue is implemented using std::deque together with std::mutex and std::condition_variable. The consumer thread sleeps when the queue is empty instead of continuously spinning and wasting CPU cycles.

# Matching logic

Limit order

-> Check the best available price on the opposite side.
-> If the order crosses, execute the trade at the resting order's price, not the incoming order's price.
-> Continue matching until the incoming order is fully filled or there are no more crossable prices.
-> Any remaining quantity is added to the order book.

Market order

-> Works similarly to a limit order but skips the price check entirely.
-> If the book runs out of liquidity, any leftover quantity is discarded and not added to the book.

Cancel

-> Look up the order ID in the index.
-> Find the order inside the corresponding deque and remove it.
-> If the price level becomes empty after removal, delete that level from the map as well. Leaving empty levels can cause bugs.

# Build and run

g++ -std=c++17 -O2 -pthread -o ome main.cpp engine/order_book.cpp feed/order_queue.cpp feed/feed_gen.cpp feed/latency.cpp

Batch mode

./ome --batch

Live mode

./ome --live --orders 1000000

Sample output
