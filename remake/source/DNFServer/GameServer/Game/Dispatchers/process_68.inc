// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081cd446 orig/ours=86/89 ndiff-blocks=17
//   帧差 sub $20 vs $44 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | sub    $0x20,%esp | mov    %eax,-0x14(%ebp)
int Dispatcher_HatchCreature::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    stHatchCreature& st = *reinterpret_cast<stHatchCreature*>(&param);

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

    // [G1Dproc 2026-09-10] ORIG 0x081cd4c5 `mov 0x10(%ebp),%eax; mov %eax,-0x10(%ebp)`
    //   ⇒ msg 引用槽在**第二段**（LineFunc 早退之后）才初始化，声明须下移。
    const MSG_HATCH_CREATURE& req = *reinterpret_cast<const MSG_HATCH_CREATURE*>(&msg);
    // [G1Dproc] ORIG 0x081cd4fd（0x081cd4fd movzwl 0xe / movswl → %esi；call
    //   GetInvenTypeFromItemSpace → %ebx）：slot 与 invenType **均无具名槽**（P0）
    //   ⇒ 内联嵌套实参；实参右→左（slot 先、invenType 次、this 最后）与 ORIG 同序。
    const Inven_Item* item =
        user->getCurCharacInvenRefR()->GetInvenRef(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)
                req.m_itemSpace),
            (int)req.m_slotIdx);
    if (item == 0)
    {
        return LineFunc(0x2533,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }

    // [G1Dproc] ORIG 0x081cd53a：`get_add_info → %ebx`、invenW/GetCreatureMgrW 直链
    //   （0x081cd547/…551 各自 `mov %eax,(%esp)` 无具名槽）⇒ P0 链式实参，删 3 个具名局部。
    //   _ZN13user_creature12CCreatureMgr5HatchEi @0x08339fa0
    user->getCurCharacInvenW()->GetCreatureMgrW()->Hatch(item->get_add_info());

    return 0;
}