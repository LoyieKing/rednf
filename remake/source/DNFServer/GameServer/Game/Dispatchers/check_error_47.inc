// ORIG 0x081d0f64（OnlyOrigBatch13 组 E 转写；3 参数符号，字段自 ParamBase 读）：
// state==3 且 curCharac 有效；guildkey==0→100；无公会驻地→0xb5；Secu Check 非 0
// 透传；物品锁→0xd5；param+0x10 与槽内 m_addInfo 不符或 find_item 失败→0xc5。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081d0f64 orig/ours=112/113 ndiff-blocks=10
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    0x10(%ebp),%eax | mov    %eax,-0x1c(%ebp) | mov    %eax,-0x18(%ebp)
unsigned int Dispatcher_GuildCargoPushItem::check_error(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    // [G1Frest 2026-09-10] ORIG 0x081e5xxx：`mov 0x10(%ebp),%eax; mov %eax,-0x1c(%ebp)`
    //   —— 0x10(%ebp) 是**第 2 参 msg**（0xc=user / 0x10=msg / 0x14=param）；其后
    //   全部字段读自该局部（+0xd/+0xe/+0x10）。旧码把视图绑在 param ⇒ **参数错位**（真 bug）。
    //   另：itemSpace/slot/reqAddInfo 三个具名局部为发明 —— ORIG 各使用点重读
    //   （`movzwl 0xe(%eax),%eax` / `mov 0x10(%eax),%eax`），P0/P48。
    (void)param;

    if (user->get_state() != 3 || user->getCurCharacR() == 0)
        return -1;
    if (user->get_charac_guildkey() == 0)
        return 100;
    if (user->IsExistGuildAgit() != 1)
        return 0xb5;

    // [G3 语义修正 2026-09-11] 原 `struct __attribute__((packed)) PushParamView{int m_4;
    //   char m_pad08[4]; uchar m_itemSpace; ...}` 在本 TU 实际产出 **+0x08/+0x09** 读
    //   （沙箱 asm 实证：0x219bb `movzbl 0x8(%eax),%eax` → call
    //   `GetInvenTypeFromItemSpace`、0x219de `movzwl 0x9(%eax)` → `CInventory::GetInvenSlot`
    //   实参），而 ORIG 0x081d0fdf 段为 `movzbl 0xd(%eax)` / `movzwl 0xe(%eax)` /
    //   `mov 0x10(%eax),%eax` ⇒ 旧视图**字段错位 5 字节**（真 bug：itemSpace/slot 读错）。
    //   改为显式 pad 的非 packed 视图（偏移由布局唯一决定：char[0xd] → +0x0d/+0x0e/+0x10
    //   与 ORIG 逐字段一致），并把引用改为具名指针（ORIG 0x081d0fe1 `mov %eax,-0x1c(%ebp)`
    //   为 msg 指针槽）。
    const MSG_GUILD_CARGO_PUSH_ITEM* pv =
        reinterpret_cast<const MSG_GUILD_CARGO_PUSH_ITEM*>(&msg);
    // [G3 2026-09-11] P28/P331 声明序=ORIG 序：ORIG 0x081d0fe4 先
    //   `call getCurCharacInvenR; mov %eax,-0x18(%ebp)`，0x081d0fff 才 call
    //   GetInvenTypeFromItemSpace ⇒ inven 声明必须在前。
    const CInventory* inven = user->getCurCharacInvenR();
    const int invenType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)pv->m_field0d);
    const Inven_Item slotItem = inven->GetInvenSlot(invenType, (int)pv->m_field0e);

    const int err =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)8);
    if (err != 0)
        return err;
    if (user->CheckItemLock(invenType, (int)pv->m_field0e))
        return 0xd5;
    if (pv->m_field10 == 0 || slotItem.m_addInfo != pv->m_field10)
        return 0xc5;
    // [G3 2026-09-11] P0：ORIG 0x081d10b5 `call find_item; mov %eax,-0xc(%ebp);
    //   cmpl $0x0,-0xc(%ebp); jne 081d10ca` ⇒ 原码有具名局部。
    CItem* const found = G_CDataManager()->find_item(slotItem.m_addInfo);
    if (found == 0)
        return 0xc5;
    return 0;
}