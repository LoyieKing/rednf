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
