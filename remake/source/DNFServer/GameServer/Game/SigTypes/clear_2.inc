void TAG_CHARAC_DATA::clear()   // ORIG W 0x822ed84
{
    memset(this, 0, sizeof(TAG_CHARAC_DATA));   // 0x78 字节清零（ORIG 实参序：size→0→this）
    m_data[0x25] = static_cast<char>(0xff);   // ORIG 尾部：+0x25 置 0xff
}