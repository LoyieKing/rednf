void SendingAPC_Info::set_data(_APCInfo const& info)
{
    // P83：ORIG 0x085fe8fd `rep movsl`（0x2f 次）＝结构整体赋值内联；
    // 显式 memcpy 生 call memcpy。
    m_apcInfo = info;                              // 0xbc
