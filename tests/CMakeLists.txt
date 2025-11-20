add_executable(test_bar test_bar.cpp)
target_link_libraries(test_bar PRIVATE qs)

add_executable(test_portfolio test_portfolio.cpp)
target_link_libraries(test_portfolio PRIVATE qs)

add_executable(test_backtest_engine test_backtest_engine.cpp)
target_link_libraries(test_backtest_engine PRIVATE qs)

add_test(NAME test_bar            COMMAND test_bar)
add_test(NAME test_portfolio      COMMAND test_portfolio)
add_test(NAME test_backtestEngine COMMAND test_backtest_engine)
