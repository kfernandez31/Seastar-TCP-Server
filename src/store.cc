#include "store.hh"

using std::string;
using std::optional;
using std::nullopt;
using seastar::future;

future<> MemoryStore::store(const string& key, const string& value) {
    storage.insert({key, value});
    return seastar::make_ready_future<>();
}

future<optional<string>> MemoryStore::load(const string &key) {
    auto query_res = storage.find(key);
    if (query_res == storage.end()) {
        return seastar::make_ready_future<optional<string>>(nullopt);
    }
    return seastar::make_ready_future<optional<string>>(query_res->second);
}
