unsigned short CUserCharacInfo::GetCurCharacEmotion()
{
    return m_selected ? m_selected->m_emotion : 0;
}