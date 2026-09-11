// stFavorableRelationShip_t::clear —— ORIG 0x089fb56c T（忠实转写）：
//   标量区清零（word@0 / dword@4 / word@8,0xa,0xc）
//   → moodChanges(+0x10)/keyItems(+0x1c) 两个 vector clear
//   → 四张礼物表 ×5 个 map（stride 0x18）clear
//   → favorRate×3 置 1.0f（0x3f800000，+0x4d8/4dc/4e0）
//   → m_rewardItems(+0x208).clear()。
// [b28d-solo-NPCScript] 同族 `_ZN25stFavorableRelationShip_tD1Ev`（清单旧记 NEAR 2）
// 现与 ORIG **AE**：ORIG 存在用户析构符号（非 trivial，P39/P317 反向）⇒ 类内
// 用户空析构保留不动，隐式体逐条对齐（无源码可改）。
void stFavorableRelationShip_t::clear()
{
    // b28d-D33（P212 全头部声明复现块级槽序）：ORIG 两个循环变量槽
    // （map-clear 环 i @-0x10 深、favorRate 环 j @-0xc 浅）⇒ 头部按 i、j 序声明
    // 且两循环均复用头部变量（旧形块内重声明 i 使 4.4 按首次使用序放 -0xc）。
    int i;
    int j;
    m_enabled = false;
    m_defaultFavor = 0;
    m_maxGiftPerDay = 0;
    m_dialogDayGap1 = 0;
    m_dialogDayGap2 = 0;
    m_fav.m_moodChanges.clear();
    m_fav.m_keyItems.clear();
    // b28c-L8：ORIG 0x089fb5b9 只有**一个**循环变量槽（-0x10）、循环体单轮发
    // 4 次 map clear（基址 +0x28/+0x118/+0xa0/+0x190，步长 i*0x18），共 5 轮。
    // 旧双层形多做 20 倍 clear（幂等故行为不显，但多 4 条外层循环控制且 ndiff
    // 无法收敛）⇒ 收敛为单层。
    for (i = 0; i < 5; ++i)
    {
        m_preferItems[i].clear();
        m_unpreferItems[i].clear();
        m_preferItemGroups[i].clear();
        m_unpreferItemGroups[i].clear();
    }
    // ORIG 0x089fb649：三档 favorRate 以 `for (j=0;j<3;++j)` 循环写常量
    // 1.0f（`mov $0x3f800000,%edx` 内联立即数、非内存读、非清零）。
    for (j = 0; j < 3; ++j)
    {
        m_favorRate[j] = 1.0f;
    }
    m_rewardItems.clear();
}