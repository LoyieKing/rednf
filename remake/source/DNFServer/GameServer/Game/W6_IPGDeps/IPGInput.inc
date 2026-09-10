void CIPGHelper::IPGInput(CUser* user,
                          const char* p2, const char* p3, const char* p4,
                          const char* p5, const char* p6, void* callback)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_INPUT, true);
    // ORIG 无 in 具名中转（data 槽单承载，lea 0xNN(%eax) 重算；P134）。
    strncpy(data->m_cmd.m_input.m_argDD, p2, 0xb);
    strncpy(data->m_cmd.m_input.m_argE8, p3, 0xc);
    strncpy(data->m_cmd.m_input.m_argF4, p4, 0x14);
    if (p5 != 0)
    {
        strncpy(data->m_cmd.m_input.m_arg108, p5, 100);
    }
    if (p6 != 0)
    {
        strncpy(data->m_cmd.m_input.m_arg16C, p6, 100);
    }
    strncpy(data->m_cmd.m_input.m_userName, user->GetUserName(), 0x32);
    strncpy(data->m_cmd.m_input.m_ssn, user->GetSsnString(), 0xe);
    strncpy(data->m_cmd.m_input.m_email, user->GetUserEMail(), 0x3c);
    memset(data->m_cmd.m_input.m_zero24C, 0, 7);
    data->m_callback = callback;
    m_mgr->PutIPGRequest(data, true);
