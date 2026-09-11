// ORIG execute 0x084aba12：GM 用户处于 state 5（组队等待态，推断）时，把本人
// 秘密商店数据的 npc id 改为参数值（限 1000/0x3ea/0x3eb/0x3ec 四个合法值），
// 条件：无队伍，或队伍不在血色地下城中（party->m_battleField.m_pDungeon == 0 或
// dungeon->m_bloodDungeonType == 0）。随后聊天通告 + 广播 0x114 npc 替换包。
// [b28d-solo-GmCmds_F] AE：①删 npcId 具名局部（P0：ORIG 0x084aba6b 起四次直读 this+0x8，
// 无槽往返）；②party 检查改具名 CDungeon* dungeon（ORIG -0x1c 有槽往返）；
// ③m_bloodDungeonType 加 static_cast<signed char> 复现 ORIG 0x084abac7 的 movzbl+test%al+jg
// （裸 >0 被 4.4 折成 jne；P196/P292 族）；④char buf[0x100]={0} 复现 ORIG rep stos 0x40
// dwords（P214③/P253）；⑤trace 改全表达式临时链 cMyTrace(...)(...)（P227：*shopData→%esi、
// name→%ebx 在 ctor 前求值跨 ctor 保活，对齐 0x084abb0d 段）；⑥删冗余 packet.clear()
// （ORIG 无该 call）；⑦分支反转 if(party!=0){send_to_party}else{Send}（P59：ORIG je 落
// Send 块）。188=188，仅 .o 重定位占位差（AE）。
// ORIG 报告：docs/class_func_reports/game_master__CSecretShopNpc/execute.md。
void CSecretShopNpc::execute()
{
    CUser* user = GetUser();
    if (user->get_state() != 5)
    {
        return;
    }

    CParty* party = static_cast<CParty*>(user->GetParty());
    // ORIG CUser::GetSecretShopData 返回秘密商店数据块指针，首 int 为 npc id。
    int* shopData = static_cast<int*>(user->GetSecretShopData());
    if (shopData == 0)
    {
        return;
    }

    if (!(m_npcIdx == 1000 || m_npcIdx == 0x3ea || m_npcIdx == 0x3eb || m_npcIdx == 0x3ec))
    {
        return;
    }
    if (party != 0)
    {
        CDungeon* dungeon = party->m_battleField.m_pDungeon;
        if (dungeon != 0 && static_cast<signed char>(dungeon->m_bloodDungeonType) > 0)
        {
            return;
        }
    }

    *shopData = m_npcIdx;

    // ORIG：256 字节栈缓冲 rep stos 清零后 sprintf 韩文通告。
    char buf[0x100] = {0};
    sprintf(buf, "\xba\xf1\xb9\xd0\xbb\xf3\xc1\xa1 npc\xb0\xa1 %d\xb7\xce \xb1\xb3\xc3\xbc\xb5\xc7\xbe\xfa\xbd\xc0\xb4\xcf\xb4\xd9", *shopData);

    cMyTrace(__PRETTY_FUNCTION__, 0x73f, 0)(
        "[SECRETSHOP] change npc(%s, %d)", user->getCurCharacName(), *shopData);

    PacketGuard chatPacket;
    G_GameWorld()->make_packet_chat_msg(chatPacket, user, buf,
                                        static_cast<int>(strlen(buf)), 2,
                                        static_cast<ENUM_NOTIPACKET>(0xc));

    PacketGuard packet;
    packet.put_header(0, 0x114);
    packet.put_int(*shopData);
    packet.finalize(true);

    if (party != 0)
    {
        party->send_to_party(chatPacket);
        party->send_to_party(packet);
    }
    else
    {
        user->Send(chatPacket);
        user->Send(packet);
    }
}