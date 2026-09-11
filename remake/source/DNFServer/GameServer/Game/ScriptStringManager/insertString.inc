// insertString @ 0x8acf720：字符串表 intern。已存在（map 树中找到，
// 8acf7f5 后 8acf802 分支）→ 返回既有 index；否则 push 到 strings_
// （8acf8f2 _M_insert_aux / 8acf85c 原地拷贝构造）→ map _M_insert_unique
// 插入 {string, 新 index}（8acf8b7）→ 返回新 index（8acf891 计算，
// (finish-start)>>2 - 1）。ORIG"已存在"路径返回树节点内字段（无意义
// 值），本实现统一按 intern 语义返回 index。
// b28d-solo-ScriptStringManager 判词：DIFF 封顶 **N89**（259 vs 196 条；
// p167: 2 MISSING 右值模板实例 + 4 EXTRA）。ORIG 3×`std::string(string)` 临时 +
// out-of-line `~string`（_ZNSsD1Ev×4）；4.4.7 头在 -O2 把 `~basic_string` 内联
// 展开（4× `_M_destroy` + pthread_cancel 段）净 +~60 条。MISSING
// `_M_insert_auxIISsEE…DpOT_`（C++0x 右值版）来自 ORIG 归档 4.4 libstdc++；
// gnu++0x 实测更差（282 条 / TU diff 12 vs 10）⇒ 不可翻。
int ScriptStringManager::insertString(const char* string)
{
    // ORIG 0x08acf720：**两次**独立构造临时 std::string(string)（8acf73d find 用、
    // 8acf89b insert 用），无 string==NULL 护栏（P171）；index 在 push_back
    // **之后**按 (finish-start)>>2 - 1 计算（8acf891）。
    std::map<std::string, int>::const_iterator it =
        string_map_.find(std::string(string));      // 8acf73d
    if (it != string_map_.end())
        return it->second;                          // 8acf807
    strings_.push_back(std::string(string));        // 8acf85c 原地 / 8acf8f2 扩容
    const int index = static_cast<int>(strings_.size()) - 1;  // 8acf891
    string_map_.insert(
        std::map<std::string, int>::value_type(std::string(string), index));  // 8acf8b7
    return index;
}