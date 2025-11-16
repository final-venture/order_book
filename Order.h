// Order.h

#pragma once

#include <cstdint>
#include <iostream>

using Quantity = uint64_t;
using Price = long double;
using Id = uint64_t;

enum class Side
{
    Buy,
    Sell
};

class Order
{
private:
    Id orderId_;
    long double price_;
    Quantity currentQuantity_;
    Quantity initialQuantity_;
    Side side_;

public:
    Order(Id id, long double price, Quantity initialQuantity, Quantity currentQuantity, Side side);
    Order(Id id, long double price, Quantity quantity, Side side);
    Id getOrderId() const;
    long double getPrice() const;
    Quantity getInitialQuantity() const;
    Quantity getCurrentQuantity() const;
    Quantity getFilledQuantity() const;
    Side getSide() const;
    void fill(Quantity fillQuantity, Price fillPrice);
};
