// ---------------------------------------------------------------------------
// 6. CraneMinigameManager::startUseMaterial(CUser*,int) —— ORIG T 0x080ebe06
//    （无独立报告，d_aa_body.cpp objdump 0x080ebe06..0x080ebcbb 转写）。
//    this+0x10（m_field10）=材料 item idx；this+0x14 低 16 位符号扩展=数量。
//    0x11=材料不符；0x16=数量校验/删除失败；0=成功。
// [b28d-62] N82：p167 CLEAR。ORIG 0x080ebe06
// [b28d-63 本 TU] P0/§4.3：删 inven/invenW/needCnt 三个具名局部（ORIG 全内联，
//   帧 0x78→ORIG 0x64），裸偏移改具名字段 m_material(+0x10)/m_field14(+0x14)
//   （ORIG `mov 0x10(%eax),%eax` / `movzwl 0x14(%eax),%eax` 单条 disp）。
int CraneMinigameManager::startUseMaterial(CUser* user, int slot)
{
    // ORIG 0x080ebe0d：getCurCharacInvenR() 结果免槽直传 GetInvenSlot。
    Inven_Item item = user->getCurCharacInvenR()->GetInvenSlot(1, slot);
    if (item.m_addInfo != m_material)                       // ORIG 0x080ebe3f
        return 0x11;
    // ORIG 0x080ebe4d：needCnt 免槽（movzwl+cwtl 直读），比较用寄存器。
    if (item.m_addInfo2 < (int)(short)m_field14)            // ORIG jge 0x080ebe5a
        return 0x16;
    // ORIG 0x080ebe63：needCnt 第二次重读（movzwl+movswl %ax,%ebx），receiver 免槽。
    if (!user->getCurCharacInvenW()->delete_item(
            (INVEN_TYPE)1, slot, (int)(short)m_field14,
            (eItemDelReason)0x34, true))
        return 0x16;
    return 0;
}