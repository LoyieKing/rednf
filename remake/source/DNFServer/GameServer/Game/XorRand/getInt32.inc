// getInt32 置于 TU 首个成员函数定义前：4.4 对未定义 callee 保守 outgoing
// 恰为 ORIG 帧 0x18（定义于 getUInt32 后会精确继承 callee 帧 0x10+4=0x14，
// 与 ORIG 槽差 4；XorRand8 大帧函数定义在前时保守值升至 0x28）。
int XorRand4::getInt32()
{
    return (int)getUInt32();
}