#pragma once

template<typename K, typename V> 
class Store<T> {
public:
    virtual seastar::future<> store(K& key, V& value) = 0;
    virtual seastar::future<std::optional<V>> load(K& key) = 0;
}

class MemoryStore : public Store<std::string, std::string> {
    std::unordered_map<std::string, std::string> storage;

    seastar::future<> store(std::string& key, std::string& value) {
        storage.insert({key, value});
        return seastar::make_ready_future<>();
    }

    seastar::future<std::optional<std::string>> load(std::string &key) {
        auto query_res = storage.find(key);
        if (query_res == storage.end()) {
            return seastar::make_ready_future<std::optional<std::string>>(std::nullopt);
        }
        return seastar::make_ready_future<std::optional<std::string>>(query_res->second);
    }
}
