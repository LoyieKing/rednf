DisPatcher_SelectDungeon::~DisPatcher_SelectDungeon() {}
// ORIG 0x081c7f32 T _ZN24DisPatcher_SelectDungeon11check_errorEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_SelectDungeon/check_error.md 汇编+反编译 C 全文转写）：
// state∉{5,0xd} 或 IsBusyDisjointStore → 0x7fffffff；m_field10∉[0,2] 或
// m_dungeonType<0 → -1；m_field11∉[0,1] → -2；m10>0 且 m11>0 → 0x7fffffff；
// m10!=2 时要求队长（getManager()==user）、party state==1、!(m11==1&&is_quick_party)；
// 两段重复 party 检查（ORIG 0x0d9 与 0x0a9 两块），全部通过 → 0。
