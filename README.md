# quant-system-cpp

A small but complete **quantitative backtesting system in modern C++**.
Designed as a portfolio-ready showcase:

- Clean architecture and clear separation of concerns.
- Pluggable strategies, data feeds, and execution models.
- Simple risk metrics (CAGR, Sharpe, max drawdown).
- Unit tests and GitHub Actions CI.

---

## Features

- **Domain model**: `Bar`, `Order`, `Trade`, `Position`, `Portfolio`,
  `TimeSeries`.
- **Strategy framework**:
  - `Strategy` / `StrategyContext` interface.
  - Built-in `MeanReversionStrategy` and `MomentumStrategy`
    (factories in `Strategy.hpp`).
- **Data feed**:
  - `DataFeed` interface.
  - `CsvDataFeed` implementation with factory
    `makeCsvDataFeed(symbols, dataDirectory)`.
- **Execution**:
  - `ExecutionModel` interface.
  - `SimpleExecutionModel` with fixed per-trade commission and
    per-share slippage.
- **Backtesting engine**:
  - `BacktestEngine` that wires strategy, data, execution, portfolio.
- **Risk metrics**:
  - `RiskMetrics` computing CAGR, Sharpe, and max drawdown from the
    equity curve.
- **Config & logging**:
  - Minimal JSON-like config parsing via `Config`.
  - Simple console logger, `Logger`.

---

## Building

```bash
mkdir build
cd build
cmake .. -DBUILD_TESTS=ON
cmake --build . --config Release
ctest --output-on-failure
