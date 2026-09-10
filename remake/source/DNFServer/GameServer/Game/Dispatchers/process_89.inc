int Dispatcher_RenameCreature::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stRenameCreature& st = *reinterpret_cast<stRenameCreature*>(&param);
    char* pMsg = (char*)&msg;

    st.m_4 = check_error(user, msg);
    if (st.m_4 >= 1)
    {
        return 0;
    }

    if (st.m_4 < 0)
    {
        return LineFunc(0x24d3,
                        __PRETTY_FUNCTION__,
                        st.m_4, user->get_acc_id());
    }

    // 名字为默认占位名"이름없음"时清空为空串再改名。
    if (strcmp(pMsg + 0x14, "\xec\x9d\xb4\xeb\xa6\x84\xec\x97\x86\xec\x9d\x8c") == 0)
    {
        memset(pMsg + 0x14, 0, 0xd);
    }

    int slot = (signed char)pMsg[0x0d];      // movsbl
    int creatureIdx = *(short*)(pMsg + 0x0e); // movswl

    CInventory* invW = user->getCurCharacInvenW();
    user_creature::CCreatureMgr* mgr = invW->GetCreatureMgrW();

    // BLOCKED(user_creature::CCreatureMgr::RenameCreature(char const*, int, int)
    //         _ZN13user_creature12CCreatureMgr14RenameCreatureEPKcii @0x08339b6e)
    // CInventory.h 内 user_creature::CCreatureMgr 尚无该声明（见 NEEDS_DECL）。
    mgr->RenameCreature(pMsg + 0x14, creatureIdx, slot); // 占位直调，待声明落地

    return 0;
