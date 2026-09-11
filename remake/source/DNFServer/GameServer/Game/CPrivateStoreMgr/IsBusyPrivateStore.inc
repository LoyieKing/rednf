// ORIG 0x85cad5e（报告 IsBusyPrivateStore.md 逐指令）：卖家在册且状态为 1/2 时，
// 未生成娃娃即忙碌；已生成娃娃仅状态 1 忙碌。
bool CPrivateStoreMgr::IsBusyPrivateStore(CUser* user)
{
    CPrivateStore* store = 0;
    store = GetInstancePrivateStoreMgr()->FindStoreSeller(user);
    // ORIG 0x085cad5e：busy 段 &&/|| 物化岛（4.4 原生复现，探针 x15 b1）；
    // isDollCreated 正向 test%al 直判（==0 写法会产 xor$1）；
    // state==1 判定走 sete 物化 + mov$1/$0 岛（`if(...) return true; return false;`）。
    if (store != 0 && (store->GetState() == 1 || store->GetState() == 2))
    {
        if (store->isDollCreated())
        {
            if (store->GetState() == 1)
                return true;
            return false;
        }
        return true;
    }
    return false;
}