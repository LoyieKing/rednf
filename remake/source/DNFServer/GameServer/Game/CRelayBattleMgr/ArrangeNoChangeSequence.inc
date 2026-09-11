// [b29 P248档② 实修] ORIG 0x085de83e 条件读形 `movzbl 0x7(%eax,%ecx,1),%eax;
// xor $0x1,%eax; test %al,%al; je` ⇒ m_requestFight bool 化 + 裸 if(!f)。
// 旧判词 N96+N18a（"寄存器物化无绑定不可达"）作废：fresh 改后 DIFF→AE（62=62）。
void CRelayBattleMgr::ArrangeNoChangeSequence(std::vector<int>& seq, int team,
                                              int& outIdx)
{
    for (int i = 0; i < 8; ++i)
    {
        if (m_battles[i].m_team == team && m_battles[i].m_state == 1)
        {
            // [b29 P248档②] 同 OnRequestFight：bool 字段 + 裸形。
            if (!m_battles[i].m_requestFight)
            {
                m_battles[i].m_seq = (short)seq[outIdx++];
            }
        }
    }
}