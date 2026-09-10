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
