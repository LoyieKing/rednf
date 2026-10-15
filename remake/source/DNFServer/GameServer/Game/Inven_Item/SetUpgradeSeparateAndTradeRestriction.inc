void UpgradeSeparateInfo::SetUpgradeSeparateAndTradeRestriction(unsigned char value)
{
    // ORIG 0x085574bc (W)：*(unsigned char*)this = value —— 整字节覆盖，
    // 同时写入 upgrade 槽位（bit0-4）与 trade-restriction 位（bit5）。
    m_packed = value;
}