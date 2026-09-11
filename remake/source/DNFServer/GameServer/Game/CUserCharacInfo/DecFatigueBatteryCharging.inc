// ORIG 0x08656784：`movzwl 0x4f,%edx;mov %edx,%ecx` 二段（4.4 折单步 N18a）；第二条件
// `mfat>old` ORIG 直 `cmp -0xa(%ebp),%ax;jle` 折返（跳返回侧），4.4 先物化 old 到 %eax 再
// `cmp %ax,%dx`，jle 落点/相位同 ORIG，无一分支反向，语义等价。
// [b28d-solo-CUserCharacInfo AE] 45=45：P254 packed enum 视角（m_fatigueBatteryEnum）。
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
}