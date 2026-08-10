// Reconstructed from secagent disassembly + DWARF (2026-08-10)
// Original path: src/commlib/zenlib/zen_data_with_version.h
//
// ZenDataWithVersion<KeyType, ValueType, EqualKey>（secagent 实例化
// <unsigned int, SecagentGameExe, std::equal_to<unsigned int>>）：
//   data_list_@0 = std::list<CNode>（_List_node_base 8 字节）
//   max_reserve_version_@8
// CNode = { KeyType key_; ValueType *value_; }（8 字节）
// 反汇编核对：
//   - add_new_data 失败（key 已存在）返回 false；
//   - 成功 push_front，若 size > max_reserve_version_ 则 delete 尾节点
//     value 并 pop_back；
//   - get_newest_data 返回头节点 value；
//   - 析构仅释放节点内存（value 原版未释放，照实复刻）。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_DATA_WITH_VERSION_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_DATA_WITH_VERSION_H_H_

#include <list>
#include <memory>
#include <stddef.h>

template<typename KeyType, typename ValueType, typename EqualKey>
class ZenDataWithVersion {
public:
    struct CNode {
        KeyType key_;
        ValueType *value_;
        CNode() : key_(), value_(NULL) {}
        CNode(const KeyType &key, ValueType *value)
            : key_(key), value_(value) {}
    };

    explicit ZenDataWithVersion(unsigned int version)
        : max_reserve_version_(version) {}

    ~ZenDataWithVersion() {}

    const ValueType *get_data(const KeyType &key) const {
        for (typename std::list<CNode>::const_iterator it = data_list_.begin();
             it != data_list_.end(); ++it) {
            if (EqualKey()(it->key_, key)) {
                return it->value_;
            }
        }
        return NULL;
    }

    const ValueType *get_newest_data() const {
        if (data_list_.empty()) {
            return NULL;
        }
        return data_list_.front().value_;
    }

    const KeyType *get_newest_key() const {
        if (data_list_.empty()) {
            return NULL;
        }
        return &data_list_.front().key_;
    }

    bool add_new_data(const KeyType &key, std::auto_ptr<ValueType> &obj) {
        if (get_data(key) != NULL) {
            return false;
        }
        CNode node(key, obj.release());
        data_list_.push_front(node);
        if (data_list_.size() > max_reserve_version_) {
            CNode &oldest = data_list_.back();
            delete oldest.value_;
            data_list_.pop_back();
        }
        return true;
    }

    bool add_new_data_cover_of(const KeyType &key,
                               std::auto_ptr<ValueType> &obj) {
        for (typename std::list<CNode>::iterator it = data_list_.begin();
             it != data_list_.end(); ++it) {
            if (EqualKey()(it->key_, key)) {
                delete it->value_;
                it->value_ = obj.release();
                return true;
            }
        }
        return add_new_data(key, obj);
    }

    void set_max_reserve_version(unsigned int version) {
        max_reserve_version_ = version;
    }

private:
    std::list<CNode> data_list_;     // 0
    size_t max_reserve_version_;     // 8
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_DATA_WITH_VERSION_H_H_
