// ----------------------------------------------------------------------------
// ORIG T 0x08143f6a _ZN8WongWork10CBossTower19handleFinishLoadingEP5CUser：
// slot 无效或 checkValidUser 为假 → 0；setMemberReady(slot,true) +
// SendCmdOkPacket(0x28)；虚调 checkAllMemberReady(get_member_count())（vptr+0xc）
// 为真时广播包(0,0x1e)/int 0/finalize/send；返回 1。
// ----------------------------------------------------------------------------
// [b28d-54 N] ORIG 0x08143f6a：帧 0x20→0x30 + 槽平移（N82）。
// [b28d-solo-CBossTower] AE（94:94，Main 代落 bool 化后翻正）：CBossPlay.h:60
//   checkAllMemberReady int→bool（ORIG 调用点 0x08143fd5 `call *%ebx; test %al,%al;
//   je` 无 setne ⇒ bool）+ party/ready 具名局部已去 + 裸短路条件化。
int CBossTower::handleFinishLoading(CUser* user)
{
    const int slot = static_cast<int>(m_party->GetMemberSlotNo(user));
    // ORIG 0x08143f86..0x08143fb5：短路物化岛无 bool 槽（mov$1/jmp/mov$0 +
    // test %al,%al;je）⇒ 裸条件内联（P0/P226）。
    if (slot != -1 && m_party->checkValidUser(slot) != 0)
    {
        setMemberReady(slot, true);
        user->SendCmdOkPacket((ENUM_CMDPACKET)0x28);
        if (checkAllMemberReady(m_party->get_member_count()))
        {
            PacketGuard packet;
            packet.put_header(0, 0x1e);
            packet.put_int(0);
            packet.finalize(true);
            m_party->send_to_party(packet);
        }
        return 1;
    }
    return 0;
}