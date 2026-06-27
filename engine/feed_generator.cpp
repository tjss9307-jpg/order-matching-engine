#include "feed_generator.h"

uint64_t current_time_ns(){
    return std::chrono::duration_cast<std::chrono::nanoseconds>
            (std::chrono::steady_clock::now().time_since_epoch()).count();
}

void feedGenerator::runLiveFeed(OrderQueue& ordQ, int num_orders){
    std::random_device rd;

    std::mt19937 engine(rd());

    std::uniform_int_distribution<int> side_dst(0,1);
    std::uniform_int_distribution<int> quantity_dst(1,100);
    std::normal_distribution<double> price_dst(100.00 , 0.5); // mean = 100 , sd = 0.5

    
    
    for (int i=1; i<=num_orders; i++){
        Side rndm_side = (side_dst(engine)==0) ? Side::BUY : Side::SELL;
        int rndm_quantity = quantity_dst(engine);
        double rndm_price = price_dst(engine);
        uint64_t id = next_order_id_; 
        Order ord = {
            id , rndm_side, OrderType::LIMIT, rndm_price, rndm_quantity, current_time_ns()};

        ordQ.push(ord);
        next_order_id_++;
    }

    ordQ.close();

}