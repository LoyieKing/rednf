int DisPatcher_RemoveFriend::process(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    stRemoveFriendParam& out = static_cast<stRemoveFriendParam&>(param);

    out.m_errNo = check_error(user, msg);
    if (out.m_errNo > 0)
    {
        return 0;
    }
    if (out.m_errNo < 0)
    {
        return LineFunc(0x2141, __PRETTY_FUNCTION__, out.m_errNo, user->get_acc_id());
    }

    // ORIG 0x081cb6a4：msg 指针具名局部 -0x10（在本分支内声明，非函数头）+ 每处重算
    // `mov -0x10(%ebp),%eax; add $0x11,%eax`（strlen 与 memcpy 各一次，P183 禁缓存）；
    // 0x081cb6d2 `mov %eax,-0xc(%ebp)` + 0x081cb6d5 回读 = strlen 结果经具名 len 槽
    // （P197② 具名结果槽）。len 槽存在使 pkt/全部局部槽基下移 4（-0x40→-0x44）。
    MSG_BASE* pReq = &msg;                                              // -0x10（先声明，
                                                                        // ORIG +0x07f store
                                                                        // 早于 pkt ctor）
    Packet_Monitor_Del_Buddy pkt;
    pkt.m_charNo = static_cast<unsigned int>(user->getCurCharacNo());   // +0x0a
    pkt.m_idByChannel = user->GetUID();                                 // +0x0e
    // P319 试验（已回滚，实测 ae -1 回退）：整型加法 cast 形不能把这处 lea 折回转；
    // 该处 `mov -0x10,%edx; add $0x11,%edx`（ORIG）vs `lea 0x11(%eax),%edx`（4.4）
    // 属 P30/N4 寻址折叠族，记 N。
    size_t len = strlen(reinterpret_cast<STFriendReqView*>(pReq)->m_name);
    memcpy(pkt.m_name, reinterpret_cast<STFriendReqView*>(pReq)->m_name, len);
    // [b28e-L5] P52 应用例：ORIG 0x081cb6f4 先 `movzwl -0x42(%ebp),%eax; movzwl %ax,%esi`
    // 与 `lea -0x44(%ebp),%ebx` 把 (size, pktAddr) 物化入 callee-saved，再跨
    // GetServerGroup→GetServerProxy 两次 call 保活，最后三压参。单条链式表达式复现
    // （拆具名 proxy 局部会使 4.4 两语句坍缩、丢 push %esi/%ebx 与物化段）。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket(reinterpret_cast<char*>(&pkt), pkt.packetSize);    // +0x02 长度字段(0x30)
    return 0;
}
