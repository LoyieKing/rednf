void CAssaultPlace::_SendHoldVictory(CUser* userA, CUser* userB, int winTeam)
{
    PacketGuard packet;
    packet.put_header(0, 0x109);

    packet.put_byte((int)((CUser*)userA)->getPowerSide());
    packet.put_int(winTeam + 1);

    CParty* partyA = (CParty*)((CUser*)userA)->GetParty();
    // [b28d-solo-W6_AssaultPlaceDepsA] ORIG 0x082ecb60 `cmpl $0x0,-0x40; je +0x10e`
    //   ⇒ 非零（有队）走 fall-through 有队分支、零落 +0x10e 无队块（P59 正极性）。
    if (partyA != 0) {
        packet.put_byte(partyA->get_member_count());
        // [b28d-solo-W6_AssaultPlaceDepsA] N82：ORIG 循环变量槽 s1=-0x3c/s2=-0x20
        //   （4.1 生命周期序）；4.4 按首次使用序（-0x38 共用 / 头部声明 -0x40/-0x3c），
        //   实测两形均不闭合 ⇒ 槽序封顶（member/name/len 随之下移 4）。
        for (int s = 0; s < 4; ++s) {
            CUser* member = (CUser*)partyA->get_user(s);
            if (member != 0) {
                const char* name =
                    ((CUserCharacInfo*)member)->getCurCharacName();
                // P0：ORIG strlen 结果落具名槽 -0x30，put_int/put_str 复用同一槽。
                size_t len = strlen(name);
                packet.put_int((int)len);
                packet.put_str(name, (int)len);
            }
        }
    } else {
        packet.put_byte(1);
        const char* name =
            ((CUserCharacInfo*)userA)->getCurCharacName();
        size_t len = strlen(name);
        packet.put_int((int)len);
        packet.put_str(name, (int)len);
    }

    CParty* partyB = (CParty*)((CUser*)userB)->GetParty();
    if (partyB != 0) {
        packet.put_byte(partyB->get_member_count());
        for (int s = 0; s < 4; ++s) {
            CUser* member = (CUser*)partyB->get_user(s);
            if (member != 0) {
                const char* name =
                    ((CUserCharacInfo*)member)->getCurCharacName();
                size_t len = strlen(name);
                packet.put_int((int)len);
                packet.put_str(name, (int)len);
            }
        }
    } else {
        packet.put_byte(1);
        const char* name =
            ((CUserCharacInfo*)userB)->getCurCharacName();
        size_t len = strlen(name);
        packet.put_int((int)len);
        packet.put_str(name, (int)len);
    }

    packet.finalize(true);
    G_GameWorld()->send_to_vill(7, packet);
