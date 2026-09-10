int Dispatcher_HatchCreature::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stHatchCreature& st = *reinterpret_cast<stHatchCreature*>(&param);
    char* pMsg = (char*)&msg;

    st.m_4 = check_error(user, msg);
    if (st.m_4 >= 1)
    {
        return 0;
    }

    if (st.m_4 < 0)
    {
        return LineFunc(0x252c,
                        __PRETTY_FUNCTION__,
                        st.m_4, user->get_acc_id());
    }

    short slot = *(short*)(pMsg + 0x0e);
    int invenType = GetInvenTypeFromItemSpace(
        (ENUM_ITEMSPACE)(unsigned char)pMsg[0x0d]);

    const Inven_Item* item =
        user->getCurCharacInvenRefR()->GetInvenRef(invenType, slot);
    if (item == 0)
    {
        return LineFunc(0x2533,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }

    int addInfo = item->get_add_info();

    CInventory* invW = user->getCurCharacInvenW();
    user_creature::CCreatureMgr* mgr = invW->GetCreatureMgrW();

    // BLOCKED(user_creature::CCreatureMgr::Hatch(int)
    //         _ZN13user_creature12CCreatureMgr5HatchEi @0x08339fa0)
    // CInventory.h 内 user_creature::CCreatureMgr 尚无该声明（见 NEEDS_DECL）。
    mgr->Hatch(addInfo); // 占位直调，待声明落地

    return 0;
