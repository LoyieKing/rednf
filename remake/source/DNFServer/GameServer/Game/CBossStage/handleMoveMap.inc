// ---- handleMoveMap（ORIG 0x0814f66a）----
// 换图入口：_checkMemberDie 短路 → 逐成员反外挂计数 + MapClear 日志 →
// moveNextStage → randomMapIndex →（GM 强制图/难度）→ find_map → reset →
// consistMap（失败 throw ENUM_ERROR(1)，catch 后广播 0x30 包并返回 0）→
// 记 startTick → SendMapInfoPacket。
// [b28d-62] N82：p167 CLEAR（0 missing/0 extra），273/280 助记符同序；残差为
//   帧/槽号（4.1 生命周期穿插 vs 4.4 分区复用），语义完成。ORIG 0x0814f66a
// [b28d-solo-CBossStage] N封顶：删 manager!=0 判空（ORIG 无，P171 照抄）
//   （ndiff 41→28，gap 0）。残差=N7 虚调用 vptr 寄存器 %eax↔%edx 3 条 +
//   N122 bool 取反 xor 物化 1 条。
bool CBossStage::handleMoveMap(unsigned short* mapIndex)
{
    if (_checkMemberDie())
        return 0;

    // 已进行时长（秒）：(m_endTick - m_startTick) / 1000
    // ORIG 0x0814f683 `movl $0x0,-0x20(%ebp)` 预清（P198① 两句式）
    int playSec = 0;
    playSec = (m_endTick - m_startTick) / 1000;

    for (int i = 0; i < 4; ++i)
    {
        CUser* user = m_party->get_user(i);
        if (user == 0 || !m_party->checkValidUser(i))
            continue;

        if (playSec <= 4)
            user->getHackAnalyzer()->addServerHackCnt(
                user, (ENUM_HACKTYPE)0x19f, 1, 0, 0);
        if (mapIndex[i] == 0)
            user->getHackAnalyzer()->addServerHackCnt(
                user, (ENUM_HACKTYPE)0x19b, 1, 0, 0);
        // ORIG this+0x9c8+i*4 逐 int 判零（CBossPlay::m_useItemCount[i]）
        if (m_useItemCount[i] == 0)
            user->getHackAnalyzer()->addServerHackCnt(
                user, (ENUM_HACKTYPE)0xdd, 1, 0, 0);

        // cUserHistoryLog 为 CUser 嵌套成员 m_historyLog（ORIG user+0x79700）
        user->m_historyLog.MapClear(mapIndex[i], playSec, 0);
    }

    // ORIG 0x0814f84e `call moveNextStage; test %al,%al; je <失败尾块>`（无 xor 物化）
    // ⇒ P292 正极性 if(bool)：写成 `if (f()) { … }` 而非 `if (!f()) return`。
    if (moveNextStage())
    {
        int mapIdx = 0;
        if (!randomMapIndex(mapIdx))
            return 0;

        try
        {
            CUser* manager = m_party->getManager();
            // ORIG 0x0814f891..89e 直接 call isGMUser（无 manager!=0 判空）⇒ P171 照抄
            if (manager->isGMUser())
            {
                // CUser GM 调试字段（"bossmap"/"bossdifficult" 指令写入，取走即清零）
                if (manager->m_gmBossMapIndex != 0)
                {
                    mapIdx = manager->m_gmBossMapIndex;
                    manager->m_gmBossMapIndex = 0;
                }
                if (manager->m_gmBossDifficulty != 0)
                {
                    m_difficulty = (int)(signed char)manager->m_gmBossDifficulty;
                    manager->m_gmBossDifficulty = 0;
                }
            }

            const CMap* map =
                (const CMap*)G_CDataManager()->find_map(mapIdx);
            if (map == 0)
                return 0;

            reset();
            if (!consistMap(map))
                throw (ENUM_ERROR)1;

            m_startTick = (int)GlobalData::s_systemTime_.getCurTickCount();
            SendMapInfoPacket();
            return 1;
        }
        // P229：ORIG 0x0814f9ac `mov (%eax),%eax; mov %eax,-0xc` ⇒ 按值 catch
        //   （引用 catch 会先落指针槽再回读）。
        catch (ENUM_ERROR e)
        {
            PacketGuard packet;
            packet.put_header(1, 0x30);
            packet.put_byte(0);
            packet.put_byte((int)e);
            packet.finalize(true);
            m_party->send_to_party(packet);
            return 0;
        }
    }
    return 0;
}