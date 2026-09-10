bool CParty::TimeoutCompleteAfterAssault()
{
    if (m_recvResultFlag != 0)
    {
        m_recvResultFlag = 0;
        return 0;
    }
    if (!recv_party_packet_all())
    {
        for (int i = 0; i <= 3; i++)
        {
            if (!_checkValidUser(i))
            {
                continue;
            }
            // b28d-45 P248 档①：IsRecv 已 bool 化，ORIG 0x085a467 为
            // `call; xor $0x1; test %al,%al; je`（正向 if 包裹），非 continue 形。
            if (!reinterpret_cast<CPartyRecvFlagView*>(this)
                     ->m_recvFlag.IsRecv(i))
            {
                // P227 全表达式临时链：R2L 求值 name(→%esi)/partyIdx(→%ebx)
                // 跨 ctor 保活（ORIG +0x0a6/+0x0b3）
                cMyTrace(__PRETTY_FUNCTION__, 0x1fa, 0)(
                    "CParty::TimeoutCompleteAfterAssault(index:%d, name:%s)",
                    GetPartyIndex(),
                    reinterpret_cast<CUserCharacInfo*>(get_user(i))
                        ->getCurCharacName());
                leave_user(get_user(i),
                           static_cast<ENUM_PARTY_INFO_TYPE>(2));
            }
        }
    }
    return 1;
