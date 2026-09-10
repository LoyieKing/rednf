void CIPGHelper::IPGOutputCancel(CUser* user,
                                 const char* p2, const char* p3, const char* p4,
                                 const char* p5, const char* p6, const char* p7,
                                 void* callback, Taiwan::BillTransaction* tran)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_OUTPUT_CANCEL, false);
    // [b28d-solo-W6_IPGDeps] ORIG 0x081004de 无 cv 具名中转（data 槽单承载，
    // 各字段 add $0x69/0x7e/0x86/0x92/0xa6/0x10a 直接重算）⇒ 全成员链（P0/P183）。
    strncpy(data->m_cmd.m_cancel.m_arg69, p2, 0x15);
    strncpy(data->m_cmd.m_cancel.m_arg7E, p3, 8);
    strncpy(data->m_cmd.m_cancel.m_arg86, p4, 0xc);
    strncpy(data->m_cmd.m_cancel.m_arg92, p5, 0x14);
    if (p6 != 0)
    {
        strncpy(data->m_cmd.m_cancel.m_argA6, p6, 100);
    }
    if (p7 != 0)
    {
        strncpy(data->m_cmd.m_cancel.m_arg10A, p7, 100);
    }
    data->m_callback = callback;
    // [b28d-solo-W6_IPGDeps] ORIG 0x081004de 尾部 `mov 0x2c(%ebp),%edx` 载 tran
    // 指针一次跨 3 个 dword store（%ecx 中转）存活 ⇒ union 视图整体赋值（P246）。
    data->m_cmd.m_cancel.m_tran = *tran;
    m_mgr->PutIPGRequest(data, true);
