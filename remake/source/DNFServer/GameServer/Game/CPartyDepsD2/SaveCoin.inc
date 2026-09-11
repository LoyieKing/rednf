// ---------------------------------------------------------------------------
// SaveCoin —— ORIG T 0x085b3520
// 报告：docs/class_func_reports/CParty/SaveCoin.md
// eCoinSubReason 参数未消费（ORIG 形态）；全员 SaveMoneyCoin。
// ---------------------------------------------------------------------------
// [X2·技巧22 精化形（Main 批准，ORIG 0x085b3520 O@9-12）] callee 结果
// xor-化：绑 %eax 具名 uchar + ^=1 → `call; xorl $1,%eax; testb; jne`。
void CParty::SaveCoin(eCoinSubReason reason)
{
    (void)reason;
    for (int i = 0; i < 4; ++i)
    {
        // [b28d-51] P0/N96 清绑（原 `register uchar v asm("eax")`）。
        if (!_checkValidUser(i))
        {
            continue;
        }
        memberUser(this, i)->SaveMoneyCoin();        // T 0x086504d8
    }
}