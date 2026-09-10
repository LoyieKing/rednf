void CUserCharacInfo::SetCurCharacEmotion(short emotion)
{
    enableSaveCharacStat();
    CUR->m_curCharacEmotion = (unsigned short)emotion;
