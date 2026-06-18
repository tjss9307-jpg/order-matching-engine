#pragma once
#include "order.h"

class OrderQueue{
    public:
        void push(const Order& o);
        Order pop(); // blocks until order available
        bool try_pop(Order& out); // non-blocing : return false if empty
        void close(); // signals : no more order coming
}