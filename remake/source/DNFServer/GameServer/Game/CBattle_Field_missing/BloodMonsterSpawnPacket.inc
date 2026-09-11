// ---------------------------------------------------------------------------
// CBattle_Field::BloodMonsterSpawnPacket(int cnt, int time, short smallRound,
//                                        short nextRound)
//   —— ORIG T 0x08305992（_ZN13CBattle_Field23BloodMonsterSpawnPacketEiiss）：
//   遍历当前图怪物表，收集 bloodSmallRoundOrder==smallRound 且未参战（killFlag!=1）
//   的怪物组包（header 0xd2）发送；数量不足则 logFormat 错误。
// ---------------------------------------------------------------------------
void CBattle_Field::BloodMonsterSpawnPacket(int cnt, int time, short smallRound,
                                            short nextRound)
{
    // [round5 收口实测] ndiff=22（231/229）：ORIG 有 `mov $0x0/$0x1,%ebx` + 尾
    // `test %ebx,%ebx`（bool 结果寄存器物化），本侧为 `jmp <尾块>`；另两处 EH cleanup
    // 尾块相对序（我们多 PacketGuard/map_monster C1Ev/D1Ev）⇒ N91/N104 族。
    // p167：MISSING `map_monsterC2Ev` = P93 同址别名伪差，其余 CLEAR（无语义缺口）。
    // [b28d-solo-CBattle_Field_missing] ORIG 0x08305992 逐条转写：
    // ①putCount(-0xc) 声明在 PacketGuard 之前；
    // ②ORIG 在 put_short(cnt) 之后构造一个全程未被读写的局部 map_monster
    //   （-0x370，仅 C1Ev/D1Ev + 撑帧 0x3a0）⇒ 照抄该原码遗留（P171/P232）；
    // ③无 mapInfo 具名局部、无判空：begin/end 处各自重调 GetCurrentMapInfo
    //   （P0/P140/P157）；④it->second 逐字段重读 operator->()，禁引用缓存；
    // ⑤m_pDungeon 无判空（ORIG 直解引用 +0x188）；⑥增量段 ORIG 调后置
    //   operator++（ppEi，sret 临时 -0x10）⇒ 写 it++。
    int putCount = 0;
    PacketGuard packet;
    packet.put_header(0, 0xd2);
    packet.put_short(cnt);
    map_monster unusedMonster;   // ORIG 0x083059f0 C1Ev / 0x08305cd4 D1Ev
    (void)unusedMonster;
    // ORIG 的 begin() sret 直构进 it 槽（-0x24），无 iterator 默认构造
    // ⇒ 初始化式（本处非 P50「先声明后赋值」形）。
    std::map<int, map_monster>::iterator it =
        GetCurrentMapInfo()->m_monsterMap.begin();
    for (; it != GetCurrentMapInfo()->m_monsterMap.end(); it++)
    {
        if (putCount == cnt)
            break;
        // ORIG 0x08305a48/0x08305a61：两条件各自失败落同一 L_false 并物化
        // `mov $1/%eax … mov $0/%eax; test; je <loop_tail>`（P214① 汇聚块）
        // ⇒ 用 P226 语句表达式条件（零槽短路物化 + 假路径直落 loop_tail）。
        if (({ it->second.m_bloodSmallRoundOrder == smallRound &&
                !it->second.m_bloodKillFlag; }))
        {
            packet.put_byte((int)it->second.m_bloodDifficulty);
            packet.put_short((int)(short)it->second.m_instId);
            packet.put_int(it->second.m_mobIndex);
            if (time == -1)
                packet.put_byte(3);
            else
                packet.put_byte(0);
            if (m_pDungeon->m_bloodDungeonType == 2)
                packet.put_byte(0x46);
            else
                packet.put_byte(0x3c);
            packet.put_short((int)it->second.m_bloodPhaseTime);
            packet.put_short((int)it->second.m_bloodRoundOrder);
            packet.put_short((int)it->second.m_bloodSpawnIdx);
            packet.put_short((int)it->second.m_bloodPhaseIdx);
            it->second.m_bloodKillFlag = 1;
            ++putCount;
        }
    }
    // ORIG 布局：`cmp; je <send 块>`，logFormat 块在前直落、send 块在后
    // （4.1 else-first 布局，P59/P191）⇒ 条件写不等形翻正块序。
    if (putCount != cnt)
    {
        LogManager::logFormat(1, "battle_field.cpp",
                              __PRETTY_FUNCTION__,
                              0xe43, "BLOOD_LOG : EROOR BLOOD SPAWN MONSTER COUNT!!\n");
    }
    else
    {
        packet.put_short(time);
        packet.finalize(true);
        m_party->send_to_party(packet);
    }
}