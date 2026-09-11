void CUser::setGmQuestFlag(bool flag)
{
    m_gameOption.m_gmQuestFlag = flag;   // ORIG 0x822fca1 bool=bool 直落（字段已 bool 化，(char) cast 产 arg-先序）
}