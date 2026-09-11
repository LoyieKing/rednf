void WarField::ConsistMap(void* packet, const CDungeon* dungeon, const CMap* map,
                          byte a, int playerCount)
{
    if (playerCount == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x3b, 5);
        trace("WarField::ConsistMap playerCount is 0");
        return;
    }
    void* buf = packet;
    ((InterfacePacketBuf*)buf)->put_header(0, 0x5b);
    ((InterfacePacketBuf*)buf)->put_byte(dungeon->m_warRoomData.m_d8);  // +0x6f4+0xd8
    int index = ((InterfacePacketBuf*)buf)->get_index();
    ((InterfacePacketBuf*)buf)->put_short(0);
    int uid = m_mapInfo.m_assignItemIdx;
    // +0x6f4+0x44；ORIG 086b8b19 add $0x738 后调 const 版 operator[]（p167 实证）。
    int monsterCount =
        (int)(dungeon->m_warRoomData.m_vec44[m_stageIdx] * playerCount) / 6;
    int minCount = 10;
    monsterCount = std::max(monsterCount, minCount);
    if (m_mapInfo.m_monsterMap.size() != 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x53, 5);
        // [M3S1 §6.5/§6.5.1 语义轴] ORIG .rodata 0x08cf85a0 实字节 =
        //   "WarField::ConsistMap, " + EUC-KR（"이미 몬스터가 존재합니다."）
        //   原重建串为英文发明文本 ⇒ 换回 ORIG 字节（源码保持 UTF-8，用 \x 转义）。
        trace("WarField::ConsistMap, \xc0\xcc\xb9\xcc \xb8\xf3\xbd\xba\xc5\xcd\xb0\xa1 \xc1\xb8\xc0\xe7\xc7\xd5\xb4\xcf\xb4\xd9.");
        m_mapInfo.Clear();
    }
    while (monsterCount > 9)
    {
        int totalProb = 0;
        int i = 0;
        for (; i <= 3; i = i + 1)
        {
            totalProb = totalProb +
                dungeon->m_warRoomData.m_spawnProb[m_stageIdx].m_probability[i];
        }
        if (totalProb == 0)
        {
            LogManager::logFormat(
                1, "WarField.cpp",
                __PRETTY_FUNCTION__,
                0x65, "WARROOM::SpawnMonster : \xb8\xf3\xbd\xba\xc5\xcd\xc5\xb8\xc0\xd4\xc8\xae\xb7\xfc\xc0\xc7 \xc3\xd1\xc7\xd5\xc0\xcc 0\xc0\xd4\xb4\xcf\xb4\xd9");
            return;
        }
        int randProb = get_rand_int(totalProb);
        totalProb = 0;
        i = 0;
        for (; i <= 3; i = i + 1)
        {
            totalProb = totalProb +
                dungeon->m_warRoomData.m_spawnProb[m_stageIdx].m_probability[i];
            if (randProb < totalProb)
            {
                break;
            }
        }
        int type = i;
        int totalWeight = 0;
        // +0x6f4+0x14+type*0xc（ORIG 每使用点重算地址，P183 禁引用缓存）
        int listSize = (int)dungeon->m_warRoomData.m_vec14[type].size();
        int weights[256];
        for (i = 0; listSize > i; i = i + 1)
        {
            CMonster* monster = (CMonster*)((const CDataManager*)G_CDataManager())
                                    ->find_monster(dungeon->m_warRoomData.m_vec14[type].at(i));
            if (monster == 0)
            {
                LogManager::logFormat(
                    1, "WarField.cpp",
                    __PRETTY_FUNCTION__,
                    0x96, "WARROOM Find Monster[%d] Error",
                    dungeon->m_warRoomData.m_vec14[type].at(i));
                return;
            }
            weights[i] = monster->m_weight;               // CMonster +0x1d24
            totalWeight = totalWeight + weights[i];
        }
        if (totalWeight == 0)
        {
            LogManager::logFormat(
                1, "WarField.cpp",
                __PRETTY_FUNCTION__,
                0x9f, "WARROOM::SpawnMonster : \xc5\xb8\xc0\xd4(%d)\xc0\xc7 \xb8\xf3\xbd\xba\xc5\xcd\xb8\xae\xbd\xba\xc6\xae \xc8\xae\xb7\xfc\xc0\xc7 \xc3\xd1\xc7\xd5\xc0\xcc 0\xc0\xd4\xb4\xcf\xb4\xd9", type);
            return;
        }
        randProb = get_rand_int(totalWeight);
        totalWeight = 0;
        i = 0;
        for (; listSize > i; i = i + 1)
        {
            totalWeight = totalWeight + weights[i];
            if (randProb < totalWeight)
            {
                break;
            }
        }
        int sel = i;
        CMonster* monster = (CMonster*)((const CDataManager*)G_CDataManager())
                                ->find_monster(dungeon->m_warRoomData.m_vec14[type].at(sel));
        if (monster == 0)
        {
            LogManager::logFormat(
                1, "WarField.cpp",
                __PRETTY_FUNCTION__,
                0xbc, "WARROOM::ConsistMap pMob is Null");
            return;
        }
        int rivalTotal = 0;
        // CMap +0x90（ORIG 每使用点重算地址）
        for (int i = 0; (int)map->m_rivalData1.size() > i; i = i + 1)
        {
            rivalTotal = rivalTotal + map->m_rivalData1[i].m_field08;
        }
        if (rivalTotal == 0)
        {
            LogManager::logFormat(
                1, "WarField.cpp",
                __PRETTY_FUNCTION__,
                0xd0, "WARROOM::SpawnMonster : \xbd\xba\xc6\xf9\xc0\xa7\xc4\xa1\xc8\xae\xb7\xfc\xc0\xc7 \xc3\xd1\xc7\xd5\xc0\xcc 0\xc0\xd4\xb4\xcf\xb4\xd9");
            return;
        }
        randProb = get_rand_int(rivalTotal);
        rivalTotal = 0;
        i = 0;
        while ((int)map->m_rivalData1.size() > i)
        {
            rivalTotal = rivalTotal + map->m_rivalData1[i].m_field08;
            if (randProb < rivalTotal)
            {
                break;
            }
            i = i + 1;
        }
        int rivalSel = i;
        map_monster mob;
        mob.m_instId = uid;
        uid = uid + 1;
        mob.m_roleType = (char)type;
        mob.m_mobIndex = monster->get_index();
        mob.m_level = a;
        *(char*)&mob.m_field11 = 1;         // 字节 +0x11（ORIG 逐字节写，非 movw）
        *((char*)&mob.m_field11 + 1) = 1;   // 字节 +0x12
        mob.m_field18 =
            dungeon->m_warRoomData.m_bc * (float)(6 - playerCount) + 1.0f;   // +0x7b0
        mob.m_field28 =
            dungeon->m_warRoomData.m_c0 * (float)(6 - playerCount) + 1.0f;   // +0x7b4
        m_mapInfo.Add_Mob(mob);
        monsterCount =
            monsterCount - dungeon->m_warRoomData.m_c4[type];                // +0x7b8+type*4
        ((InterfacePacketBuf*)buf)->put_short(mob.m_instId);
        ((InterfacePacketBuf*)buf)->put_byte(type);
        ((InterfacePacketBuf*)buf)->put_byte((int)a);
        ((InterfacePacketBuf*)buf)->put_byte(sel);
        ((InterfacePacketBuf*)buf)->put_byte(rivalSel);
        int rewardCount = dungeon->m_warRoomData.m_ec +                        // +0x7e0
                          get_rand_int(dungeon->m_warRoomData.m_f0);           // +0x7e4
        rewardCount = (int)((float)rewardCount *
                            dungeon->m_warRoomData.m_dc[type]);                // +0x7d0+type*4
        if (playerCount == 0)
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x107, 5);
            trace("WarField::ConsistMap playerCount is 0");
            playerCount = 1;
        }
        ((InterfacePacketBuf*)buf)->put_short(
            (int)((double)rewardCount / (0.3 * (double)(playerCount - 1) + 1.0)));
    }
    m_mapInfo.m_assignItemIdx = uid;
    m_stageIdx = m_stageIdx + 1;
    ((InterfacePacketBuf*)buf)->put_short(index, (int)m_mapInfo.m_monsterMap.size());
    ((InterfacePacketBuf*)buf)->finalize(true);
}