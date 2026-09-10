void CUsedFatigueGiveItem::sendEventItemFromFatigue(CUser* user, unsigned int fatigue)
{
    // ORIG 0x81105f6：按已用疲劳档位周期发放奖励邮件。
    // 档位间隔 m_usedFatigue、上限 m_fatigueLimit（<0 不限）、邮件 m_mail；
    // 循环先判当前已用疲劳/上限，再 ++step 后按档位取模并发信（ORIG do-while），
    // 发信成功读 m_mail.m_itemIdx 记日志收尾。全程不缓存脚本指针（与 ORIG 同）。
    // [b28c-L0 N80/N37] ORIG 两处句尾 `jmp;nop` 中继岛与 4.4 岛形不同；
    // 日志段 ORIG 立即数为链接后绝对地址、我方 .o 为重定位显示差（AE 口径内）。
    // 语义等价，落 N80（cMyTrace 变参跨 ctor）/P223 岛形封顶。
    if (static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_usedFatigue == 0)
        return;
    if (user->GetCurCharacUsedGiftFatigueQuantity() == 0)
        return;
    unsigned int step =
        (unsigned int)(user->GetCurCharacUsedGiftFatigueQuantity() - fatigue);
    // 入口护栏：m_fatigueLimit >= 0 且 step >= 上限 → 一档都不发。
    if (static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_fatigueLimit >= 0 &&
        (unsigned int)static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_fatigueLimit <= step)
        return;

    CEventActionMng mng;
    while ((unsigned int)user->GetCurCharacUsedGiftFatigueQuantity() > step)
    {
        if (static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_fatigueLimit >= 0 &&
            (unsigned int)static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_fatigueLimit <= step)
            return;
        ++step;                                   // 先自增再判档位/发信（ORIG do-while）
        if (step % (unsigned int)static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_usedFatigue != 0u)
            continue;
        int sent = mng.process_action_send_mail(
            user,
            EVENT_MAIL_REF(static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_mail),
            true);
        if (sent != 0)
        {
            LogManager::logFormat(1, "EventClassify.cpp",
                __PRETTY_FUNCTION__,
                0x83f,
                "CUsedFatigueGiveItem::sendEventItemFromFatigue(m_id:%d, charac_no:%d, reward:%d)",
                user->get_acc_id(), user->getCurCharacNo(),
                static_cast<UsedFatigueGiveItemScript*>(m_script1c)->m_mail.m_itemIdx);
        }
    }
