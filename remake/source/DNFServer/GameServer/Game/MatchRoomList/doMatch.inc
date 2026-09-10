bool CWaitingRoomList::doMatch(IMatch* me, std::multimap<float, IMatch*>& out)
{
    // b28d-solo-MatchRoomList VERDICT: NEAR（447，110=110；446 后端 AE）| 修法：
    // if(!empty)包裹 + 死局部 found/bestGap/bestIt + gap 具名左值 make_pair +
    // goto 尾块布局 | N7（虚调 GetGap/CheckMatchable 目标 %eax↔%edx，4.4.7 恒 %eax）
    // b28d-solo-MatchRoomList: 还原 ORIG 0x08566d9c——
    // ①`if (!m_roomSet.empty()) { 主流程 } return 0;`（ORIG call empty 后 xor$1/test/je
    //   直跳 return 0；正极性 if 少该 xor 段）；
    // ②ORIG 保留三个未参与主流程的死局部（P171/P232：-O0 仍发初始化 store）：
    //   found(-0xd)/bestGap(-0xc)/bestIt=end()(-0x2c)；
    // ③gap 具名左值 → make_pair<float&,IMatch*&>（ORIG 085694f9 实例），
    //   而非 GetGap 右值直传的 make_pair<float,IMatch*&>。
    if (!m_roomSet.empty()) {
        bool found = false;
        float bestGap = 0.0f;
        std::set<IMatch*>::iterator bestIt = m_roomSet.end();
        (void)found;
        (void)bestGap;
        (void)bestIt;
        for (std::set<IMatch*>::iterator it = m_roomSet.begin();
             it != m_roomSet.end(); ++it) {
            IMatch* cand = *it;
            if (cand == me)
                continue;
            if (!me->CheckMatchable(cand))
                continue;
            float gap = me->GetGap(cand);
            out.insert(std::make_pair(gap, cand));
        }
        goto check_out;
    }
    return 0;
check_out:
    if (out.empty())
        return 0;
    return 1;
