int Inter_PowerWarProcessInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_PowerWarProcessInfoPktView* pPkt = reinterpret_cast<Inter_PowerWarProcessInfoPktView*>(data);
    // [b28d-lane2] N22 判词推翻（探针 /tmp/l2p/p2.cpp fA..fF）：ORIG `mov 0xe(%eax),%ebx;
    // cmp %eax,%ebx` 只在「字段读**内联进比较**、无具名局部」时复现；具名（含 register）
    // 局部必产 `mov mem,%eax; mov %eax,%ebx`。属 P0（无槽往返 ⇒ 原码无该局部）。
    if (pPkt->m_e != user->get_charac_no(-1))
        return 0;

    user->SetPowerWarProcessInfoLastUpdateTime(
        GlobalData::s_systemTime_.getCurSec());

    PacketGuard packet;
    packet.put_header(0, 0xc9);
    packet.put_int(pPkt->m_12);
    packet.put_int(pPkt->m_16);
    packet.put_int(pPkt->m_1a);
    packet.put_int(pPkt->m_1e);
    packet.put_int(pPkt->m_22);
    packet.finalize(true);
    user->Send(packet);
    return 0;
