// [b28d-solo-BlueMarble AE] P251/P261：if-else 链（带 -0xc grade 槽 + 逐次重载
// 比较）改 `switch (getGrade())` 后 4.4 复现 ORIG 决策树（cmp1/je、cmp1/jg、
// test/je、cmp2、cmp3）与 case 块序 0,1,2,3，104=104 ndiff 0。ORIG 0x080d7e8e
int BlueMarble::setResultItem(int seat, Inven_Item& item)
{
    // ORIG 0x080d7eb5-：每处 `(*rewards)[g]` 均从 this+0x38+0xa4 重新算
    // 基址再 call vector::operator[]（4 组 5 条装载链，无 rewards 指针
    // 局部缓存——旧缓存形每位省 2 条即 we-short 主因，P172 去缓存）。
    // ORIG 决策树 `cmp$1; je; cmp$1; jg; test; je; jmp` + case 块序 0,1,2,3
    // 且 getGrade() 结果直用 %eax（无 grade 局部槽）⇒ 原码是 switch
    // （P251/P261；if-else 链会带 -0xc 状态槽并逐次重载比较）。
    switch (m_users[seat].getGrade())
    {
    case 0:
        item.m_addInfo = m_pScript->m_info.m_rewardList[0].m_a;
        item.m_addInfo2 = m_pScript->m_info.m_rewardList[0].m_b;
        break;
    case 1:
        item.m_addInfo = m_pScript->m_info.m_rewardList[1].m_a;
        item.m_addInfo2 = m_pScript->m_info.m_rewardList[1].m_b;
        break;
    case 2:
        item.m_addInfo = m_pScript->m_info.m_rewardList[2].m_a;
        item.m_addInfo2 = m_pScript->m_info.m_rewardList[2].m_b;
        break;
    case 3:
        item.m_addInfo = m_pScript->m_info.m_rewardList[3].m_a;
        item.m_addInfo2 = m_pScript->m_info.m_rewardList[3].m_b;
        break;
    }
}