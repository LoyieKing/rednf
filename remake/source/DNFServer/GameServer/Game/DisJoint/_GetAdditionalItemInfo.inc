// ============================================================================
// DisJoint::_GetAdditionalItemInfo @ 0x08473224
//   m_additional[rarity]（+0x5c8+rarity*0x10）m_count==0 → 0；
//   否则 idx = get_rand_int(m_count*1000)/1000，返回 m_items[idx]。
//   （旧裸偏移转写迁出 ItemUpgradeVtableB2.cpp，改按 DisJoint.h 真实成员访问；
//   保留 ORIG 的 result(-0x10)/bound(-0xc) 槽结构，pick 内联进 operator[]。）
// ============================================================================
int DisJoint::_GetAdditionalItemInfo(int rarity)
{
    if (m_additional[rarity].m_count == 0)
    {
        return 0;
    }
    int result = 0;
    const int bound = m_additional[rarity].m_count * 1000;
    result = m_additional[rarity]
        .m_items[get_rand_int(bound) / 1000];
    return result;
}