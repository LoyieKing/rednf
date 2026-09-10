void CBattle_Field::MakePacket(int packetId, void* packet)
{
    InterfacePacketBuf* pkt = (InterfacePacketBuf*)packet;

    // ORIG 怪物序列化首个 put_short 读取未初始化栈槽 [ebp-0x114]
    // （报告 asm 0x8309368 为全函数唯一引用、无任何写入点）。以未初始化局部
    // 变量复现同一行为。
    short staleStackValue;

    switch (packetId)
    {
    case 0x1d:
    {
        pkt->put_header(0, 0x1d);
        int posX = -1;
        int posY = -1;
        getCurPosXY(posX, posY);
        pkt->put_byte(posX);
        pkt->put_byte(posY);
        // ORIG @0xd8-0xea：两处独立 put_byte 调用点（then 落 $1、else 落 $0），
        // 非三目合并（p167 MISSING put_byte×1 即此）。
        if (GetCurrentMapInfo()->m_layeredMap != 0)                          // +0xe8
            pkt->put_byte(1);
        else
            pkt->put_byte(0);
        pkt->put_int(GetCurrentMapInfo()->m_mapBasisLevel);              // +0x48
        pkt->put_byte((char)m_hellPartyDifficulty);          // +0x1cc
        pkt->put_byte((char)m_hellPartyEventType);           // +0x1cd
        if (GetCurrentMapInfo()->m_visited == '\0')          // +0x01
        {
            pkt->put_byte((char)GetCurrentMapInfo()->m_mapType);         // +0x44
            pkt->put_short(GetCurrentMapInfo()->m_mapIndex);             // +0x04
            pkt->put_byte((int)GetCurrentMapInfo()->m_monsterMap.size());  // +0x0c

            map_monster mob;
            for (std::map<int, map_monster>::iterator it =
                     GetCurrentMapInfo()->m_monsterMap.begin();
                 it != GetCurrentMapInfo()->m_monsterMap.end(); it++)
            {
                mob = it->second;
                pkt->put_short(staleStackValue);             // 未初始化槽复现
                pkt->put_int(mob.m_mobId);                   // +0x00
                pkt->put_short((short)mob.m_instId);         // +0x04 低 16 位
                pkt->put_int(mob.m_mobIndex);                // +0x0c
                pkt->put_byte((unsigned char)mob.m_level);   // +0x10（movzbl）
                if (mob.m_flag34 != 0)                       // +0x34
                    pkt->put_byte(3);
                else
                    pkt->put_byte((char)mob.m_roleType);     // +0x08
                pkt->put_byte((unsigned char)mob.m_field13); // +0x13
                pkt->put_byte((unsigned char)mob.m_field14); // +0x14
            }

            pkt->put_byte((int)GetCurrentMapInfo()->m_itemMap.size());   // +0x24
            map_item item;
            for (std::map<int, map_item>::iterator it =
                     GetCurrentMapInfo()->m_itemMap.begin();
                 it != GetCurrentMapInfo()->m_itemMap.end(); it++)
            {
                item = it->second;                           // 21 dword 复制
                pkt->put_byte((char)item.m_standardLevel);       // +0x01
                pkt->put_short(item.m_itemIndex);                // +0x04
                pkt->put_int(item.m_item.m_addInfo);             // +0x12
                pkt->put_int(item.m_item.get_add_info());        // +0x10 对象
                pkt->put_short(item.m_item.m_fieldb);            // +0x1b
                pkt->put_byte(item.m_item.m_amp.getAbilityType());   // +0x21
                pkt->put_short(item.m_item.m_amp.getAbilityValue());
            }
            pkt->put_byte((unsigned char)m_hellPartyActive);     // +0x1b8
            CheckAppearRidableObject(reinterpret_cast<PacketGuard*>(pkt));
            if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap())
                pkt->put_byte(m_curSeatNo);                      // +0x114
            else
                pkt->put_byte(0xff);
            pkt->finalize(true);
        }
        else
        {
            pkt->put_byte(0);
            CheckAppearRidableObject(reinterpret_cast<PacketGuard*>(pkt));
            if (m_pDungeon != 0 && m_pDungeon->IsEnterEachMap())
                pkt->put_byte(m_curSeatNo);
            else
                pkt->put_byte(0xff);
            pkt->finalize(true);
        }
        break;
    }
    case 0x1c:
    {
        pkt->put_byte(m_mazeType);                         // +0x1ac
        pkt->put_byte(m_endX);                             // +0x0c
        pkt->put_byte(m_endY);                             // +0x10
        pkt->put_byte(m_hellPosX);                         // +0x1c0
        pkt->put_byte(m_hellPosY);                         // +0x1c4
        pkt->put_byte(m_bloodState);                       // +0x1b4
        pkt->put_byte(m_ridable.GetObjectCnt());
        std::vector<std::pair<int, int> > ridablePos;
        m_ridable.GetObjectPos(ridablePos);
        // ORIG @0x79d 尾测 `call size; cmp i,size; setg`（有符号比较）⇒
        // 索引 int 化且比较写 size > i。
        for (int i = 0; (int)ridablePos.size() > i; ++i)
        {
            pkt->put_byte(ridablePos[i].first);
            pkt->put_byte(ridablePos[i].second);
        }

        CUser* manager = 0;
        TOD_UserState* todState = 0;
        unsigned short todayLayer = 0;                     // -0x46（ushort 写）
        if (m_party != 0 && (manager = m_party->getManager()) != 0)
        {
            todState = reinterpret_cast<TOD_UserState*>(
                manager->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xd));
            TOD_Layer layer = todState->getTodayEnterLayer();
            todayLayer = layer.GetLayer();
        }
        pkt->put_short(todayLayer);
        if (m_party != 0)
                    pkt->put_byte(m_party->get_random_buff_type());
                else
                    pkt->put_byte(0xb);
        pkt->finalize(true);
        break;
    }
    case 0xd3:
    {
        pkt->put_header(0, 0xd3);
        int posX = -1;
        int posY = -1;
        getCurPosXY(posX, posY);
        pkt->put_byte(posX);
        pkt->put_byte(posY);
        pkt->put_int(GetCurrentMapInfo()->m_mapBasisLevel);  // +0x48
        pkt->put_byte(0);
        if (GetCurrentMapInfo()->m_visited == '\0')          // +0x01
        {
            pkt->put_byte((char)GetCurrentMapInfo()->m_mapType);       // +0x44
            pkt->put_short(GetCurrentMapInfo()->m_mapIndex);            // +0x04
            const CDataManager* dataMgr = G_CDataManager();
            // ORIG @0x9ff：结果先落具名槽 -0x44 再槽比较（`cmpl $0,-0x44; jne`），
            // 内联实参形 4.4 产 test/sete/test/je（+2）。
            void* mapData = dataMgr->find_map(GetCurrentMapInfo()->m_mapIndex);
            if (mapData == 0)
            {
                LogManager::logFormat(
                    1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x1746,
                    "Find Map[%d] Error(In Map Maker)(%d/%d)",
                    GetCurrentMapInfo()->m_mapIndex, m_curX, m_curY);
                // 错误路径不 finalize，直接返回
            }
            else
            {
                pkt->put_byte(0);
                pkt->put_byte(0);
                pkt->put_byte(0);
                pkt->finalize(true);
            }
        }
        else
        {
            pkt->put_byte(0);
            pkt->finalize(true);
        }
        break;
    }
    case 0xd4:
    {
        pkt->put_byte(m_mazeType);
        pkt->put_byte(m_endX);
        pkt->put_byte(m_endY);
        pkt->put_byte(0);
        pkt->put_byte(0);
        pkt->put_byte(m_bloodState);
        if (m_party != 0)
                    pkt->put_byte(m_party->get_random_buff_type());
                else
                    pkt->put_byte(0xb);
        pkt->finalize(true);
        break;
    }
    case 0x182:
    {
        char memberCount = (char)m_party->get_member_count();
        pkt->put_byte(memberCount);                        // movsbl
        MapInfo* mapInfo = GetCurrentMapInfo();            // ORIG 缓存 map ptr
        std::map<int, map_monster>::iterator mobIt =
            mapInfo->m_monsterMap.begin();
        for (char round = 1; round <= 4; ++round)
        {
            pkt->put_byte(round);
            short roundKey = round;                        // -0x62 short 槽
            std::map<short, std::vector<TournamentInfo> >::iterator roundIt =
                m_tournamentRoundMap.find(roundKey);       // +0x13c
            std::vector<TournamentInfo>& infos = roundIt->second;
            pkt->put_byte((int)infos.size());
            for (unsigned int j = 0; j < infos.size(); ++j)
            {
                pkt->put_byte(infos[j].m_roundOrder);
                for (int k = 0; k < memberCount; ++k)
                {
                    pkt->put_int(infos[j].m_monster[k].m_mobId);
                    pkt->put_short(infos[j].m_monster[k].m_rate);
                }
            }
        }
        for (char round2 = 1; round2 <= 4; ++round2)
        {
            pkt->put_byte(round2);
            int written = 0;
            while (written < memberCount &&
                   mobIt != mapInfo->m_monsterMap.end())
            {
                const map_monster& mob = mobIt->second;
                pkt->put_short(mob.m_instId);
                pkt->put_int(mob.m_mobIndex);
                pkt->put_byte((unsigned char)mob.m_level);
                pkt->put_byte((char)mob.m_roleType);
                ++written;
                mobIt++;
            }
        }
        pkt->finalize(true);
        break;
    }
    case 0x183:
    {
        pkt->put_header(0, 0x183);
        int posX = -1;
        int posY = -1;
        getCurPosXY(posX, posY);
        pkt->put_byte(posX);
        pkt->put_byte(posY);
        pkt->put_int(GetCurrentMapInfo()->m_mapBasisLevel);  // +0x48
        pkt->put_byte(0);
        if (GetCurrentMapInfo()->m_visited == '\0')          // +0x01
        {
            pkt->put_byte((char)GetCurrentMapInfo()->m_mapType);       // +0x44
            pkt->put_short(GetCurrentMapInfo()->m_mapIndex);            // +0x04
            const CDataManager* dataMgr = G_CDataManager();
            // ORIG @0xf8a：同 0xd3 分支（结果落具名槽 -0x1c 后槽比较）。
            void* mapData = dataMgr->find_map(GetCurrentMapInfo()->m_mapIndex);
            if (mapData == 0)
            {
                LogManager::logFormat(
                    1, "battle_field.cpp", __PRETTY_FUNCTION__, 0x17d2,
                    "Find Map[%d] Error(In Map Maker)(%d/%d)",
                    GetCurrentMapInfo()->m_mapIndex, m_curX, m_curY);
                // 错误路径不 finalize，直接返回
            }
            else
            {
                pkt->put_byte(0);
                pkt->put_byte(0);
                pkt->put_byte(0);
                pkt->finalize(true);
            }
        }
        else
        {
            pkt->put_byte(0);
            pkt->finalize(true);
        }
        break;
    }
    default:
        break;
    }
