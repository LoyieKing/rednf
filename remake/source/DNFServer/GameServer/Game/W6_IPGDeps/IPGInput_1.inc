void CIPGHelper::IPGInput(CUser* user, unsigned char serviceType, unsigned int price,
                          const char* p4, const char* p5, const char* p6,
                          const char* p7, const char* p8, void* callback)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_INPUT, true);
    // [b28d-solo-W6_IPGDeps] ORIG 0x080ffca4 无 in 具名中转（data 槽单承载，
    // 各字段 add/lea $0xdd/0xe8/0xf4/0x108/0x16c/0x1d0/0x202/0x210/0x24c/0x26d 重算）
    // ⇒ 全成员链（P0/P183）。
    strncpy(data->m_cmd.m_input.m_argDD, p4, 0xb);
    strncpy(data->m_cmd.m_input.m_argE8, p5, 0xc);
    strncpy(data->m_cmd.m_input.m_argF4, p6, 0x14);
    if (p7 != 0)
    {
        strncpy(data->m_cmd.m_input.m_arg108, p7, 100);
    }
    if (p8 != 0)
    {
        strncpy(data->m_cmd.m_input.m_arg16C, p8, 100);
    }
    strncpy(data->m_cmd.m_input.m_userName, user->GetUserName(), 0x32);
    strncpy(data->m_cmd.m_input.m_ssn, user->GetSsnString(), 0xe);
    strncpy(data->m_cmd.m_input.m_email, user->GetUserEMail(), 0x3c);
    memset(data->m_cmd.m_input.m_zero24C, 0, 7);
    strncpy(data->m_account, user->get_acc_name(), 0x20);     // ORIG n=0x20 > m_account[0x14]【越界写】
    strncpy(data->m_characName, user->getCurCharacName(), 0x1e); // ORIG n=0x1e > m_characName[0x14]【越界写】
    data->m_cmd.m_input.m_serviceType = serviceType;
    data->m_cmd.m_input.m_price = price;
    data->m_callback = callback;
    m_mgr->PutIPGRequest(data, true);
