#include <iostream>
#include <thread>
#include <cassert>
#include "../shared/queue.h"

void producer(OrderQueue& ordQ){
    for (uint64_t i=1 ; i<=1000 ; i++){
        Side side = (i%2) ? Side::BUY : Side::SELL;
        Order ord = {i , side , OrderType::LIMIT , 100.0 , 10 , 0};

        ordQ.push(ord);
    }
    ordQ.close();
}

void consumer(OrderQueue& ordQ , int& cnt){
    while (true){
        Order ord = ordQ.pop();

        if (ord.order_id == 0){
            break;
        }

        cnt++;
    }
}

int main(){
    OrderQueue ordQ;
    int items_processed = 0;

    std::thread producer_thread(producer , std::ref(ordQ));
    std::thread consumer_thread(consumer , std::ref(ordQ) , std::ref(items_processed));

    consumer_thread.join();
    producer_thread.join();

    std::cout << "Test completed. Total items processed: " << items_processed << std::endl;
    assert(items_processed == 1000);
    std::cout << "SUCCESS: Thread-safe queue verified!" << std::endl;

    return 0;
}