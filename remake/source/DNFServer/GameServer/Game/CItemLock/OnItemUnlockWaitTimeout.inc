// ORIG 0x8542a6a：定时器到期入口 → PopItemLock，恒返 1。
int CItemLock::OnItemUnlockWaitTimeout(CUser* user)
{
    PopItemLock(user);
    return 1;
}