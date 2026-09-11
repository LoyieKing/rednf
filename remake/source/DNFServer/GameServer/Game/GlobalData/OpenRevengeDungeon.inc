void village_attacked::CRevengeDungeon::OpenRevengeDungeon()   // ORIG W 0x0847dd1e
{
    // movb $0x1,(%eax)：置 +0 字节为 1。
    m_isOpen = true;
}