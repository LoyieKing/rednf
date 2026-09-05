// AvatarCoin — DNF 币（ORIG 0x817feXX..0x8180XXX；
// 权威参照 docs/class_func_reports/AvatarCoin.md）
// 布局：+0x00 unsigned int m_coin（ORIG 类尺寸 4，嵌于 Inven 区 +0x658）
// 原始工程中 AvatarCoin 方法分布在两个源文件：本文件 +
// Arad_AvatarRoulette.cpp（Use/BuyProcess/GetPackage/SaveToDB/
// SendSyncPacket）。Use 与 IsUsable 跨 TU 调用（PLT）是 ORIG 帧形态
// （sub $0x18）的来源，勿合并到同一 TU。
// 另含 AvatarRoulette::isAvatarCoin（ORIG T 0x08911da0，mangled
// _ZN14AvatarRoulette12isAvatarCoinEm；权威类声明 DataManagerInitDepends.h）。

#include "CDataManager.h"           // DataManagerInitDepends.h 前置依赖
#include "DataManagerInitDepends.h"   // AvatarRoulette 权威布局

class AvatarCoin
{
public:
    AvatarCoin();
    void Set(unsigned int value);
    int Add(unsigned int value);
    unsigned int Use();
    unsigned int Get() const;
    bool IsUsable() const;

    unsigned int m_coin;  // +0x00
};

AvatarCoin::AvatarCoin() : m_coin(0) {}

void AvatarCoin::Set(unsigned int value)
{
    m_coin = value;
}

int AvatarCoin::Add(unsigned int value)
{
    m_coin += value;
    return 1;  // ORIG 恒返回 1
}

unsigned int AvatarCoin::Get() const
{
    return m_coin;
}

// ORIG 0x0817ff44（IsUsable.md）：mov (%eax),%eax / test / setne %al / ret ——
// 直返 m_coin != 0，无 movzbl 规范化（P9/P22：char 返回只需 %al）。
bool AvatarCoin::IsUsable() const
{
    return m_coin != 0;
}

// ============================================================================
// AvatarRoulette::isAvatarCoin —— ORIG T 0x08911da0（range 0x10）
// 汇编：mov 0x8(%ebp),%eax / mov (%eax),%eax（读 +0x00 m_avatarCoinInfo）/
//       cmp 0xc(%ebp),%eax / sete %al / pop / ret
// 即 bool 直返 m_avatarCoinInfo == avatarCoin（sete %al 后无 movzbl 规范化，
// bool 返回形态；Ghidra 反编译的 *(ulong*)this 为 8 字节读伪影，真实只读 4 字节）。
// ============================================================================
bool AvatarRoulette::isAvatarCoin(unsigned long avatarCoin)
{
    return m_avatarCoinInfo == avatarCoin;
}
