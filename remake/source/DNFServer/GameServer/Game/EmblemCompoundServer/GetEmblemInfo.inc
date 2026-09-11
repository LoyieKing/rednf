// [b28d-solo-EmblemCompoundServer] GetEmblemInfo 判词（ORIG 0x08191818，126 insn）：
// IDENTICAL/AE。修法：SigAradEmblemCompound 加 materialInfo[2] 匿名 union 数组视角
// （P235/P246，scaled 索引 `(%eax,%edx,8)`）；slot 内联实参无槽（P0/P157）；
// materialInfo::itemIdx 同型化为 unsigned int 复现 ORIG `mov 0x2(%eax),%ecx` 单条
// 直装（P211 档③/P248；int 字段恒 `mov→%eax; mov %eax,%ecx` 两段）。
bool EmblemCompoundServer::GetEmblemInfo(CUser* user,
                                         arad::SigAradEmblemCompound* sig,
                                         int idx)
{
    // ORIG 0x08191818：slot 为内联实参（无槽），每次使用点重读 m_material[idx].slot
    // （P157），scaled 索引 (%eax,%edx,8) 由数组视角复现。
    const Inven_Item* ref = user->getCurCharacInvenR()->GetInvenRef(
        1, sig->m_material[idx].slot);
    if (ref == 0 || ref->isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0x16);
        ARAD::PRINT_ERROR::isEmpty("EMBLEM COMPOUND", user,
                                   sig->m_material[idx].slot);
        return false;
    }
    if (user->CheckItemLock(1, sig->m_material[idx].slot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0xd5);
        ARAD::PRINT_ERROR::CheckItemLock("EMBLEM COMPOUND", user,
                                         sig->m_material[idx].slot);
        return false;
    }
    CItem* item = G_CDataManager()->find_item((int)ref->m_addInfo);
    if (item == 0 || item->GetItemType() != 0x14)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24e, 0x11);
        ARAD::PRINT_ERROR::find_item("EMBLEM COMPOUND", user,
                                     (int)ref->m_addInfo);
        return false;
    }
    // 写回条目 grade(+2)/itemIdx(+4)：ORIG scaled store。
    sig->m_material[idx].grade = (short)item->get_grade();
    sig->m_material[idx].itemIdx = ref->m_addInfo;
    return true;
}