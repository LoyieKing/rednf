// ORIG getMailContent 0x08165004：sret，拷贝构造自 this+0x18 = condition +0x10。
std::string BaseHeroMissionCondition::getMailContent()
{
    return m_condition.m_mailContent;
}