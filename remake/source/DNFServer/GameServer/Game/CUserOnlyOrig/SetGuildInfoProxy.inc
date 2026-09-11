// ---------------------------------------------------------------------------
// CUser::SetGuildInfoProxy(STGuildProxy& proxy) T 0x0865cd0a sz0x48
// 报告：docs/class_func_reports/CUser/SetGuildInfoProxy.md
// ORIG：m_guildDB.m_fundLevel←proxy.m_level(+0x17)；memcpy(m_guildDB,&proxy,0x16)；
// m_guildDB.m_field95←proxy.m_powerSide(+0x18)。STGuildProxy 定义在
// CGuildServerProxy.h:39（0x16=名字区）。
// ---------------------------------------------------------------------------
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x0865cd0a：N102/P75 封顶——ORIG memcpy dest 为
// `mov this,%edx; add $0x8d020,%edx` 两段，4.4 恒折 `lea disp(%eax),%edx`（N102 七形全折，
// 唯一出 add 的裸偏移禁用）；src/this 装载时刻差属 P75。
void CUser::SetGuildInfoProxy(STGuildProxy& proxy)
{
    m_guildDB.m_fundLevel = proxy.m_level;
    // ORIG 0x0865cd20：`mov 0xc(%ebp),%eax` src 先载 + dst `mov 0x8(%ebp),%edx;
    // add $0x8d020,%edx` 两段。
    // [b28d-solo-CUserOnlyOrig P0] ORIG 无 src 槽 ⇒ 原码 src 是内联表达式
    // （旧 `void* sp = &proxy;` 生 -0xc 槽 + 回读，帧 0x28 vs ORIG 0x18）。
    // [H6B 2026-09-11 第二轮负结论·已回滚] 试 `memcpy((void*)(unsigned int)&m_guildDB, …)`
    // （P319 中转型，原理：int 加法逼出 ORIG 的两段 `add $0x8d020,%edx`）：实测
    // **20→20、ndiff 6→6 中性**（4.4 仍折 `lea 0x8d020(%eax),%edx`，差集保持
    // ORIG-only add:1 / OURS-only lea:1）⇒ 回滚；N102「4.4 恒折 lea」复核成立。
    memcpy(&m_guildDB, &proxy, 0x16);
    // ORIG 0x0865cd43：`mov 0xc(%ebp),%eax; movzbl 0x18(%eax),%edx; mov 0x8(%ebp),%eax;
    // mov %dl,0x8d0b5(%eax)`——uchar=uchar 同型单步零扩展直落（技巧5；Main 已落
    // CUser.h:1307 m_field95 char→uchar，同宽 sizeof/偏移不变）。
    m_guildDB.m_field95 = proxy.m_powerSide;
}