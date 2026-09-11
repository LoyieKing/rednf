// ORIG T 0x08651322：填充单角色的可见数据区（avatar 外观槽 / 租赁槽 /
// 非当前角色缓存直拷 / 改名状态）。
// b28d-lane5 N封顶: N7/N18a 跨版本寄存器分配 | ORIG 0x08651322 | _ZN5CUser27updateCharacViewVisibleDataERK12_Charac_infoR17CHARAC_LOGIN_DATA
// [b28d-GrpD 判词·N] ORIG 0x08651322：残差全是**同址等价形式差** ——
//   ① ORIG `imul $0x3d; add $0xe0; add $0x16`（=0xf6）vs 我方折叠 `add $0xf6`；
//   ② ORIG `add $0xea,%edx; mov 0xe(%eax,%edx,8)` vs 我方 `shl $3; add $0x75e; add`
//      （同址：0xe+0xea*8 = 0x75e，副偏移 0x12 = 0xe+4、0x14 = 0xe+6）；
//   ③ 0x75e/0x762/0x764 与 ORIG 逐字节相等 ⇒ 字段偏移/极性/符号性全一致，无 bug。
void CUser::updateCharacViewVisibleData(const _Charac_info& info,
