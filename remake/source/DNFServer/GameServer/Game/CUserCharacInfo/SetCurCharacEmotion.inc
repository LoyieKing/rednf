void CUserCharacInfo::SetCurCharacEmotion(short emotion)
{
    enableSaveCharacStat();
    m_selected->m_emotion = (unsigned short)emotion;
}