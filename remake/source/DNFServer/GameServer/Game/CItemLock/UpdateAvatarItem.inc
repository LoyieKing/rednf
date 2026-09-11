// ORIG 0x8541d42：包 0x1c2 = (0x1c2, uid, item.m_addInfo2(+7), GetLock() & 0xff)。
void CItemLock::UpdateAvatarItem(CUser* user, Inven_Item* item)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("ItemLock.cpp", 0xbf), true);
    **guard << 0x1c2;
    **guard << (int)user->GetUID();
    **guard << item->m_addInfo2;
    **guard << (int)(item->m_amp.GetLock() & 0xff);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}