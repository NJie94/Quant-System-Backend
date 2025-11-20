# Architecture Overview

This project is a small but complete example of a C++ quantitative
backtesting system. The design is intentionally modular and oriented
around clean interfaces so that strategies, data sources, and execution
models can be swapped easily.

## Core Components

### 1. Domain Model

Located under `include/qs` and `src/core`:

- `Bar`: Single OHLCV bar with a `Date` (daily granularity).
- `Order`: Strategy's intent to trade.
- `Trade`: Filled order with price, commission, and slippage.
- `Position`: Per-symbol holdings, including average price.
- `Portfolio`: Tracks positions, cash, and marks to market to compute equity.
- `TimeSeries`: Small helper class for (date, value) series.
- `RiskMetrics`: Computes CAGR, Sharpe ratio, and max drawdown from an equity curve.

### 2. Strategy Layer

Defined in `Strategy.hpp` with concrete implementations in
`src/strategy`:

- `Strategy`: Abstract base, exposes `onBar(...)`.
- `StrategyContext`: Gives strategies read-only access to prices,
  positions, and cash.
- `makeMeanReversionStrategy(...)`: Factory for a simple z-score
  mean-reversion strategy.
- `makeMomentumStrategy(...)`: Factory for a moving-average crossover
  momentum strategy.

### 3. Data Layer

Defined in `DataFeed.hpp` and implemented by `CsvDataFeed`:

- `DataFeed`: Abstract interface with `next()`, `currentBars()`, and
  `latestPrice(symbol)`.
- `makeCsvDataFeed(symbols, dataDirectory)`: Loads per-symbol CSV
  files and provides bars in lockstep across symbols.

CSV format (per symbol):

```text
date,open,high,low,close,volume
2020-01-02,100,101,99,100.5,123456
2020-01-03,100.5,102,100,101.7,234567
...
