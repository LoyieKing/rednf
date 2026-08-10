// Restored from zergsvr binary (reverse-engineered, behavior matched).
// Original source: /data/secci/ci/jenkins/workspace/g3_release_suse32_bugfix_tag296/src/commlib/zenlib/zen_config_property_tree.cpp
// Compiler: GNU C++ 4.1.0 (SUSE Linux)
// zergsvr 与 gunnersvr 的函数体逐字节一致（仅类名 Zen_Conf_PropertyTree -> ZenConfPropertyTree）。

#include "src/commlib/zenlib/zen_predefine.h"
#include "src/commlib/zenlib/zen_config_property_tree.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <strings.h>

ZenConfPropertyTree::ZenConfPropertyTree()
{
}

ZenConfPropertyTree::~ZenConfPropertyTree()
{
}

int ZenConfPropertyTree::get_child(const std::string &path_str, PROPERTY_TREE_NODE *&child_data)
{
    child_data = NULL;
    if (path_str.empty()) {
        child_data = this;
        return 0;
    }

    size_t str_pos = path_str.find("|", 0, 1);
    std::string start_str(path_str, 0, str_pos);

    std::multimap<std::string, ZenConfPropertyTree>::iterator iter_tmp =
        child_node_map_.find(start_str);
    if (iter_tmp == child_node_map_.end())
        return -1;

    PROPERTY_TREE_NODE *child_tree = &(iter_tmp->second);
    if (str_pos == std::string::npos) {
        child_data = child_tree;
        return 0;
    }

    std::string remain_str(path_str, str_pos + 1, std::string::npos);
    return child_tree->get_child(remain_str, child_data);
}

int ZenConfPropertyTree::get_child(const std::string &path_str, const PROPERTY_TREE_NODE *&const_child_data) const
{
    const_child_data = NULL;
    if (path_str.empty()) {
        const_child_data = this;
        return 0;
    }

    size_t str_pos = path_str.find("|", 0, 1);
    std::string start_str(path_str, 0, str_pos);

    std::multimap<std::string, ZenConfPropertyTree>::const_iterator iter_tmp =
        child_node_map_.find(start_str);
    if (iter_tmp == child_node_map_.end())
        return -1;

    const PROPERTY_TREE_NODE *child_tree = &(iter_tmp->second);
    if (str_pos == std::string::npos) {
        const_child_data = child_tree;
        return 0;
    }

    std::string remain_str(path_str, str_pos + 1, std::string::npos);
    return child_tree->get_child(remain_str, const_child_data);
}

int ZenConfPropertyTree::get_leaf_str(const std::string &path_str, const std::string &key_data, std::string &value_data) const
{
    const PROPERTY_TREE_NODE *child_note = NULL;
    int ret = get_child(path_str, child_note);
    if (ret != 0)
        return ret;

    std::multimap<std::string, std::string>::const_iterator iter =
        child_note->leaf_node_map_.find(key_data);
    if (iter == child_note->leaf_node_map_.end())
        return -1;

    value_data.assign(iter->second);
    return 0;
}

int ZenConfPropertyTree::get_leaf_str(const std::string &path_str, const std::string &key_data, char *&str_data, size_t max_str_len) const
{
    std::string value_data;
    int ret = get_leaf_str(path_str, key_data, value_data);
    if (ret != 0)
        return ret;

    strncpy(str_data, value_data.c_str(), max_str_len);
    return 0;
}

int ZenConfPropertyTree::get_leaf_int(const std::string &path_str, const std::string &key_data, int32_t &value_int) const
{
    std::string value_data;
    int ret = get_leaf_str(path_str, key_data, value_data);
    if (ret != 0)
        return ret;

    sscanf(value_data.c_str(), "%d", &value_int);
    return 0;
}

int ZenConfPropertyTree::get_leaf_uint(const std::string &path_str, const std::string &key_data, uint32_t &value_uint) const
{
    std::string value_data;
    int ret = get_leaf_str(path_str, key_data, value_data);
    if (ret != 0)
        return ret;

    sscanf(value_data.c_str(), "%u", &value_uint);
    return 0;
}

int ZenConfPropertyTree::get_leaf_bool(const std::string &path_str, const std::string &key_data, bool &value_bool) const
{
    value_bool = false;
    std::string value_data;
    int ret = get_leaf_str(path_str, key_data, value_data);
    if (ret != 0)
        return ret;

    if (strcasecmp(value_data.c_str(), "TRUE") == 0) {
        value_bool = true;
        return 0;
    }
    if (strtol(value_data.c_str(), NULL, 10) == 1) {
        value_bool = true;
        return 0;
    }
    return 0;
}

int ZenConfPropertyTree::put_child(const std::string &path_str, const std::string &new_child_name, const PROPERTY_TREE_NODE &new_child_note)
{
    PROPERTY_TREE_NODE *child_note = NULL;
    int ret = get_child(path_str, child_note);
    if (ret != 0)
        return ret;

    ZenConfPropertyTree new_note(new_child_note);
    std::multimap<std::string, ZenConfPropertyTree>::iterator iter =
        child_note->child_node_map_.insert(
            std::make_pair(new_child_name, new_note));
    return (iter != child_note->child_node_map_.end()) ? 0 : ret;
}

int ZenConfPropertyTree::new_child(const std::string &path_str, const std::string &new_child_name)
{
    PROPERTY_TREE_NODE null_node;
    return put_child(path_str, new_child_name, null_node);
}

int ZenConfPropertyTree::put_leaf_str(const std::string &path_str, const std::string &key_data, const std::string &value_data)
{
    PROPERTY_TREE_NODE *child_note = NULL;
    int ret = get_child(path_str, child_note);
    if (ret != 0)
        return ret;

    std::multimap<std::string, std::string>::iterator iter =
        child_note->leaf_node_map_.insert(std::make_pair(key_data, value_data));
    return (iter != child_note->leaf_node_map_.end()) ? 0 : ret;
}

int ZenConfPropertyTree::put_leaf_int(const std::string &path_str, const std::string &key_data, int value_int)
{
    const size_t BUF_LEN = 24;
    char str_int[BUF_LEN];
    snprintf(str_int, BUF_LEN, "%d", value_int);
    std::string value_data(str_int);
    return put_leaf_str(path_str, key_data, value_data);
}

int ZenConfPropertyTree::put_leaf_bool(const std::string &path_str, const std::string &key_data, bool value_bool)
{
    if (value_bool)
        return put_leaf_str(path_str, key_data, std::string("TRUE"));
    else
        return put_leaf_str(path_str, key_data, std::string("FALSE"));
}
