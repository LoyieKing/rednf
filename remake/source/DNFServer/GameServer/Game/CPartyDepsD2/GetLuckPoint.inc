void CParty::GetLuckPoint(CLuckPoint* luckPoint, bool onlyAlive)
{
    if (luckPoint == 0)
        return;
    for (int i = 0; i < 4; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        // [X2·技巧22 精化形] ORIG 0x085b61be O@17-22：`cmpb $0,onlyAlive;
        // je` 短路 + `movzbl liveFlag; xor$1,%eax; test; je` 真值取反判。
        // [b28d-51] P0/N96 清绑（原 `register uchar lf asm("eax")`）；arr54
        // 已 bool 化，`!arr54[i]` 即 ORIG `xor$1; test; je` 语义。
        if (onlyAlive && !m_battleData.m_arr54[i])
            continue;
        luckPoint->AddUser(m_member[i].m_pUser);
    }
