int CraneMinigameManager::sendItem(CUser* user, int slot)
{
    CraneItemInfo* item = reinterpret_cast<CraneMinigameScript*>(this)->getItem(static_cast<short>(slot));
    // ORIG +0x22/+0x36：item==0 → je 尾块 +0x84（`mov $0x15,%eax` 直落 leave）；
    // m_cnt==0xffff → fall 内联块 +0x31（`mov $0x15,%eax; jmp leave`）；body 尾
    // +0x82 `jmp leave` 跳过尾块。该布局 = 正向 if 包主流程、return 0x15 落
    // else（P169/P285 同族：4.4 对平铺两条早退恒把两块都内联在前部，反产
    // `jne 跳过; mov 0x15; jmp` ×2）。
    if (item != 0)
    {
        if (item->m_cnt == 0xffff)
            return 0x15;

        // ORIG +0x48/+0x5e：&mail 两次独立计算（P52 不落局部）；
        // +0x3b/+0x3f：movzwl 0x8 + movswl %ax（(int)(short)m_cnt）。
        this->m_sendMail.set_item(
            item->m_itemIdx, static_cast<int>(static_cast<short>(item->m_cnt)));

        EventClassify::CEventActionMng actionMng;
        return actionMng.process_action_send_mail(user, this->m_sendMail, true);
    }
    return 0x15;
