#include "server.hh"
#include <seastar/core/reactor.hh>
#include <seastar/core/app-template.hh>
#include <seastar/core/distributed.hh>
#include <iostream>

using namespace seastar;
namespace bpo = boost::program_options;

static const uint16_t default_port = 2137;

int main(int ac, char** av) {
    app_template app;
    app.add_options()
        ("port", bpo::value<uint16_t>()->default_value(default_port), "TCP server port") ;
    return app.run_deprecated(ac, av, [&] {
        auto&& config = app.configuration();
        uint16_t port = config["port"].as<uint16_t>();
        auto server = new distributed<tcp_server>;
        (void)server->start().then([server = std::move(server), port] () mutable {
            engine().at_exit([server] {
                return server->stop();
            });
            // Start listening in the background.
            (void)server->invoke_on_all(&tcp_server::listen, ipv4_addr{port});
        }).then([port] {
            std::cout << "Seastar TCP server listening on port " << port << " ...\n";
        });
    });
}
