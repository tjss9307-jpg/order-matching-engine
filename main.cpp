#include <iostream>
#include <vector>
#include "shared/order.h"
#include "engine/order_book.h"
#include "tests/test_batch_feed.h"

int main() {
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

    return 0;
}