# Top level flow

## Phase 1
Subscribe Exchanges --> Unify Data Schema --> Store TS data

### Capture data
Prioritized -- Gemini, Kraken, OKX, Binance, Coinbase -- https://coinmarketcap.com/rankings/exchanges/ 


## Phase 2
Stored TS data (play) --> generate orderbook  --> Message Queue (kafka/solace) --> UI

## Phase 3
Stored TS data (play) --> generate alerts (arbitrage opportunity) --> Message Queue (kafka/solace) --> UI