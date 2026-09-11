// ORIG T 0x0859a48c _ZN6CParty10ConnectP2PEP5CUserPci（ConnectP2P.md）。
// 队伍状态必须为 1（入队/出发态），否则抛 ENUM_ERROR(0x13)；
// 目标成员 IP 缓冲（cMember+0x0e 起 4 字节）复制 min(port,4) 字节并把
// cMember+0x0c 置 1；全员连接完成后清坏 P2P 成员。ip 参数仅作 memcpy 源
// （ORIG 未读其长度，端口参数被 min 截断使用【忠实转写】）。
void CParty::ConnectP2P(CUser* user, char* ip, int port)
{
    if (get_state() != 1)
    {
        throw static_cast<ENUM_ERROR>(0x13);
    }

    bool allConnected = true;
    for (int i = 0; i < 4; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        if (m_member[i].m_pUser == user)
        {
            if (m_member[i].m_fieldc == 1)
            {
                throw static_cast<ENUM_ERROR>(0x12);
            }
            m_member[i].m_fieldc = 1;
            memcpy(m_member[i].m_fielde, ip, std::min(port, 4));
        }
        if (m_member[i].m_fieldc != 1)
        {
            allConnected = false;
        }
    }
    if (allConnected)
    {
        ClearBadP2PUser();
    }
}