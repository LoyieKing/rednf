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
    sprintf(buf, "비밀상점 npc가 %d로 교체되었습니다", *shopData);

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
