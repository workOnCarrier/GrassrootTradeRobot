#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/client.hpp>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

typedef websocketpp::client<websocketpp::config::asio> client;
typedef client::message_ptr message_ptr;

class MarketDataClient {
public:
    MarketDataClient() {
        m_client.init_asio();
        m_client.set_open_handler(bind(&MarketDataClient::on_open, this, ::_1));
        m_client.set_message_handler(bind(&MarketDataClient::on_message, this, ::_1, ::_2));
        m_client.set_fail_handler(bind(&MarketDataClient::on_fail, this, ::_1));
    }

    void connect(const std::string& uri) {
        websocketpp::lib::error_code ec;
        client::connection_ptr con = m_client.get_connection(uri, ec);
        if (ec) {
            std::cerr << "Connection error: " << ec.message() << std::endl;
            return;
        }
        m_client.connect(con);
    }

    void run() {
        m_client.run();
    }

private:
    void on_open(websocketpp::connection_hdl hdl) {
        json subscribe_msg = {
            {"event", "subscribe"},
            {"subscription", {{"name", "ticker"}}},
            {"pair", {"BTC/USD"}}
        };
        m_client.send(hdl, subscribe_msg.dump(), websocketpp::frame::opcode::text);
        std::cout << "Subscribed to market data feed\n";
    }

    void on_message(websocketpp::connection_hdl, message_ptr msg) {
        try {
            json data = json::parse(msg->get_payload());
            
            // Handle different message types
            if (data.contains("event") && data["event"] == "heartbeat") {
                return;  // Ignore heartbeats
            }
            
            if (data.contains("channelID")) {
                std::cout << "Market update: " << data.dump(2) << "\n";
                // Add your order book/trade processing logic here
            }
        } catch (const json::parse_error& e) {
            std::cerr << "JSON parse error: " << e.what() << "\n";
        }
    }

    void on_fail(websocketpp::connection_hdl hdl) {
        std::cerr << "Connection failed!\n";
    }

    client m_client;
};

int main() {
    MarketDataClient client;
    client.connect("wss://ws.kraken.com");  // Kraken endpoint
    client.run();
    return 0;
}
