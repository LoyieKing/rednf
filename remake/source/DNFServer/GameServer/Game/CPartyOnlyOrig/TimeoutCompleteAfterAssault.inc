// ORIG T 0x0859a34e（TimeoutCompleteAfterAssault.md）：+0x118 守卫非 0 时
// 复位并返回 0；为 0 时按未回包成员逐个踢出（trace 后 leave_user type2），
// 返回 1（ORIG 主路径不写 +0x118；调用方 CAssaultMgr 两处均 test %al 消费返回值）。
// b28d-solo-CPartyOnlyOrig 判词：DIFF→AE（+0x1dc 视图具名 receiver）。
// b28d-45 TimeoutCompleteAfterAssault 0x0859a34e：IsRecv bool 化（P248 档①）+
// 正向 if 包裹（P214①）后 85:85。
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
}