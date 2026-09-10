// [P8S1 2026-09-11 本波复核] 维持封顶：N206 整数提升窄化(ORIG 0x081475ec movzwl+shr %ax vs 我方 movswl+shr %eax)；P139/P339/P341/disp 折叠/掩码 cast
//   诸新钥匙逐条核过，无翻案。证据见 res_1.tsv 同件 note。
void LevelupSupportEventManger::InsertItem(CUser* user, int eventIdx, ENUM_ERROR& err)
