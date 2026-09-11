// ORIG T 0x08172fea
unsigned int GarenaAuthData::getTimeStamp()
{
    return ntohl(m_timeStamp) + m_patchTime;
}