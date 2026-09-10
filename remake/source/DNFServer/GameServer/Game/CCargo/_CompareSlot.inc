int CCargo::_CompareSlot(const void* a, const void* b)
{
    const Inven_Item* ia = (const Inven_Item*)a;
    const Inven_Item* ib = (const Inven_Item*)b;
    // ORIG 反汇编：ta/tb 用 jge/jle（有符号 int）；m_addInfo 用 jae（无符号）。
    int ta = ia->m_addInfo == 0 ? 9999 : (int)ia->m_field1;
    int tb = ib->m_addInfo == 0 ? 9999 : (int)ib->m_field1;
    if (ta < tb)
        return -1;
    if (ta > tb)
        return 1;
    // [b28d-P223] ORIG 0x0850bc5a 两 return 0 出口为**尾部独立块**：
    // `je +0xaa`（ta==9999）与 `je +0xa3`（addInfo 相等）各自 `mov $0`，块序
    // +0xa3 在 +0xaa 前 ⇒ 双 goto 尾标签串（平铺 return 会被 4.4 就地物化）。
    if (ta == 9999)
        goto ZERO_AA;
    if (ia->m_addInfo == ib->m_addInfo)
        goto ZERO_A3;
    if ((unsigned int)ia->m_addInfo < (unsigned int)ib->m_addInfo)
        return -1;
    return 1;
ZERO_A3:
    return 0;
ZERO_AA:
    return 0;
