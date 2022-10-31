#include "store.hh"

using std::optional;
using seastar::future;

MemoryStore::MemoryStore(): storage() {}

future<> MemoryStore::store(const std::string& key, const std::string& value) {
    storage.insert({key, value});
    return seastar::make_ready_future<>();
}

future<optional<std::string>> MemoryStore::load(const std::string &key) {
    auto query_res = storage.find(key);
    if (query_res == storage.end()) {
        return seastar::make_ready_future<optional<std::string>>(std::nullopt);
    }
    return seastar::make_ready_future<optional<std::string>>(query_res->second);
}
