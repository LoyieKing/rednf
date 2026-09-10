int DisPatcher_DieCharacInDungeon::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [G5-DisPatcherCore] P0/P183：ORIG 0x081c2a1d 唯一具名引用是 **param**（`mov %eax,-0x1c`），
    // msg 不具名（`mov 0x10(%ebp),%eax; mov %eax,0x8(%esp)` 直传 check_error）——
    // 我方原 `MSG_DIE_CHARACTER& req = ...` 多一个 -0x20 槽；改 msg 成员在使用点内联 cast。
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

    // ORIG 读包体 ushort：+0xf 先入 esi、+0xd 后入 ebx（callee-saved 保活，**无具名槽**），
    // 作 CharacDie 第 2/3 参（实参右→左 ⇒ +0xf 先求值）。我方原两个 const 具名局部
    // 各占 4B 槽并多出 store/load 对（P0/P329）。
    user->m_historyLog.CharacDie(
        user->getCurCharacName(),
        *reinterpret_cast<const unsigned short*>(
            reinterpret_cast<const MSG_DIE_CHARACTER*>(&msg)->m_pad),      // +0xd
        *reinterpret_cast<const unsigned short*>(
            reinterpret_cast<const MSG_DIE_CHARACTER*>(&msg)->m_pad + 2));  // +0xf
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
