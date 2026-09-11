// [b28d-23 N·N82 → b28d-M2S3 复测] ORIG 0x085ddad0：192/192，归一化后仅 8 处
// 「参数压栈立即数」差（.o 未链接值 vs ORIG .rodata 地址的显示差）⇒ identical 级。
// [P8S3 2026-09-11] 新口径复核 n1_v=AE（reloc 归一）；旧判词 DIFF 系 objdump 无 -r 的假残差
void PvP_GuildWar_Log::WriteGuildWarPvPLog(int roomIdx, char mode, CUser** users)
{
    // [L14 round3] N78 突破口（P214③/P253）：ORIG 0x085ddad0 两处清零是**内联
    // `rep stos`**（0x85ddb5b 0x800 字节 / 0x85ddcbd 0x100 字节）而非 `call memset`；
    // 4.4 对 `memset` 恒发 call（p167 EXTRA +2 memset）。改「声明处 `= {'\0'}`」
    // 并把声明下移到原 memset 位置，令 4.4 在同一位置发常量 rep stos。
    int i;
    int j;
    int k;

    for (i = 0; i < 8; ++i)
    {
        if (users[i] != 0)
        {
            // b28d-M2S3（P0/P183）：ORIG 0x085ddb0b `call get_guildwar_point_
            // per_pvpplay; mov %eax,%ebx`（无 point 具名槽）+ 0x085ddb20
            // `call get_charac_guildkey`（无 guildKey 槽）；两具名局部多余。
            // cdecl 实参右→左求值 ⇒ ORIG 先算第 2 参 point 存 %ebx。
            InsertPvPGuild(
                ((CUserCharacInfo*)users[i])->get_charac_guildkey(),
                users[i]->get_guildwar_point_per_pvpplay());
        }
    }
    char logBuf[2048] = { '\0' };
    sprintf(logBuf, "Room_Idx:%d\tPvP_Mode:%d\tElasp_Time:%d\tG_Cnt:%d\n",
            roomIdx, (int)mode,
            GlobalData::s_systemTime_.getCurSec() - m_startTime,
            (unsigned char)m_guildCnt);
    // b28d-M2S3（P222/P164 块序）：ORIG 0x085ddbae `cmp $0x8,%al; jbe <then 块
    // +0x126>` ⇒ else(logFormat) 块**物理前置**、尾部 `jmp <epilogue>`（0x085ddbf1）
    // ⇒ 源码是早返回形（`>= 9` 打印后 return），then 块被跳到后面。
    // 旧源 `if (< 9) {...} else {...}` 令 then 直落、else 跳后（我方 `ja`）。
    if ((unsigned char)m_guildCnt >= 9)
    {
        LogManager::logFormat(
            1, "pvp.cpp",
            __PRETTY_FUNCTION__,
            0x12cc,
            "PvP_GuildWar_Log::WriteGuildWarPvPLog() m_guildCnt(%d) > MAX_PVP_PLAYER(8) ",
            (unsigned char)m_guildCnt);
        return;
    }
    // b28d-M2S3：ORIG 槽序 i(-0x24)/j(-0x20)/k(-0x1c) 连续 ⇒ j 用函数头
    // 已声明的外层 j（旧源 for 内再声明遮蔽，令 j 槽落到 k 之后）。
    for (j = 0; j < (unsigned char)m_guildCnt; ++j)
    {
        if (m_guilds[j].m_guildKey != 0)
        {
            sprintf(logBuf, "%s->\t\tG_Key:%d\tPoint:%d\tM_Cnt:%d\n", logBuf,
                    m_guilds[j].m_guildKey, m_guilds[j].m_point,
                    (unsigned char)m_guilds[j].m_count);
        }
    }
    char logPath[256] = { '\0' };
    sprintf(logPath, "./log/%s/GuildWarResult",
            G_CEnvironment()->get_file_name());
    // b28d-M2S3（P139/§6.5）：ORIG 0x085ddcf6 实参 =
    // `&_ZZN16PvP_GuildWar_Log19WriteGuildWarPvPLogEicPP5CUserE19__
    // PRETTY_FUNCTION__` 具名局部静态数组，非手写串。
    CMyFileLog fileLog(__PRETTY_FUNCTION__, 0x12dc);
    fileLog(logPath, "%s", logBuf);
    Clear();
    for (k = 0; k < 8; ++k)
    {
        if (users[k] != 0)
        {
            users[k]->set_guildwar_point_per_pvpplay(0);
        }
    }
}