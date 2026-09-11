// ORIG 0x085ca7b8（报告 GetStoreBuyer_085ca7b8.md 逐指令）：买家未登记则按卖家
// 查其 store 并登记买家表；两者皆无时回退 +0x04 嵌入 store 的 IBuyer 子对象
// （this+8）并 SetBuyer 返回 0；命中时 *out 为 store+4 之 IBuyer 接口。
// [b28d-24] DIFF 残1: ORIG 遗留死 cmpl（P171 照抄）；P167 归零。ORIG 0x085ca7b8
// [b28d-solo-CPrivateStoreMgr] 残 1 行 = ORIG 0x085ca7b8 段 `cmpl $0x0,-0xc(%ebp)`
// 无 jcc 的死比较（4.1 对 `if(p==0){}` 删 je 留 cmp；4.4 前端整块 DCE）⇒ N99 封顶。
// [L14] 本轮复核：27/26，唯一差异 = 该 `cmpl $0x0,-0xc(%ebp)`（ORIG 0x085ca7b8+0x2d）。
// 再测 `if(p==0){}`（4.4 前端 DCE 整块）与 `while(p==0)break;`（多 setne+test 两条，
// P225 四段形）均不产「孤立 cmp」⇒ 4.1 空 then 残留无合法源形，维持 N99。
int CPrivateStoreMgr::GetStoreBuyer(IBuyer** out, CUser* user, CUser* sellerUser)
{
    CPrivateStore* store;
    if ((store = FindStoreBuyer(user)) == 0)
    {
        if ((store = FindStoreSeller(sellerUser)) == 0)
        {
            *out = &m_embeddedStore;   // 派生→IBuyer 隐式转换即 +4（ORIG this+8）
            m_embeddedStore.SetBuyer(user);
            return 0;
        }
        m_storeBuyers[user] = store;
    }
    *out = (store != 0) ? store : 0;   // 派生→IBuyer 隐式转换产生 store+4
    store->SetBuyer(user);
    return 1;
}