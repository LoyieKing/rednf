// ORIG 0x85ca994（报告 Exit.md 逐指令）：买家身份在册则经 IBuyer 虚槽 +0xc
// （ExitPrivateStore）通知后 FreeStoreBuyer；卖家身份在册则经 ISeller 虚槽 +4
// （RemovePrivateStore(true)）通知后 FreeStoreSeller；user 空指针返 0。
int CPrivateStoreMgr::Exit(CUser* user)
{
    if (user == 0)
        return 0;
    CPrivateStore* store;
    if ((store = FindStoreBuyer(user)) != 0)
    {
        IBuyer* buyer = 0;
        GetStoreBuyer(&buyer, user, (CUser*)0);
        buyer->ExitPrivateStore();
        FreeStoreBuyer(user);
    }
    if ((store = FindStoreSeller(user)) != 0)
    {
        ISeller* seller = 0;
        GetStoreSeller(&seller, user);
        seller->RemovePrivateStore(true);
        FreeStoreSeller(user);
    }
    return 1;
}