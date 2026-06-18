#include "order_book.h"
#include <chrono>

template <typename MatchMap, typename RestMap>
std::vector<Trade> matchLimit(Order ord, MatchMap& match_map, RestMap& rest_map, bool is_buy){
    int rem_quantity = ord.quantity;

    vector<Trade> ret_trades;
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
        

        int match_size = min(rem_quantity , rest_front.quantity);
        rem_quantity -= match_size;
        rest_front.quantity -= match_size;

        trd.trade_id = next_trade_id_;
        next_trade_id_++;
        trd.buy_order_id = is_buy ? ord.order_id : rest_front.order_id;
        trd.sell_order_id = is_buy ? rest_front.order_id : ord.order_id;
        trd.price = rest_front.price;
        trd.quantity = match_size;
        trd.timestamp_ns =  std::chrono::duration_cast<std::chrono::nanoseconds>
                            (std::chrono::steady_clock::now().time_since_epoch()).count();

        ret_trades.push_back(trd);

        if (rest_front.quantity == 0){
            rest_orders.pop_front();
            index_.erase(rest_front.order_id);
        }
        if (match_map[rest_price].empty()){
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
        std::vector<Trade> matchMarket(Order ord, MatchMap& match_map, bool is_buy){
    int rem_quantity = ord.quantity;

    vector<Trade> ret_trades;
    while (rem_quantity > 0 && !match_map.empty()){ 
        Trade trd;

        auto& [rest_price , rest_orders] = *match_map.begin();  
        Order& rest_front = rest_orders.front();  
        
        // no need of checking constraint . for match type we want to do until match empty.

        int match_size = min(rem_quantity , rest_front.quantity);
        rem_quantity -= match_size;
        rest_front.quantity -= match_size;

        trd.trade_id = next_trade_id_;
        next_trade_id_++;
        trd.buy_order_id = is_buy ? ord.order_id : rest_front.order_id;
        trd.sell_order_id = is_buy ? rest_front.order_id : ord.order_id;
        trd.price = rest_front.price;
        trd.quantity = match_size;
        trd.timestamp_ns =  std::chrono::duration_cast<std::chrono::nanoseconds>
                            (std::chrono::steady_clock::now().time_since_epoch()).count();

        ret_trades.push_back(trd);

        if (rest_front.quantity == 0){
            rest_orders.pop_front();
            index_.erase(rest_front.order_id);
        }
        if (match_map[rest_price].empty()){
            match_map.erase(rest_price); 
        }
    }

    // no need of pushing rem orders left

    return ret_trades;
}

std::vector<Trade> OrderBook::addOrder(const Order& o){
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
}


bool OrderBook::cancelOrder(uint64_t order_id){
    if (index_.find(order_id)==index_.end()){
        return false;
    }

    OrderLocation ordloc = index_[order_id];
    double price = ordloc.price;

    index_.erase(order_id);

    if (ordloc.side == Side::BUY){
        for (auto it = bids_[price].begin() ; it!=bids_[price].end() ; ++it){
            Order ord = *it;
            if (ord.order_id == order_id){
                bids_[price].erase(it);
                if (bids_[price].empty()){
                    bids_.erase(price);
                }
                break;
            }
        }
    } else{
        for (auto it : asks_[price].begin() ; it!=asks_[price].end() ; ++it){
            Order ord = *it;
            if (ord.order_id == order_id){
                asks_[price].erase(it);
                if (asks_[price].empty()){
                    asks_.erase(price);
                }
                break;
            }
        }
    }

    return true;
}