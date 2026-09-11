// ---------------------------------------------------------------------------
// CBattle_Field::CBloodClearRewardData::getUltimateRewardItem()
//   —— ORIG T 0x083070ca：按 m_ultimateClearPoint 查 CDataManager
//      m_bloodDungeon.m_map358 概率表；rand<field4 → 0x4e4，
//      field4<=rand<field0+field4 → 0x4e3，否则 -1。
// ---------------------------------------------------------------------------
int CBattle_Field::CBloodClearRewardData::getUltimateRewardItem()
{
    // [b28d-solo-CBattle_Field_missing] ORIG 0x083070ca：
    // ①声明序 it(-0x20)/itEnd(-0x1c)/clearPoint(-0x18)/f4(-0x14)/f8(-0x10)
    //   （4.1 先声明深；语句序 clearPoint→f4=0→f8=0→itEnd=end()→it=find()）；
    // ②非 const iterator（ORIG `_ZNKSt17_Rb_tree_iterator…eq` 非 const_iterator，P96）；
    // ③P157 两次独立 operator->() 取 second.m_field4/second.m_field8，禁引用缓存。
    int clearPoint = getUltimateRoundClearPoint();
    int f0 = 0;
    int f4 = 0;
    std::map<int, STUltimateRewardProb>::iterator itEnd =
        G_CDataManager()->m_bloodDungeon.m_map358.end();
    std::map<int, STUltimateRewardProb>::iterator it =
        G_CDataManager()->m_bloodDungeon.m_map358.find(clearPoint);
    if (it == itEnd)
        return -1;
    int roll = get_rand_int(100);
    f0 = it->second.m_field0;
    f4 = it->second.m_field4;
    // [b28d-solo-CBattle_Field_missing] ORIG 布局：`jg then(在后)` + else 直落
    // ⇒ 否定早返形（P222/P59）。
    if (roll <= f4)
        return 0x4e4;
    f0 += f4;
    if (roll <= f0)
        return 0x4e3;
    return -1;
}