void CAvatarItemMgr::SaveEndurance()
{
    if (m_enduranceUpdateMap.empty())
        return;
    // [b28d-38] ORIG begin@-0xc/end@-0x10；4.4 对两迭代器 sret 恒 begin 结果深、
    // end 结果浅（与声明序无关，N93/N82附 聚合对象槽分区）⇒ 槽号互换 NEAR 封顶。
    std::map<int, int>::iterator it = m_enduranceUpdateMap.begin();
    std::map<int, int>::iterator endIt = m_enduranceUpdateMap.end();
    for (; it != endIt; ++it)
    {
        AvatarRechargeServer::SaveAvatarEnduranceUnit(it->first, it->second);
    }
    m_enduranceUpdateMap.clear();
}