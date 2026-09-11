// ORIG getMailTitle 0x08164fda：sret，std::string 拷贝构造自 this+0x14 =
// condition +0x0c。
std::string BaseHeroMissionCondition::getMailTitle()
{
    return m_condition.m_mailTitle;
}