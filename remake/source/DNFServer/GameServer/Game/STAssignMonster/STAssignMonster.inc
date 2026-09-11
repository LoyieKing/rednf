// ORIG 0x836c5e0：默认构造 +0x28/+0x40/+0x5c 三个 vector（编译器按成员自动完成），
// 随后调用 Clear() 置初值。
STAssignMonster::STAssignMonster()
{
    Clear();
}