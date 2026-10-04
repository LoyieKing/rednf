// ORIG 0x086566e4：`movzwl 0x4f,%edx;mov %edx,%ecx` 二段（4.4 折单步 N18a）；两处比较
// ORIG 直 `cmp -0xa(%ebp),%ax` 折返，4.4 先物化寄存器再 `cmp %ax,%dx`，jl/jle 落点相位同，
// 无一分支反向，语义等价。
// [b28d-solo-CUserCharacInfo AE] 49=49：P254 packed enum 视角（EFatigueBatteryValue /
// m_fatigueBatteryEnum）⇒ 累加段两段 regmove 复现（old/比较段维持 ushort 视角不变）。
void CUserCharacInfo::IncFatigueBatteryCharging(short value)
{
    if (m_selected)
    {
        enableSaveCharacStat();
        // ORIG IncFatigueBatteryCharging（0x86566e4）：movzwl 0x5c0(%eax) 读
        // CDataManager+0x68 处 ServerParameterScript 的疲劳电池常量表上限。
        ServerParameterScript* sp =
            (ServerParameterScript*)((char*)G_CDataManager() +
                                     offsetof(CDataManager, m_serverParameter));
        // P209 翻案：old 声明 signed short 出 ORIG 直 `cmp -0xa(%ebp),%ax`
        // 内存比较（探针 /tmp/bl10/p5.cpp h1/h2）。
        short old = (short)m_selected->m_fatigueBattery;
        // P254 packed enum 视角：复现 ORIG `movzwl 0x4f(%edx),%edx; mov %edx,%ecx` 两段。
        m_selected->m_fatigueBattery =
            (EFatigueBatteryValue)((int)m_selected->m_fatigueBattery + value);
        if ((short)m_selected->m_fatigueBattery < old ||
            (int)(short)m_selected->m_fatigueBattery > (int)sp->m_fatigueTable5c0.m_capLimit)
        {
            m_selected->m_fatigueBattery = sp->m_fatigueTable5c0.m_capLimit;
        }
    }
}