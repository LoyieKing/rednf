// ORIG W 0x0815decc：dst = this+4+(count<<4)，strncpy 15 字节后 ++count。
void SigLoadGeolocation::set(char* country)
{
    strncpy(m_countries[m_count], country, 15);
    ++m_count;
}