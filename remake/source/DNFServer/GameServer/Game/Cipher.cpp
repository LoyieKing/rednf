// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Cipher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Cipher（G7 分组算法容器）还原（2026-08-16）。
// 依据 docs/class_func_reports/Cipher.md、ICryptoGraph.md、_lt_global_gt_.md：
//   - ctor 建 14 个 ICryptoGraph 并累加 GetKeySize 得到总钥长；
//   - Initialize 把总钥顺序分发给各图（不足则失败）；
//   - Encrypt/Decrypt 按 key % 14 选图，长度先向上对齐到块大小；
//   - 成功返回 1（0x6fffffff 由各图返回）。

#include "Cipher.h"
#include "CEnvironment.h"  // GlobalInstance<T>（G_Cipher→inst_ptr 单例模板）

#include <cstdlib>
#include <cstring>
#include <iostream>  // P338：ORIG 本 TU 含 std::ios_base::Init __ioinit
                     //（_GLOBAL__I_ 体 sub $0x18，非 0x8）

#include "CNCrypto.h"

// ============================================================================
// 工厂 / 全局
// ============================================================================

#include "Cipher/GetCryptoType.inc"


// ============================================================================
// ICryptoGraph
// ============================================================================

#include "Cipher/ICryptoGraph.inc"


#include "Cipher/ICryptoGraph_1.inc"


#include "Cipher/dtor_ICryptoGraph.inc"


ICryptoGraph& ICryptoGraph::operator=(const ICryptoGraph& other)
{
    if (this != &other)
        m_apiType = other.m_apiType;
    return *this;
}

#include "Cipher/GetApiType.inc"


// ============================================================================
// Cipher
// ============================================================================

#include "Cipher/Cipher.inc"


#include "Cipher/dtor_Cipher.inc"


#include "Cipher/Cipher_1.inc"


Cipher& Cipher::operator=(const Cipher& other)
{
    return *this;
}

#include "Cipher/GetTotalKeyLength.inc"


#include "Cipher/Initialize.inc"


#include "Cipher/GetOriginalKey.inc"


#include "Cipher/AdjustDataSize.inc"


#include "Cipher/Encrypt.inc"


#include "Cipher/Decrypt.inc"


ICryptoGraph* CreateCryptoGraph(CRYPTO_GRAPH_TYPE type)
{
    switch (type)
    {
    case CRYPTO_GRAPH_SHIFT:     return new CNCryptoShift;
    case CRYPTO_GRAPH_RIJNDAEL:  return new CNCryptoRijndael;
    case CRYPTO_GRAPH_BLOWFISH:  return new CNCryptoBlowFish;
    case CRYPTO_GRAPH_RC6:       return new CNCryptoRc6;
    case CRYPTO_GRAPH_TWOFISH:   return new CNCryptoTwoFish;
    case CRYPTO_GRAPH_TEA:       return new CNCryptoTea;
    case CRYPTO_GRAPH_KASUMI:    return new CNCryptoKasumi;
    case CRYPTO_GRAPH_XTEA:      return new CNCryptoXTea;
    case CRYPTO_GRAPH_NOEKEON:   return new CNCryptoNoekeon;
    case CRYPTO_GRAPH_KHAZAD:    return new CNCryptoKhazad;
    case CRYPTO_GRAPH_CAST5:     return new CNCryptoCast5;
    case CRYPTO_GRAPH_SKIPJACK:  return new CNCryptoSkipjack;
    case CRYPTO_GRAPH_MULTI2:    return new CNCryptoMulti2;
    case CRYPTO_GRAPH_ANUBIS:    return new CNCryptoAnubis;
    default:                     return 0;
    }
}

#include "Cipher/DestroyCryptoGraph.inc"


// [判据㉓ 符号绑定] ORIG `_Z8G_Cipherv` 实测为 W/0x0d（头内 inline、由使用点
// TU 发射的弱符号）；重建在 .cpp 单点 weak 定义产出同绑定（Main 2026-09-10 裁决：
// 不做全仓头内 inline 统一改，各 TU 属主对自身 G_* 定义套用同一修法）。
__attribute__((weak)) Cipher* G_Cipher()
{
    // ORIG 0x82a713a（6 insns）：尾调 GlobalInstance<Cipher>::inst_ptr()（0x82ab9da），
    // 非 Meyers static。返回 GlobalInstance<T>::inst_ptr()→create()->m_p。
    return GlobalInstance<Cipher>::inst_ptr();
}

// ============================================================================
// 随机串 / 随机钥（ORIG @0x809e473 / @0x809e58c / @0x809e6b2）
// ============================================================================

#include "Cipher/GenerateRandomString.inc"


#include "Cipher/GenerateRandomString_1.inc"


#include "Cipher/GenerateRandomKey.inc"


// ============================================================================
// 最近错误码（ORIG @0x0809e45c / @0x0809e469；全局 .data @0x0936e4b0）
// ============================================================================

// ORIG .data @0x0936e4b0 初值实测 0x6fffffff（文件字节 ff ff ff 6f）⇒ 非零初值、
// 符号类型 D、尺寸 4、外部链接。ORIG 全库仅本文件两函数引用该地址（2 处）。
int gNCryptoLastError = 0x6fffffff;

#include "Cipher/NCryptoSetLastError.inc"


unsigned int NCryptoGetLastError()
{
    return gNCryptoLastError;
}
