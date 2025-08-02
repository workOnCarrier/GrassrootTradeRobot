#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>

#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <nlohmann/json.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>

using Timestamp = std::chrono::system_clock::time_point;

struct BookLevel {
    double price;
    double qty;
};

struct OrderBookSnapshot {
    Timestamp timestamp;
    std::map<double, double> bids; // price -> qty
    std::map<double, double> asks;
};

std::vector<OrderBookSnapshot> orderbook_time_series; // store snapshots over time
OrderBookSnapshot current_book;

OrderBookSnapshot fetch_snapshot(const std::string& symbol, boost::asio::io_context& ioc, boost::asio::ssl::context& ctx) {
    using tcp = boost::asio::ip::tcp;
    namespace beast = boost::beast;
    namespace http = beast::http;
    using namespace std::chrono;

    std::string host = "api.gemini.com";
    std::string target = "/v1/book/" + symbol;

    // Connect (as in Boost.Beast HTTP SSL client example)
    tcp::resolver resolver{ioc};
    auto const results = resolver.resolve(host, "443");
    beast::ssl_stream<beast::tcp_stream> stream{ioc, ctx};
    beast::get_lowest_layer(stream).connect(results);

    stream.set_verify_mode(boost::asio::ssl::verify_peer);
    stream.set_verify_callback(boost::asio::ssl::host_name_verification(host));
    stream.handshake(boost::asio::ssl::stream_base::client);

    http::request<http::string_body> req{http::verb::get, target, 11};
    req.set(http::field::host, host);
    req.set(http::field::user_agent, "orderbook-example");

    http::write(stream, req);

    beast::flat_buffer buffer;
    http::response<http::string_body> res;
    http::read(stream, buffer, res);

    nlohmann::json snapshot = nlohmann::json::parse(res.body());
    std::cout << "\t" << snapshot << std::endl;
    OrderBookSnapshot ob;
    ob.timestamp = system_clock::now();

    for (const auto& x : snapshot["bids"]) ob.bids[std::stod(x["price"].get<std::string>())] = std::stod(x["amount"].get<std::string>());
    for (const auto& x : snapshot["asks"]) ob.asks[std::stod(x["price"].get<std::string>())] = std::stod(x["amount"].get<std::string>());

    return ob;
}

void subscribe_and_update(const std::string& symbol,
                         boost::asio::io_context& ioc, boost::asio::ssl::context& ctx) {
    using tcp = boost::asio::ip::tcp;
    namespace websocket = boost::beast::websocket;
    namespace beast = boost::beast;

    std::string host = "api.gemini.com";
    std::string port = "443";
    std::string target = "/v1/marketdata/" + symbol;

    tcp::resolver resolver{ioc};
    auto results = resolver.resolve(host, port);
    beast::ssl_stream<beast::tcp_stream> stream{ioc, ctx};
    beast::get_lowest_layer(stream).connect(results);

    // SNI needed (see OKX/modern exchanges; not strictly needed for api.gemini.com, but best practice)
    SSL_set_tlsext_host_name(stream.native_handle(), host.c_str());
    stream.handshake(boost::asio::ssl::stream_base::client);

    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws{std::move(stream)};
    ws.handshake(host, target);

    // Market data feed loop
    for (;;) {
        beast::flat_buffer buffer;
        ws.read(buffer);
        auto json_msg = nlohmann::json::parse(beast::buffers_to_string(buffer.data()));
        
        auto events = json_msg["events"];
        for (const auto& ev : events) {
            // Update bids
            if (ev.contains("delta")) {
                std::cout << "found delta:" <<  ev << std::endl;
                for (const auto& change : ev["delta"]) {
                    std::string side = change[0];
                    double price = std::stod(change[1].get<std::string>());
                    double qty = std::stod(change[2].get<std::string>());
                    if (side == "buy") {
                        if (qty == 0) current_book.bids.erase(price);
                        else current_book.bids[price] = qty;
                    }
                    if (side == "sell") {
                        if (qty == 0) current_book.asks.erase(price);
                        else current_book.asks[price] = qty;
                    }
                }
            }
            // At each event, capture a timeslice if desired
            OrderBookSnapshot snap = current_book;
            snap.timestamp = std::chrono::system_clock::now();

            orderbook_time_series.push_back(std::move(snap));
        }
    }
}

int main() {
    boost::asio::io_context ioc;
    boost::asio::ssl::context ctx{boost::asio::ssl::context::sslv23_client};
    ctx.set_default_verify_paths();
    current_book = fetch_snapshot("btcusd", ioc, ctx);
    orderbook_time_series.push_back(current_book);

    // Now launch WebSocket delta listener (blocks forever)
    subscribe_and_update("btcusd", ioc, ctx);

    // (Optional) Write snapshots to disk or further analysis...
}

