_Charac_info::~_Charac_info()
{
    // [L6 判据⑨重核 + L31CharacPad 成员化 2026-09-10] body 清空：7 个非平凡子对象
    // （map@0x1254 / map@0xecc / vector@0xea6 / vector@0xe9a / CCargo@0xdaa /
    // SkillSlot@0x892 / CInventory@0xda）现全为真实成员 ⇒ 编译器在 body 之后按
    // **逆声明序 = 偏移降序**隐式析构，与 ORIG 0x08697982 的 7 个 D1 call 序
    // （0x1254→0xecc→0xea6→0xe9a→0xdaa→0x892→0xda）逐点一致；旧两处显式
    // `~vector()` 属二次析构（double free），已删。
