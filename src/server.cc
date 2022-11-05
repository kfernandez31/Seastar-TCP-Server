//TODO: think of `seastar::repeat` (chapter 11)

#include "server.hh"
#include "parsing.hh"
#include "validation.hh"
#include <iostream>
#include <optional>
#include <seastar/core/reactor.hh>

using namespace seastar;
using namespace net;

MemoryStore& tcp_server::get_store() {
    return store;
}

future<> tcp_server::listen(ipv4_addr addr) {
    listen_options lo;
    lo.proto = transport::TCP;
    lo.reuse_address = true;
    _tcp_listeners.push_back(seastar::listen(make_ipv4_address(addr), lo));
    do_accepts(_tcp_listeners);
    return make_ready_future<>();
}

future<> tcp_server::stop() {
    return make_ready_future<>();
}

void tcp_server::do_accepts(std::vector<server_socket>& listeners) {
    std::cout << "do_accepts" << "\n";
    int which = listeners.size() - 1;
    // Accept in the background.
    (void)listeners[which]
        .accept()
        .then([this, &listeners] (accept_result ar) mutable {
            std::cout << "accept" << "\n";
            connected_socket fd = std::move(ar.connection);
            socket_address addr = std::move(ar.remote_address);
            auto conn = new tcp_connection(*this, std::move(fd), addr);
            (void)conn->process().then_wrapped([conn] (auto&& f) {
                delete conn;
                try {
                    f.get();
                } catch (std::exception& ex) {
                    std::cout << "request error " << ex.what() << "\n";
                }
            });
            do_accepts(listeners);
        }).then_wrapped([] (auto&& f) {
            try {
                f.get();
            } catch (std::exception& ex) {
                std::cout << "accept failed: " << ex.what() << "\n";
            }
        });
}

tcp_server::tcp_connection::tcp_connection(
    tcp_server& server, 
    connected_socket&& fd, 
    socket_address addr
)   : _server(server)
    , _fd(std::move(fd))
    , _read_buf(_fd.input())
    , _write_buf(_fd.output()) {}


future<> tcp_server::tcp_connection::process() {
    std::cout << "process\n";
    return 
    this->read()
        .then([this] (std::string cmd) {
            std::cout << "Process(" << cmd << ")\n";
            return this->handle(cmd);
        });
}

future<> tcp_server::tcp_connection::handle(const std::string& cmd) {
    std::smatch matched_args;
    switch (get_cmd_type(cmd, matched_args)) {
        case cmd_type::STORE:
            return handle_store(matched_args[1], matched_args[2]);
        case cmd_type::LOAD:
            return handle_load(matched_args[1]);
        default:
            return handle_unknown_cmd();
    }
}

future<std::string> tcp_server::tcp_connection::read() {
    if (_read_buf.eof()) {
        return make_ready_future<std::string>("");
    }
    std::string cmd_buf;
    return seastar::do_until(
        [&cmd_buf] { return std::regex_match(cmd_buf, rgx_store) || std::regex_match(cmd_buf, rgx_load); }, 
        [this, &cmd_buf] {
            return _read_buf
            .read_exactly(1)
            .then([] (temporary_buffer<char> buf) { return buf[0]; })
            .then([&cmd_buf](char c) { cmd_buf.push_back(c); });
        }).then([this, &cmd_buf] { return cmd_buf; });
}

future<> tcp_server::tcp_connection::write_and_flush(const std::string& msg) {
    return _write_buf.write(msg)
        .then([this] {
            return _write_buf.flush();
        });
}

future<> tcp_server::tcp_connection::handle_store(const std::string& key, const std::string& val) {
    if (is_valid(key)) {
        return handle_invalid_args();
    }
    return _server.get_store()
        .store(key, val)
        .then([this] {
            return write_and_flush(resp_done);
        })
        .then([this] {
            return this->process();
        });
}

future<> tcp_server::tcp_connection::handle_load(const std::string& key) {
    if (is_valid(key)) {
        return handle_invalid_args();
    }
    return _server.get_store()
        .load(key)
        .then([this] (std::optional<std::string> val) {
            if (val.has_value()) {
                return write_and_flush(val.value());
            } else {
                return write_and_flush(resp_not_found);
            }
        })
        .then([this] {
            return this->process();
        });
}

future<> tcp_server::tcp_connection::handle_unknown_cmd() {
    return write_and_flush(resp_unknown_cmd)
        .then([] {
            return make_ready_future();
        });
}

future<> tcp_server::tcp_connection::handle_invalid_args() {
    return write_and_flush(resp_invalid_args)
        .then([] {
            return make_ready_future();
        });
}