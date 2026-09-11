// ORIG 0x849f5ac：先 CData::alter() 再写 +0x06。
void CCharacExpertJob::set_giveup_count(unsigned short c)
{
    alter();
    m_giveupCount = c;
}