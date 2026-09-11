// ---- P327 GI 键名键控：本 TU 第一个函数定义 ----
// ORIG `_GLOBAL__I__ZN8WongWork11CDeathTower14CPacketHandler20makePickupItemHeaderEtt`
// @0x0846991d 的静态初始化体 0x084698dd 仅 `std::ios_base::Init __ioinit`（无用户
// 全局动态初始化对象）⇒ 键名回退到 TU 内第一个函数定义。ORIG 该函数即
// CPacketHandler::makePickupItemHeader（T 0x0845fc6c），故其定义必须位于本文件
// 所有函数定义之前（原定义在 WongWorkDeathTower_A.cpp，P327「定义放错 TU」迁回）。
// ORIG T 0x0845fc6c：header(0,0x27)；short(b)；short(a)；不 finalize。
void WongWork::CDeathTower::CPacketHandler::makePickupItemHeader(unsigned short a,
                                                                unsigned short b)
{
    put_header(0, 0x27);
    put_short((int)b);
    put_short((int)a);
}