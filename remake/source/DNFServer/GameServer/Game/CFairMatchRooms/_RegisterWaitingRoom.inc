void CFairMatch::_RegisterWaitingRoom(PvP_Room* /*room*/)
{
    // ORIG 0x8563a38 实证无 null 检查直接解引用（pWaitingRoomList_ 恒非 0）
    pWaitingRoomList_->AddRoom(this);
