// ORIG 0x084cf7fa T dispatch_sig —— 邮箱列表装载：多重校验后设置邮箱信息。
// 报告 docs/class_func_reports/Inter_MailBox_Req_List/dispatch_sig.md 全文转写。
// [b28d-lane2] verdict=DIFF ndiff=46（ORIG 0x084cf7fa）：寄存器中转(N18a/N7)+混合残差；gap+3。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=37（ORIG 0x84cf7fa）：N82/P112 帧槽平移+P30/N4 add-vs-lea。
int Inter_MailBox_Req_List::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 1 || user->getCurCharacR() == 0)
        return 0;

    SIG_MAILBOX_LIST* recv =
        reinterpret_cast<SIG_MAILBOX_LIST*>(data);     // -0x20(%ebp) 缓存消息指针

    // ORIG 0x084cf845 `mov 0x3332(%eax),%ebx`：具名字段单段 disp（§4.3），值跨
    // getCurCharacNo 调用保活于 %ebx 且无栈槽 ⇒ 裸 register 局部（P248 四条件全满足）。
    register int characNo = recv->m_curCharacNo;
    if (characNo != user->getCurCharacNo())
        return 0x2ca0;                                 // 角色号不匹配

    WongWork::CMailBox* mailBox = (WongWork::CMailBox*)user->GetMailBox();  // CUser.h 冻结：GetMailBox 返回 void*
    if (mailBox == 0)
        return 0x2ca4;                                 // 无邮箱

    // ORIG 0x084cf8??：计数两处独立读（4.1 -0x3c 槽物化被 4.4 折并，实测具名反而
    // 少 2 行失真，取直读形）。
    if (recv->m_mailLoadCount != mailBox->getMailLoadCount())
    {   // 重复装载追踪
        cMyTrace(__PRETTY_FUNCTION__,
            0x2ca9, 5)("Postal loaded twice. %s(%d)(%d->%d)",
              NumberToString(user->get_acc_id(), 0),
              user->getCurCharacNo(),
              mailBox->getMailLoadCount(),
              recv->m_mailLoadCount);
        return 0x2caa;
    }

    WongWork::CMailBoxHelper::SetMailBoxInfo(user, recv);
    return 0;
}