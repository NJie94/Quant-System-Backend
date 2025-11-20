cmake_minimum_required(VERSION 3.20)
project(quant_system_cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build unit tests" ON)

add_library(qs
    src/core/Bar.cpp
    src/core/TimeSeries.cpp
    src/core/Portfolio.cpp
    src/core/RiskMetrics.cpp
    src/data/CsvDataFeed.cpp
    src/strategy/MeanReversionStrategy.cpp
    src/strategy/MomentumStrategy.cpp
    src/exec/SimpleExecutionModel.cpp
    src/backtest/BacktestEngine.cpp
    src/util/Logger.cpp
    src/util/Config.cpp
)

target_include_directories(qs PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
