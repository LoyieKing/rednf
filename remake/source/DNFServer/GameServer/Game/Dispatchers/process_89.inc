// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081cd110 orig/ours=79/78 ndiff-blocks=14
//   帧差 sub $2c vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %edi | push   %esi | push   %ebx
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
    MSG_RENAME_CREATURE* pMsg = reinterpret_cast<MSG_RENAME_CREATURE*>(&msg);

    // 名字为默认占位名"이름없음"时清空为空串再改名。

    // [K1-parent §6.5.1 编码盲区] ORIG .rodata 实字节为 EUC-KR
    //   `\xc0\xcc\xb8\xa7\xbe\xf8\xc0\xbd`（"이름없음"），非 UTF-8。
    if (strcmp(pMsg->m_name, "\xc0\xcc\xb8\xa7\xbe\xf8\xc0\xbd") == 0)

    {
        memset(pMsg->m_name, 0, 0xd);

    }

    // [G1Dproc] ORIG 0x081cd1ce 起：slot→%esi（`movzbl 0xd; movsbl %al,%esi`）、
    //   creatureIdx→%ebx（`movzwl 0xe; movswl %ax,%ebx`）、name→%edi（`lea 0x14(%eax)`）、
    //   getCurCharacInvenW/GetCreatureMgrW 直链（0x081cd1e4/…1ee 无具名槽，P0）
    //   ⇒ 删 4 个具名局部；实参右→左（slot 先、creatureIdx 次、name 最后）与 ORIG 同序。
    //   _ZN13user_creature12CCreatureMgr14RenameCreatureEPKcii @0x08339b6e
    user->getCurCharacInvenW()->GetCreatureMgrW()->RenameCreature(
        pMsg->m_name,
        (int)pMsg->m_field0e,
        (int)pMsg->m_field0d);


    return 0;
}