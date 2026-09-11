void CUser::setBreakAwayQuestTime(unsigned int time)
{
    m_gameOption.m_breakAwayQuestTime = time;   // ORIG 0x822fc7c this-先 uint 直写（字段已 uint 化）
}