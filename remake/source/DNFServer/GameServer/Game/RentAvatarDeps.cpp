// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 RentAvatarDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// RentAvatarDeps.cpp — avatar 租借域自由函数（LinkE2 批次）
// 需登记 CMake：source/cmake/game/CMakeLists.txt
#include "PacketGuard.h"   // PacketGuard（InterfacePacketBuf 子类）
#include "CUser.h"         // CUser::Send(PacketGuard&)

// ORIG T _Z18getAvatarRentPriceii @ 0x0898c999 (size 0x39)
// int getAvatarRentPrice(int, int) — 逐指令转写：
//   arg2(type) == 0 或 1 → price
//   == 2                → price * 2
//   == 3                → price * 4（shl 2）
//   其它                → price
#include "RentAvatarDeps/getAvatarRentPrice.inc"
{
    if (rentType == 0)
        return price;
    if (rentType == 1)
        return price;
    if (rentType == 2)
        return price * 2;
    if (rentType == 3)
        return price * 4;
    return price;
}

// ORIG W _Z18RentAvatarErrorProcP5CUseri @ 0x082352c5 (size 0xb2)
// void RentAvatarErrorProc(CUser*, int) — 逐指令转写：
//   PacketGuard 构造 → clear() → put_header(0x1, 0x16)
//   → put_byte(0) → put_byte(errCode) → finalize(true) → user->Send(guard)
// ORIG 无返回值（ebp 恢复前不写 eax），签名 void 与调用点语句级用法一致。
#include "RentAvatarDeps/RentAvatarErrorProc.inc"
{
    PacketGuard guard;
    guard.clear();
    guard.put_header(0x1, 0x16);
    guard.put_byte(0);
    guard.put_byte(errCode);
    guard.finalize(true);
    user->Send(guard);
}
