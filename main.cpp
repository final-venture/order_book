// main.cpp

#include "OrderBook.h"
#include <iostream>
#include <iomanip>

int main()
{
    OrderBook book;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Starting Simulation...\n";

    book.printBook();

    // Add some orders
    book.addOrder(Order(1, 100.50, 10, Side::Buy));
    book.addOrder(Order(2, 100.50, 5, Side::Buy));
    book.addOrder(Order(3, 100.25, 20, Side::Buy));
    book.addOrder(Order(4, 101.00, 12, Side::Sell));
    book.addOrder(Order(5, 101.50, 8, Side::Sell)); 
    book.addOrder(Order(6, 101.00, 3, Side::Sell)); 

    book.printBook();

    // Test getters
    std::cout << "Best Bid: " << book.getBestBid() << std::endl;
    std::cout << "Best Ask: " << book.getBestAsk() << std::endl;
    std::cout << "Spread: " << book.getSpread() << std::endl;
    std::cout << "Volume at 100.50 (BID): " << book.getVolumeAtPrice(100.50, Side::Buy) << std::endl;
    std::cout << "Volume at 101.00 (ASK): " << book.getVolumeAtPrice(101.00, Side::Sell) << std::endl;

    // Test cancelling an order
    book.cancelOrder(2); // Cancel the 5-share bid at 100.50
    book.printBook();

    // Test getters again
    std::cout << "Volume at 100.50 (BID) after cancel: "
              << book.getVolumeAtPrice(100.50, Side::Buy) << std::endl;

    // Test canceling an invalid order
    book.cancelOrder(999);
    book.printBook();

    // Test canceling the last order at a price level
    book.cancelOrder(3); // Cancel the 20-share bid at 100.25
    book.printBook();

    book.modifyOrder({1, 102.0, 20});
    book.modifyOrder({999, 100.0, 20});
    book.printBook();

    std::cout << "Best Bid after cancel: " << book.getBestBid() << std::endl;

    return 0;
}
