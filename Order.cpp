// Order.cpp

#include "Order.h"

Order::Order(Id id, long double price, Quantity initialQuantity, Quantity currentQuantity, Side side)
    : orderId_(id), price_(price), initialQuantity_(initialQuantity), currentQuantity_(currentQuantity), side_(side) {}
Order::Order(Id id, long double price, Quantity quantity, Side side)
    : orderId_(id), price_(price), initialQuantity_(quantity), currentQuantity_(quantity), side_(side) {}

Id Order::getOrderId() const { return orderId_; }
long double Order::getPrice() const { return price_; }
Quantity Order::getInitialQuantity() const { return initialQuantity_; }
Quantity Order::getCurrentQuantity() const { return currentQuantity_; }
Quantity Order::getFilledQuantity() const { return getInitialQuantity() - getCurrentQuantity(); }
Side Order::getSide() const { return side_; }
void Order::fill(Quantity fillQuantity, Price fillPrice)
{
    if (fillQuantity > getCurrentQuantity())
    {
        throw std::logic_error("Fill FAILED: Fill Quantity exceeds current quantity.");
    }
    else
    {
        currentQuantity_ -= fillQuantity;
    }
}
