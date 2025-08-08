#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

int main() {
    try {
        std::string host = "api.gemini.com";
        std::string port = "443";
        std::string target = "/v1/marketdata/btcusd"; // Example endpoint

        net::io_context ioc;
        tcp::resolver resolver(ioc);
        net::ssl::context ctx(net::ssl::context::tlsv12_client); // 1. Persistent context
        beast::ssl_stream<tcp::socket> stream(ioc, ctx); // 2. Pass context by reference

        auto const results = resolver.resolve(host, port);

        // Connect the TCP socket
        net::connect(beast::get_lowest_layer(stream), results);

        // Perform SSL handshake
        stream.handshake(net::ssl::stream_base::client);

        websocket::stream<beast::ssl_stream<tcp::socket>> ws(std::move(stream));
        ws.handshake(host, target);

        // Send a subscribe message if required by the API
        // ws.write(net::buffer(R"({"type":"subscribe","channels":["ticker"]})"));

        for (;;) {
            beast::flat_buffer buffer;
            ws.read(buffer);
            std::cout << beast::make_printable(buffer.data()) << std::endl;
        }
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

