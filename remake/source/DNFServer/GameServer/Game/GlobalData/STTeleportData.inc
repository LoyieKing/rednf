// ORIG W 0x0822d78e：仅清零 +0x00 的 int（m_field00），不触碰 m_tp 子对象。
STTeleportData::STTeleportData()
{
    // ORIG 0x0822d794：movl $0x4,0x8(%esp) → memset 长度 4；this 实参最后
    // 装填（0x0822d7a4）⇒ 原码为 memset(this,0,4) 非 &m_field00（后者使 4.4
    // 提前缓存 this 指针）。
    memset(this, 0, 4);
}