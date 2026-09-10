float CParty::CalcAvatarBonus()
{
    float bonus = 0.0f;
    bool found = false;
    for (int i = 0; i <= 3; ++i)
    {
        // P133 拆独立守卫（ORIG 各 call 直落统一 continue 出口）；
        // P194 `bool z=(expr); if(z)` 复现 setle 物化岛。
        if (!_checkValidUser(i))
            continue;
        // b28d-49 @0x85ad033：ORIG `call IsEquipAvatar; test %al,%al; je <continue 岛>`
        // ⇒ 正极性（无 `!`）；负极性产 xor$1;jne 并多一条 jmp 岛（ndiff 6）。
        if (m_member[i].m_pUser->IsEquipAvatar())
        {
            found = true;
            break;
        }
    }
    if (found)
    {
        // b28d-49 @0x85ad05e：ORIG `cmp $0x1,%eax; setle %al; test %al,%al; je`
        // = if/else 直条件（探针 f1 逐条同形；三目形产 jg、具名 bool 落槽并使
        // 循环变量/found 槽位整体下移）。
        if (get_member_count() <= 1)
            bonus = 0.02f;
        else
            bonus = 0.05f;
    }
    return bonus;
