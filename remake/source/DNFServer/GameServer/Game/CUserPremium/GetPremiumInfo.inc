SUserPremiumInfo* CUserPremium::GetPremiumInfo(int type) const
{
    // P0：ORIG 无局部槽（sub 0，`add 0x8(%ebp),%eax`）——原码内联右值直返，
    // 禁 entries 具名局部（4.4 恒落 -0x4 槽差帧）。
    // ORIG：仅 type<0x6a 且 type!=0x270f 时按下标偏移；其余（含
    // 9999 和正向越界值）均返回 entries[0]，负值仍按原始负偏移。
    // [b28c-L7] P222 互试（第二形）：ORIG 0x086adf52 `cmpl $0x69; jg FAIL;
    // cmpl $0x270f; jne SUCC; FAIL: mov 0x8(%ebp),%eax; jmp end; SUCC:<calc>`
    // ⇒ 否定短路 || 早返（then=FAIL 落前、SUCC fall-through 落后）；三目形
    // 实测产 `jg X; je X; jmp Y` 镜像布局（+1 jmp）。
    if (type >= 0x6a || type == 0x270f)
        return const_cast<SUserPremiumInfo*>(m_entries);
    return const_cast<SUserPremiumInfo*>(&m_entries[type]);
