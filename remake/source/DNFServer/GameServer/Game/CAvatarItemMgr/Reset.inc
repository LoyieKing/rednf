void CAvatarItemMgr::Reset()
{
    m_tempKeyCounter = 1;
    if (m_registeredItems.empty() && m_waitAssignItems.empty())
        return;

    // [b28d-solo-CAvatarItemMgr] 判词：NEAR（15 差，全为槽号/reloc 显示）＝
    // ORIG 具名迭代器分区 it1@-0x24/endIt2@-0x20，4.4 按首次使用序给
    // it1@-0x20/endIt2@-0x24（互换）；4.4 恒先求 end 再 begin，声明序/块作用域
    // 穷举不可翻 ⇒ N93/N82附 槽向封顶。p167 CLEAR 25/25。
    // [b28d-38] P167：ORIG 用 _Rb_tree_const_iterator（转换构造×4/op->×3/op++×2/
    // op!=×2）⇒ 两侧均先转 const_iterator（混合比较会生成 operator!=<PK,P> 变体）。
    std::map<int, Avatar_Item*>::const_iterator it = m_registeredItems.begin();
    std::map<int, Avatar_Item*>::const_iterator endIt = m_registeredItems.end();
    for (; it != endIt; ++it)
    {
        // ORIG 无 null 检查、无 item 具名局部（每点重发 operator->）。
        it->second->init();
        m_AvatarItemPool.Free(it->second);
    }
    std::map<int, Avatar_Item*>::const_iterator it2 = m_waitAssignItems.begin();
    std::map<int, Avatar_Item*>::const_iterator endIt2 = m_waitAssignItems.end();
    for (; it2 != endIt2; ++it2)
    {
        m_AvatarItemPool.Free(it2->second);
    }
    m_registeredItems.clear();
    m_waitAssignItems.clear();
    m_emblemInfo.init();
    m_emblemEnduranceMap.clear();
    m_enduranceUpdateMap.clear();
