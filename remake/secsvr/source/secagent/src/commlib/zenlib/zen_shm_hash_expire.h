// Reconstructed from secagent disassembly + DWARF (2026-08-10)
// Original path: src/commlib/zenlib/zen_shm_hash_expire.h
//
// shm_hashtable_expire 模板（secagent 唯一实例化）：
//   shm_hashtable_expire<SecAgentUserInfo, unsigned long long,
//                        ZEN_LIB::smem_hash<unsigned long long>,
//                        ExtractSecAgentUserKey, std::equal_to<unsigned long long>,
//                        TimeOutProc>
//
// 共享内存布局（head 20 字节 + hash_factor(4n) + hash_index(4n) +
// lst_index((n+2)*8) + priority(4n) + value(320n) + 4n 尾部余量）：
//   [0]  _hashtable_expire_head（size_of_mmap_/num_of_node_/sz_freenode_/
//                                sz_usenode_/sz_useindex_）
//   [0x14] hash_factor_base_   = bucket 头索引数组（size_t[n]）
//   [0x14+4n] hash_index_base_ = 同桶 next 链（size_t[n]，同时存 bucket 头）
//   [0x14+8n] lst_index_base_  = _shm_list_index[(n+2)]（节点 0..n-1 +
//                                 use 哨兵 n + free 哨兵 n+1）
//   [0x14+16n] lst_use_node_   = &lst_index_base_[n]
//   [0x14+16n+8] lst_free_node_ = &lst_index_base_[n+1]
//   [0x14+16n+16] priority_base_ = &lst_index_base_[n+2]
//   [0x24+20n] value_base_     = ValueType[n]
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHM_HASH_EXPIRE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHM_HASH_EXPIRE_H_H_

#include "src/commlib/zenlib/zen_shm_hash_table.h"
#include "src/commlib/zenlib/zen_shm_predefine.h"
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <utility>

namespace ZEN_LIB {

// 双向链表节点（8 字节）
struct _shm_list_index {
    size_t idx_next_;
    size_t idx_prev_;
    _shm_list_index() : idx_next_((size_t)-1), idx_prev_((size_t)-1) {}
    _shm_list_index(size_t idx_next, size_t idx_prev)
        : idx_next_(idx_next), idx_prev_(idx_prev) {}
    ~_shm_list_index() {}
};

// 共享内存头（20 字节）
struct _hashtable_expire_head {
    size_t size_of_mmap_;
    size_t num_of_node_;
    size_t sz_freenode_;
    size_t sz_usenode_;
    size_t sz_useindex_;
    _hashtable_expire_head()
        : size_of_mmap_(0), num_of_node_(0), sz_freenode_(0), sz_usenode_(0),
          sz_useindex_(0) {}
    ~_hashtable_expire_head() {}
};

template<typename ValueType, typename KeyType, typename HashFun,
         typename ExtractKey, typename EqualKey, typename WashFun>
class shm_hashtable_expire;

// LRU 迭代器（serial_ = 节点下标，-1 = end）
template<typename ValueType, typename KeyType, typename HashFun,
         typename ExtractKey, typename EqualKey, typename WashFun>
class _hashtable_expire_iterator {
public:
    typedef shm_hashtable_expire<ValueType, KeyType, HashFun, ExtractKey,
                                 EqualKey, WashFun> hash_table_type;
    typedef _hashtable_expire_iterator<ValueType, KeyType, HashFun, ExtractKey,
                                       EqualKey, WashFun> _self;

    _hashtable_expire_iterator() : serial_((size_t)-1), lruht_instance_(NULL) {}
    _hashtable_expire_iterator(size_t serial, hash_table_type *lruht_instance)
        : serial_(serial), lruht_instance_(lruht_instance) {}
    ~_hashtable_expire_iterator() {}

    ValueType &operator*() const {
        return lruht_instance_->value_base_[serial_];
    }
    ValueType *operator->() const {
        return &lruht_instance_->value_base_[serial_];
    }

    // 迭代顺序：先同桶链，再后续桶
    _self &operator++() {
        size_t next = lruht_instance_->hash_index_base_[serial_];
        if (next != (size_t)-1) {
            serial_ = next;
            return *this;
        }
        KeyType key = ExtractKey()(lruht_instance_->value_base_[serial_]);
        size_t num_of_node = lruht_instance_->lru_hash_head_->num_of_node_;
        size_t bucket = lruht_instance_->bkt_num_key(key) + 1;
        for (; bucket < num_of_node; ++bucket) {
            next = lruht_instance_->hash_factor_base_[bucket];
            if (next != (size_t)-1) {
                serial_ = next;
                return *this;
            }
        }
        serial_ = (size_t)-1;
        return *this;
    }

