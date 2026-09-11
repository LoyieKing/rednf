// [b28d-solo-CEventScriptMng] getEventScript：NEAR — N93 封顶。
// ORIG 0x08982f38：find 目标槽 -0x10 / end 目标槽 -0xc（4.1 先声明占深槽且调用序
// find→end）；4.4.7 按首次使用序分配，恒反向（find→-0xc / end→-0x10）。
// 实测三形：①本形（it 先声明）find 槽 -0xc；②交换声明序（endIt 先）调用序变
// end→find，指令序整体错位；③先声明后赋值引入 ORIG 不存在的
// _Rb_tree_iterator::C1Ev（nm 实证）⇒ 槽方向与调用序不可同时满足，封顶。
// p167 CLEAR（6/6），语义闭合。
// [L6S5 P326 推翻 N93] 旧判词前提是「end 具名（endIt）」——ORIG 实为 it 具名 +
//   end() 右值临时**内联进 operator!= 实参位**。去掉第二个具名迭代器后：
//   find 目标槽 -0x10 / end 临时槽 -0xc、调用序 find→end 与 ORIG 逐条全等
//   ⇒ 已 **IDENTICAL**（TU 读数 near 2→1、identical+ae 363→364）。
InGameEventScript* InGameEventScriptLoader::getEventScript(int id,
