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
    // [b29x] ORIG 0x08295284/0x0829529e 是**两次独立** `cmpl $0x0,0x10(%ebp)`（name 与
    //   accName 各一枚三目），故保持两个独立三目（合并 if/else 会少一次 cmpl）；
    //   空串分支 ORIG `mov $0x8c161de,%esi/%ebx` 单条 vs 4.4 经 %eax 中转属 N7。
    // [K9GameRestA.CGameMgr12Run 2026-09-11 复核] 读 fresh .o：81=81 持平，残差仅
    //   0x08295299/0x082952b3 的 `mov $0x8c161de,%esi/%ebx`（imm 直装）vs 我方
    //   `mov $imm,%eax; mov %eax,%esi/%ebx`（4.4 imm 装载模板，N7）；串/实参序与
    //   ORIG 0x082952f6（`pvp@log %s,%s, room(%d), %d`）一致。
    register int roomIdx = it->second->get_index();
    // [R10a09 ④修复 P329/P0] 旧形把两枚三目提成具名 register 局部 ⇒ 4.4 产
    // 「jmp 跳过 else；mov $imm,%eax；join: mov %eax,%esi」共享尾（与 ORIG 0x08295284/
    // 0x0829529e 的「then 臂 `mov %eax,%esi; jmp join` + else 臂 `mov $imm,%esi`」
    // 两臂直赋不同）。把三目**内联回实参位**即复现 ORIG 逐臂赋值形（右→左求值序
    // param→roomIdx→name→accName 亦与 ORIG 地址序 0x...84(name) < 0x...9e(accName) 一致）。
    CSwitchLog(__PRETTY_FUNCTION__, 0xacc, 0, 0)(
        "pvp@log %s,%s, room(%d), %d",
        user == 0 ? "" : user->get_acc_name(),
        user == 0 ? "" : user->getCurCharacName(),
        roomIdx, param);
    return it->second;
}