    _self &goto_next_equal() {
        size_t next = lruht_instance_->hash_index_base_[serial_];
        if (next != (size_t)-1) {
            serial_ = next;
        } else {
            serial_ = (size_t)-1;
        }
        return *this;
    }

    _self operator++(int) {
        _self tmp = *this;
        ++*this;
        return tmp;
    }

    bool operator==(const _self &arg) const {
        return serial_ == arg.serial_;
    }
    bool operator!=(const _self &arg) const {
        return serial_ != arg.serial_;
    }

    size_t getserial() const {
        return serial_;
    }

protected:
    size_t serial_;
    hash_table_type *lruht_instance_;
};

template<typename ValueType, typename KeyType, typename HashFun,
         typename ExtractKey, typename EqualKey, typename WashFun>
class shm_hashtable_expire : public _shm_memory_base {
public:
    typedef _hashtable_expire_iterator<ValueType, KeyType, HashFun, ExtractKey,
                                       EqualKey, WashFun> iterator;
    typedef std::pair<iterator, bool> pair_iterator_bool;

    _hashtable_expire_head *lru_hash_head_;   // +8
    size_t *hash_factor_base_;                // +12 bucket 头索引
    size_t *hash_index_base_;                 // +16 同桶 next 链
    _shm_list_index *lst_index_base_;         // +20
    _shm_list_index *lst_free_node_;          // +24 free 哨兵
    _shm_list_index *lst_use_node_;           // +28 use 哨兵
    unsigned int *priority_base_;             // +32
    ValueType *value_base_;                   // +36

    // 行 272：按 head->num_of_node_ 计算各成员地址（与二进制逐字节一致）
    explicit shm_hashtable_expire(char *smem_base) : _shm_memory_base(smem_base) {
        lru_hash_head_ = (_hashtable_expire_head *)smem_base;
        hash_factor_base_ =
            (size_t *)(smem_base + sizeof(_hashtable_expire_head));
        hash_index_base_ = hash_factor_base_ + lru_hash_head_->num_of_node_;
        lst_index_base_ =
            (_shm_list_index *)(hash_index_base_ + lru_hash_head_->num_of_node_);
        lst_use_node_ = lst_index_base_ + lru_hash_head_->num_of_node_;
        lst_free_node_ = lst_index_base_ + lru_hash_head_->num_of_node_ + 1;
        priority_base_ = (unsigned int *)(lst_free_node_ + 1);
        value_base_ = (ValueType *)((char *)lst_free_node_ +
                                    sizeof(_shm_list_index) +
                                    lru_hash_head_->num_of_node_ *
                                        sizeof(unsigned int));
    }

    virtual ~shm_hashtable_expire() {}

    // 行 293：返回不小于 num 的安全素数；超界返回 _INVALID_POINT(-5)
    static size_t get_next_prime(unsigned int num) {
        static const size_t num_primes_list[] = {
            53,         97,         193,        389,        769,
            1543,       3079,       6151,       12289,      21101,
            31957,      42953,      53657,      63761,      73009,
            83207,      93199,      101051,     111187,     120851,
            130699,     141283,     150383,     161503,     170899,
            181639,     191657,     203431,     252817,     310823,
            408869,     510989,     603881,     710531,     803171,
            909613,     1008853,    1220027,    1572869,    1820009,
            2120009,    3145739,    4220039,    4620001,    5120029,
            6291469,    7320011,    8120023,    9320009,    12582917,
            21200021,   31200007,   40200071,   50331653,   65200013,
            74200013,   83200009,   90200017,   100663319,  201326611,
            312000019,  402653189,  512000009,  622000003,  732000011,
            805306457,  922000007,  1610612741, 3221225473, 4294967291};
        for (size_t i = 0;
             i < sizeof(num_primes_list) / sizeof(num_primes_list[0]); ++i) {
            if (num <= num_primes_list[i]) {
                return num_primes_list[i];
            }
        }
        return (size_t)-5; // _INVALID_POINT
    }

