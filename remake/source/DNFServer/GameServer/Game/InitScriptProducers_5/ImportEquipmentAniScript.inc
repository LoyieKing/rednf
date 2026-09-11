// ---- 装备动画脚本域 ----
// STEquipmentAniScript 权威布局见 docs/class_func_reports/STEquipmentAniScript/；
// ImportEquipmentAniScript 完整解析器（ORIG T 0x89c05f8，0x16c1 字节）报告在
// docs/class_func_reports/_lt_global_gt_/ImportEquipmentAniScript.md，
// 定义 ScriptDepsA.cpp（本批已还原真实语义）。ORIG 尾 `mov %ebx,%eax; ret`
// 返回 0/1（P159 判据：非 void；mangled 不含返回类型，调用点忽略返回值）。
int ImportEquipmentAniScript(std::map<std::string, STEquipmentAniScript>* table); // _Z24..
