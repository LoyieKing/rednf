// ---- CErrorHandler::Do（ORIG T 0x85c4c7c，报告 private_store__CErrorHandler/Do.md 全文）----
// 构造错误回执包 MakeErrorReturnPacket(m_sendError, m_code, errorCode) 发给目标用户，
// 返回目标是否有效。
bool CErrorHandler::Do(int errorCode)
{
    if (m_user == 0)
        return false;

    CPacketBufPrivate packet;
    packet.MakeErrorReturnPacket(m_sendError, m_code, errorCode);
    m_user->Send(packet);
    return true;
}