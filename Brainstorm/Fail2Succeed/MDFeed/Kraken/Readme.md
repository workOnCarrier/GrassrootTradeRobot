# Source of api
https://docs.kraken.com/api/docs/websocket-v2/ticker


# test handshake

```
wscat -c wss://ws.kraken.com
```
on the command prompt
```
{"event":"subscribe","pair":["XBT/USD"],"subscription":{"name":"ticker"}}
```
expected output
```
< {"channelID":119930888,"channelName":"ticker","event":"subscriptionStatus","pair":"XBT/USD","status":"subscribed","subscription":{"name":"ticker"}}
< [119930888,{"a":["118328.60000",1,"1.11230593"],"b":["118328.50000",0,"0.23518590"],"c":["118328.50000","0.00026278"],"v":["75.26516148","1005.51523045"],"p":["118151.50754","118439.35201"],"t":[4865,42815],"l":["117731.40000","116929.60000"],"h":["118443.00000","120833.70000"],"o":["118011.90000","120504.50000"]},"ticker","XBT/USD"]
< {"event":"heartbeat"}
< {"event":"heartbeat"}
< {"event":"heartbeat"}
< {"event":"heartbeat"}
< {"event":"heartbeat"}
< {"event":"heartbeat"}
< {"event":"heartbeat"}
< [119930888,{"a":["118328.60000",0,"0.88460475"],"b":["118328.50000",0,"0.43691139"],"c":["118328.60000","0.00008451"],"v":["75.26524599","1005.51531496"],"p":["118151.50773","118439.35200"],"t":[4866,42816],"l":["117731.40000","116929.60000"],"h":["118443.00000","120833.70000"],"o":["118011.90000","120504.50000"]},"ticker","XBT/USD"]
```

