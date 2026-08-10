// Reconstructed from secagent binary DWARF + disassembly (ZenDataWithVersion template).
// 布局（sizeof = 12）：list 头节点内联(+0, 8) + max_reserve_version_(+8)。
// 语义已按二进制符号对拍：add_new_data 头插（newest 在前），超限删尾。
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_DATA_WITH_VERSION_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_DATA_WITH_VERSION_H_H_

#include <functional>
#include <list>
#include <memory>
#include <stddef.h>

template <typename Key, typename Data, typename KeyCompare = std::equal_to<Key> >
class ZenDataWithVersion { // line 26
public:
    struct CNode {
        Key key;
        Data *data;
    };

    ZenDataWithVersion(unsigned int max_reserve_version); // line 67
    ~ZenDataWithVersion(); // line 73

    void set_max_reserve_version(unsigned int max_reserve_version); // line 90
    const Data * get_data(const Key &key) const; // line 97
    const Data * get_newest_data() const; // line 114
    const Key * get_newest_key() const; // line 126
    bool add_new_data_cover_of(const Key &key, std::auto_ptr<Data> &obj); // line 138
    bool add_new_data(const Key &key, std::auto_ptr<Data> &obj); // line 169

private:
    std::list<CNode> data_list_;      // +0 (8)
    size_t max_reserve_version_;      // +8 (4)
};

template <typename Key, typename Data, typename KeyCompare>
ZenDataWithVersion<Key, Data, KeyCompare>::ZenDataWithVersion(unsigned int max_reserve_version)
    : max_reserve_version_(max_reserve_version) {
}

template <typename Key, typename Data, typename KeyCompare>
ZenDataWithVersion<Key, Data, KeyCompare>::~ZenDataWithVersion() {
    for (typename std::list<CNode>::iterator iter = data_list_.begin();
         iter != data_list_.end(); ++iter) {
        delete iter->data;
        iter->data = NULL;
    }
    data_list_.clear();
}

template <typename Key, typename Data, typename KeyCompare>
void ZenDataWithVersion<Key, Data, KeyCompare>::set_max_reserve_version(unsigned int max_reserve_version) {
    max_reserve_version_ = max_reserve_version;
}

template <typename Key, typename Data, typename KeyCompare>
const Data * ZenDataWithVersion<Key, Data, KeyCompare>::get_data(const Key &key) const {
    for (typename std::list<CNode>::const_iterator iter = data_list_.begin();
         iter != data_list_.end(); ++iter) {
        if (KeyCompare()(iter->key, key)) {
            return iter->data;
        }
    }
    return NULL;
}

template <typename Key, typename Data, typename KeyCompare>
const Data * ZenDataWithVersion<Key, Data, KeyCompare>::get_newest_data() const {
    if (data_list_.empty()) {
        return NULL;
    }
    return data_list_.front().data;
}

template <typename Key, typename Data, typename KeyCompare>
const Key * ZenDataWithVersion<Key, Data, KeyCompare>::get_newest_key() const {
    if (data_list_.empty()) {
        return NULL;
    }
    return &data_list_.front().key;
}

template <typename Key, typename Data, typename KeyCompare>
bool ZenDataWithVersion<Key, Data, KeyCompare>::add_new_data(const Key &key, std::auto_ptr<Data> &obj) {
    if (get_data(key) != NULL) {
        return false;
    }
    CNode node;
    node.key = key;
    node.data = obj.release();
    data_list_.push_front(node);
    if (data_list_.size() > max_reserve_version_) {
        delete data_list_.back().data;
        data_list_.pop_back();
    }
    return true;
}

template <typename Key, typename Data, typename KeyCompare>
bool ZenDataWithVersion<Key, Data, KeyCompare>::add_new_data_cover_of(const Key &key, std::auto_ptr<Data> &obj) {
    for (typename std::list<CNode>::iterator iter = data_list_.begin();
         iter != data_list_.end(); ++iter) {
        if (KeyCompare()(iter->key, key)) {
            delete iter->data;
            iter->data = obj.release();
            return true;
        }
    }
    return add_new_data(key, obj);
}

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_DATA_WITH_VERSION_H_H_
