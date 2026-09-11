// ORIG processVendingMachineItemLottery(CMTRand&, info const*, Inven_Item&)
// 0x084b0550：在 [0, m_giveCount-1] 掷点，扫 m_vecGive 取首个累计权重
// 大于掷点的输出，把 itemId/m_count 写入物品 +0x02/+0x07。
// b28d-solo-GmCmds_E：62=62 AE——stVendingMachineOutput_t::m_itemId 改
// unsigned int（CDataManager.h，P211 字段侧同型化，探针 /tmp/probe_fields.cpp）
// 后 `item.m_addInfo = it->m_itemId` 复现 ORIG 单段 `mov (%eax),%edx`；
// 原 N18a/N89 判词作废。
void CRandomObjectTest::processVendingMachineItemLottery(CMTRand& rng,
                                                         const stVendingMachineInfo_t* info,
                                                         Inven_Item& item)
{
    unsigned long range = static_cast<unsigned long>(info->m_giveCount - 1);
    // b28 BM3：roll 槽按 ORIG -0xc unsigned 32 位直存；比较两侧同为 unsigned
    // 时 4.4 折叠 `cmp -0xc(%ebp),%eax`（探针 /tmp/bm3/p18 fE vs fF：int 槽+双侧
    // cast 恒生三段 mov %eax,%edx;mov mem,%eax;cmp）。
    unsigned roll = rng.randInt(range);

    for (std::vector<stVendingMachineOutput_t>::const_iterator it =
             info->m_vecGive.begin();
         it != info->m_vecGive.end(); ++it)
    {
        if (static_cast<unsigned int>(it->m_cumWeight) > roll)
        {
            item.m_addInfo = static_cast<unsigned int>(it->m_itemId);
            item.m_addInfo2 = static_cast<int>(it->m_count);
            return;
        }
    }
}