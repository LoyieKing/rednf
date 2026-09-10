int Inter_AvatarChangeStat::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)
        return 0;

    Inter_AvatarChangeStatPktView* pPkt = reinterpret_cast<Inter_AvatarChangeStatPktView*>(data);
    if (user->getCurCharacInvenW()->GetAvatarItemMgrR()->GetExpireDate(pPkt->m_0) == -1)
    {
        user->getCurCharacInvenW()->GetAvatarItemMgrW()->RegistItem(
            pPkt->m_0,
            pPkt->m_e,
            pPkt->m_a,
            reinterpret_cast<char*>(pPkt) + 0x12,
            *reinterpret_cast<const stAvatarEmblemInfo_t*>(reinterpret_cast<char*>(pPkt) + 0x2f),
            *reinterpret_cast<const stAvatarExpansionInfo_t*>(reinterpret_cast<char*>(pPkt) + 0x4d),
            false);
    }
    user->SendUpdateItemList(
        (CUser::eSendTarget)1, (ENUM_ITEMSPACE)1,
        (int)pPkt->m_8 - 10);
    return 0;
