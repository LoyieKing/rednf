// Restored from binary (gunnersvr) + DWARF type info.
// Original path: src/commlib/zenlib/zen_config_property_tree.h
#ifndef SECSVR_SRC_COMMLIB_ZENLIB_ZEN_CONFIG_PROPERTY_TREE_H_H_
#define SECSVR_SRC_COMMLIB_ZENLIB_ZEN_CONFIG_PROPERTY_TREE_H_H_

#include <map>
#include <stddef.h>
#include <stdint.h>
#include <string>

struct Zen_Conf_PropertyTree;
typedef Zen_Conf_PropertyTree PROPERTY_TREE_NODE;

// sizeof = 48
struct Zen_Conf_PropertyTree {
protected:
    std::multimap<std::string, std::string> leaf_node_map_;
    std::multimap<std::string, Zen_Conf_PropertyTree> child_node_map_;
public:
    Zen_Conf_PropertyTree();
    ~Zen_Conf_PropertyTree();
    int get_child(const std::string &path_str, const PROPERTY_TREE_NODE *&const_child_data) const;
    int get_child(const std::string &path_str, PROPERTY_TREE_NODE *&child_data);
    int put_child(const std::string &path_str, const std::string &new_child_name, const PROPERTY_TREE_NODE &new_child_note);
    int new_child(const std::string &path_str, const std::string &new_child_name);
    int get_leaf_str(const std::string &path_str, const std::string &key_data, std::string &value_data) const;
    int get_leaf_str(const std::string &path_str, const std::string &key_data, char *&str_data, size_t max_str_len) const;
    int get_leaf_int(const std::string &path_str, const std::string &key_data, int32_t &value_int) const;
    int get_leaf_uint(const std::string &path_str, const std::string &key_data, uint32_t &value_uint) const;
    int get_leaf_bool(const std::string &path_str, const std::string &key_data, bool &value_bool) const;
    int put_leaf_str(const std::string &path_str, const std::string &key_data, const std::string &value_data);
    int put_leaf_int(const std::string &path_str, const std::string &key_data, int value_int);
    int put_leaf_bool(const std::string &path_str, const std::string &key_data, bool value_bool);
};

#endif // SECSVR_SRC_COMMLIB_ZENLIB_ZEN_CONFIG_PROPERTY_TREE_H_H_