    // 行 320：分配总大小（secagent: 344*n + 36）
    static size_t getallocsize(size_t num_of_node) {
        return sizeof(_hashtable_expire_head) +
               2 * num_of_node * sizeof(size_t) +
               (num_of_node + 2) * sizeof(_shm_list_index) +
               2 * num_of_node * sizeof(unsigned int) +
               num_of_node * sizeof(ValueType);
    }

    // 行 343：初始化 head / 哨兵 / 空闲链 / bucket 头与 next / 优先级
    void initialize(char *smem_base, size_t num_of_node, size_t sz_mmap) {
        lru_hash_head_ = (_hashtable_expire_head *)smem_base;
        lru_hash_head_->size_of_mmap_ = sz_mmap;
        lru_hash_head_->num_of_node_ = num_of_node;
        lru_hash_head_->sz_freenode_ = num_of_node;
        lru_hash_head_->sz_usenode_ = 0;
        lru_hash_head_->sz_useindex_ = 0;
        lst_use_node_[0].idx_next_ = num_of_node;
        lst_use_node_[0].idx_prev_ = num_of_node;
        lst_free_node_[0].idx_next_ = num_of_node + 1;
        lst_free_node_[0].idx_prev_ = num_of_node + 1;
        if (num_of_node != 0) {
            for (size_t i = 0; i < num_of_node; ++i) {
                hash_factor_base_[i] = (size_t)-1;
                hash_index_base_[i] = (size_t)-1;
                priority_base_[i] = 0;
                lst_index_base_[i].idx_next_ = i + 1;
                lst_index_base_[i].idx_prev_ = i - 1;
                if (i == 0) {
                    lst_index_base_[0].idx_prev_ =
                        lst_free_node_[0].idx_next_;
                    lst_free_node_[0].idx_next_ = 0;
                }
            }
            lst_index_base_[num_of_node - 1].idx_next_ =
                lst_free_node_[0].idx_prev_;
            lst_free_node_[0].idx_prev_ = num_of_node - 1;
        }
    }

    // 行 400：清空所有 bucket / next / 优先级，重建空闲链
    void clear() {
        size_t num_of_node = lru_hash_head_->num_of_node_;
        lru_hash_head_->sz_freenode_ = num_of_node;
        lru_hash_head_->sz_usenode_ = 0;
        lru_hash_head_->sz_useindex_ = 0;
        lst_use_node_[0].idx_next_ = num_of_node;
        lst_use_node_[0].idx_prev_ = num_of_node;
        lst_free_node_[0].idx_next_ = num_of_node + 1;
        lst_free_node_[0].idx_prev_ = num_of_node + 1;
        if (num_of_node != 0) {
            for (size_t i = 0; i < num_of_node; ++i) {
                hash_factor_base_[i] = (size_t)-1;
                hash_index_base_[i] = (size_t)-1;
                priority_base_[i] = 0;
                lst_index_base_[i].idx_next_ = i + 1;
                lst_index_base_[i].idx_prev_ = i - 1;
                if (i == 0) {
                    lst_index_base_[0].idx_prev_ =
                        lst_free_node_[0].idx_next_;
                    lst_free_node_[0].idx_next_ = 0;
                }
            }
            lst_index_base_[num_of_node - 1].idx_next_ =
                lst_free_node_[0].idx_prev_;
            lst_free_node_[0].idx_prev_ = num_of_node - 1;
        }
    }

    // 行 449：从空闲链头取一个节点
    size_t create_node() {
        size_t node = lst_free_node_[0].idx_next_;
        lst_free_node_[0].idx_next_ = lst_index_base_[node].idx_next_;
        lst_index_base_[lst_free_node_[0].idx_next_].idx_prev_ =
            lst_index_base_[node].idx_prev_;
        return node;
    }

    // 行 483：从 use 链摘除并压回空闲链头
    void destroy_node(size_t pos) {
        size_t next = lst_index_base_[pos].idx_next_;
        size_t prev = lst_index_base_[pos].idx_prev_;
        lst_index_base_[next].idx_prev_ = prev;
        lst_index_base_[prev].idx_next_ = next;

        size_t head_next = lst_free_node_[0].idx_next_;
        lst_index_base_[pos].idx_next_ = head_next;
        lst_index_base_[pos].idx_prev_ =
            lst_index_base_[head_next].idx_prev_;
        lst_free_node_[0].idx_next_ = pos;
        lst_index_base_[head_next].idx_prev_ = pos;

        lru_hash_head_->sz_usenode_--;
        lru_hash_head_->sz_freenode_++;
        hash_index_base_[pos] = (size_t)-1;
        priority_base_[pos] = 0;
    }

