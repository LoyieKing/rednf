unsigned int DisPatcher_DropItem::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    const MSG_DROP_ITEM& req = *reinterpret_cast<const MSG_DROP_ITEM*>(&msg);
    stDropItemParam& out = static_cast<stDropItemParam&>(param);
    int deadLocal;               // ORIG -0x14 槽（+0x18b 写 6 未再读，帧形状保留）
    if (user->getCurCharacR() == 0)
    {
        return -1;
    }
    if (req.m_14 < 1)
    {
        return -1;
    }
    if (user->get_state() == 10)
    {
        return 0x13;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party != 0 && party->m_battleField.m_pDungeon != 0 &&
        party->m_battleField.m_pDungeon->m_bloodDungeonType != 0)
    {
        return 0x13;
    }
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        return 0x13;
    }
    if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
            user, (RestrictCategory::Enum)1, 1))
    {
        out.m_c = 1;
        return 0xd1;
    }
    if ((unsigned char)req.m_padTail[0] != 1)
    {
        int secu = GlobalData::s_pSecuProtectionField->Check(
            user, static_cast<SECURITY_PROTCTION>(0x1e));
        if (secu != 0)
        {
            out.m_c = 1;
            return secu;
        }
    }
    if (user->CheckItemLock(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)(char)req.m_11), (int)req.m_12))
    {
        return 0xd5;
    }
    deadLocal = 6;               // ORIG +0x18b（未再读）
    if (user->isHackUser())
    {
        return 0x13;
    }
    if ((int)(char)req.m_11 == 1)
    {
        return 0x17;
    }
    if ((int)(char)req.m_11 == 3 && req.m_12 <= 9)
    {
        return 0x17;
    }
    if ((unsigned char)req.m_11 == 0 && (unsigned short)req.m_12 == 0 &&
        (unsigned int)req.m_14 > 0x3e8)
    {
        return 0x46;
    }
    return 0;
