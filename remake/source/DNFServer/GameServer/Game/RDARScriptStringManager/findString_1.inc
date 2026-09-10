const char* RDARScriptStringManager::findString(int table, const wchar_t* key,
                                                bool* flag) const
{
    // ORIG 0x8aa594c
    // b28d-21: DIFF/N 封顶 — ORIG 0x8aa594c 共 49 条 vs 重建 94 条：ORIG 走
    // 窄串直接路径，重建多出临时 std::string 构造/析构与转换（N86/N89 模板族）。
    // 语义等价（同 key 查表），byte 层 N。
    if (key == 0)
    {
        return "RDARScriptStringManager::ERROR::input NULL string";
    }

    static char errStr[256];
    // ORIG 0x8aa5969：toTString(const wchar_t*) 返回 TLS 缓冲 const char*，
    // 直接作 get 实参（无 std::string 临时；N86/N89 源侧根因）。ORIG 无表范围检查。
    const char* result = dictionaries_[table]->get(toTString(key));
    if (flag != 0)
        *flag = (result != 0);
    if (result != 0)
        return result;

    // ORIG 0x8aa59c9：第 3 变参是原始 wchar_t* key（"%s" 按窄串打印，原码遗留 P171）。
    ss_sprintf(errStr, "%s == NULL, %s : ",
               key, fileList_.GetFileName(table));
    return errStr;
