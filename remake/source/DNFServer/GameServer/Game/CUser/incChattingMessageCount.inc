void CUser::incChattingMessageCount(int count)
{
    m_postChecksumData.m_field64 += count;
