void CAttendance::send_attend_reward_item(CUser* user, int day)
{
    // 判断段 ORIG 为三个独立 if+return（跳转目标各异）：size()==0 判空、
    // day<0、signed size<=day，每次内联解引用脚本（不缓存）；m_attends[day]
    // 两次独立取用。
    if (m_script1c->m_attends.size() == 0)
        return;
    if (day < 0)
        return;
    if ((int)m_script1c->m_attends.size() <= day)
        return;

    // [b28d-D9] ORIG 0x0810e8a0：NumberToString 结果无具名槽，直接经 `mov %eax,%ebx`
    // 跨 cMyTrace ctor 保活为 operator() 实参（P227 全表达式临时链 + P0 无局部）。
    cMyTrace(__PRETTY_FUNCTION__, 0x5ad, 0)(
        "send_attend_reward_item m_id:%s day:%d",
        NumberToString(user->get_acc_id(), 0), day);

    // [b28d-D9] ORIG 0x0810e8d2 段每个语句独立重载 m_script1c（`mov 0x8(%ebp),%eax;
    // mov 0x1c(%eax),%ebx` 多次，无跨语句缓存）⇒ 逐表达式内联 static_cast（P0/P183）。
    m_script1c->m_mail.m_itemIdx = m_script1c->m_attends[day].first;
    m_script1c->m_mail.m_addInfo = m_script1c->m_attends[day].second;
    m_script1c->m_mail.m_field34 = m_script1c->m_str70;

    CEventActionMng mng;
    mng.process_action_send_mail(user, EVENT_MAIL_REF(m_script1c->m_mail), true);
