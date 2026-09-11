// [b26-NL28] GetFinalProbability：删 4 个无 ORIG 证据的编造局部后帧 sub $0x38=ORIG；
//   残差=rate/weight 槽位平移（4.1 把 FPU cw 临时夹在具名槽之间，4.4 恒置槽区尾部，
//   N82 族源形不可消除），封顶。
// [b28d-26] NEAR 11 保持，N82 封顶（实测三形）：ORIG 0x0854c470 具名槽
//   goods(-0xc)/rate(-0x1c)/weight(-0x24) 且 cw 临时夹在 -0x1e/-0x20；4.4 恒
//   goods(-0xc)/rate(-0x10)/weight(-0x14)、cw 落 -0x1a/-0x1c（分配方向相反）。
//   试形①char padD[4]+padX[0xc] 声明穿插 → 仅帧 0x38→0x48、具名槽不动；
//   试形②double framePad;(void)&framePad（P190）→ 同样只撑帧不动槽基 ⇒ 封 N82。
int ItemVendingMachine::GetFinalProbability(int goodsIdx)
{
    // [P6S6 2026-09-11 复核·仍 N] 试 P339 声明序反转（goods→rate→weight）：
    //   4.4 槽位随声明序变（goods -0x14/rate -0x10/weight -0xc），difflib 块 3→4
    //   反劣（ORIG 为 goods -0xc/rate -0x1c/weight -0x24 稀疏深槽 + cw 夹 -0x1e/-0x20，
    //   4.4 恒紧凑 + cw 置尾）⇒ 已回滚，维持 N82。
    float weight;
    int rate;
    CCeraShopGoods* goods;
    goods = reinterpret_cast<CCeraShopGoods*>(G_CDataManager()->FindGoods(goodsIdx));
    weight = goods->GetBonusWeight();
    rate = reinterpret_cast<VendingItemListView*>(G_CDataManager())->m_list->GetVendingBonusRate();
    rate = (int)((float)rate * weight);
    return rate;
}