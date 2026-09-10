void village_attacked::CRevengeDungeon::CloseRevengeDungeon()  // ORIG W 0x0847dd2a
{
    // movb $0x0,(%eax)：置 +0 字节为 0。
    m_isOpen = false;
