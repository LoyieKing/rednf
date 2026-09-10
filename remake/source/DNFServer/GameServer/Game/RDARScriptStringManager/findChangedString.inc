const char* RDARScriptStringManager::findChangedString(int table, const char* key,
                                                       bool* flag)
{
    // ORIG 0x8aa6448：函数级 static std::string ret（__cxa_guard 单次初始化），
    // 首次调用以 findString 结果填充，随后 changeSpacilTagStr 并返回 c_str。
    static std::string ret = findString(table, key, flag);
    changeSpacilTagStr(ret);
    return ret.c_str();
