// ORIG W 0x08697750：this+0x1a86+levelIdx*0x100（0x100/级 strncpy 槽）。
char* CLevelRewardTable::GetMailString(int levelIdx)
{
    return &m_messages[levelIdx * 0x100];
}