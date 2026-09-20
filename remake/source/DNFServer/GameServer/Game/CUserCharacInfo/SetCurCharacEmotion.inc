void CUserCharacInfo::SetCurCharacEmotion(short emotion)
{
    enableSaveCharacStat();
    m_selected->m_field122C = (unsigned short)emotion;
}