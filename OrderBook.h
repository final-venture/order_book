// OrderBook.h

#pragma once

#include "Order.h"
#include "OrderModify.h"
#include <iostream>
#include <map>
#include <limits>
#include <iomanip>
#include <list>
#include <unordered_map>
#include <optional>

class OrderBook
{
private:
    std::map<Price, std::list<Order>, std::greater<>> bids_;
    std::map<Price, std::list<Order>> asks_;
    std::unordered_map<Id, std::list<Order>::iterator> orderIdMap_;
    bool addOrderImpl(Order &&order);
    bool cancelOrderImpl(Id targetId);
    bool modifyOrderImpl(OrderModify &&modifyInfo);

public:
    OrderBook();
    Price getBestBid() const;
    Price getBestAsk() const;
    Quantity getVolumeAtPrice(Price price, Side side) const;
    Price getSpread() const;
    void matchOrders();
    void addOrder(const Order &order);
    void addOrder(Order &&order);
    std::optional<std::list<Order>::iterator> getOrderPointerOpt(Id targetId) const;
    void cancelOrder(Id targetId);
    void modifyOrder(const OrderModify &modifyInfo);
    void modifyOrder(OrderModify &&modifyInfo);
    void printBook() const;
};
