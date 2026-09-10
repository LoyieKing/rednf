bool CAvatarItemMgr::GetItem(AvatarDetailInfo* info, int* count) const
{
    // [b28d-solo-CAvatarItemMgr] P13/P191：end 迭代器具名缓存（ORIG end() 在循环外
    // 单次求值），i 双初始化（`int i=0;` + `for(i=0;…)` 两条 movl $0 都要写）。
    std::map<int, Avatar_Item*>::const_iterator it = m_registeredItems.begin();
    std::map<int, Avatar_Item*>::const_iterator endIt = m_registeredItems.end();
    int i = 0;
    for (i = 0; it != endIt && i <= 0x72; ++it, ++i)
    {
        // [b28d-38] P157/P167：ORIG 4 次 operator->（每点重发 it->second），无 item 缓存。
        info[i].m_key = it->first;
        info[i].m_expireDate = it->second->GetExpireDate();
        info[i].m_registTime = it->second->GetRegistTime();
        strncpy(info[i].m_ipgAgencyNo, it->second->GetIPGAgencyNo(), 0x19);
        // [b28d-solo-CAvatarItemMgr] P0/P167：ORIG 无 jewelData 具名槽（值寄存器直传）
        // ⇒ 内联表达式；结构体直接赋值（逐字段展开，无 memcpy call）。
        info[i].m_jewelSocket =
            *(const stAvatarEmblemInfo_t*)it->second->getJewelSocketData();
        info[i].m_expansionInfo =
            *(const stAvatarExpansionInfo_t*)it->second->getExnansionInfoRef();
    }
    *count = i;
    return 1;
