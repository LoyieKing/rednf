// ---- _Charac_info ctor/dtor（LinkE2）----
// 收敛口径（2026-09-05 EmoC11，Main 裁决记封顶不批成员化）：ORIG W 0x08695cf8
// o=150 形 = 18 子对象按偏移升序隐式成员构造（每处 4 条 mov/add/mov/call T::C1）
// + 尾 call Reset；本重建 CUser.h（冻结）保留 pad 区、此处 placement new 补构，
// 4.4.7 对每处 placement operator new 发射 _ZnwjPv 实调用 + size 装载 + 判空 +
// _ZdlPvS_ EH 清理岛 ≈ +10 条 ⇒ n=304（we-bigger 154）。范式库 N83（SIG_LOAD_ETC
// ctor 49 处同族已豁免）×16；+0xecc/+0x1254 两真成员先构亦属声明序隐式形约束。
// 语义完成（18 ctor 与 Reset 全部真实对位 ORIG 符号），byte 层 N83 封顶，勿再耗轮次。
// [b28d-22 N82] ORIG ?：p167=M0/E1:_ZNSt3mapIi18STSkillCommandDataSt4lessIiESaISt4pairIKiS0_EEED1Ev，gap=-36；语义完成、byte 层帧槽/物化族封顶。
// [b28d-solo-CUserOnlyOrig DIFF] ORIG 0x08695cf8：N83 封顶——p167 EXTRA `_ZnwjPv`×16 +
// `_ZdlPvS_`×9（4.1 placement new 前端内联 0 call，4.4 恒 extern call+EH cleanup）。
_Charac_info::_Charac_info()
{
    // [L6 判据⑨重核 + L31CharacPad 成员化 2026-09-10] 13 处 placement new 全部删除：
    // CUser.h 已把 +0x29/+0x2a/+0x88/+0xe40/+0xe8d/+0xe9a/+0xea6/+0xeed/+0x1086/
    // +0x129d/+0x12a5/+0x12b7/+0x14ca 建模为真实子对象成员（offsetof 断言锁定），
    // +0xda/+0x892/+0xdaa/+0xecc/+0x1254 早已成员化 ⇒ 编译器在 body 前按**声明序
    // =偏移升序**隐式构造，正是 ORIG 0x08695cf8 的 18 个 call 序列。N83
    // （4.4 placement operator new 恒 extern `_ZnwjPv` call + size 装载 + EH cleanup
    // ≈+10 条/处）随之消除；旧形同时构成二次构造（真 bug）。
    Reset();                                              // call 0x86822b8
}