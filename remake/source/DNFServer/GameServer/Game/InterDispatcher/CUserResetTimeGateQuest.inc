CUserResetTimeGateQuest::CUserResetTimeGateQuest() {}

// ORIG CUserResetTimeGateQuest::Modify(CUser*) W 0x084f07ec：
//   get_state()>2 && getCurCharacR() 时，取 GlobalData::s_timeGate_->getQuestIndex()，
//   若 getCurCharacQuestR()->IsDoingQuest(qi) 则 getCurCharacQuestW()->giveup_quest(qi)。
