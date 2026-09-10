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
