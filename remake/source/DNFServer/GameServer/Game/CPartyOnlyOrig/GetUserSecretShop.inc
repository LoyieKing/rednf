// ORIG T 0x085b9e80（GetUserSecretShop.md）：+0x2a8 + slot*0x1c 起的逐成员
// secret 商店数据视图；slot 出界返回空。
// b28d-solo-CPartyOnlyOrig 判词：DIFF→AE。修法：CParty.h +0x2a8 落
// POD 具名数组成员 `m_secretShopInfo[4][0x1c]`（R2 已广播，sizeof/偏移不变）
// ⇒ `&this->m_secretShopInfo[slot]` 复现 ORIG 的 28x 强度归约链与
// `lea 0x2a0(%edx),%eax; add this; add $8` 常量拆分；cast 取址形恒多
// `mov this,%ecx` 中转（探针 /tmp/b28d_s187/p11/p14/p18）。
void* CParty::GetUserSecretShop(CUser* user)
{
    int slot = GetUserPosition(user);
    // [Ghidra 0x085b9e80 双证] 命中区间写正向 `0<=slot<=3`（ORIG js/jg 双
    // 跳共享 0 岛）；ORIG 总偏移 = this+0x2a0+8+slot*0x1c = **0x2a8**+slot*0x1c
    //（CParty.h 中 SECRET_SHOP_DATA 头 m_padShop@+0x2a4 + 数组
    // m_secretShopInfo@+0x2a8；前人误将区域基址 0x2a0 当子对象基址写 8，实产
    // 0x2ac 差 4B）。
    if (slot >= 0 && slot <= 3)
    {
        // ORIG 地址 this+0x2a8+slot*0x1c = SECRET_SHOP_DATA(+0x2a4).m_info
        // （+0x04，SECRET_SHOP_INFO 0x1c）。
        // [b28d-solo-CParty2] CParty.h 字段化：+0x2a4 区为真实成员
        // m_secretShop（SECRET_SHOP_DATA），m_info 数组 @+0x04 ⇒ 本式偏移
        // 仍 = this+0x2a8+slot*0x1c（与原 m_secretShopInfo 同址）。
        return reinterpret_cast<secretshop::SECRET_SHOP_INFO*>(
            &m_secretShop.m_info[slot]);
    }
    return 0;
}