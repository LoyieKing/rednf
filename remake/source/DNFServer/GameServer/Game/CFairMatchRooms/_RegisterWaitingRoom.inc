// _RegisterWaitingRoom ORIG T 0x8563a32 报告 _RegisterWaitingRoom.md
// (**pWaitingRoomList_)->vtable[0] = AddRoom —— 经 IRoomList::AddRoom(this)
// [b28 BM2 判词·N7] 同 OnDestroyRoom：AddRoom 虚槽 %ecx vs %eax 承载（N7 族），
// 其余逐条同。
void CFairMatch::_RegisterWaitingRoom(PvP_Room* /*room*/)
{
    // ORIG 0x8563a38 实证无 null 检查直接解引用（pWaitingRoomList_ 恒非 0）
    pWaitingRoomList_->AddRoom(this);
}