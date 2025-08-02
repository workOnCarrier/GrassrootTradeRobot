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
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

int main() {
    const std::string host = "ws.okx.com";
    const std::string port = "8443";
    const std::string target = "/ws/v5/public";
    net::io_context ioc;
    ssl::context ctx{ssl::context::tlsv12_client};
    ctx.set_default_verify_paths();

    // Resolve and connect
    tcp::resolver resolver{ioc};
    auto results = resolver.resolve(host, port);
    beast::ssl_stream<beast::tcp_stream> stream{ioc, ctx};
    beast::get_lowest_layer(stream).connect(results);
    // [Add SNI fix here]
    if(!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
        throw beast::system_error(
            beast::error_code(
                static_cast<int>(::ERR_get_error()),
                net::error::get_ssl_category()),
            "Failed to set SNI Hostname");
    }
    stream.handshake(ssl::stream_base::client);

    // WebSocket handshake
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws{std::move(stream)};
    ws.handshake(host, target);

    // OKX subscribe message for BTC-USDT ticker
    nlohmann::json subscribe_msg = {
        {"op", "subscribe"},
        {"args", {{
            {"channel", "tickers"},
            {"instId", "BTC-USDT"}
        }}}
    };
    ws.write(net::buffer(subscribe_msg.dump()));

    // Main receive loop
    for (;;) {
        beast::flat_buffer buffer;
        ws.read(buffer);
        std::cout << "OKX:" << beast::make_printable(buffer.data()) << std::endl;
    }
}


