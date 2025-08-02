// gemini_timeseries_rest.cpp
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/asio/ssl.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <curl/curl.h>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;
using ssl_stream = net::ssl::stream<tcp::socket>;


static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

// timeseries
int main_1() {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        std::string url = "https://api.gemini.com/v2/candles/BTCUSD/1m"; // 1 minute candlesticks

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if(res == CURLE_OK)
            std::cout << "Response: \n" << readBuffer << std::endl;
        else
            std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
    }
    return 0;
}

// market data
int main_2() {
    try {
        net::io_context ioc;
        net::ssl::context ctx(net::ssl::context::sslv23_client);

        tcp::resolver resolver(ioc);
        websocket::stream<ssl_stream> ws(ioc, ctx);

        auto const results = resolver.resolve("api.gemini.com", "443");

        net::connect(ws.next_layer().next_layer(), results.begin(), results.end());
        ws.next_layer().handshake(ssl::stream_base::client);
        ws.handshake("api.gemini.com", "/v1/marketdata/BTCUSD");

        std::cout << "Connected to Gemini WebSocket\n";

        while (true) {
            beast::flat_buffer buffer;
            ws.read(buffer);
            std::cout << beast::make_printable(buffer.data()) << std::endl;
            sleep(1);
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

int main(){
    main_1();
    std::cout << "-------" << std::endl;
    main_2();
    return EXIT_SUCCESS;
}
