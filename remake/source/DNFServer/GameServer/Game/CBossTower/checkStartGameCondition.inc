bool CBossTower::checkStartGameCondition(unsigned char& flag, bool useItems)
{
    Inven_Item items[4];                       // ebp-0x168 起 4*0x3d，整体清零 0xf4
    memset(items, 0, 0xf4);
    int itemParams[4];                         // ebp-0x44 起 0x10 字节清零
    memset(itemParams, 0, sizeof(itemParams));

    if (m_dungeon == 0 || !m_dungeon->IsOpenScheduleDungeon())
    {
        cMyTrace trace("bool WongWork::CBossTower::checkStartGameCondition("
                       "unsigned char&, bool)", 0xdb, 5);
        trace("boss Tower dungeon is not open");
        PacketGuard packet;
        packet.clear();              // ORIG 0x0814309a（本分支 C1 后显式 clear）
        packet.put_header(1, 0x10);
        packet.put_byte(0);
        packet.put_int(0x15);
        packet.finalize(true);
        m_party->send_to_party(packet);
        return 0;
    }

    for (int i = 0; i <= 3; ++i)
    {
        CUser* member = m_party->get_user(i);
        // ORIG 0x081431b6..+0x1e3：正向包裹（P214①）——`member != 0 &&
        // checkValidUser(i)` 产成功物化岛 `mov$1;jmp/mov$0 + test %al; je
        // <循环尾>`；`|| !f()` 的 continue 形在 4.4 多一条 `xor $1`。
        if (member != 0 && m_party->checkValidUser(i))
        {
            if (member->get_charac_level() < m_dungeon->get_min_level())
            {
                // P227 全表达式临时链：ORIG 0x081431e9..+0x209 在 ctor call 前
                // 右起求值 varargs（charac_level→%ebx、min_level→%esi 跨 ctor 保活）。
                cMyTrace("bool WongWork::CBossTower::checkStartGameCondition("
                         "unsigned char&, bool)", 0xf0, 5)(
                    "dungeon level check charac_level : %d, dungeon_min_level : %d",
                    member->get_charac_level(), m_dungeon->get_min_level());
                PacketGuard packet;
                packet.clear();      // ORIG 0x08143229（本分支 C1 后显式 clear）
                packet.put_header(1, 0x10);
                packet.put_byte(0);
                packet.put_int(0xe);
                packet.finalize(true);
                m_party->send_to_party(packet);
                return 0;
            }
        }
    }

    if (m_party->m_battleField.m_bloodState == 1)
    {
        return 1;
    }

    // ORIG 0x08143328/0x0814342f：两处调用结果共用 -0x11 具名 bool 槽
    // （`mov %al,-0x11; movzbl; xor $1; test; je`）⇒ 源码是同一个 `bool ok`
    // 局部两次赋值（P158 形），第二次用 if(useItems) 包裹而非 `&&` 短路。
    bool ok = m_party->CheckEnterAncientDungeon(m_dungeon, flag, items, itemParams);
    if (!ok)
    {
        PacketGuard packet;
        packet.put_header(1, 0x10);
        packet.put_byte(0);
        packet.put_int(0x11);
        packet.put_byte(static_cast<unsigned char>(flag));
        packet.finalize(true);
        m_party->send_to_party(packet);
        return 0;
    }

    if (useItems)
    {
        ok = m_party->UseAncientDungeonItems(m_dungeon, items, itemParams);
        if (!ok)
        {
            PacketGuard packet;
            packet.put_header(1, 0x10);
            packet.put_byte(0);
            packet.put_int(1);
            packet.finalize(true);
            m_party->send_to_party(packet);
            return 0;
        }
    }
    return 1;
