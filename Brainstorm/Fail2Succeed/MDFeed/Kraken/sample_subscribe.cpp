#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ssl.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <nlohmann/json.hpp>  // Optional: use Boost.JSON if you prefer

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using json = nlohmann::json;

const std::string kraken_host = "ws.kraken.com";
const std::string kraken_port = "443";
const std::string kraken_target = "/";

void subscribe_to_ticker(websocket::stream<beast::ssl_stream<tcp::socket>>& ws) {
    json sub_msg = {
        {"event", "subscribe"},
        {"pair", {"XBT/USD"}},
        {"subscription", {
            {"name", "ticker"}
        }}
    };

    std::string msg_str = sub_msg.dump();
    ws.write(net::buffer(msg_str));
    std::cout << ">> Sent subscription: " << msg_str << std::endl;
}

int main() {
    try {
        net::io_context ioc;
        net::ssl::context ssl_ctx(net::ssl::context::sslv23_client);

        tcp::resolver resolver(ioc);
        auto const results = resolver.resolve(kraken_host, kraken_port);

        websocket::stream<beast::ssl_stream<tcp::socket>> ws(ioc, ssl_ctx);
        if (!SSL_set_tlsext_host_name(ws.next_layer().native_handle(), kraken_host.c_str())){
            throw beast::system_error(beast::error_code(static_cast<int>(::ERR_get_error()), 
            net::error::get_ssl_category()), "Failed to set SNI Hostname");
        }

        // SSL handshake
        // beast::get_lowest_layer(ws).connect(results);
        net::connect(beast::get_lowest_layer(ws), results);
        ws.next_layer().handshake(net::ssl::stream_base::client);

        // WebSocket handshake
        ws.handshake(kraken_host, kraken_target);

        subscribe_to_ticker(ws);

        beast::flat_buffer buffer;

        while (true) {
            ws.read(buffer);

            std::string message = beast::buffers_to_string(buffer.data());
            buffer.consume(buffer.size()); // clear buffer

            // Skip heartbeat/ping messages
            if (message.find("event") != std::string::npos)
                continue;

            // Parse ticker update
            try {
                auto parsed = json::parse(message);

                // Example response:
                // [channelID, { "a": ["price", "wholeLotVolume", "lotVolume"], ... }, "ticker", "XBT/USD"]
                if (parsed.is_array() && parsed.size() >= 4 && parsed[2] == "ticker") {
                    std::string pair = parsed[3];
                    std::string ask_price = parsed[1]["a"][0];
                    std::string bid_price = parsed[1]["b"][0];

                    std::cout << ">> " << pair
                              << " Ask: " << ask_price
                              << " | Bid: " << bid_price << std::endl;
                }
            } catch (const std::exception& e) {
                std::cerr << "JSON Parse Error: " << e.what() << "\n";
                std::cerr << "Raw Message: " << message << "\n";
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
