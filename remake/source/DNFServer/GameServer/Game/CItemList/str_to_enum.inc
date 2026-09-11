// ============================================================================
// Arad_GiftItem_Set::str_to_enum（ORIG 0x081a2e3e，_ZN17Arad_GiftItem_Set11str_to_enumEPKc）
// 语义：在 this+0x1c 的 map<string,int> 中按 name 查找，命中返回 second，否则 0。
// ============================================================================
int Arad_GiftItem_Set::str_to_enum(const char* name)
{
    // [b28c-L18] P140：ORIG 0x081a2e3e 入口直构 string(name)，无 null 防御（P171
    // 照抄原则）。ORIG 尾段实证调用符号为 iterator::operator!=、this=end 临时
    // ((esp)=-0xc)、实参 it(0x4(esp))，true→hit fall-through ⇒ 原码
    // `if (end() != it) return it->second; return 0;`（P222 正向包裹）。
    std::map<std::string, int>::iterator it = m_kindMap.find(std::string(name));
    if (m_kindMap.end() != it)
        return it->second;
    return 0;
}