void CItemGeneratorMgr::onTimerEvent(int timer)
{
    // ORIG 控制流即 `if (param_1 == 0x6b) { … } return;`（正向判定，函数尾
    // 直落 epilogue，无额外 jmp）。
    if (timer == 0x6b)
    {
        // ORIG（0x08534322）每次调用独立 call G_CEnvironment() 后
        // `mov 0x1b0(%eax),%eax` —— env+0x1b0 = Server_Envir(+0x198)::m_channelNo(+0x18)，
        // 故首参具名为 m_serverEnvir.m_channelNo（原写法 `G_CEnvironment() + 0x1b0`
        // 是 CEnvironment* 上的指针算标度，非字节偏移，属真实缺陷，本次一并修正）。
        // 四组统计指针即本对象计数表 m_dropStat[g][1..4]（每行 6 uint，
        // 行距 0x18、genType 距 0x78；ORIG `add $imm,%eax` 序列同源）。
        DB_UpdateItemDropStatistics::makeRequest(
            G_CEnvironment()->m_serverEnvir.m_channelNo, 0,
            m_dropStat[0][1], m_dropStat[0][2], m_dropStat[0][3], m_dropStat[0][4]);

        DB_UpdateItemDropStatistics::makeRequest(
            G_CEnvironment()->m_serverEnvir.m_channelNo, 1,
            m_dropStat[1][1], m_dropStat[1][2], m_dropStat[1][3], m_dropStat[1][4]);

        DB_UpdateItemDropStatistics::makeRequest(
            G_CEnvironment()->m_serverEnvir.m_channelNo, 2,
            m_dropStat[2][1], m_dropStat[2][2], m_dropStat[2][3], m_dropStat[2][4]);

        // 头部计数区整体清零（ORIG 0x08534432 `mov 0x8(%ebp),%eax` 先行物化
        // arg1，再压 0x3c0/0 ⇒ 实参必须写成员数组 m_head（非 `this`），
        // 否则 GCC 把 arg1 计算延到最后一条 store，指令序与 ORIG 不同）。
        std::memset(m_head, 0, 0x3c0);
    }
}