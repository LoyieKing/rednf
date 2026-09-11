// ============================================================================
// Arad_GiftItem_Set::find（ORIG 0x081a311e，_ZN17Arad_GiftItem_Set4findEiii）
// 语义：按 64 位 key = ((long long)kind << 32) | (((long long)job << 16) + usableLevel)
// 查 this+0x04 的 map<long long, Data>，命中返回 &it->second，否则 0。
// ============================================================================
Arad_GiftItem_Set::Data* Arad_GiftItem_Set::find(int kind, int job, int usableLevel)
{
    // ORIG：64bit 进位链 + 形（与 open 同构，| 会改 codegen）
    long long key =
        ((long long)kind << 32) +
        (long long)((unsigned short)job << 16) +
        (long long)usableLevel;
    std::map<long long, Data>::iterator it = m_map.find(key);
    // [b28c-L18] P222 正向包裹（两形互试）：ORIG 块序 = hit 紧跟条件 fall-through、
    // `mov $0` 尾岛直落 epilogue；负极性 `if(it==end)return 0;` 在 4.4 恒产反序岛。
    if (it != m_map.end())
        return &it->second;
    return 0;
}