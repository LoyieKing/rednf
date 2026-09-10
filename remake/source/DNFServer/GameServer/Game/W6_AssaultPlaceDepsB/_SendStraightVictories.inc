void CAssaultPlace::_SendStraightVictories(CUser* user)
{
    PacketGuard packet;
    unsigned short victories = 0;
    int threshold = G_CDataManager()->m_powerParameter.m_straightVictories;

    if (user != 0) {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0) {
            party->incStraightVictories();
            victories = party->getStraightVictories();
            if (threshold <= static_cast<int>(victories)) {
                packet.put_header(0, 0xe0);
                packet.put_byte(party->get_member_count());
                for (int i = 0; i <= 3; ++i) {
                    CUser* member = party->get_user(i);
                    // b28 BL6: guard-continue 岛形——ORIG 0x82ec61f je→nop 岛 +
                    // 体尾前向 jmp 统一尾（4.4 直落形被折叠，continue 岛复现）。
                    if (member == 0)
                        continue;
                    const char* name = member->getCurCharacName();
                    int len = static_cast<int>(strlen(name));
                    packet.put_int(len);
                    packet.put_str(name, len);
                }
                packet.put_int(victories & 0xffff);
                packet.put_byte(static_cast<int>(static_cast<signed char>(user->getPowerSide())));
                packet.finalize(true);
                G_GameWorld()->send_to_vill(7, packet);
            }
        } else {
            user->incStraightVictories();
            victories = user->getStraightVictories();
            if (threshold <= static_cast<int>(victories)) {
                packet.put_header(0, 0xe0);
                packet.put_byte(1);
                const char* name = user->getCurCharacName();
                int len = static_cast<int>(strlen(name));
                packet.put_int(len);
                packet.put_str(name, len);
                packet.put_int(victories & 0xffff);
                packet.put_byte(static_cast<int>(static_cast<signed char>(user->getPowerSide())));
                packet.finalize(true);
                G_GameWorld()->send_to_vill(7, packet);
            }
        }
    }