    size_t bkt_num_value(const ValueType &value) {
        return bkt_num_key(ExtractKey()(value));
    }

    size_t bkt_num_key(const KeyType &key) {
        return HashFun()(key) % lru_hash_head_->num_of_node_;
    }

    iterator begin() {
        size_t num_of_node = lru_hash_head_->num_of_node_;
        for (size_t i = 0; i < num_of_node; ++i) {
            if (hash_factor_base_[i] != (size_t)-1) {
                return iterator(hash_factor_base_[i], this);
            }
        }
        return iterator((size_t)-1, this);
    }

    iterator end() {
        return iterator((size_t)-1, this);
    }

    size_t size() {
        return lru_hash_head_->sz_usenode_;
    }

    size_t capacity() {
        return lru_hash_head_->num_of_node_;
    }

    bool empty() {
        return lru_hash_head_->sz_usenode_ == 0;
    }

    bool full() {
        return lru_hash_head_->sz_freenode_ == 0;
    }

    // 行 575：插入（已存在返回 false；空闲不足返回 false；成功返回 true）
    pair_iterator_bool insert(const ValueType &value, unsigned int priority) {
        KeyType key = ExtractKey()(value);
        size_t bucket = bkt_num_key(key);
        size_t pos = hash_factor_base_[bucket];
        while (pos != (size_t)-1) {
            if (EqualKey()(key, ExtractKey()(value_base_[pos]))) {
                return pair_iterator_bool(iterator(pos, this), false);
            }
            pos = hash_index_base_[pos];
        }
        if (lru_hash_head_->sz_freenode_ == 0) {
            return pair_iterator_bool(iterator((size_t)-1, this), false);
        }
        size_t node = create_node();
        size_t old_head_next = lst_use_node_[0].idx_next_;
        lst_index_base_[node].idx_next_ = old_head_next;
        lst_index_base_[node].idx_prev_ =
            lst_index_base_[old_head_next].idx_prev_;
        lst_index_base_[old_head_next].idx_prev_ = node;
        lst_use_node_[0].idx_next_ = node;
        value_base_[node] = value;
        priority_base_[node] = priority;
        lru_hash_head_->sz_usenode_++;
        lru_hash_head_->sz_freenode_--;
        hash_index_base_[node] = hash_factor_base_[bucket];
        hash_factor_base_[bucket] = node;
        return pair_iterator_bool(iterator(node, this), true);
    }

    pair_iterator_bool insert(const ValueType &value) {
        return insert(value, (unsigned int)time(NULL));
    }

    // 行 617：允许重复（同 insert_equal 语义按二进制模式实现）
    pair_iterator_bool insert_equal(const ValueType &value,
                                    unsigned int priority) {
        if (lru_hash_head_->sz_freenode_ == 0) {
            return pair_iterator_bool(iterator((size_t)-1, this), false);
        }
        KeyType key = ExtractKey()(value);
        size_t bucket = bkt_num_key(key);
        size_t node = create_node();
        size_t old_head_next = lst_use_node_[0].idx_next_;
        lst_index_base_[node].idx_next_ = old_head_next;
        lst_index_base_[node].idx_prev_ =
            lst_index_base_[old_head_next].idx_prev_;
        lst_index_base_[old_head_next].idx_prev_ = node;
        lst_use_node_[0].idx_next_ = node;
        value_base_[node] = value;
        priority_base_[node] = priority;
        lru_hash_head_->sz_usenode_++;
        lru_hash_head_->sz_freenode_--;
        hash_index_base_[node] = hash_factor_base_[bucket];
        hash_factor_base_[bucket] = node;
        return pair_iterator_bool(iterator(node, this), true);
    }

    iterator find(const KeyType &key) {
        size_t bucket = bkt_num_key(key);
        size_t pos = hash_factor_base_[bucket];
        while (pos != (size_t)-1) {
            if (EqualKey()(key, ExtractKey()(value_base_[pos]))) {
                return iterator(pos, this);
            }
            pos = hash_index_base_[pos];
        }
        return iterator((size_t)-1, this);
    }

