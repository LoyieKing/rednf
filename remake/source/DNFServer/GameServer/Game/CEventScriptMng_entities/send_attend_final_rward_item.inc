void CAttendance::send_attend_final_rward_item(CUser* user, int attendFlags,
                                               int record)
{
    cMyTrace(__PRETTY_FUNCTION__, 0x5ba, 0)(
        "send_attend_final_rward_item m_id:%s day:%d record:%u",
        NumberToString(user->get_acc_id(), 0), record, attendFlags);

    int setBits = 0;
    for (int bit = 0; bit < 0x20; ++bit)
    {
        if (isSetBit_Uint(static_cast<unsigned int>(attendFlags), bit))
            ++setBits;
    }
    if (record == setBits)
    {
        // ORIG 发奖段每次内联解引用脚本（不缓存局部）。
        m_script1c->m_mail.m_itemIdx = m_script1c->m_pair1c.first;
        m_script1c->m_mail.m_addInfo = m_script1c->m_pair1c.second;
        m_script1c->m_mail.m_field34 = m_script1c->m_str74;

        CEventActionMng mng;
        mng.process_action_send_mail(user, EVENT_MAIL_REF(m_script1c->m_mail),
                                     true);
    }
