_Charac_info::~_Charac_info()
{
    // [L6 判据⑨重核 + L31CharacPad 成员化 2026-09-10] body 清空：7 个非平凡子对象
    // （map@0x1254 / map@0xecc / vector@0xea6 / vector@0xe9a / CCargo@0xdaa /
    // SkillSlot@0x892 / CInventory@0xda）现全为真实成员 ⇒ 编译器在 body 之后按
    // **逆声明序 = 偏移降序**隐式析构，与 ORIG 0x08697982 的 7 个 D1 call 序
    // （0x1254→0xecc→0xea6→0xe9a→0xdaa→0x892→0xda）逐点一致；旧两处显式
    // `~vector()` 属二次析构（double free），已删。
    // [G6Sub3 2026-09-10 复核判 N] 97/99、帧 0x10、7 个 D1 call 序全等；残差=EH cleanup
    // **区域划分**差：ORIG 把 (CCargo@0xdaa, SkillSlot@0x892) 归入**同一** cleanup 区
    // （0x08697a2f `call CCargo::D1` 之后**直落** 0x08697a3c `call SkillSlot::D1`，中间仅
    // `mov %esi,%eax; mov %ebx,%edx; mov %edx,%ebx; mov %eax,%esi` 重存异常寄存器）；
    // 4.4 拆成两个独立区块——前块尾多 `jmp`、后块另起 ⇒ +2 指令（= 全部 gap）。
    // P141（单次 delete[] 守卫）/P317（删用户空析构）**均不适用**：本 dtor 为编译器隐式
    // 生成（body 已清空、7 个非平凡子对象按逆声明序析构），无 delete[]、无用户体可删。
    // 属负例表「delete[]+成员 dtor EH cleanup / EH 双份 dtor epilogue vs merge」跨版本族 ⇒ 封顶。
