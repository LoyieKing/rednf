bool ScriptLoader::FindIpgNo(unsigned int ipgNo)
{
    // ORIG 0x8abf70a（Ghidra C）：`return it == end() ? 0 : 1`，即 `find() != end()`
    // （未找到=1）。[b28c-4] ORIG 尾为 operator== + test/je + mov$0/$1 常量岛 ⇒
    // 保留分支的 if 形（直接 `!=` 表达式被 4.4 折成 xor;ret，少 4 条）。
    if (m_infoMap.find(ipgNo) == m_infoMap.end())
        return false;
    return true;
