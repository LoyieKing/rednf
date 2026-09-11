stSelectQuestParam::stSelectQuestParam(CUser* user)
{
    // b28c-28（P29 函数体赋值序照 ORIG 发射序）：ORIG 0x083480b4 写入序为
    // +0x04 job → +0x08 firstGrow → +0x0c secondGrow → +0x00 level →
    // +0x10 pvpGrade → +0x14 powerSide → +0x1c expertJobType →
    // +0x20 expertJobLevel → +0x24 quest（**非**声明序）。
    m_field04 = user->get_charac_job();
    m_field08 = user->getCurCharFirstGrowType();
    m_field0c = user->getCurCharSecondGrowType();
    m_field00 = user->get_charac_level();
    m_field10 = user->get_pvp_grade();
    m_field14 = user->getPowerSide();
    m_field1c = user->GetCurCharacExpertJobType();
    m_field20 = user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp());
    // b28c-28（P0/P183 禁指针缓存）：ORIG 0x0834815c 段为
    // `call getCurCharacQuestR; mov 0x75f0(%eax),%edx; mov this,%eax;
    // mov %edx,0x24(%eax)`——返回值无槽往返 ⇒ 原码是内联成员链，
    // 具名 questR 局部多占 1 槽使帧 0x18→0x28、全帧槽号漂移。
    m_field24 = user->getCurCharacQuestR()->m_authenData.m_urgentQuest;
}