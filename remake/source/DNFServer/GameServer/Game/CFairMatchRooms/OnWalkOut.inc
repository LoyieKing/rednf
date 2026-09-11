// OnWalkOut ORIG T 0x8564fb2 报告 OnWalkOut.md。ORIG 返回 1。
// 转发到本类虚槽 +0x2c（OnLeaveRoom(user,room,team,seat)）
// b28d-75（N82附/P75 封顶，语义等价）：vtable[+0x2c] 虚调用目标寄存器两侧同为
// %ecx（b28d 复核，旧 N7 判词已不适用）；唯一真差 = 五实参装载时刻：
// ORIG 4.1 先载第4参 team(-0x10)→%eax，再 seat→0x10；4.4 先载第2参
// userSeat(-0xc)→%eax。b28d 复测 (int)team 冗余 cast 形无变化（P41 在此不触发）
// ⇒ 跨版本实参装载调度，禁绑定形（P0）⇒ N82附 封顶。
// b28d-solo-CFairMatchRooms 判词：N82附——五实参物化序差：ORIG 先物化 team
// (-0x10)→%eax 再压 seat/team/room/userSeat；4.4 先物化 userSeat(-0xc)→%eax、
// team/room 走 %edx。unsigned team / `*&team` 两形实测均引 push %ebx（fnptr 落
// %ebx 而非 ORIG %ecx）⇒ 净负回退。槽位集全等、call 差集 2=2 CLEAR。封顶。
// fresh ndiff=5（33=33 NEAR）。
// [GW20Run 2026-09-11 复核] fresh 33/33；ORIG 0x08564fec 先物化第 4 参 team(-0x10)→%eax
// 再压 seat/room，4.4 先物化 userSeat(-0xc)→%eax；P231 register 形历史实测反劣
// （33→35，NEAR→DIFF）已回滚；p167 2=2 CLEAR ⇒ 维持 N82附。
int CFairMatch::OnWalkOut(PvP_Room* room, int seat)
{
    // [b28f-SweepCFairMatch 负结论] P231 纯 register 试形（翻转 5 实参装载序）
    // 实测反劣：our 33→35（多 `push %ebx` + 帧 0x38→0x34）、verdict NEAR→DIFF
    // ⇒ 恢复原形。ORIG 先物化 team(-0x10)→%eax vs 4.4 先物化 userSeat(-0xc)→%eax
    // 属跨版本实参装载调度（P75/N82附），禁绑定形（P0），封顶。
    const int team = room->get_team(seat);
    const int userSeat = room->get_user_seat(seat);
    OnLeaveRoom(reinterpret_cast<CUser*>(userSeat), room, team, seat);
    return 1;
}