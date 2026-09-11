// ORIG W 0x080ef25c：动作 2（postal）时重置 ROI 域并置 m_count=1；
// 尾部统一保证强化字节非 0（低字节为 0 → 置 1）。
void eventReward::defaultOption(char action, itemData& out)
{
    switch (action)                        // ORIG：movsbl 后与 2 比较
    {
    case 2:
        out.m_count = 1;                     // *(int*)(d+8) = 1
        out.m_seal = 0;   // +0x01 = 0
        out.m_roi = 0;    // +0x02 = 0
        break;
    default:
        break;
    }
    if (out.m_upgrade == 0)   // movzbl/test
        out.m_upgrade = 1;    // +0x00 = 1
}