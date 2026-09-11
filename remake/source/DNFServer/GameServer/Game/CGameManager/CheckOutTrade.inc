void CGameManager::CheckOutTrade(CUser* user)
{
    // ORIG 0x08297c25（Ghidra: cVar1 == '\x01'）：空 then + else 结构命中
    // xor $0x1,%eax + test %al,%al + jne skip（body fall-through，探针 wb 实证）
    if ((bool)(user->CheckInTrade() ^ 1))
    {
    }
    else
    {
        CTradeSpace* tradeSpace = m_tradeSpacePool.Get(user->GetTradeSpace());
        tradeSpace->cancel_trade_by_dis(user);
        PutTradeSpace(tradeSpace);
        return;
    }
    // ORIG +0x5c 尾岛：else 块尾 return + 函数尾显式 return → call;jmp;merge;leave
    return;
}