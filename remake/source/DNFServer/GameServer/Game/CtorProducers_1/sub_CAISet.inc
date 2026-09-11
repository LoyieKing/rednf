// §4.5 例外：CAICharacter::set 符号经 asm 别名桥提供（保留）。
// 无法转真实成员调用的原因：`STAICharacterCommonScript` 完整 0x1010 布局仅
// 存在本 TU / CAICharacterList.cpp 的局部视图，sync_script.h 现存的 8 字节
// 占位（m_field0+m_str4）与本 TU 冲突（本 TU 已 include sync_script.h），
// 无法在此重定义全布局类型以产生精确 mangled `_ZN12CAICharacter3setEiRK25
// STAICharacterCommonScript`。`CAICharacter` 亦无跨 TU 权威头。待 sync_script
// 域统一 STAICharacterCommonScript 权威全布局 + 建 CAICharacter 权威头后，
// 改真实成员定义。
// 【b22 建模评估·维持例外】统一权威头专项核账发现本域四视图（本 TU 两 view/
// CAICharacterList 0x1014/sync_script 占位/CharacterAIA2 完整类）存在未裁决
// 的头部偏移矛盾：ORIG C1 0x088c8a78 首指令 `movl $0x8e0a1f0,(%eax)` 实证
// vptr@+0x00（多态），而三个消费视图以 +0x00 作 m_field0/int 承载；且
// +0x04 槽在 sync_script.cpp:921 按 std::string(m_str4) 消费、在
// CharacterAIA2 类视角为 int——视图仅靠"各自函数不触矛盾字段"维持二进制
// 兼容。统一即需逐字段重校四视图+迁移 sync_script.h 全部 10+ 消费 TU 与
// CharacterAIA2/S22x 在途工作面，非本单粒度可收口；盲改任何一处即静默偏移
// 错位。判定：asm 别名桥保留为终判，域统一另开 AI script 专项单（本注释即
// 其工单输入）。
// §4.5 asm 别名桥例外（四视图偏移矛盾未裁决，无法转真实成员定义）。
// 形参直接取权威视图类型：ABI 与 ORIG 成员函数 `(int, const&)` 同布局
// （this@0x8 / idx@0xc / script@0x10），避免旧 `void*` 形参 + 具名 cast 局部
// 产生的两级槽往返（P0：ORIG 无槽往返 ⇒ 原码直接使用形参）。
void sub_CAISet(CAICharacterFull* self, int idx, const STAICharScriptView* script)
