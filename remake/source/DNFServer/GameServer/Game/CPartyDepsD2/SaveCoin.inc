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
