// ---------------------------------------------------------------------------
// get_useJobFatigueMatchGiftMsg(int, std::string&, std::string&) ——
//   ORIG T 0x0836598e _ZN12CDataManager29get_useJobFatigueMatchGiftMsgEiRSsS0_
//   （报告缺失，按 nm 定址 + objdump 转写）。
//   反汇编逐条转写：
//     it = m_useJobFatigue.m_giftMap.find(job)（map::find 直构 -0x10）；
//     end = m_useJobFatigue.m_giftMap.end()（直构 -0xc，P13 缓存 end）；
//     it != end 时 msg1 = it->second.m_msg1（pair+0x20 = value+0x1c）、
//     msg2 = it->second.m_msg2（pair+0x24 = value+0x20）。
// ---------------------------------------------------------------------------
void CDataManager::get_useJobFatigueMatchGiftMsg(int job, std::string& msg1,
                                                std::string& msg2)
{
    std::map<int, useCharacterJobFatigue::fatigueMatchGift>::iterator it =
        m_useJobFatigue.m_giftMap.find(job);
    if (it != m_useJobFatigue.m_giftMap.end())
    {
        msg1 = it->second.m_msg1;
        msg2 = it->second.m_msg2;
    }
}