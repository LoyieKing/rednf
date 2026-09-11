// ORIG T 0x0821b71c _ZNK28Dispatcher_UseVendingMachine16_putItemIntoUserEP5CUsermjRNS_17stPutItemResult_tE
//   把 itemIdx（0=金币/1=活动币/2=胜点/else=实物）交给用户，填充 result。
// b28d-solo-Dispatcher_UseVendingMachine_impl [N7+N18a+setcc-fold]：ORIG 547 insn /
//   我方 552；p167 调用点差集 CLEAR 62/62（0 缺 0 多）⇒ 语义完成，byte 层封顶（§6.2）。
//   残差：①4 处虚调用 vptr 链目标寄存器（ORIG `mov (%eax),%edx; … call *%edx`
//   vs 4.4.7 恒目标 %eax/this %edx，N7；本 TU 未路由 446）；②`mov 0x8(%eax),%eax;
//   mov %eax,%edi` 被 4.4 折成单步（N18a）；③`A && B` 第二条件 ORIG setne 物化
//   vs 4.4 分支+常量物化（setcc-fold 镜像）；④registNewEffect arg1（int 字段→
//   unsigned 形参）恒多 `mov %eax,%edx` 中转（P265 族；字段侧需动共享头，未改）。
//   语义修正：补 `outItem.m_addInfo = itemIdx;`（ORIG 0x0821b883 实证，旧实现缺失）。
void Dispatcher_UseVendingMachine::_putItemIntoUser(
