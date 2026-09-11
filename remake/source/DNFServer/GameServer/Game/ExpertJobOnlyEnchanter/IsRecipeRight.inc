// ---------------------------------------------------------------------------
// IsRecipeRight（报告 IsRecipeRight.md，ORIG T 0x849ca06 sz 0x63）：
// m_field40 非空、非 stackable、虚槽 vtable+0xc（GetItemType）== 3 才为真。
// ---------------------------------------------------------------------------
// b28c-1 N7: 虚调用 callee %eax↔%edx 互换。
bool CEnchanter::IsRecipeRight()
{
    if (reinterpret_cast<CItem*>(m_field40) == 0)
        return 0;
    if (!reinterpret_cast<CItem*>(m_field40)->is_stackable())
        return 0;
    if (reinterpret_cast<CItem*>(m_field40)->GetItemType() != 3)
        return 0;
    return 1;
}