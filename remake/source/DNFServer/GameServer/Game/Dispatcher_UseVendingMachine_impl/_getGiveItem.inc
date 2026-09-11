// ORIG T 0x0821bf82 _ZNK28Dispatcher_UseVendingMachine12_getGiveItemEPK22stVendingMachineInfo_t
//   依 m_giveCount 权重（范围 = m_giveCount-1）经 this->m_rand 随机选 m_vecGive 中
//   一个输出；命中写 m_itemId → m_addInfo(+0x02)、m_count → m_addInfo2(+0x07)。
//   按值返回 Inven_Item（sret，0x3d 字节；ORIG 调用处逐字段拷贝实证）。
// b28d-solo-Dispatcher_UseVendingMachine_impl [AE]：删除 range<0 防御（ORIG 无）、
//   randInt 直传具名 range（去中间 r）、迭代器循环（operator-> 每点重调）。
Inven_Item Dispatcher_UseVendingMachine::_getGiveItem(
    const stVendingMachineInfo_t* info) const
{
    Inven_Item out;               // ORIG：Inven_Item::Inven_Item(sret)
    out.m_addInfo = -1;           // 初始 idx = -1（“未选择”标记）
    unsigned long range = info->m_giveCount - 1;
    unsigned long pick = m_rand->randInt(range);
    for (std::vector<stVendingMachineOutput_t>::const_iterator it = info->m_vecGive.begin(),
             e = info->m_vecGive.end(); it != e; ++it)
    {
        if (pick < it->m_cumWeight)
        {
            out.m_addInfo = it->m_itemId;
            out.m_addInfo2 = (int)it->m_count;
            return out;
        }
    }
    return out;
}