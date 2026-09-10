bool CParty::HasAllMemberSelection()
{
    for (int i = 0; i < 4; ++i)
    {
        // [b28d-51] P0/N96 清绑：删 `register uchar v asm("eax")` 与
        // `register bool s asm("al")`（全域禁绑定）；改裸 bool 形。
        // ORIG 0x085ba1c0 O@9-21 原形 `call; xor$1; test; jne` +
        // `call; test;sete%al; test; je`（4.4 清绑后物化路径见函数头判词）。
        if (!_checkValidUser(i))
        {
            continue;
        }
        if (m_routingData.GetMemberRoutingState(i) == 0)
        {
            return false;
        }
    }
    return true;
