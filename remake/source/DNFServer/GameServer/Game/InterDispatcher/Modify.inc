void CUserResetTimeGateQuest::Modify(CUser* user)
{
    if (user->get_state() > 2 && user->getCurCharacR() != 0)
    {
        register int questIndex = GlobalData::s_timeGate_->getQuestIndex();  // ORIG 0x084f082f: mov %eax,%ebx（callee-saved 跨调用缓存）
        if (user->getCurCharacQuestR()->IsDoingQuest(questIndex))
        {
            register int questIndex2 = GlobalData::s_timeGate_->getQuestIndex();  // ORIG 0x084f0859: 第二次独立 call，同落 %ebx
            user->getCurCharacQuestW()->giveup_quest(questIndex2);
        }
    }
