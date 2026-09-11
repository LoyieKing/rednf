// ----------------------------------------------------------------------------
// ORIG T 0x0812d278（summonSlotUnit.md 全文）：cfg 同上；it=m_slotItems.find(slot)；
// 未命中 → 4；cost=**(int**)(cfg+0x18)*it->second.m_field8（iterator-> +0xc=
// pair second@+4 内 +0x08 字段）；条件 it->second.m_needGage==need（+0x08）
// 且 cost<=m_gage → 扣费 ret0；否则 ret2。
// ----------------------------------------------------------------------------
unsigned int CharacData::summonSlotUnit(unsigned char slot, int need)
{
    ConfigParameter* cfg = &G_CDataManager()->m_altarConfig;  // CDataManager+0x6364（summonSlotUnit.md `add $0x6364`）
    std::map<unsigned char, _SlotItem>::const_iterator it = m_slotItems.find(slot);
    if (it == m_slotItems.end())
    {
        return 4;
    }
    // +0x18 = m_strPtr 双解引（mov 0x18(%eax);mov(%eax)）。
    // ORIG 0x812d…：先读 it->second.m_field8（0xc(%eax)→%edx）再解 m_strPtr，
    // imul %edx —— 乘法左操作数为 m_field8（求值顺序复现）。
    const int cost = it->second.m_field8 * *reinterpret_cast<int*>(cfg->m_strPtr);
    // b28d-solo-ProcStage 真语义 bug 修正：ORIG 0x0812d330 `mov 0x8(%eax),%eax`
    //   （eax=pair* ⇒ pair+0x8 = second+0x04 = m_itemId）与 need 比较；
    //   Ghidra C `if (*(int*)(iVar4 + 8) == param_2)` 联证。旧形误用 union 视角
    //   m_needGage（+0x08），比错字段 4 字节。
    if (it->second.m_itemId == need && cost <= m_gage)
    {
        m_gage -= cost;
        return 0;
    }
    return 2;
}