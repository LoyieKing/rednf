// ---- ::DBDispatcher::dispatch（ORIG T 0x083fbf7c）----
// slot==-1 恒通过；否则要求 CGameManager::getUser(slot, userIdx+0x7d0) 非空。
bool DBDispatcher::dispatch(int userIdx, int slot, Stream* stream)
{
    (void)stream;
    if (slot != -1)
    {
        CUser* user = G_CGameManager()->getUser(slot, userIdx + 0x7d0);
        if (user == 0)
        {
            return false;
        }
    }
    return true;
}