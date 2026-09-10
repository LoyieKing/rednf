unsigned int GarenaAuthData::getTimeStamp()
{
    return ntohl(m_timeStamp) + m_patchTime;
