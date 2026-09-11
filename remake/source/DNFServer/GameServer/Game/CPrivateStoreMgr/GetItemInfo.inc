// ---- GetItemInfo（ORIG T 0x85c9f22，报告 GetItemInfo.md L15-218 全文；跳转表 .rodata 0x8cbd6e4 实读）----
// 按 item space 分派来源槽读取：[0]invR type1 [1]invR type2 [2]cargo [3]invR type0
// [7]invR type3；4/5/6 与越界同走默认失败。读回后置 +2 非零并过 CheckValidItem。
// [b28d-24] DIFF 残84: 帧/槽分配（P112/N82）。ORIG 0x085c9f22
// [b28d-solo-CPrivateStoreMgr] 已修：case 叶块序 0,2,3,1,7（P46，跳转表
// @0x8cbd6e4 实证）+ 尾部 if(!CheckValidItem) return 0; return 1;（P214②）。
// 残 16 行 = 4 个 case 块 Inven_Item 局部槽方向镜像（ORIG -0x13d/-0x100/-0x86/
// -0x49 vs 我们 -0x49/-0x86/-0x100/-0x13d）⇒ N93 封顶（互斥块对象槽方向
// 跨版本相反，4.1 先写块占更深槽、4.4 占更浅；提至函数头会引入 5 次默认构造）。
bool CPrivateStore::GetItemInfo(CUser* user, PrivateStoreItem& item)
{
    int prevAddInfo2 = (int)item.m_item.m_addInfo2;

    // ORIG 跳转表 @0x8cbd6e4：[0]=0x85c9f51 [2]=0x85c9fa1 [3]=0x85c9fe9
    // [1]=0x85ca039 [7]=0x85ca086 [4/5/6]=0x85ca0cd ⇒ 叶块物理序 = case
    // 书写序 0,2,3,1,7（P46），case→体映射与源码一致，仅需按 ORIG 重排。
    switch ((signed char)item.m_field46)
    {
    case 0:
    {
        // [b28d-solo-L43] P0 限定①：ORIG 4.1 对互斥 case 块临时对象按「先书写占更深
        // 槽」分配（-0x13d/-0x100/-0xc3/-0x86/-0x49 逐块 0x3d 递减），具名局部在
        // 4.4 下反向（先书写占更浅）⇒ 去掉具名 slotData、临时对象内联进实参，
        // 4.4 复现 ORIG 深→浅方向（探针 /tmp/l43_p3.cpp g_inline vs g_named）。
        item.m_item.setCopy(
            user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)1, (short)item.m_field48));
        break;
    }
    case 2:
    {
        item.m_item.setCopy(
            user->getCurCharacCargoR()->get_cargo_slot((short)item.m_field48));
        break;
    }
    case 3:
    {
        item.m_item.setCopy(
            user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)0, (short)item.m_field48));
        break;
    }
    case 1:
    {
        item.m_item.setCopy(
            user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)2, (short)item.m_field48));
        break;
    }
    case 7:
    {
        item.m_item.setCopy(
            user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)3, (short)item.m_field48));
        break;
    }
    default:
        return false;   // 表项 4/5/6 与 ja 默认路径同址 0x85ca0cd
    }

    if ((int)item.m_item.m_addInfo == 0)
        return false;
    // P214②：ORIG 0x85ca0b6 xor$1;test;je;mov$0;jmp;mov$1 ⇒ if(!f())return 0;
    // 成功路径 fall-through 到单一 return 1。
    if (!CheckValidItem(item, prevAddInfo2))
        return false;
    return true;
}