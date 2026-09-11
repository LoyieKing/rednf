// ---------------------------------------------------------------------------
// CUser::use_stackable(short slot, int space, int p3, int p4) T 0x0865e0ae sz0x8e5
// 报告：docs/class_func_reports/CUser/use_stackable.md（## 汇编+## 反编译 C 已完整阅读）
// 校验链（trade/find_item/is_stackable/CAlchemist/CoolTime）→use_item→
// BossTower/连续道具/CAlchemist 消费→LV70Buff 与疲劳药品表→结果包 0x2f。
// ---------------------------------------------------------------------------
void SendLV70Buff(CUser* user, unsigned long buff);   // ORIG T 0x0865defc（owner：Buff 域批次）
