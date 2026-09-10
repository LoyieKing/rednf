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
