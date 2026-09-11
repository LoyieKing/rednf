// ORIG 0x085ca760（报告 GetStoreBuyer.md 逐指令）：按 world unique id 解析卖家
// CUser（初始置空）后委托 GetStoreBuyer(3 参版)。
void CPrivateStoreMgr::GetStoreBuyer(IBuyer** out, CUser* user, unsigned short sellerIndex)
{
    CUser* sellerUser = 0;
    // ORIG：参数直落（mov %ax,-0x1c 单槽），无具名中转。
    sellerUser = G_GameWorld()->find_from_world(sellerIndex);
    // [L14] ORIG +0x2d `cmpl $0x0,-0xc(%ebp)`（无 jcc）= 4.1 对空 then 保留
    // 比较、jump-opt 删掉「目标即下一条」的 je；4.4 前端把空条件整块 DCE。
    // 唯一可复现形 = 空 switch（探针 /tmp/prc/p3.cpp fU：`switch(p==0){}` 产
    // `cmpl $0x0,-0xc(%ebp)` 后直落，零分支/零 setcc）；`if(p==0){}`、
    // `while(p==0)break;`（多 sete+test 两条）、`if(p==0){asm("")}`（多 jne）
    // 均不可。语义等价（无 case/无副作用）。
    switch (sellerUser == 0) { }
    GetStoreBuyer(out, user, sellerUser);
}