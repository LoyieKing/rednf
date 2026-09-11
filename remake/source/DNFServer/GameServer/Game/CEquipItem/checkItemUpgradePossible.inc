// [b28d-solo-CEquipItem] ORIG 0x0851388e（34 条，N 封顶）：值全程寄存器直传（无槽往返）
// ⇒ P0 判原码为内联表达式右值；4.1 对 `&&` 两侧同一 this->m_itemType 的两次读复用为单条
// `mov 0x234(%eax),%eax`（cmp 与 shl 共用 %eax）。4.4 -O0 无该复用：内联形恒发第二次读
// （36 条），register 局部形（P231 合法）改出 `mov %eax,%ebx; cmp %ebx; ja; mov %ebx,%eax`
// 两条中转（同 36 条，掩码段寄存器分配差异）。两形实测（/tmp/p1/t.cpp f_inline/f_reg、
// t2.cpp g1..g7、t3.cpp h1..h6、4.4.6 后端同形）均差 2 条 ⇒ 跨版本 CSE 结构性封顶。
// [b2c 复核·维持 N18a] 父泳道建议的 P251 `switch (m_field234)` 形**不适用**：ORIG
//   0x085138c0..0x085138d4 是 `mov $1,%edx; shl %cl,%ebx; and $0x3ff400,%eax; test` 的
//   **位测试**语义（掩码 0x3ff400 的 7 个命中值写成 case 会发 7 条 cmp/je 比较链 = 必劣），
//   且 ORIG `je 085138dd`（return 0 出口）证明两条件为短路结构 ⇒ 源码必是两次成员读、
//   4.1 在 RTL 层 CSE 掉第二次载入，4.4 `-O0` 无该 pass。本 TU 现状 = 内联形（36 条），
//   第二次载入在 0x...8b5 位置（我方 `mov 0x8(%ebp),%eax; mov 0x234(%eax),%eax`）。
// [P6S4 2026-09-11 头轴备忘] 本 TU .o 有 `_ZNSt12_Destroy_auxILb0EE9__destroyIP24
//   STAvatarAbilityVariationEE…`，ORIG 仅 ILb1 同参 ⇒ 我方 EquipmentParameterInfo.h:118
//   多写 `~STAvatarAbilityVariation() {}`；影响面 9 TU，已上报 P7HeaderAxis 统一裁决。
//   本件残余与该 ILb0/ILb1 轴相关，定义侧不动。
bool CEquipItem::checkItemUpgradePossible() const
{
    if (IsImpossibleUpgrade())
    {
        return 0;
    }
    // [L23-r3 试形记录·回退] ORIG 0x085138b2 单次 `mov 0x234(%eax),%eax` 后在
    //   %eax/%ecx 复用；试 `register unsigned int itemType = (unsigned int)m_itemType;`
    //   后实测仍 36 条（= 试前，ORIG 34）⇒ 4.4 寄存器复用不可造（N18a 族），已回退。
    //   [b28e 试形记录] P226 语句表达式形（`({unsigned int t=(unsigned)m_itemType;
    //   t<0x16 && ((1<<t)&0x3ff400U)!=0;})`）fresh 实测 **41 条**（t 落槽 →
    //   `mov %eax,-slot` + 两次回读，比内联形 36 更劣）⇒ 回退，维持内联形。
    // [P6S4 2026-09-11 负结论·掩码轴实测] 试掩码侧 `(unsigned long)` cast 破
    // 4.4 的 `(1<<s)&C → (C>>s)&1` fold（同 CUserStatC 落地形）：复编读数
    // blocks=1 / ours=36 orig=34 **不变**，残差仍只有 `mov 0x8(%ebp),%eax;
    // mov 0x234(%eax),%eax` 第二次成员载入（N18a 跨版本 CSE，非 fold）⇒ 本件
    // 掩码段与 ORIG（`mov $1,%edx; shl %cl,%ebx; and $0x3ff400,%eax`）本就同形，
    // cast 属多余 ⇒ 回滚。
    if ((unsigned int)m_itemType < 0x16 &&
        ((1 << (unsigned int)m_itemType) & 0x3ff400U) != 0)
    {
        return 1;
    }
    return 0;
}