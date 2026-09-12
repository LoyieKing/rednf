// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081e731a orig/ours=149/151 ndiff-blocks=22
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x1c(%ebp) | mov    0x14(%ebp),%eax | mov    %eax,-0x18(%ebp)
int Dispatcher_UseRenameCard::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [G1Dproc 2026-09-11] ORIG 0x081e731a 起 槽序直证：msg(-0x1c) 先、st(-0x18) 后；
    //   check_error 结果**直写 st+0x04**（call 后紧邻 `mov %eax,%edx; mov -0x18(%ebp),%eax;
    //   mov %edx,0x4(%eax)` 三条，值先入 %edx = P211 档③）
    //   ⇒ P0：无 `int err` 具名槽；两处使用均从 st.m_4 重读。
    // [G6-74 负结论] 试补 msg 具名引用（ORIG 序言有 -0x1c msg 槽，声明在 st 前）：
    //   实测 149→151（+2，4.4 对单用点引用不产槽形）⇒ 已回滚；保留 pMsg 删除 + 具名成员。
    // [SemA03-exp V1b] ORIG 序言把 &msg 存入独立槽（-0x1c）并在两处 m_0e 使用处回读；
    //   4.4 按「首次使用序」分配槽（先 → 更深）⇒ pMsg 声明须在 st 之前。
    MSG_USE_RENAME_CARD* pMsg = reinterpret_cast<MSG_USE_RENAME_CARD*>(&msg);

    stUseRenameCard& st = *reinterpret_cast<stUseRenameCard*>(&param);

    st.m_4 = check_error(user, msg);

    if (st.m_4 != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fe, (unsigned char)st.m_4);
        return 0;
    }

    Inven_Item item;
    // ORIG 0x081e738a `movzwl 0xe(%eax),%eax; cwtl`（disp 折叠 + 1 操作数 cwtl）；
    // 裸偏移 `*(short*)(pMsg+0xe)` 产 add+movzwl 两条 ⇒ 改具名成员。
    int ret = getItem(*user, 1,
                      static_cast<short>(pMsg->m_0e),
                      item);
    if (ret != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fe, (unsigned char)ret);
        return 0;
    }

    CItem* stackItem = G_CDataManager()->find_item(item.m_addInfo); // +0x02 item idx
    if (stackItem == 0)
    {
        return 0;
    }

    if (!stackItem->is_stackable())
    {
        return 0;
    }

    // TYPE_CHARACTER_NAME_CHANGE = 0x33
    // [R18-03 2026-09-13] ORIG 具名派生指针局部（0x081e73f5 `mov -0x10(%ebp),%eax;
    //   mov %eax,-0xc(%ebp)` 后该槽作 verifyStackableAction 的 this）——原码即
    //   `CStackableItem* p = (CStackableItem*)stackItem;`；cast 表达式接收者在 4.4 恒折
    //   内联（少 1 具名槽、帧 0x74 vs ORIG 0x84）⇒ 恢复具名（P341 接收者去 cast）。
    CStackableItem* stackable = static_cast<CStackableItem*>(stackItem);
    if (!stackable->verifyStackableAction(
            StackableAction::TYPE_CHARACTER_NAME_CHANGE))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fe, (unsigned char)ret);
        return 0;
    }

    user->SetCharacViewState(0xff);

    // [G6-74] P0/P161：ORIG 无 pMsg 具名指针（槽偏移多 4）——直接用 msg 引用 +
    //   具名成员 m_0e（ORIG 0x081e74xx `mov -0x1c(%ebp),%eax; movzwl 0xe(%eax),%eax`）。
    // [G4-74 2026-09-11 · P0/P183/P22] ORIG 三证：0x081e7494 段 call getCurCharacInvenW
    //   后直接回读 %eax 传参（**无 invenW 具名槽**）；0x081e748a `movzwl 0xe(%eax),%eax;
    //   movswl %ax,%ebx` ⇒ m_0e 按 (short) 符号扩展（类内为 unsigned short）；
    //   0x081e74cb `xor $0x1,%eax; test %al,%al; je 081e752d` ⇒ delete_item 结果
    //   **无具名槽**、失败(0)落错误分支（旧码 `if (delRet == 0)` 曾反向）。
    if (!user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)1,
            static_cast<short>(pMsg->m_0e), 1,
            (eItemDelReason)3, true))
    {
        user->SetCharacViewState(0);
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x1fe, 0x11);
        return LineFunc(0x632e,
                        __PRETTY_FUNCTION__,
                        0, user->get_acc_id());
    }
    user->SendCmdOkPacket((ENUM_CMDPACKET)0x1fe);
    user->ReturnToSelectCharacList(true);
    user->UpdateCharacView();
    return 0;
}