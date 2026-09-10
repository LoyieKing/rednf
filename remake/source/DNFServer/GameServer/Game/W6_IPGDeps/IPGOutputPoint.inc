void CIPGHelper::IPGOutputPoint(CUser* user,
                                const char* p2, const char* p3, const char* p4,
                                const char* p5, const char* p6, void* callback)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_POINT, true);
    // [b28d-solo-W6_IPGDeps] ORIG 0x081000f6 无 pt 具名中转（data 槽单承载，
    // 各字段 add $0x8c/0x94/0xac/0xb0/0xb1/0xb2 直接重算）⇒ 全成员链访问（P0/P183）。
    sprintf(data->m_account, "%d", user->get_acc_id());
    strncpy(data->m_accName, user->get_acc_name(), 0x28);        // ORIG n=0x28【越界写】
    strncpy(data->m_gameCode, user->getCurCharacName(), 0x28);   // ORIG n=0x28 > m_gameCode[5]【越界写】
    strncpy(data->m_cmd.m_point.m_itemNo, p2, 8);
    strncpy(data->m_cmd.m_point.m_arg94, p4, 0x15);  // asm 只写 p4，p5 不落盘
    data->m_cmd.m_point.m_point = (unsigned int)atoi(p3);
    data->m_cmd.m_point.m_command = 0x55;            // 'U'：输出点模式
    data->m_cmd.m_point.m_cmdA = 0;
    data->m_cmd.m_point.m_cmdB = 0;
    data->m_callback = callback;
    m_mgr->PutIPGRequest(data, true);
}