    iterator find_value(const ValueType &value) {
        return find(ExtractKey()(value));
    }

    size_t count(const KeyType &key) {
        size_t bucket = bkt_num_key(key);
        size_t num = 0;
        size_t pos = hash_factor_base_[bucket];
        while (pos != (size_t)-1) {
            if (EqualKey()(key, ExtractKey()(value_base_[pos]))) {
                ++num;
            }
            pos = hash_index_base_[pos];
        }
        return num;
    }

    size_t count_value(const ValueType &value) {
        return count(ExtractKey()(value));
    }

    // 行 721：erase(iterator)
    iterator erase(iterator itr) {
        return erase(itr.getserial());
    }

    // 行 763：erase(node)
    iterator erase(size_t pos) {
        KeyType key = ExtractKey()(value_base_[pos]);
        size_t bucket = bkt_num_key(key);
        size_t prev = (size_t)-1;
        size_t cur = hash_factor_base_[bucket];
        while (cur != (size_t)-1) {
            if (cur == pos) {
                if (prev == (size_t)-1) {
                    hash_factor_base_[bucket] = hash_index_base_[pos];
                } else {
                    hash_index_base_[prev] = hash_index_base_[pos];
                }
                destroy_node(pos);
                return iterator((size_t)-1, this);
            }
            prev = cur;
            cur = hash_index_base_[cur];
        }
        return iterator((size_t)-1, this);
    }

    size_t erase_value(const ValueType &value) {
        iterator itr = find_value(value);
        if (itr != end()) {
            erase(itr);
            return 1;
        }
        return 0;
    }

    size_t erase_equal(const ValueType &value) {
        size_t num = 0;
        iterator itr = find_value(value);
        while (itr != end()) {
            erase(itr);
            ++num;
            itr = find_value(value);
        }
        return num;
    }

    // 行 861：把节点移到 use 链头并刷新优先级
    void active(size_t pos, unsigned int priority) {
        priority_base_[pos] = priority;
        size_t next = lst_index_base_[pos].idx_next_;
        size_t prev = lst_index_base_[pos].idx_prev_;
        lst_index_base_[next].idx_prev_ = prev;
        lst_index_base_[prev].idx_next_ = next;
        size_t old_head_next = lst_use_node_[0].idx_next_;
        lst_index_base_[pos].idx_next_ = old_head_next;
        lst_index_base_[pos].idx_prev_ =
            lst_index_base_[old_head_next].idx_prev_;
        lst_index_base_[old_head_next].idx_prev_ = pos;
        lst_use_node_[0].idx_next_ = pos;
    }

    void active_value(const ValueType &value, unsigned int priority) {
        size_t pos = find(ExtractKey()(value)).getserial();
        if (pos != (size_t)-1) {
            active(pos, priority);
        }
    }

    void active_equal(const ValueType &value, unsigned int priority) {
        active_value(value, priority);
    }

    // 行 998：从 use 链尾（最旧）开始，把优先级 <= max_priority 的节点
    // 逐一出队并调用 wash_fun（越界节点即过期）。
    size_t expire(unsigned int max_priority, WashFun wash_fun) {
        size_t expire_num = 0;
        size_t num_of_node = lru_hash_head_->num_of_node_;
        size_t pos = lst_use_node_[0].idx_prev_;
        while (pos != num_of_node) {
            if (priority_base_[pos] >= max_priority) {
                break;
            }
            wash_fun(value_base_[pos]);
            erase(pos);
            ++expire_num;
            pos = lst_use_node_[0].idx_prev_;
        }
        return expire_num;
    }

    // 行 1032：disuse（按模式实现，secagent 未实例化）
    void disuse(size_t pos) {
        priority_base_[pos] = 0;
    }

    // 行 1063：mark（按模式实现，secagent 未实例化）
    void mark(size_t pos, unsigned int priority) {
        priority_base_[pos] = priority;
    }

    void mark_value(const ValueType &value, unsigned int priority) {
        active_value(value, priority);
    }

    void mark_equal(const ValueType &value, unsigned int priority) {
        active_value(value, priority);
    }

    // 行 1162：washout（按模式实现，secagent 未实例化）
    size_t washout(unsigned int max_priority, WashFun wash_fun) {
        return expire(max_priority, wash_fun);
    }
};

} // namespace ZEN_LIB

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_SHM_HASH_EXPIRE_H_H_
