// OrderModify.h

#pragma once

#include <cstdint>

using Quantity = uint64_t;
using Price = long double;
using Id = uint64_t;

class OrderModify
{
public:
    OrderModify(Id id, Price price, Quantity quantity) : id_(id), price_(price), quantity_(quantity) {}
    Quantity getQuantity() const;
    Price getPrice() const;
    Id getId() const;

private:
    Id id_;
    Price price_;
    Quantity quantity_;
};
