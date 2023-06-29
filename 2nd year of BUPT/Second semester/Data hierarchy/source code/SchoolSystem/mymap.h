// 防止头文件被多次包含
#ifndef MYMAP_H
#define MYMAP_H

// 引入所需的头文件
#include <iostream>
#include <list>
#include <vector>
#include <functional>
#include <stdexcept>
#include <iterator>
#include <initializer_list>

// 定义模板类MyMap
template<typename K, typename V>
class MyMap {
private:
    // 定义默认桶数量
    static const size_t defaultBucketCount = 10;

    // 定义内部结构KeyValuePair，用于存储键值对
    struct KeyValuePair {
        K key;
        V value;

        KeyValuePair(const K& key, const V& value) : key(key), value(value) {}
    };

    // 使用vector和list实现哈希表，每个桶包含一个list
    std::vector<std::list<KeyValuePair>> buckets;
    // 桶的数量
    size_t bucketCount;
    // 哈希表中元素的数量
    size_t itemCount;

    // 计算给定键的哈希值
    size_t hash(const K& key) const {
        std::hash<K> hasher;
        return hasher(key) % bucketCount;
    }

public:
    // 构造函数，初始化哈希表的大小
    MyMap(size_t bucketCount = defaultBucketCount) : itemCount(0), bucketCount(bucketCount) {
        buckets.resize(bucketCount);
    }

    // 定义迭代器类
    class iterator {
    public:
        // 定义迭代器的类型别名
        using difference_type = std::ptrdiff_t;
        using value_type = KeyValuePair;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = std::forward_iterator_tag;

    private:
        // 声明MyMap为友元类，以便访问私有成员变量
        friend class MyMap;

        // 定义迭代器的成员变量
        typename std::vector<std::list<KeyValuePair>>::iterator bucketIter;
        typename std::vector<std::list<KeyValuePair>>::iterator bucketEnd;
        typename std::list<KeyValuePair>::iterator itemIter;

        // 迭代器构造函数
        iterator(typename std::vector<std::list<KeyValuePair>>::iterator bucketIter,
                 typename std::vector<std::list<KeyValuePair>>::iterator bucketEnd,
                 typename std::list<KeyValuePair>::iterator itemIter)
            : bucketIter(bucketIter), bucketEnd(bucketEnd), itemIter(itemIter) {}

        // 移动到下一个非空桶
        void moveToNextItem() {
            while (bucketIter != bucketEnd && itemIter == bucketIter->end()) {
                ++bucketIter;
                if (bucketIter != bucketEnd) {
                    itemIter = bucketIter->begin();
                }
            }
        }

    public:
        // 重载前置++运算符
        iterator& operator++() {
            ++itemIter;
            moveToNextItem();
            return *this;
        }

        // 重载后置++运算符
        iterator operator++(int) {
            iterator temp(*this);
            ++(*this);
            return temp;
        }

        // 重载解引用运算符
        reference operator*() {
            return *itemIter;
        }

        // 重载箭头运算符
        pointer operator->() {
            return &(*itemIter);
        }

        // 重载相等运算符
        bool operator==(const iterator& other) const {
            return bucketIter == other.bucketIter && (bucketIter == bucketEnd || itemIter == other.itemIter);
        }

        // 重载不等运算符
        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
    };

    // 返回指向哈希表第一个元素的迭代器
    iterator begin() {
        auto bucketIter = buckets.begin();
        auto itemIter = (bucketIter != buckets.end()) ? bucketIter->begin() : std::list<KeyValuePair>::iterator();
        iterator iter(bucketIter, buckets.end(), itemIter);
        return iter;
    }

    // 返回指向哈希表尾部的迭代器
    iterator end() {
        return iterator(buckets.end(), buckets.end(), std::list<KeyValuePair>::iterator());
    }

    // 插入一个键值对
    void insert(const K& key, const V& value) {
        size_t index = hash(key);
        auto& bucket = buckets[index];

        for (const auto& kv : bucket) {
            if (kv.key ==key) {
                throw std::runtime_error("Key already exists");
            }
        }

        bucket.push_back(KeyValuePair(key, value));
        itemCount++;
    }

    // 通过键查找值，如果找不到则抛出异常
    V& at(const K& key) {
        size_t index = hash(key);
        auto& bucket = buckets[index];

        for (auto& kv : bucket) {
            if (kv.key == key) {
                return kv.value;
            }
        }

        throw std::runtime_error("Key not found");
    }

    // 重载[]运算符，通过键查找值，如果找不到则插入一个具有默认值的键值对
    V& operator[](const K& key) {
        size_t index = hash(key);
        auto& bucket = buckets[index];

        for (auto& kv : bucket) {
            if (kv.key == key) {
                return kv.value;
            }
        }

        // 找不到键，插入一个带有键和默认构造值的新元素。
        V defaultValue{};
        bucket.push_back(KeyValuePair(key, defaultValue));
        itemCount++;
        return bucket.back().value;
    }

    // 删除指定键的元素，如果找不到则抛出异常
    void erase(const K& key) {
        size_t index = hash(key);
        auto& bucket = buckets[index];

        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->key == key) {
                bucket.erase(it);
                itemCount--;
                return;
            }
        }

        throw std::runtime_error("Key not found");
    }

    // 检查哈希表中是否包含指定的键
    bool contains(const K& key) const {
        size_t index = hash(key);
        const auto& bucket = buckets[index];

        for (const auto& kv : bucket) {
            if (kv.key == key) {
                return true;
            }
        }

        return false;
    }

    // 返回哈希表中元素的数量
    size_t size() const {
        return itemCount;
    }

    // 接受初始化列表的构造函数，用于初始化哈希表
    MyMap(std::initializer_list<std::pair<K, V>> initList, size_t bucketCount = defaultBucketCount)
        : MyMap(bucketCount) {
        for (const auto& pair : initList) {
            insert(pair.first, pair.second);
        }
    }
};

#endif // MYMAP_H
