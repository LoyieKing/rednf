// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 PIHash/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// U3 PI 反作弊 hash 链 TU（U3ShopDb 批次，2026-08-23）。
// 逐符号对照 docs/class_func_reports（_lt_global_gt_/PISenderManager/CUser）与
// objdump 反汇编：
//   DoSuperFastHash(char*, uint, uint&)                     0x0808ca7a
//   MakeHash(uchar*, uint, uchar)                           0x0808c311 (W)
//   PISenderManager::MakeHashForSending(uchar*, uint, bool) 0x0808be6e
//   _NS_PI_2ND_GetPacketIndex(char*)                        0x082772a9 (W)
//   _NS_PI_2ND_ReconstructCheckSumRandomkey(uint&, uint)    0x0808ca60
//   _NS_PI_2ND_SendHash_SC(char*, ushort, CUser*)           0x082751ee
//   SetUserManagerToolIP(CUser*, SIG_UPDATE_LOGOUT*)        0x08655327
//
// [SweepPIHash 2026-09-09] 7 件 DIFF 处置：MakeHashForSending DIFF→NEAR（79/79；
// 修真语义 bug：合成式末项 key>>6 → rnd>>6，另补 nHash/ret/h 具名局部与
// m_pad uchar 左值视图）；SendHash_SC 51/50、DoSuperFastHash ×2 139/138、
// _NS_PI_2ND_MakeHash 40/39、SetUserManagerToolIP 114/115 余量均为 N 类
// 跨版本形态/工具链差异（逐函数头判词）；_Z41__static_initialization_and_
// destruction_0ii 按 N123 同名撞名豁免。

#include "CUser.h"
#include "SigTypes.h"
#include <cstring>
#include <cstdlib>
#include <map>

// ---- ORIG 全局：hashkey_map（B 0x093f5810 size 0x18，CU=PacketIntegrityManager.cpp）----
// 类型由 ORIG 自身 static_init 实证：`movl $0x93f5810,(%esp); call
// _ZNSt3mapIjhSt4lessIjESaISt4pairIKjhEEEC1Ev`（0x0808c0ab）⇒
// std::map<unsigned int, unsigned char>（sizeof 0x18，4.4 实测一致）。
// ORIG 全二进制除同 TU 的 __static_initialization_and_destruction_0
// （0x0808c0a7 / 0x0808c0c1）外零业务读写引用 ⇒ 补定义对齐符号账（REAL_GAP_SYMBOL）。
// 置于本 TU 首个强符号位：GI 键名对齐 ORIG `_GLOBAL__I_hashkey_map`（0x0808c0cf）。
std::map<unsigned int, unsigned char> hashkey_map;

// [SweepPIHash] PISenderManager::m_pad（+0x04，char[4]）的无符号字节左值视图。
// 依据：`char = unsigned char` 属符号性异型 ⇒ 4.4 store 走 RHS-first
// （`movzbl -0x15,%edx; mov 0x8(%ebp),%eax; mov %dl,4(%eax)`）；
// 经本视图（uchar 左值 = uchar 右值，同型）复现 ORIG LHS-first
// （`mov 0x8(%ebp),%eax; movzbl -0x15,%edx; mov %dl,0x4(%eax)`，P211 档③）。
// 布局：vptr(+0x00) + m_pad[4](+0x04)，sizeof/偏移/mangled 零影响（仅 .cpp 局部类型）。
struct PISenderPadView
{
    void* m_vptr;              // +0x00（PISenderManager 虚析构 vptr）
    unsigned char m_pad[4];    // +0x04
};

#include "PIHash/DoSuperFastHash.inc"
#include "PIHash/DoSuperFastHash_1.inc"


