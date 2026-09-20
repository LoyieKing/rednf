// ---------------------------------------------------------------------------
// CUser::set_MultimailMsgData(MSG_MULTI_MAILBOX_SEND* data) T 0x0868a4e8 sz0x32
// 报告：docs/class_func_reports/CUser/set_MultimailMsgData.md
// 0x8e3ec multimail flag=1 + memcpy(0x8e246, data, 0x1a6)。0x8e 区访问与
// InitOnAccept 既有写法一致（m_pad8e101 为 0x8e101 起块视图）。
// ---------------------------------------------------------------------------
void CUser::set_MultimailMsgData(MSG_MULTI_MAILBOX_SEND* data)
{
    m_multiMailFlag = 1;
    memcpy(m_multiMailMsgData, data, sizeof(m_multiMailMsgData));
}