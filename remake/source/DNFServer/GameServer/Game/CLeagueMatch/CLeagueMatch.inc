// ORIG C1/C2 0x0855cd74：IMatch ctor → PVP_ROOM_INFO@+8 → 懒建 pWaitingRoomList_
// （new 0x1c 置零后 C1 0x08568fb0）→ 引用计数自增。
// [b22判词·N族] ORIG new(0x1c) 后、C1 call 前有 6 条 value-init 标量置零
// （+0x00/08/0c/10/14/18），GCC4.1 语义；4.4 视 user-provided ctor 类免置零直调。
// placement-new+try 实测反引入 operator new(void*,ptr) call + EH spill，C++98
// 无法复现该置零序列，语义等价（残差 6 insns），按 N 族结案，勿再开轮。
// [b28d-solo-CLeagueMatch] AE（TU 口径）：67=67 全等。原 N77 残差（new
// PVP_ROOM_INFO 的 6 条 value-init 清零 +0x00/08/0c/10/14/18）随 PvPTypes.h
// PVP_ROOM_INFO 保持「无用户 ctor」而恢复（P299：用户 ctor 抑制 value-init）；
// EH 尾块序亦已归一。本件无需再改。
CLeagueMatch::CLeagueMatch()
{
    if (pWaitingRoomList_ == 0)
        pWaitingRoomList_ = new CLeagueMatchRoomList();
    ++nWaitingRoomListRefCount_;
}