// ORIG T 0x0808c9a0（P210：定义须前置于 DoSuperFastHash 定义，保 0x38 保守大帧）：
// DoSuperFastHash(data,len,&h) → 4 字节异或 ^ seed ^ 0x53
// → (v & 0xf) ^ ((v & 0xf0) >> 4)（返回低半字节折叠，ORIG 为 `shrb $4,%al`）。
// [b28c-L23 N(N18a 反向)·已翻 3 中 2] P210 定义前置把帧/槽号整体翻正
// （0x38 + -0x1c/-0x14/-0x10/-0x9 与 ORIG 全等）；`((v & 0xf0) >> 4)` 复现 ORIG 的
// `shrb $4,%al`（4.1 字节移位，4.4 对 `v >> 4` 恒出 32 位 sar）。
// 唯一残差 = 末次 `acc ^ seed`：4.1 发 `movzbl seed,%eax; xor %edx,%eax`（就地寄存器
// 物化），4.4 恒折 `xor -0x14(%ebp),%al`（8 位内存 xor）——6 形探针
// （(int)seed / (unsigned int) 双侧 cast / 分组括号 / seed 置左 / 先折 0x53）全产同
// 一 xorb ⇒ N18a 反向封顶；语义（值域 0..0xff 单字节折叠）完全一致。
// [b28d-L1] N18b 封顶：ORIG 0x0808c9a0 末次 `movzbl -0x1c(%ebp),%eax; xor %edx,%eax`（seed 装载后 xor）；4.4 恒折 `xor -0x1c(%ebp),%al`（8 位内存 xor）。探针 m1..m9 六形全折内存 xor（m9 具名 uchar 局部可产装载形但引入 ORIG 无的槽）。P167 CLEAR（1/1）语义等价。
unsigned char _NS_PI_2ND_MakeHash(char* data, unsigned int len,
                                  unsigned char seed)
{
    // ORIG h 槽无预清（DoSuperFastHash 覆写）。
    unsigned int h;
    DoSuperFastHash(data, len, h);
    unsigned char* p = reinterpret_cast<unsigned char*>(&h);
    // [P1T5 修法 x2 · 保留] ORIG（0x0808c9a0+）尾部：27 `xor %eax,%edx`（p[3] 异或
    // 进累积 edx）→ 28 `movzbl -0x1c(%ebp),%eax` → 29 `xor %edx,%eax` → 30 `xor $0x53,%eax`。
    // 基线左结合式 4.4 在 27 出 `xor %edx,%eax` + 28 `xor -0x1c(%ebp),%al`（8 位内存 xor）
    // ⇒ 2 差异块。改 `p[3] ^ seed` 先结合后 4.4 在 27 换向、且仍折 8 位 seed xor ⇒
    // 差异块 2→1（ours 39 / ORIG 40 不变）。负结论：右结合全括号 x1=42（更差）；
    // `(seed ^ 0x53)` 成组 x3 与基线同（2 块）；`static_cast<unsigned int>`/`(unsigned short)`
    // /具名 seed/`&0xff` 等六形探针（b28d-L1 m1..m9 + 本波 andprobe.cpp）均折 8 位内存 xor
    // ⇒ 残余 1 条为 4.4 结果窄化优化（ORIG 4.1 无），封顶。
    unsigned char v = static_cast<unsigned char>(
        p[0] ^ p[1] ^ p[2] ^ (p[3] ^ seed) ^ 0x53);
    return static_cast<unsigned char>((v & 0xf) ^ ((v & 0xf0) >> 4));
}

#include "PIHash/DoSuperFastHash_2.inc"


#include "PIHash/DoSuperFastHash_3.inc"


#include "PIHash/_NS_PI_2ND_GetPacketIndex.inc"


#include "PIHash/MakeHashForSending.inc"


#include "PIHash/_NS_PI_2ND_ReconstructCheckSumRandomkey.inc"


#include "PIHash/_NS_PI_2ND_SendHash_SC.inc"


#include "PIHash/SetUserManagerToolIP.inc"

// ============================================================================
// OnlyOrigBatch11 落地：_NS_PI_2ND_MakeHash / GetRandomHashKey /
// ReconstructCheckSum（报告 _lt_global_gt_ 三份，objdump 逐指令转写）
// ============================================================================

// ORIG T 0x0808ca1c：return rand() & 0xf。
unsigned char _NS_PI_2ND_GetRandomHashKey()
{
    return static_cast<unsigned char>(rand() & 0xf);
}

#include "PIHash/_NS_PI_2ND_ReconstructCheckSum.inc"

