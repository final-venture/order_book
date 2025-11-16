// OrderBook.cpp

#include "OrderModify.h"
#include "OrderBook.h"

OrderBook::OrderBook()

    : bids_{}, asks_{}
{
}

Price OrderBook::getBestBid() const
{
    if (!bids_.empty())
    {
        return bids_.begin()->first;
    }
    else
    {
        return 0.0;
    }
}

Price OrderBook::getBestAsk() const
{
    if (!asks_.empty())
    {
        return asks_.begin()->first;
    }
    else
    {
        return std::numeric_limits<Price>::infinity();
    }
}

Quantity OrderBook::getVolumeAtPrice(Price price, Side side) const
{
    Quantity totalVolume = 0;
    if (side == Side::Buy)
    {
        auto it = bids_.find(price);
        if (it != bids_.end())
        {
            for (const Order &order : it->second)
            {
                totalVolume += order.getCurrentQuantity();
            }
        }
    }
    else if (side == Side::Sell)
    {
        auto it = asks_.find(price);
        if (it != asks_.end())
        {
            for (const Order &order : it->second)
            {
                totalVolume += order.getCurrentQuantity();
            }
        }
    }
    return totalVolume;
}

Price OrderBook::getSpread() const
{
    Price bestBid = getBestBid();
    Price bestAsk = getBestAsk();
    if (bestBid != 0.0 && bestAsk != std::numeric_limits<Price>::infinity())
    {
        return bestAsk - bestBid;
    }
    else
    {
        return std::numeric_limits<Price>::infinity();
    }
}

void OrderBook::matchOrders()
{
    while (!bids_.empty() && !asks_.empty() && (getBestBid() >= getBestAsk()))
    {
        Price bestBidPrice = getBestBid();
        Price bestAskPrice = getBestAsk();
        std::list<Order> &bestBidOrders = bids_.begin()->second;
        std::list<Order> &bestAskOrders = asks_.begin()->second;

        Order &bidOrderToMatch = bestBidOrders.front();
        Order &askOrderToMatch = bestAskOrders.front();

        Quantity quantityToFill = std::min(bidOrderToMatch.getCurrentQuantity(), askOrderToMatch.getCurrentQuantity());
        Price priceToFill = bestAskPrice;

        bidOrderToMatch.fill(quantityToFill, priceToFill);
        if (bidOrderToMatch.getCurrentQuantity() == 0)
        {
            orderIdMap_.erase(bidOrderToMatch.getOrderId());
            bestBidOrders.pop_front();
            if (bestBidOrders.empty())
            {
                bids_.erase(bestBidPrice);
            }
        }
        askOrderToMatch.fill(quantityToFill, priceToFill);
        if (askOrderToMatch.getCurrentQuantity() == 0)
        {
            orderIdMap_.erase(askOrderToMatch.getOrderId());
            bestAskOrders.pop_front();
            if (bestAskOrders.empty())
            {
                asks_.erase(bestAskPrice);
            }
        }
        std::cout << "--- EXECUTED " << quantityToFill << " @ " << priceToFill
                  << " (B: " << bidOrderToMatch.getOrderId() << ", A: " << askOrderToMatch.getOrderId() << ")\n";
    }
}

bool OrderBook::addOrderImpl(Order &&order)
{
    Price price = order.getPrice();
    Id orderId = order.getOrderId();

    if (orderIdMap_.find(orderId) != orderIdMap_.end())
    {
        return false;
    }

    if (order.getSide() == Side::Buy)
    {
        bids_[price].push_back(std::move(order));
        orderIdMap_[orderId] = --bids_[price].end();
    }
    else if (order.getSide() == Side::Sell)
    {
        asks_[price].push_back(std::move(order));
        orderIdMap_[orderId] = --asks_[price].end();
    }
    matchOrders();
    return true;
}

void OrderBook::addOrder(const Order &order)
{
    bool isSuccessful = addOrderImpl(Order(order));
    if (isSuccessful)
    {
        std::cout << "--- ADDED Order: " << (order.getSide() == Side::Buy ? "BID" : "ASK")
                  << " " << order.getCurrentQuantity() << " @ " << order.getPrice() << std::endl;
    }
    else
    {
        std::cout << "--- FAILED to Add Order: " << (order.getSide() == Side::Buy ? "BID" : "ASK")
                  << " " << order.getCurrentQuantity() << " @ " << order.getPrice() << std::endl;
    }
}

void OrderBook::addOrder(Order &&order)
{
    Price orderPrice = order.getPrice();
    Quantity orderCurrentQuantity = order.getCurrentQuantity();
    Side orderSide = order.getSide();
    bool isSuccessful = addOrderImpl(std::move(order));
    if (isSuccessful)
    {
        std::cout << "--- ADDED Order: " << (orderSide == Side::Buy ? "BID" : "ASK")
                  << " " << orderCurrentQuantity << " @ " << orderPrice << std::endl;
    }
    else
    {
        std::cout << "--- FAILED to Add Order: " << (orderSide == Side::Buy ? "BID" : "ASK")
                  << " " << orderCurrentQuantity << " @ " << orderPrice << std::endl;
    }
}

