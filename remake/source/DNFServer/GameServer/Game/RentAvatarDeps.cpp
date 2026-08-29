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
int getAvatarRentPrice(int price, int rentType)
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
void RentAvatarErrorProc(CUser* user, int errCode)
{
    PacketGuard guard;
    guard.clear();
    guard.put_header(0x1, 0x16);
    guard.put_byte(0);
    guard.put_byte(errCode);
    guard.finalize(true);
    user->Send(guard);
}
