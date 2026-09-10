PvP_Room* CGameManager::GetPvp(int index, CUser* user, int param)
{
    // b28d-lane12: CSwitchLog 改临时链（P221）60→42；残余 CSwitchLog/find sret/
    // name/accName 槽平移 → N82附。ORIG 0x08295206。
    // b28d-solo-CGameManager 修法（ORIG 0x08295206）：早返 `it==end → return 0`；
    // name/accName 用三元 `user==0 ? "" : ...` 内联（ORIG 0x08295284 段 cmpl/je 双分支 +
    // `mov $0x8c161de,%esi` 空串）；末尾 `return it->second` 重新 ptEv。
    // b28d-solo-CGameManager: P231/P307 纯 register（非绑定，P0 合规）使 roomIdx/
    // name/accName 升格 callee-saved：帧/指令数 76→81 与 ORIG 全等（ndiff 101→22），
    // 余差仅 ORIG 空串立即数落 %esi/%ebx vs 4.4 落 %eax → N7 寄存器名族封顶。
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> >::iterator it = m_pvpRoomMap.find(index);
    if (it == m_pvpRoomMap.end())
        return 0;
    // b28d-solo 实验（P231/P307/P87）：ORIG 用 edi/esi/ebx 跨 CSwitchLog ctor 保活。
    register int roomIdx = it->second->get_index();
    register const char* name = user == 0 ? "" : user->getCurCharacName();
    register const char* accName = user == 0 ? "" : user->get_acc_name();
    CSwitchLog(__PRETTY_FUNCTION__, 0xacc, 0, 0)(
        "pvp@log %s,%s, room(%d), %d", accName, name, roomIdx, param);
    return it->second;
