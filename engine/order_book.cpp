#include "order_book.h"
#include <chrono>
#include <algorithm>

void OrderBook::printBook(){
    std::cout << "\n===// ORDER BOOK //===\n";
    
    std::cout << "--- ASKS  ---\n";
    for (auto it = asks_.begin(); it != asks_.end(); ++it) {
        std::cout << "Price: " << it->first << " -> ";
        
        for (size_t i = 0; i < it->second.size(); ++i) {
            std::cout << "[id:" << it->second[i].order_id << " qty:" << it->second[i].quantity << "] ";
        }
        std::cout << "\n";
    }

    std::cout << "--- BIDS  ---\n";
    for (auto it = bids_.begin(); it != bids_.end(); ++it) {
        std::cout << "Price: " << it->first << " -> ";
        
        for (size_t i = 0; i < it->second.size(); ++i) {
            std::cout << "[id:" << it->second[i].order_id << " qty:" << it->second[i].quantity << "] ";
        }
        std::cout << "\n";
    }
    std::cout << "/========================/\n";
}

uint64_t current_time_ns(){
    return std::chrono::duration_cast<std::chrono::nanoseconds>
            (std::chrono::steady_clock::now().time_since_epoch()).count();
}

template<typename MapType>
bool OrderBook::performCancel(MapType& book_side , double price , uint64_t order_id){
    auto price_it = book_side.find(price);
    if (price_it != book_side.end()){
        std::deque<Order>& ordQueue = price_it->second;

        for (auto it = ordQueue.begin() ; it!=ordQueue.end() ; ++it){
            if (it->order_id == order_id){
                ordQueue.erase(it);

                if (ordQueue.empty()){
                    book_side.erase(price_it);
                }
                return true;
            }
        }
    }

    return false;
}

template <typename MatchMap, typename RestMap>
std::vector<Trade> OrderBook::matchLimit(Order ord, MatchMap& match_map, RestMap& rest_map, bool is_buy){
    int rem_quantity = ord.quantity;

    std::vector<Trade> ret_trades;
    while (rem_quantity > 0 && !match_map.empty()){ 
        Trade trd;

        auto& [rest_price , rest_orders] = *match_map.begin();  
        Order& rest_front = rest_orders.front();  

        if (is_buy){
            if (rest_price > ord.price){ 
                break;
            }
        } else {
            if (rest_price < ord.price){ 
                break;
            }
        }
        

        int match_size = std::min(rem_quantity , rest_front.quantity);
        rem_quantity -= match_size;
        rest_front.quantity -= match_size;

        trd.trade_id = next_trade_id_;
        next_trade_id_++;
        trd.buy_order_id = is_buy ? ord.order_id : rest_front.order_id;
        trd.sell_order_id = is_buy ? rest_front.order_id : ord.order_id;
        trd.price = rest_front.price;
        trd.quantity = match_size;
        trd.timestamp_ns =  current_time_ns();

        ret_trades.push_back(trd);

        if (rest_front.quantity == 0){
            uint64_t dead_order_id = rest_front.order_id;
            rest_orders.pop_front();
            index_.erase(dead_order_id);
        }
        if (rest_orders.empty()){
            match_map.erase(rest_price); 
        }
    }

    if (rem_quantity > 0){
        ord.quantity = rem_quantity;
        rest_map[ord.price].push_back(ord); 

        OrderLocation ordloc;
        ordloc.side = ord.side;
        ordloc.price = ord.price;
        index_[ord.order_id] = ordloc;
    }

    return ret_trades;
}

template <typename MatchMap>
        std::vector<Trade> OrderBook::matchMarket(Order ord, MatchMap& match_map, bool is_buy){
    int rem_quantity = ord.quantity;

    std::vector<Trade> ret_trades;
    while (rem_quantity > 0 && !match_map.empty()){ 
        Trade trd;

        auto& [rest_price , rest_orders] = *match_map.begin();  
        Order& rest_front = rest_orders.front();  
        
        // no need of checking constraint . for match type we want to do until match empty.

        int match_size = std::min(rem_quantity , rest_front.quantity);
        rem_quantity -= match_size;
        rest_front.quantity -= match_size;

        trd.trade_id = next_trade_id_;
        next_trade_id_++;
        trd.buy_order_id = is_buy ? ord.order_id : rest_front.order_id;
        trd.sell_order_id = is_buy ? rest_front.order_id : ord.order_id;
        trd.price = rest_front.price;
        trd.quantity = match_size;
        trd.timestamp_ns =  current_time_ns();

        ret_trades.push_back(trd);

        if (rest_front.quantity == 0){
            uint64_t dead_order_id = rest_front.order_id;
            rest_orders.pop_front();
            index_.erase(dead_order_id);
        }
        if (rest_orders.empty()){
            match_map.erase(rest_price); 
        }
    }

    // no need of pushing rem orders left

    return ret_trades;
}

std::vector<Trade> OrderBook::addOrder(const Order& o){
    if (o.type == OrderType::CANCEL) {
        cancelOrder(o.order_id);
        return {};
    }
    if (o.type == OrderType::LIMIT){
        if (o.side == Side::BUY){
            return matchLimit(o , asks_ , bids_ , true); // BUY -> opponents / matchmap = sellers (asks_ map) home / restmap = buyers (bids_ map)
        } else {
            return matchLimit(o , bids_ , asks_ , false); // SELL -> opponents/matchmap = buyers (bids_ map)  home/restmap = sellers (asks_ map)
        }
    }

    else if (o.type == OrderType::MARKET){
        if (o.side == Side::BUY){
            return matchMarket(o , asks_ , true);
        } else{
            return matchMarket(o , bids_ , false);
        }
    }
    return {};
}


bool OrderBook::cancelOrder(uint64_t order_id){
    auto index_it = index_.find(order_id);
    if (index_it == index_.end()){
        return false;
    }

    const OrderLocation& ordloc = index_it->second;
    double price = ordloc.price;
    Side side = ordloc.side;

    index_.erase(index_it);

    if (side == Side::BUY){
        return performCancel(bids_ ,price , order_id);
    } else{
        return performCancel(asks_ , price , order_id);
    }

}