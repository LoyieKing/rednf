void CSession::SetChannelNo(int channelNo)      // ORIG 0x848ffbe
{
    // int 字段 + int 形参同型直赋（无转换节点）：4.4 LHS 先载=ORIG 实证。
    m_channelNo = channelNo;
