int CRandomOptionItemHandle::_get_level_limit(ENUM_RARITY rarity, int level)
{
    // [b28d-solo-RandomRentalA2 判词] AE 74/74（ORIG 0x085f0f9a）：①P0 bound 内联
    // 为 randInt 匿名临时实参（key/it/end/bound 槽全等）；②unsigned r 复现单段
    // `cmp -0xc(%ebp),%eax`；③三目表达式返回 `(level==1)?(level+4):(level+5)`
    // （ORIG 为表达式返回、非赋值写回）。
    RandomApplicationKey key((unsigned char)rarity, (unsigned char)level);
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 每处 map 访问独立调 GetRandomOption
    // （`call; lea 0x18(%eax),%ecx` 直传，无具名 script 槽）。
    std::map<RandomApplicationKey, LevelLimitDecisionValue>::const_iterator it =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map18.find(key);
    if (it != reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_map18.end())
    {
        // [b28d-solo-RandomRentalA2 P0] ORIG bound@-0x10 是 randInt(const unsigned long&)
        // 的匿名临时（`mov %eax,-0x10; lea -0x10,%edx`），非具名局部（具名恒落 -0x20
        // 并挤走 end 临时槽）；内联实参形复现。r 声明 unsigned int 复现
        // ORIG `cmp -0xc(%ebp),%eax` 单段 mem 比较（P209附/P310）。
        unsigned int r = (unsigned int)m_rand[1].randInt(
            (unsigned long)(it->second.m_field0 + it->second.m_field4));
        if ((unsigned int)it->second.m_field0 < r)
        {
            // [b28d-solo-RandomRentalA2] ORIG 0x85f1069 `cmpl $0x1,0x10(%ebp);
            // jne; mov level,%eax; add $0x4,%eax; jmp` = 表达式返回形
            // （level==1 时 level+4），非赋值写回 level 槽（写回形多 1 条且块序反）。
            return (level == 1) ? (level + 4) : (level + 5);
        }
    }
    return level;
}