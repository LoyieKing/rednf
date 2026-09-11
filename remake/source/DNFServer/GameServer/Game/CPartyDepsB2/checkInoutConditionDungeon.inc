// ============================================================================
// CParty::checkInoutConditionDungeon —— ORIG 0x085abc80（三参重载，单出口）
// 返回错误码（0=通过）；错误路径同步向全队发包。
// ============================================================================
// [b28d-30] 复核 DIFF：巨件残差（N82/N89 族），ORIG 0x085abc80。
ENUM_ERROR CParty::checkInoutConditionDungeon(CDungeon const* dungeon,
                                       ENUM_DUNGEON_MODE mode,
                                       ENUM_CMDPACKET cmd)
{
    int ret;                                            // ebx（单出口寄存）
    if (dungeon == 0)
        return static_cast<ENUM_ERROR>(1);

    PacketGuard packet;                                 // ORIG -0x28
    unsigned char ancientFlag = 0;                      // -0x29（ORIG 先存 flag 后构 packet）
    Inven_Item ancientItems[4];                         // -0x140（元素步长 0x3d）
    memset(ancientItems, 0, 0xf4);                      // ORIG ctor 后整体清零
    int itemParams[4];                                  // -0x3c
    memset(itemParams, 0, sizeof(itemParams));

    if (cmd == (ENUM_CMDPACKET)0x1bc)
    {
        int discard;                                    // 结果弃置（ORIG 同形死码测试）
        for (int i = 0; i <= 3; ++i)                    // -0x18
            discard = !checkValidUser(i);
        (void)discard;

        for (int j = 0; j <= 3; ++j)                    // -0x14
        {
            if (!checkValidUser(j))
                continue;
            // 疲劳 != 'd'(100) → 广播 0xea 错误
            if (m_member[j].m_pUser->getCurCharacStamina() != 100)
            {
                packet.clear();
                packet.put_header(1, 0x1bc);
                packet.put_byte(0);
                packet.put_byte(0xea);
                packet.put_byte((int)ancientFlag);
                packet.finalize(true);
                send_to_party(packet);
                ret = 0xea;
                goto DONE;
            }
        }
    }

    if (!CheckEnterAncientDungeon(dungeon, ancientFlag, ancientItems, itemParams))
    {
        packet.clear();
        packet.put_header(1, (int)cmd);
        packet.put_byte(0);
        packet.put_byte(0x11);
        packet.put_byte((int)ancientFlag);
        packet.finalize(true);
        send_to_party(packet);
        ret = 0x11;
        goto DONE;
    }

    if (dungeon->get_index() != 0)
    {
        if (!UseAncientDungeonItems(dungeon, ancientItems, itemParams))
        {
            PacketGuard failPacket;                     // -0x4c
            failPacket.put_header(1, 0x10);
            failPacket.put_byte(0);
            failPacket.put_int(1);
            failPacket.finalize(true);
            send_to_party(failPacket);
            ret = 0x11;
            goto DONE;
        }
    }

    if ((signed char)dungeon->get_dimension_possible() > 0)
    {
        ancientFlag = 0;
        int dimErr = CheckEnterDimensionDungeon(dungeon, ancientFlag, true);  // -0x10
        if (dimErr == 0xac)
        {
            packet.clear();
            packet.put_header(1, (int)cmd);
            packet.put_byte(0);
            packet.put_byte(0xac);
            packet.put_byte((int)ancientFlag);
            packet.finalize(true);
            send_to_party(packet);
            ret = 0xac;
            goto DONE;
        }
        if (dimErr == 0xad)
        {
            packet.clear();
            packet.put_header(1, (int)cmd);
            packet.put_byte(0);
            packet.put_byte(0xad);
            packet.put_byte((int)ancientFlag);
            packet.finalize(true);
            send_to_party(packet);
            ret = 0xad;
            goto DONE;
        }
        if (dimErr == 0xab)
        {
            packet.clear();
            packet.put_header(1, (int)cmd);
            packet.put_byte(0);
            packet.put_byte(0xab);
            packet.finalize(true);                      // 不带 flag 字节
            send_to_party(packet);
            ret = 0xab;
            goto DONE;
        }
        if (!setStandardDimensionLevel(dungeon))
        {
            packet.clear();
            packet.put_header(1, (int)cmd);
            packet.put_byte(0);
            packet.put_byte(0xab);
            packet.finalize(true);
            send_to_party(packet);
            ret = 0xab;
            goto DONE;
        }
    }

    {
        int startErr = 0;                               // -0x1c
        int startSlot = 0;                              // -0x40
        if (mode != (ENUM_DUNGEON_MODE)1)
        {
            startErr = check_dungeon_start(dungeon, startSlot);
            if (startErr != 0)
            {
                // 错误码分流（ORIG 比较树）：带槽 {10,0x5f,0xf5,0xf8}；
                // 不带槽 {0xf3..0xf4,0xf6..0xf7}；其余落入锦标赛兜底。
                if (startErr > 0xf4)
                {
                    if (startErr > 0xf7)
                    {
                        if (startErr == 0xf8)
                            goto EMIT_SLOT_ERR;
                        goto TOURNAMENT_CHECK;
                    }
                    if (startErr >= 0xf6)
                        goto EMIT_PLAIN_ERR;
                    goto EMIT_SLOT_ERR;                 // ==0xf5
                }
                if (startErr >= 0xf3)
                    goto EMIT_PLAIN_ERR;                // 0xf3/0xf4
                if (startErr == 10 || startErr == 0x5f)
                    goto EMIT_SLOT_ERR;
                goto TOURNAMENT_CHECK;

EMIT_SLOT_ERR:
                packet.clear();
                packet.put_header(1, (int)cmd);
                packet.put_byte(0);
                packet.put_byte(startErr);
                packet.put_byte(startSlot);
                packet.finalize(true);
                send_to_party(packet);
                ret = startErr;
                goto DONE;

EMIT_PLAIN_ERR:
                packet.clear();
                packet.put_header(1, (int)cmd);
                packet.put_byte(0);
                packet.put_byte(startErr);
                packet.finalize(true);
                send_to_party(packet);
                ret = startErr;
                goto DONE;
            }
        }
    }

TOURNAMENT_CHECK:
    // 锦标赛图（0x78/0x79）：人数≥2 或单参模式时报错日志并广播 0xf3
    if (dungeon->get_index() == 0x78 || dungeon->get_index() == 0x79)
    {
        if (get_member_count() >= 2 || mode == (ENUM_DUNGEON_MODE)1)
        {
            for (int t = 0; t <= 3; ++t)                // -0xc
            {
                if (!_checkValidUser(t))
                    continue;
                LogManager::logFormat(
                    1, "party.cpp", __PRETTY_FUNCTION__, 0x25eb,
                    "TOURNAMENT_DUNGEON_ERROR_LOG : CHARAC_NO (%d) MODE (%d)",
                    m_member[t].m_pUser->getCurCharacNo(), (int)mode);
            }
            packet.clear();
            packet.put_header(1, (int)cmd);
            packet.put_byte(0);
            packet.put_byte(0xf3);
            packet.finalize(true);
            send_to_party(packet);
            ret = 1;
            goto DONE;
        }
    }
    ret = 0;

DONE:
    return static_cast<ENUM_ERROR>(ret);
}