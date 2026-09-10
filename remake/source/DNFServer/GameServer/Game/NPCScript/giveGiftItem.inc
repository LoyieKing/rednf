unsigned short CNPCScript::giveGiftItem(unsigned int mood, CItem const* item, unsigned int count,
                                        ENUM_NPC_FAVOR_GIFT_RESULT_CONST& result) const
{
    // [b28d-solo-NPCScript] P309 槽序：4.4 对函数头声明按「先声明浅」分配
    // （ORIG 浅→深：gn2 -0x10 / key2 -0x18 / gn1 -0x20 / key1 -0x28 /
    // give -0x2a / it -0x30）⇒ 块内局部全部提至函数头、声明序按 ORIG 槽浅→深；
    // key 用声明+赋值分离以保持 ORIG 执行序 give=0 → it 构造 → get_index。
    // [round5 复测] 两声明序实测均 NEAR（N82附：4.1 按生命周期**穿插** vs 4.4
    // 分区/首次使用）：序A（本形 gn2/key2/gn1/key）⇒ key -0x28 ✓ / gn1 -0x18 /
    // key2 -0x24 / gn2 -0x14；序B（key/gn1/key2/gn2）⇒ key -0x1c / gn1 -0x20 ✓ /
    // key2 -0x24 / gn2 -0x28。ORIG 相邻槽距恒 8（-0x28→-0x20→-0x18→-0x10）=
    // 块内迭代器/临时对象穿插所致，非源码声明序可择 ⇒ 回退序 A。
    unsigned long groupName2;
    unsigned long key2;
    unsigned long groupName1;
    unsigned long key;
    short give = 0;
    std::map<unsigned long, stFavorableRelationShip_t::stGiftItemInfo_t>::const_iterator it;
    key = item->get_index();
    it = m_npcScript.m_favorableRelationShip.m_preferItems[mood].find(key);
    if (it != m_npcScript.m_favorableRelationShip.m_preferItems[mood].end())
    {
        if (static_cast<unsigned short>(it->second.m_b) <= count)
        {
            give = it->second.m_c;
        }
        else
        {
            result = ENUM_NPC_FAVOR_GIFT_RESULT_FAIL;
        }
    }
    // [b28d-solo-NPCScript] ORIG 0x085809e4：groupName 为**块内**局部且
    // getItemGroupName() 调两次（条件一次 + 块内取值一次；Ghidra C 同形），
    // 四个块各占独立槽（-0x20/-0x1c/-0x14/-0x10）。后半段是 `if (give == 0)`
    // 包裹（ORIG +0x1c6 `jne <尾返回>`），非早返回。
    if (item->getItemGroupName() != 0)
    {
        groupName1 = static_cast<unsigned long>(item->getItemGroupName());
        it = m_npcScript.m_favorableRelationShip.m_preferItemGroups[mood].find(groupName1);
        if (it != m_npcScript.m_favorableRelationShip.m_preferItemGroups[mood].end())
        {
            if (static_cast<unsigned short>(it->second.m_b) <= count)
            {
                if (it->second.m_c > give)
                {
                    give = it->second.m_c;
                }
            }
            else
            {
                result = ENUM_NPC_FAVOR_GIFT_RESULT_FAIL;
            }
        }
    }
    if (give == 0)
    {
        key2 = item->get_index();
        it = m_npcScript.m_favorableRelationShip.m_unpreferItems[mood].find(key2);
        if (it != m_npcScript.m_favorableRelationShip.m_unpreferItems[mood].end())
        {
            give = it->second.m_c;
        }
        if (item->getItemGroupName() != 0)
        {
            groupName2 = static_cast<unsigned long>(item->getItemGroupName());
            it = m_npcScript.m_favorableRelationShip.m_unpreferItemGroups[mood].find(groupName2);
            if (it != m_npcScript.m_favorableRelationShip.m_unpreferItemGroups[mood].end())
            {
                // ORIG +0x304 `test %ax,%ax; je <m_c 块>`（m_c 块在 max 块之后）
                // ⇒ 源形取 `give != 0 → max / give == 0 → m_c`（P222 极性）。
                if (give != 0)
                {
                    give = std::max<short>(give, it->second.m_c);
                }
                else
                {
                    give = it->second.m_c;
                }
            }
        }
    }
    return give;
