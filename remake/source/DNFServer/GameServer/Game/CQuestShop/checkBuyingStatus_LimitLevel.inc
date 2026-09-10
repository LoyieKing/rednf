bool CQuestShop::checkBuyingStatus_LimitLevel(unsigned char status, short count,
                                              int characLevel)
{
    // ORIG 0x85ef0ce：m_map0 局部 map 拷贝 + 非 const find/end；
    // value（+0x4 = second）直接按顶层镜像 stQuestShopStatus 调 getMaxCount。
    std::map<int, stQuestShopStatus> table = reinterpret_cast<const std::map<int,
        stQuestShopStatus>&>(G_CDataManager()->m_questShop.m_map0);
    std::map<int, stQuestShopStatus>::iterator it =
        table.find(static_cast<int>(status));     // b28c-L10：同 findStatusQp（ORIG 0x85ef0ce table=-0x2c/临时=-0x14）
    if (it != table.end())                        // ORIG ne + je 尾部 false 块
    {
        const int maxCount = it->second.getMaxCount(characLevel); // +0x4 value + getMaxCount
        if (static_cast<int>(count) > maxCount)   // +0xc0 cmp/jle
            return false;
        return true;                              // +0xaa
    }
    return false;                                 // +0xb3: 未找到 → false
