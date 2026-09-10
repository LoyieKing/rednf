const std::string* ScriptStringManager::getString(int index) const
{
    // [b28c-21] ORIG 8acf1f0：正极性嵌套 if（js 负索引 → 尾部 DEFAULT），
    // `||` 合并形 4.4 生 setne 物化 + 3 callee-saved（81 vs 45 insn）。
    static const std::string DEFAULT_STRING("");  // ORIG DEFAULT_STRING
    const std::string* result = &DEFAULT_STRING;
    if (index >= 0 && index < static_cast<int>(strings_.size()))
        result = &strings_[index];
    return result;
