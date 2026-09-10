int TimerRefreshPowerWarProcessInfo::dispatch_sig(int p1, int p2, unsigned int p3)
{
    if (GlobalData::s_power_manager->IsPowerWarEventOn()) {
        // ORIG 0x086382xx：G_GameWorld 结果 %eax 直落 (%esp) 实参（无槽）。
        G_GameWorld()->OnRefreshPowerWarProcessInfo();
        // dm+0xa658 = m_powerParameter.m_refreshProcessInfo(+0x30)；ORIG 按 8 字节
        // long 读取（覆盖 +0x30..+0x37 连续两 int），保持块读语义。
        // ORIG 0x086382f0：`mov 0xa658(%eax),%eax` 单级字段读（int 值语义，
        // 4B 等宽；reinterpret long* 取址形会反产 add+mov 两级）。
        registNextTimer(G_CDataManager()->m_powerParameter.m_refreshProcessInfo);
    }
    return 1;
