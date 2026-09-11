// ---------------------------------------------------------------------------
// _TryEnchantOther（报告 _TryEnchantOther.md，ORIG T 0x849da3e sz 0x285）
// ---------------------------------------------------------------------------
// b28c-1 N7: _TryEnchantOther 虚调用 callee %eax↔%edx 互换。
int CEnchanter::_TryEnchantOther(CUser* user, stItemPos_t* pos)
{
    (void)pos;   // ORIG 形参 pos 未被函数体消费（Ghidra 同）

    if (m_materials[0].Empty())
        return 0x15;

    if (IsRecipeRight() != 1)
        return 0x11;

    if (IsLearnRecipe(user,
                      static_cast<CStackableItem const*>(
                          reinterpret_cast<CStackableItem*>(m_field40))) != true)
        return 0x11;

    if (m_materials[1].Empty())
        return 0x14;

    m_field3c = m_materials[1].m_item;
    if (reinterpret_cast<CItem*>(m_field3c) == 0)
        return 0x11;

    if (IsEqualMonsterCad() != 1)
        return 0x11;

    if (!::EnableWorld(reinterpret_cast<CItem*>(m_materials[0].m_item),
                     reinterpret_cast<CItem*>(m_field40)))
        return 0x11;

    // 与 _TryEnchantMine 相同的双虚槽链（getUsableItemType(GetItemType())）。
    if (!reinterpret_cast<CItem*>(m_field3c)->getUsableItemType(
            reinterpret_cast<CItem*>(m_materials[0].m_item)->GetItemType()))
        return 0x17;

    int variation = 0;
    stCompoundRate* rate = GetSuccessRate(user,
                                          static_cast<CStackableItem const*>(
                                              reinterpret_cast<CStackableItem*>(m_field40)),
                                          variation);
    if (rate == 0)
        return 0x17;

    int percent = rate->m_successRate + variation;
    if (percent > 100)
        percent = 100;

    if (m_peer != 0 && m_peer->getCurCharacR() != 0)
    {
        PacketGuard packet;
        packet.put_header(0, 0x9c);
        packet.put_byte(1);
        packet.put_short(percent);
        packet.put_byte(1);
        packet.put_int(reinterpret_cast<CItem*>(m_field40)->get_index());
        packet.finalize(true);
        m_peer->Send(packet);
    }
    return 0;
}