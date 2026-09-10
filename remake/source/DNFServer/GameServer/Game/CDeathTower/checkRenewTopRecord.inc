bool CDeathTowerRanking::checkRenewTopRecord(unsigned int towerIdx,
                                             unsigned int clearTime,
                                             unsigned int playTime)
{
    // [b28d-solo-CDeathTower] verdict=AE（31/32，仅尾 nop 填充 §6.1 豁免）：
    // 修法 = 返回类型 int→bool（ORIG 尾 `call operator>; leave; ret` 无 movzbl
    // 加宽，P9/P292；调用点 test %al 联判），头/定义/调用点三处同步。
    // 旧残差注记：ORIG `add $0x128` 单条 vs 4.4 拆
    // 0x120+disp8（N94，0x128%8==0）+ 返回值 bool 加宽 movzbl（P9）；P167 CLEAR。
    // ORIG 0x08468290：塔块 +0x128（= m_towers[i][1] 起点 +0x98 内
    // flag +0x90）movzbl 读取，!=1 直接返回 0；否则构造默认 stBestRecord_t，
    // 将 param_2（clearTime）写 m_stage(+0x88)、param_3（playTime）写
    // m_playTime(+0x8c)（0x3f/0x45 两条 movl），调 stBestRecord_t::operator>
    // (tmp, m_towers[towerIdx][1])（ORIG call 0x08469ce4）。
    // 范式36：删 topSlot 具名中转（4.4 多 1 个 4B 槽 ⇒ ORIG tmp 基址 -0xa0 平移
    // -0xa4）；ORIG 0x084682a5/0x08468320 两处独立 imul 0x390 地址重算。
    // flag 判形 ORIG 0x084682ae：movzbl (%eax); xor $0x1; test %al; je —— (x^1) truthy。
    // [b27 封顶] flag 判形 ORIG xor $1 于 %eax 内 + test %al（8 位直测）；
    // 4.4 恒 movzbl %al,%eax 加宽（直写形折 cmp$1，变量形多槽两步——P168a③
    // 同族），test %al 形不可造。另 m_flag90 偏移 0x128 拆 0x120+disp8 为
    // 4.4 常量折叠策略（N94 邻族），ORIG add $0x128 全常量形不可定向。
    if (!m_towers[towerIdx][1].m_flag90)
        return 0;
    stBestRecord_t tmp;
    tmp.m_playTime = playTime;
    tmp.m_stage = clearTime;
    return tmp > m_towers[towerIdx][1];
