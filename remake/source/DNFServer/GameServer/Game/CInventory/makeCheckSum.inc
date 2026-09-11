// ORIG 0x0850ae7a 返回类型为 int（尾调用 finishItemChecksum 的 eax 流出；
// mangled 不编码返回类型，符号不变）。CUser::getCurCharacInvenCheckSum 直接
// 返回该值（docs/class_func_reports/CUser/getCurCharacInvenCheckSum.md）。
// [N82 封顶] batch-10：差集 7=7 归零（三分支各 1 处 GetUpgrade 独立物化，循环界改 <=）。
// 残余 |gap|=4 = ORIG movzwl 双段零扩展 vs register 变量 spill 形态差（勿再按 ndiff 迭代）。
int CInventory::makeCheckSum(int a, int b) const
{
    // b28d-solo-CInventory：169/173，ORIG v 零扩展（movzwl %ax,%ebx）跨 GetUpgrade 保活 vs 我方 %esi ⇒ N18a 封顶。
    // b28d-37 [N18a 封顶，ORIG 0x84f?]：movzwl 两段 vs 单段 + 寄存器分配（169/173）。
    int sum = 0;
    // ORIG 三处 GetUpgrade 独立调用点（两循环分支各自物化，勿合并共享）。
    for (int i = 10; i <= 0x15; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            register unsigned short v = (unsigned short)m_invenItem[i].m_fieldb;
            register unsigned char upgrade = m_invenItem[i].GetUpgrade();
            // [b28d-36 复测] P200「短整实参直传」形实测 nd 144→148 反劣 ⇒ 回滚，保留 register v 形
            // （ORIG 双段 movzwl %ax,%ebx 跨 GetUpgrade 保活在此不可造，判 N18a/P200 边界）。
            sum += getItemChecksum(i, m_invenItem[i].m_addInfo, 1, upgrade, v);
        }
    }
    for (int i = 3; i <= 0x137; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            if (m_pEquipSlot[i].m_field1 == 1)
            {
                register unsigned short v = (unsigned short)m_pEquipSlot[i].m_fieldb;
                register unsigned char upgrade = m_pEquipSlot[i].GetUpgrade();
                sum += getItemChecksum(i, m_pEquipSlot[i].m_addInfo, 1, upgrade, v);
            }
            else
            {
                register unsigned short v = (unsigned short)m_pEquipSlot[i].m_fieldb;
                register unsigned char upgrade = m_pEquipSlot[i].GetUpgrade();
                sum += getItemChecksum(i, m_pEquipSlot[i].m_addInfo,
                                       m_pEquipSlot[i].m_addInfo2, upgrade, v);
            }
        }
    }
    return finishItemChecksum(&sum, m_money, m_coin + m_payCoin + m_eventCoin, a, b);
}