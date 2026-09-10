int DisPatcher_DieCharacInDungeon::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    MSG_DIE_CHARACTER& req = *reinterpret_cast<MSG_DIE_CHARACTER*>(&msg);
    stDieCharacInDungeon& out = static_cast<stDieCharacInDungeon&>(param);
    out.m_errNo = check_error(user, msg);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0xcbb, __PRETTY_FUNCTION__, out.m_errNo, user->get_acc_id());
    }

    // ORIG 读包体 ushort：+0xf 先入 esi、+0xd 后入 ebx，作 CharacDie 第 2/3 参
    const unsigned short fieldF =
        *reinterpret_cast<const unsigned short*>(req.m_pad + 2);   // +0xf
    const unsigned short fieldD =
        *reinterpret_cast<const unsigned short*>(req.m_pad);       // +0xd
    user->m_historyLog.CharacDie(user->getCurCharacName(), fieldD, fieldF);
    // ORIG thunk：0x822fe4e=GetPartyIndex、0x80cc18e=G_CGameManager、0x8294f32=GetParty(int)
    CParty* partyByIdx = G_CGameManager()->GetParty(user->GetPartyIndex());

    if (!Global::CBossTowerFunc::DieCharacInDungeon(user))
    {
        if (user->get_state() == 0xd)
        {
            advancealtar::Manager::dieCharac(user);
        }

        CParty* myParty = static_cast<CParty*>(user->GetParty());
        CPartyDungeonView* view =
            reinterpret_cast<CPartyDungeonView*>(myParty);
        if (myParty == 0 || view->m_dungeon == 0 ||
            static_cast<signed char>(view->m_dungeon->m_bloodDungeonType) < 1)
        {
            if (partyByIdx != 0)
            {
                partyByIdx->die_user(user);
            }
        }
        else
        {
            myParty->BloodHandleDieUser(user);
            out.m_errNo = 0x7fffffff;
        }
    }
    return 0;
