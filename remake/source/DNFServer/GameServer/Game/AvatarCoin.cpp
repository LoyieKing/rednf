// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 AvatarCoin/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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

#include "AvatarCoin/AvatarCoin.inc"

#include "AvatarCoin/Set.inc"
{
    m_coin = value;
}

#include "AvatarCoin/Add.inc"
{
    m_coin += value;
    return 1;  // ORIG 恒返回 1
}

#include "AvatarCoin/Get.inc"
{
    return m_coin;
}

// ORIG 0x0817ff44（IsUsable.md）：mov (%eax),%eax / test / setne %al / ret ——
// 直返 m_coin != 0，无 movzbl 规范化（P9/P22：char 返回只需 %al）。
#include "AvatarCoin/IsUsable.inc"
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
#include "AvatarCoin/isAvatarCoin.inc"
{
    return m_avatarCoinInfo == avatarCoin;
}
