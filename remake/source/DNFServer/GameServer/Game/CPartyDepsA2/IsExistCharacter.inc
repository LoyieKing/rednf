// ============================================================================
// IsExistCharacter —— ORIG T 0x085b624e（报告 CParty/IsExistCharacter.md）
// [b28d-solo-CPartyDepsA2 判词·AE] 比较式交换操作数（call 在左）消除 4.4 跨
// call 缓存形参入 %ebx 的 push/pop 与帧差（P52/P0）。
// ============================================================================
bool CParty::IsExistCharacter(int characNo)
{
    for (int local_10 = 0; local_10 <= 3; ++local_10)
    {
        if (!_checkValidUser(local_10))
            continue;
        // [b28d-solo-CPartyDepsA2] ORIG 0x085b62a8 段 `mov 0xc(%ebp),%edx;
        // cmp %edx,%eax`（call 后读形参入 edx）；把 call 放左操作数避免 4.4
        // 跨 call 缓存形参到 %ebx（P52/P0：左操作数先物化）。
        if (m_member[local_10].m_pUser->getCurCharacNo() == characNo)
        {
            return true;
        }
    }
    return false;
}