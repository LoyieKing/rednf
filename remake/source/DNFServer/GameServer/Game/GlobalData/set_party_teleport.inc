void Teleport::set_party_teleport(stPartyTelePort const& tp)
{
    // ORIG 0x08283d98：拷贝 stPartyTelePort 前 10 字节（8 字节 + short）。
    // ORIG 形：this/tp 基址各载一次，字段全折叠 `(%edx)→0x4(%eax)` 等 ⇒
    // 结构整体赋值（POD 逐字段展开，10B 实拷不含尾 pad）。
    m_tp = tp;
