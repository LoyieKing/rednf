// ORIG 0x88d1450：无参，直接返回 true（10 字节：push/mov/xor 后 mov $1）。
// 定义位置按 ORIG 布局置于 ImportAttackInfoScript（ORIG 0x88d145a）之前。
bool InitAttackInfoScript()
{
    return true;
}