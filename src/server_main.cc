#include "server.hh"

#include <seastar/net/ip.hh>
#include <seastar/net/virtio.hh>
#include <seastar/net/tcp.hh>
#include <seastar/net/native-stack.hh>
#include <seastar/core/reactor.hh>

using namespace seastar;
using namespace net;

const std::string address = "127.0.0.1";

int main() {
    native_stack_options opts;
    auto vnet = create_virtio_net_device(opts.virtio_opts, opts.lro);
    interface netif(std::move(vnet));
    ipv4 inet(&netif);
    inet.set_host_address(ipv4_address(address));
    TCPServer server(inet);
    (void) engine().when_started().then([&server] { server.accept(); });
    engine().run();
}