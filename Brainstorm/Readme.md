# Top level flow

## Phase 1
Subscribe Exchanges --> Unify Data Schema --> Store TS data

### Capture data
-- https://coinmarketcap.com/rankings/exchanges/ 
#### Phase 1.1
Prioritized -- Gemini, Kraken, OKX 
#### Phase 1.2
Binance, Coinbase

## Phase 2
Stored TS data (play) --> generate orderbook  --> Message Queue (kafka/solace) --> UI

## Phase 3
Stored TS data (play) --> generate alerts (arbitrage opportunity) --> Message Queue (kafka/solace) --> UI

## Phase 4
alerts -> order booking
