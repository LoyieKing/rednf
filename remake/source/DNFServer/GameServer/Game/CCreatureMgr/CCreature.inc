CCreature::CCreature()
{
    // [b28d-Main·检查点14 真多态化] 基类 ctor 由编译器隐式调用（ORIG 0x833705e
    //   `call CCreatureItem::C1Ev` + `movl $0x8c2f4c0,(%eax)` 自动复现）；
    //   原 `new (this) CCreatureItem` + 手工 m_vptr 写已删（消 N83 placement-new）。
    // [b28d-Main·检查点14] 删除冗余成员初始化：ORIG ctor 体仅 Reset()，
    //   +0x34/+0x38/+0x50 的初始化由 CStomach::C1Ev/COverSkillTime::C1Ev 完成
    //   （本仓库两成员 ctor 已同形）⇒ 复现 ORIG 21 条。
    Reset();
}
