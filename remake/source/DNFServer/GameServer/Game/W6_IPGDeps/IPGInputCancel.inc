void CIPGHelper::IPGInputCancel(CUser* user,
                                const char* p2, const char* p3, const char* p4,
                                const char* p5, const char* p6, const char* p7,
                                void* callback)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_INPUT_CANCEL, false);
    // ORIG 无 ic 具名中转（P134，data 槽单承载重算）。
    strncpy(data->m_cmd.m_inputCancel.m_arg69, p2, 0x15);
    strncpy(data->m_cmd.m_inputCancel.m_arg7E, p3, 0xb);
    strncpy(data->m_cmd.m_inputCancel.m_arg89, p4, 0xc);
    strncpy(data->m_cmd.m_inputCancel.m_arg95, p5, 0x14);
    if (p6 != 0)
    {
        strncpy(data->m_cmd.m_inputCancel.m_argA9, p6, 100);
    }
    if (p7 != 0)
    {
        strncpy(data->m_cmd.m_inputCancel.m_arg10D, p7, 100);
    }
    data->m_callback = callback;
    m_mgr->PutIPGRequest(data, true);
