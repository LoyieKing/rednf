int DisPatcher_DisJointItem::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }  // ORIG 0x082333a6 W _ZN23DisPatcher_DisJointItem7processEP5CUserR8MSG_BASER9ParamBase: push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret —— 原始二进制即为空实现，全部逻辑位于 dispatch_sig(0x081f9fb8)->_disjoint(0x081f92ca)

// ---------------------------------------------------------------------------
// LinkF1 批次（2026-08-28）：DisPatcher_DisJointItem::_disjoint —— ORIG T
// _ZN23DisPatcher_DisJointItem10_disjointEP5CUserii14ENUM_CMDPACKETS1_t @0x081f92ca
// （报告 docs/class_func_reports/DisPatcher_DisJointItem/_disjoint.md 全文转写，
// 草稿 /tmp/df_batches/linkf1/_disjoint.body.cpp + notes.md）。
// 依赖 _check_disjointable_user_state/_check_disjointable_item_state/
// _check_disjoint_need_slot：真实 C++ 名定义在 ExpertJobLink4.cpp（DispDebugJoint4.cpp
// 中的 _Z30 前缀名是带 asm 标签的桥接，勿混淆）。
// ---------------------------------------------------------------------------
