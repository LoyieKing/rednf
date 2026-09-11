bool CMultiMatchRoomList::Match(IMatch* me, IMatch** out)
{
    // b28d-solo-MatchRoomList VERDICT: NEAR（447，280=280；446 后端 AE）| 修法：
    // perTeam/myCnt 内联表达式（P0）、失败块直落/成功跳后、*vec.begin() 形、
    // doMatch/GetWaitingRoomCombination 返回类型 bool 化（P292）| N7
    // b28d-solo-MatchRoomList: 还原 ORIG 0x085675d0——
    // ①perTeam/myCnt 无局部槽（P0）：比较与实参均内联表达式，GetPlayerCount
    //   先求值存 %ebx，GetPlayCountPerOneTeam 后求值（movzbl 零扩展）；
    // ②成功/失败块序 = `if (!GetWaitingRoomCombination(...)) { AddRoom 循环; return
    //   false; }` 失败直落、成功跳后（ORIG xor$1/test/je <成功块>）；
    // ③vec 元素一律 `*vec.begin()` 形（ORIG 每处调用 begin()+operator*，非 _M_start 直读）。
    if (m_roomMap.empty())
        return false;
    std::vector<IMatch*> vec;
    if (me->GetPlayerCount() <
        (unsigned char)((fair_pvp::CFairMatch*)0)->GetPlayCountPerOneTeam()) {
        if (!GetWaitingRoomCombination(me,
                (unsigned char)((fair_pvp::CFairMatch*)0)->GetPlayCountPerOneTeam() -
                me->GetPlayerCount(), vec))
            return false;
        for (std::vector<IMatch*>::iterator it = vec.begin();
             it != vec.end(); ++it)
            DelRoom(*it);
    }
    if (!GetWaitingRoomCombination(me,
            (unsigned char)((fair_pvp::CFairMatch*)0)->GetPlayCountPerOneTeam(),
            vec)) {
        for (std::vector<IMatch*>::iterator it = vec.begin();
             it != vec.end(); ++it)
            AddRoom(*it);
        return false;
    }
    if (vec.empty())
        return false;
    if (vec.size() == 1) {
        DelRoom(*vec.begin());
        *out = *vec.begin();
        return true;
    }
    IMatch* prev = *vec.begin();
    *out = prev;
    for (std::vector<IMatch*>::iterator it = vec.begin() + 1;
         it != vec.end(); ++it) {
        DelRoom(*it);
        prev->SetNextMatch(*it);
        prev = *it;
    }
    return true;
}