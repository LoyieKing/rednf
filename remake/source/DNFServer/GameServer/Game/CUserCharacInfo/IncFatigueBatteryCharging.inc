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
        short old = (short)CUR->m_fatigueBattery;
        // P254 packed enum 视角：复现 ORIG `movzwl 0x4f(%edx),%edx; mov %edx,%ecx` 两段。
        CUR->m_fatigueBatteryEnum =
            (EFatigueBatteryValue)((int)CUR->m_fatigueBatteryEnum + value);
        if ((short)CUR->m_fatigueBattery < old ||
            (int)(short)CUR->m_fatigueBattery > (int)sp->m_fatigueTable5c0.m_capLimit)
        {
            CUR->m_fatigueBattery = sp->m_fatigueTable5c0.m_capLimit;
        }
    }
