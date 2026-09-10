void CFairMatch::OnDestroyRoom(CUser* /*user*/)
{
    m_field04 = 0;
    pWaitingRoomList_->DelRoom(this);        // 等待表 vtable[+0x04]=DelRoom(this)
