// ================= CBattle_Field::GetEnableHellDungeon() — ORIG 0822cf26 =================
// 定义置于本 TU（与调用者分离）：4.4 unit-at-a-time 对 TU 内可见 callee 省略
// outgoing 帧（sub $0x10/槽-0x4），ORIG 4.1 恒发 sub $0x18/槽-0xc（探针 p6/p7）。
bool* CBattle_Field::GetEnableHellDungeon()
{
    // mov $0x93701ec,%eax; ret（POD 无 guard 的函数局部
    // static，V 符号 _ZZN13CBattle_Field20GetEnableHellDungeonEvE19enable_hell_dungeon）。
    // [R21scanEC §27.1 静态初值族] ORIG .data/PROGBITS 0x93701ec = `01`（raw 读，
    // 文件偏移 0x13281ec）⇒ 原码初值 **true**；旧无初值形落 .bss（false）⇒
    // 未 Set 时 IsEnableHellDungeon 默认值与 ORIG 分叉。
    static bool enable_hell_dungeon = true;   // ORIG V 0x93701ec = 0x01
    return &enable_hell_dungeon;
}