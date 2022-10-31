#pragma once

#include <unordered_map>
#include <seastar/core/future.hh>
#include <string>

template<typename K, typename V> 
class Store {
public:
    virtual seastar::future<> store(const K& key, const V& value) = 0;
    virtual seastar::future<std::optional<V>> load(const K& key) = 0;
};

class MemoryStore : public Store<std::string, std::string> {
private:
    std::unordered_map<std::string, std::string> storage;
public:
    MemoryStore();
    seastar::future<> store(const std::string& key, const std::string& value);
    seastar::future<std::optional<std::string>> load(const std::string &key);
};
