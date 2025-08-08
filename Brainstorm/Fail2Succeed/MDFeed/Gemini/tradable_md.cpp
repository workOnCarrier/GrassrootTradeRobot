// gemini_ws_price_tracker.cpp
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/asio/ssl.hpp>
#include <nlohmann/json.hpp>
#include <map>
#include <iostream>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using json = nlohmann::json;
// using tcp = boost::asio::ip::tcp;
using tcp = net::ip::tcp;
using ssl_stream = boost::beast::ssl_stream<tcp::socket>;
using ws_stream = boost::beast::websocket::stream<ssl_stream>;


std::map<std::string, double> bid_book;
std::map<std::string, double> ask_book;

void process_snapshot(const json& events) {
    for (auto& ev : events) {
        if (ev["type"] == "change" && ev["reason"] == "initial") {
            std::string side = ev["side"];
            std::string price = ev["price"];
            double remaining = std::stod(ev["remaining"].get<std::string>());
            if (side == "buy") bid_book[price] = remaining;
            else if (side == "sell") ask_book[price] = remaining;
        }
    }
}

void process_update(const json& events) {
    for (auto& ev : events) {
        std::string type = ev["type"];
        if (type == "change") {
            std::string side = ev["side"];
            std::string price = ev["price"];
            double remaining = std::stod(ev["remaining"].get<std::string>());

            auto& book = (side == "buy") ? bid_book : ask_book;
            if (remaining == 0)
                book.erase(price);
            else
                book[price] = remaining;
        }
    }
}

double get_best_price(const std::map<std::string, double>& book, bool highest) {
    if (book.empty()) return 0.0;
    return highest ? std::stod(book.rbegin()->first) : std::stod(book.begin()->first);
}

int main() {
    try {
        net::io_context ioc;
        net::ssl::context ctx(net::ssl::context::sslv23_client);
        tcp::resolver resolver(ioc);
        websocket::stream<ssl_stream> ws(ioc, ctx);

        auto results = resolver.resolve("api.gemini.com", "443");
        net::connect(ws.next_layer().next_layer(), results.begin(), results.end());
        ws.next_layer().handshake(net::ssl::stream_base::client);
        ws.handshake("api.gemini.com", "/v1/marketdata/BTCUSD");

        std::cout << "Connected to Gemini WebSocket...\n";

        while (true) {
            beast::flat_buffer buffer;
            ws.read(buffer);
            auto data = beast::buffers_to_string(buffer.data());

            auto msg = json::parse(data);
            if (msg.contains("type") && msg["type"] == "update") {
                auto events = msg["events"];
                process_update(events);
            }
            if (msg.contains("type") && msg["type"] == "snapshot") {
                auto events = msg["events"];
                process_snapshot(events);
            }

            double best_bid = get_best_price(bid_book, true);
            double best_ask = get_best_price(ask_book, false);
            if (best_bid > 0 && best_ask > 0) {
                double mid_price = (best_bid + best_ask) / 2.0;
                std::cout << "Best Bid: " << best_bid << " | Best Ask: " << best_ask
                          << " | Mid Price: " << mid_price << "\n";
            }
        }

    } catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << "\n";
    }
    return 0;
}
