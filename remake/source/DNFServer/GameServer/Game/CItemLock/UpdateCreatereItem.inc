// ORIG 0x8541c36：包 0x1c1，同构。
void CItemLock::UpdateCreatereItem(CUser* user, Inven_Item* item)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ItemLock.cpp", 0xb5), true);
    **guard << 0x1c1;
    **guard << (int)user->GetUID();
    **guard << item->m_addInfo2;
    **guard << (int)(item->m_amp.GetLock() & 0xff);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}