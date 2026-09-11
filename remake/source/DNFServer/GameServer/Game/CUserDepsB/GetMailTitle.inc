// ORIG W 0x08697730：this+0x14c8+levelIdx*21（21B/级 strncpy 槽）。
char* CLevelRewardTable::GetMailTitle(int levelIdx)
{
    return &m_nameStrings[levelIdx * 21];
}