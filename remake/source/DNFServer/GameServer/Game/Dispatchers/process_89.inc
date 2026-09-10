int Dispatcher_RenameCreature::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stRenameCreature& st = *reinterpret_cast<stRenameCreature*>(&param);

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

    // [G1Dproc 2026-09-10] ORIG 0x081cd1a8 `mov 0x10(%ebp),%eax; mov %eax,-0x1c(%ebp)`
    //   ⇒ msg 指针槽在**第二段**（LineFunc 早退之后）才初始化，声明须下移。
    char* pMsg = (char*)&msg;

    // 名字为默认占位名"이름없음"时清空为空串再改名。
    if (strcmp(pMsg + 0x14, "\xec\x9d\xb4\xeb\xa6\x84\xec\x97\x86\xec\x9d\x8c") == 0)
    {
        memset(pMsg + 0x14, 0, 0xd);
    }

    // [G1Dproc] ORIG 0x081cd1ce 起：slot→%esi（`movzbl 0xd; movsbl %al,%esi`）、
    //   creatureIdx→%ebx（`movzwl 0xe; movswl %ax,%ebx`）、name→%edi（`lea 0x14(%eax)`）、
    //   getCurCharacInvenW/GetCreatureMgrW 直链（0x081cd1e4/…1ee 无具名槽，P0）
    //   ⇒ 删 4 个具名局部；实参右→左（slot 先、creatureIdx 次、name 最后）与 ORIG 同序。
    //   _ZN13user_creature12CCreatureMgr14RenameCreatureEPKcii @0x08339b6e
    user->getCurCharacInvenW()->GetCreatureMgrW()->RenameCreature(
        pMsg + 0x14,
        (int)*(short*)(pMsg + 0x0e),
        (int)(signed char)pMsg[0x0d]);

    return 0;
