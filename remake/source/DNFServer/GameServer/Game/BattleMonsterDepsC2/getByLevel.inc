// ORIG 0x0834aa60 _ZN16CAICharacterList10getByLevelEj（桥接签名 static (void*, uint)）：
// idx=get_rand_int(10)+level-5；idx==0 或 (uint)idx>0x62 → 0；
// m_byLevelList[idx] 空 → 0；否则 at(get_rand_int(size()))。
// ORIG 0x0834aa60 _ZN16CAICharacterList10getByLevelEj（成员 this）：
// idx=get_rand_int(10)+level-5；idx==0 或 (uint)idx>0x62 → 0；
// m_byLevelList[idx] 空 → 0；否则 at(get_rand_int(size()))。
CAICharacter* CAICharacterList::getByLevel(unsigned int level)
{
    // P0/P209：ORIG -0x10 槽为 unsigned（`cmpl $0x62,-0x10` 直读内存 + jbe），
    // 且 `add 0xc(%ebp),%eax` 无装载中转 ⇒ 表达式不带 int cast。
    // P183/P48：empty 判定的地址全量物化一次（不入槽），判定后再物化并存
    // -0xc 槽供 size/at 两次复用（ORIG 0x0834aae5/0x0834aae8 实证回读）。
    unsigned int idx = static_cast<unsigned int>(get_rand_int(10)) + level - 5;
    if (idx == 0 || idx > 0x62)
        return 0;
    if (m_byLevelList[idx].empty())
        return 0;
    // ORIG 第二次物化走 `lea 0x18(%eax),%ecx; lea (%ecx,%eax,1)`（数组基址先算）
    // ⇒ 用数组名退化的指针算术（`arr + idx`）而非下标形。
    std::vector<CAICharacter*>* bucket = m_byLevelList + idx;
    return bucket->at(get_rand_int(static_cast<int>(bucket->size())));
}