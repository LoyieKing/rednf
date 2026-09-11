// [b28d-solo-TaiwanInternalDispatcher] AE（proxy 具名局部去槽 + GetServerProxy()->SendPacket 链式）
int InterUpdateP2pHolePunchingState::dispatch_sig(CUser* user, char* buf, int len)
{
    // ORIG 0x08161952：统计包 → 国家代码 + 对端 IP → 经统计代理 UDP 转发。
    // ORIG 0x081619xx：buf 入口单槽物化 -0xc（无 char*+view 双中转，p 删除），
    // 三处字段读均经该槽 reload（技巧36 反向）。
    P2pHoleMsgV* v = reinterpret_cast<P2pHoleMsgV*>(buf);
    Packet_GameServer2Statisctics2DBServer pkt;
    pkt.m_serverGroup = (short)G_CEnvironment()->get_server_group();
    // ORIG `movzbl 0xd(%eax)`/`mov 0xe(%eax)`/`mov 0x12(%eax)` 单段 disp ⇒
    // pack(1) 视图具名字段（§4.3；裸 add 形实测不折 disp）。
    pkt.m_connectedType = v->m_connectedType;
    pkt.m_requiredTime = v->m_requiredTime;
    pkt.m_checkTime = v->m_checkTime;
    user->m_network.GetPeerIP2(pkt.m_peerAddress, 0x10);
    RestrictGeolocation::Get()->getCountryCode(
        pkt.m_nationCode, 0x10, std::string(pkt.m_peerAddress));
    GlobalData::s_statistic_proxy_mgr->GetServerProxy(
        (ENUM_SERVER_GROUP)0)->SendPacket((char*)&pkt, 0x35);
    return 0;
}