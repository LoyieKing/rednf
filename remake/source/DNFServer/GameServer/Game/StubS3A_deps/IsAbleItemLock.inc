// ---------------------------------------------------------------------------
// 1. CUser::IsAbleItemLock(int,int) const —— ORIG T 0x08646a9c
//    报告 docs/class_func_reports/CUser/IsAbleItemLock.md 全文转写。
//    0x15=21 无物品/无 item 定义；0x13=19 业务不可锁；0x11=17 attach 型不可锁。
// [b28d-62] N82：p167 CLEAR。ORIG 0x08646a9c
int CUser::IsAbleItemLock(int space, int slot) const
{
    const Inven_Item* item =
        GetCurCharacInventoryRef((unsigned short)space, (unsigned short)slot);
    if (item == 0)
        return 0x15;
    CItem* it = G_CDataManager()->find_item(item->m_addInfo);
    if (it == 0)
        return 0x15;
    if (it->is_stackable())
        return 0x13;
    // [b28d-63 本 TU] P225 推广：ORIG 0x08646b2c 两条件各自物化
    //   （`cmp $0x16; sete; test; je L` + `movzbl; xor $1; test; je L`，L=then 之后）
    //   ⇒ 拆嵌套 if；`&&` 合写在 4.4 恒折成短路直跳 + bool 汇合（多 4 条）。
    if (it->GetItemType() == 0x16)
    {
        if (item->m_seal != 1)
            return 0x13;
    }
    if (it->GetItemType() == 0xb && it->GetAttachType() == 2)
        return 0x11;
    if (space == 0xc)
        return 0x13;
    unsigned char lock = item->m_amp.GetLock();
    if (lock != 0)
        return 0x13;
    // ORIG 0x08646bb6：result 声明点预清 0（movl $0x0,-0xc）先于 GetCharacExpandDataR；
    // 结果免槽直传（ORIG 无 expand 局部），返回值落 result 槽后按 P225 X3 判返。
    // GetCharacExpandDataR(this,2) 返回数据区指针，直接作为
    // item_lock::CItemLock* this 传入 IsAbleItemLock（ORIG 同款怪癖）。
    int result = 0;
    result = ((item_lock::CItemLock*)GetCharacExpandDataR(
                  (ENUM_CHARAC_EXPAND_TYPE)2))
                 ->IsAbleItemLock(lock);
    while (result != 0) {
        return result;
    }
    return 0;
}