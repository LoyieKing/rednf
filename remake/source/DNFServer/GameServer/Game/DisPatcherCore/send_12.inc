// ORIG 0x081be20a T _ZN18DisPatcher_BuyItem4sendEP5CUserR9ParamBase
// 报告：docs/class_func_reports/DisPatcher_BuyItem/send.md（汇编+反编译 C 全读）
// [b28c-L19 N18a] 唯一正文差（165/164）：`UseFinishPoint(out.m_8c)` 处 ORIG
// [b28d-29] N18a/P254：ORIG `mov 0x8c(%eax),%eax; mov %eax,%edx` 冗余 regmove（int 字段->int 形参），4.4 单段直载 edx 不可反向造  (ORIG 0x081be20a insn 165->165)
// `mov 0x8c(%eax),%eax; mov %eax,%edx` 冗余 regmove vs 4.4 单步直载 edx（反向不可造）。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 165 / ours 165 insn 全等（AE 口径，重定位归一）。
void DisPatcher_BuyItem::send(CUser* user, ParamBase& param)

{
    BuyItemParam& out = static_cast<BuyItemParam&>(param);
    PacketGuard guard;
    guard.put_header(1, 0x15);
    guard.put_byte(1);
    guard.put_int(user->getCurCharacInvenR()->get_money());
    guard.put_int(user->UseWinPoint(out.m_88, (eWPSubReason)0));
    guard.put_int(user->UseFinishPoint(out.m_8c));
    guard.put_int(user->UseCera(out.m_84));
    guard.put_short(out.m_a0);
    guard.put_int(out.m_item.m_addInfo);        // param+0x06（m_item+0x02）
    guard.put_int(out.m_item.m_addInfo2);       // param+0x0b（m_item+0x07）
    guard.put_short(out.m_item.m_endurance);       // param+0x0f（m_item+0x0b）
    guard.put_byte((int)out.m_item.m_amp.getAbilityType() & 0xff);   // param+0x15
    guard.put_short((int)out.m_item.m_amp.getAbilityValue() & 0xffff);
    guard.put_packet(out.m_item);
    guard.put_int(out.m_pair.first);            // param+0x98
    const Inven_Item* ref =
        user->getCurCharacInvenR()->GetInvenRef(1, out.m_a4);
    if (ref == 0)
    {
        guard.put_int(0);
    }
    else
    {
        guard.put_int(ref->m_addInfo2);         // ref+0x07
    }
    guard.finalize(true);
    user->Send(guard);
}
