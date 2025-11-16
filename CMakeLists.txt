cmake_minimum_required(VERSION 3.10)

project(OrderBookProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(OrderBookApp 
    main.cpp 
    Order.cpp 
    OrderBook.cpp
    OrderModify.cpp
)
