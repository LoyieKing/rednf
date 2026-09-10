bool CPvPLiveEventData::loadData(CUser* /*user*/, char* buffer)
{
    m_playCount = *reinterpret_cast<int*>(buffer);
    return true;
