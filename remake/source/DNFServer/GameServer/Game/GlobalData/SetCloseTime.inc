void village_attacked::CRevengeDungeon::SetCloseTime(unsigned int sec)  // ORIG W 0x0847dd36
{
    // mov %edx,0x4(%eax)：arg 写 +0x04 int。
    m_field04 = sec;
}