std::optional<std::list<Order>::iterator> OrderBook::getOrderPointerOpt(Id targetId) const
{
    auto it = orderIdMap_.find(targetId);
    if (it == orderIdMap_.end())
    {
        return std::nullopt;
    }
    return it->second;
}

bool OrderBook::cancelOrderImpl(Id targetId)
{
    auto orderIterOpt = getOrderPointerOpt(targetId);

    if (!orderIterOpt)
    {
        return false;
    }

    auto orderIter = *orderIterOpt;
    Side side = orderIter->getSide();
    Price price = orderIter->getPrice();
    if (side == Side::Buy)
    {
        bids_[price].erase(orderIter);
        if (bids_[price].empty())
        {
            bids_.erase(price);
        }
    }
    else if (side == Side::Sell)
    {
        asks_[price].erase(orderIter);
        if (asks_[price].empty())
        {
            asks_.erase(price);
        }
    }
    orderIdMap_.erase(targetId);
    return true;
}

void OrderBook::cancelOrder(Id targetId)
{
    bool isSuccessful = cancelOrderImpl(targetId);
    if (isSuccessful)
    {
        std::cout << "--- CANCELLED Order " << targetId << std::endl;
    }
    else
    {
        std::cout << "--- FAILED TO CANCEL Order " << targetId << ": Not Found" << std::endl;
    }
}

bool OrderBook::modifyOrderImpl(OrderModify &&modifyInfo)
{
    Id orderId = modifyInfo.getId();

    auto orderIterOpt = getOrderPointerOpt(orderId);
    if (!orderIterOpt)
    {
        return false;
    }
    auto orderIter = *orderIterOpt;
    Order newOrder{orderId, modifyInfo.getPrice(), modifyInfo.getQuantity(), modifyInfo.getQuantity(), orderIter->getSide()};
    bool cancelSuccessful = cancelOrderImpl(orderId);
    if (!cancelSuccessful)
    {
        return false;
    }
    bool addSuccessful = addOrderImpl(std::move(newOrder));
    if (!addSuccessful)
    {
        return false;
    }

    return true;
}

void OrderBook::modifyOrder(const OrderModify &modifyInfo)
{
    bool isSuccessful = modifyOrderImpl(OrderModify(modifyInfo));
    if (isSuccessful)
    {
        std::cout << "--- MODIFIED Order " << modifyInfo.getId() << std::endl;
    }
    else
    {
        std::cout << "--- FAILED TO MODIFY Order " << modifyInfo.getId() << ": Not Found" << std::endl;
    }
}

void OrderBook::modifyOrder(OrderModify &&modifyInfo)
{
    Id orderId = modifyInfo.getId();
    bool isSuccessful = modifyOrderImpl(std::move(modifyInfo));
    if (isSuccessful)
    {
        std::cout << "--- MODIFIED Order " << orderId << std::endl;
    }
    else
    {
        std::cout << "--- FAILED TO MODIFY Order " << orderId << ": Not Found" << std::endl;
    }
}

void OrderBook::printBook() const
{
    std::cout << "\n==================== ORDER BOOK ====================\n";
    std::cout << std::fixed << std::setprecision(2);

    // Print Asks
    std::cout << "--------- ASKS (SELL) ---------\n";
    std::cout << "Price   | Volume\n";
    std::cout << "-------------------------------\n";
    if (!asks_.empty())
    {
        for (const auto &priceLevel : asks_)
        {
            Quantity volume = getVolumeAtPrice(priceLevel.first, Side::Sell);
            std::cout << std::setw(7) << priceLevel.first << " | " << volume << "\n";
        }
    }
    else
    {
        std::cout << "          (Empty)\n";
    }
    std::cout << "-------------------------------\n";
    std::cout << "Spread: ";
    Price spread = getSpread();
    if (spread != std::numeric_limits<Price>::infinity())
    {
        std::cout << spread << '\n';
    }
    else
    {
        std::cout << "(N/A)" << '\n';
    }
    std::cout << "-------------------------------\n";

    // Print Bids
    std::cout << "--------- BIDS (BUY) ----------\n";
    std::cout << "Price   | Volume\n";
    std::cout << "-------------------------------\n";
    if (!bids_.empty())
    {
        for (const auto &priceLevel : bids_)
        {
            Quantity volume = getVolumeAtPrice(priceLevel.first, Side::Buy);
            std::cout << std::setw(7) << priceLevel.first << " | " << volume << "\n";
        }
    }
    else
    {
        std::cout << "          (Empty)\n";
    }
    std::cout << "====================================================\n\n";
}
