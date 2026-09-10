void CIPGHelper::IPGOutput(CUser* user,
                           const char* p2, const char* p3, const char* p4,
                           const char* p5, const char* p6, void* callback)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_OUTPUT, true);
    // ORIG 无 out 具名指针中转（P134/P183：data 槽单承载，各 strncpy 目标位
    // 逐点重读 -0xc + add $0x69/0x71/0x7d/0x91/0xf5/0x1e8 两步寻址）。
    strncpy(data->m_cmd.m_output.m_price, p2, 8);
    strncpy(data->m_cmd.m_output.m_pay, p3, 0xc);
    strncpy(data->m_cmd.m_output.m_arg7D, p4, 0x14);
    if (p5 != 0)
    {
        strncpy(data->m_cmd.m_output.m_desc91, p5, 100);
    }
    if (p6 != 0)
    {
        strncpy(data->m_cmd.m_output.m_descF5, p6, 100);
    }
    strncpy(data->m_cmd.m_output.m_recvCharac, user->getCurCharacName(), 0x3a);
    data->m_callback = callback;
    m_mgr->PutIPGRequest(data, true);
}
