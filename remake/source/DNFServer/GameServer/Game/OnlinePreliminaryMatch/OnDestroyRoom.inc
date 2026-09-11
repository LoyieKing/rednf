void COnlinePreliminary::OnDestroyRoom(CUser* /*user*/)
{
    m_field04 = 0;
    pWaitingRoomList_->DelRoom(this);             /* vtable+4 虚调实证 */
}