void CUserCharacInfo::DecFatigueBatteryCharging(short value)
{
    if (m_selected)
    {
        enableSaveCharacStat();
        // P209 翻案：old 必须声明为 **signed short**——unsigned short 局部使 4.4
        // 恒物化 `movzwl 槽,%eax; cmp %ax,%dx` 三段；signed 局部出 ORIG 直
        // `cmp -0xa(%ebp),%ax` 内存比较（探针 /tmp/bl10/p5.cpp h1/h2 对照）。
        short old = (short)CUR->m_fatigueBattery;
        // P254 packed enum 视角（同 IncFatigueBatteryCharging）。
        CUR->m_fatigueBatteryEnum =
            (EFatigueBatteryValue)((int)CUR->m_fatigueBatteryEnum - value);
        if ((short)CUR->m_fatigueBattery < 0 || (short)CUR->m_fatigueBattery > old)
            CUR->m_fatigueBattery = 0;
    }
