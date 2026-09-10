ScriptStringManager::~ScriptStringManager() {}

// b28d-solo-ScriptStringManager 判词：DIFF 封顶 **N82**（238 vs 242 条；
// p167: 3 MISSING 右值模板实例 + 3 EXTRA）。已按 Ghidra C 逐句复现：双独立守卫、
// offset 循环尾读 + 哨兵 goto、memcpy 后查越界（P171）、删 len<0 护栏。
// 残差 = 槽整体深 4-8B（ORIG push_back 内联临时槽 -0x2c/-0x28，4.4 无）＋
// %esi/%ebx 角色互换；MISSING 为 C++0x 右值 `_M_insert_aux`（同 insertString）。
