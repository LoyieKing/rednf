void CIPGHelper::IPGInputPoint(CUser* user, unsigned int point, char a, char b,
                               void* callback)
{
    SIPGData* data = _SetIPGData(user, IPG_TYPE_POINT, true);
    // ORIG 无 pt 具名中转（data 槽单承载，各字段 add $k 重算；P134）。
    sprintf(data->m_account, "%d", user->get_acc_id());
    strncpy(data->m_accName, user->get_acc_name(), 0x28);        // ORIG n=0x28【越界写】
    strncpy(data->m_gameCode, user->getCurCharacName(), 0x28);   // ORIG n=0x28 > m_gameCode[5]【越界写】
    strncpy(data->m_cmd.m_point.m_itemNo, "", 8);
    strncpy(data->m_cmd.m_point.m_arg94, "", 0x15);
    data->m_cmd.m_point.m_point = point;
    data->m_cmd.m_point.m_command = 0x41;              // 'A'：输入点模式
    data->m_cmd.m_point.m_cmdA = (unsigned char)a;
    data->m_cmd.m_point.m_cmdB = (unsigned char)b;
    // ORIG 0x0810022e 全区间无 +0x2d8(callback) 写（0xb2 后直呼
    // PutIPGRequest）——callback 形参未用即丢弃（N-88b 反向多行为删除）。
    // [b27] 残差仅形参槽序（a,b @-0x1c/-0x20 对调，74=74 全等；形参槽由
    // ABI 序固定，N93 标量例外不可控）+ view 链 reloc。语义/调用链全等。
    m_mgr->PutIPGRequest(data, true);
}
