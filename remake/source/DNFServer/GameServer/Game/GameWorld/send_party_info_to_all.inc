// [b28d-11b] N：槽位差 4（N82）@0x086c878a
// [b28d-solo-GameWorld] _ZN9GameWorld22send_party_info_to_allEP6CPartyi verdict=AE 309=309
// 修法：P0/P197④ 内联 noTitle 谓词（去具名 bool 落槽）+ P41/P162 删冗余 (short) cast
// （GetPartyIndex 返回 int，ORIG 无 cwtl）。
void GameWorld::send_party_info_to_all(CParty* party, int flag)
{
    int memberCount = party->get_member_count();
    if (!(memberCount > 0))   // ORIG +0x016 cmpl $0/jg 取反极性
    {
        LogManager::logFormat(
            1, "world.cpp", __PRETTY_FUNCTION__,
            0xc37, "GameWorld::send_party_info_to_all get_member_count() is %d",
            memberCount);
        return;
    }

    PacketGuard packet;
    packet.put_header(0, 9);
    packet.put_short(1);
    // [b28d-solo-GameWorld] P41/P162：ORIG 086c8822 `mov %eax,0x4(%esp)` 直传
    // （GetPartyIndex 返回 int，无 cwtl）⇒ 删冗余 (short) cast（4.4 恒多 1 条 cwtl）。
    packet.put_short(party->GetPartyIndex());
    packet.put_byte(flag);

    if (flag == 0 || flag == 1)
    {
        packet.put_byte((unsigned char)party->getTitleIndex());
        // [b28d-solo-GameWorld] P0/P197④：ORIG +0x0ec 第 2 次调用 getTitleIndex +
        // +0x0f1 `test %eax,%eax; sete %al; test %al,%al; je`（无槽）⇒ 去具名
        // noTitle（落槽 -0x25 多 2 条 mov/cmpb），内联谓词直判。
        if (party->getTitleIndex() == 0)
        {
            // ORIG getTitle×3 / strlen×2 逐访问点重取（P157）；第二 strlen 直落 %ebx（+0x12a，块作用域 register）
            packet.put_int(strlen(party->getTitle()));       // +0x100/+0x108/+0x117
            {
                register int titleLen = strlen(party->getTitle());   // +0x122/+0x12a → %ebx
                packet.put_str(party->getTitle(), titleLen);         // +0x137 getTitle 后于 strlen（P23）
            }
        }
        packet.put_byte((unsigned char)party->IsReturnUserParty());
        packet.put_byte((unsigned char)party->getUserMax());
        packet.put_short(party->getDungIndex());
        packet.put_byte((unsigned char)party->getDungDiffi());
        packet.put_byte((unsigned char)party->IsEventCharacParty());
    }

    if (flag == 0 || flag == 2)
    {
        int managerIdx = 0;
        for (int i = 0; i <= 3; i++)   // ORIG +0x2ad cmp $3（setle 底测形）
        {
            CUser* member = party->get_user(i);
            if (member != NULL)        // ORIG +0x21c：非 NULL 为 fall-through（块反转）
            {
                packet.put_short((unsigned short)member->get_unique_id());
                if (party->getManager() == member)
                {
                    managerIdx = i;
                }
                char sex = member->getSex();   // +0x25c 槽 spill（版本形）
                if (sex == -1)
                {
                    sex = 1;
                }
                packet.put_byte((int)sex);
            }
            else
            {
                packet.put_short(0xffff);   // +0x283
                packet.put_byte(0xff);
            }
        }
        packet.put_byte((unsigned char)party->IsReturnUserParty());
        packet.put_byte(managerIdx);
        packet.put_byte((unsigned char)party->is_quick_party());
        packet.put_byte((unsigned char)party->IsEventCharacParty());
    }

    if (flag == 0 || flag == 1 || flag == 2)
    {
        bool matching = is_dungeon_tag_matching_channel((char*)"[impossible]");
        packet.put_byte((unsigned char)matching);   // +0x34e..0x368 槽物化+movzbl 传参
        if (matching)
        {
            for (int i = 0; i <= 3; i++)
            {
                CUserCharacInfo* info = (CUserCharacInfo*)party->get_user(i);
                if (info != NULL)        // +0x393 非 NULL fall-through（反转）
                {
                    packet.put_byte(6);
                    for (int j = 0; j <= 5; j++)   // +0x403 cmp $5
                    {
                        packet.put_byte((int)G_CDataManager()->get_dimensionInout(j));
                        // ORIG +0x3e2 movzbl 0xeb9(零扩展) 后 +0x3ed movsbl %al（符号再扩展）⇒ (int)(char)
                        packet.put_byte((int)(char)info->getCurCharacR()->m_demensionInout[j]);
                    }
                }
                else
                {
                    packet.put_byte(0);   // +0x410
                }
            }
        }
    }

    packet.finalize(true);
    send_all(packet);
}