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

// [b28d-L1] 修法：同 char* 版（ORIG W 0x0808c174），结构重写后残余为寄存器/槽位形态差（N7/N82）。P167 CLEAR（0/0）语义等价。
#include "PIHash/DoSuperFastHash.inc"
// [b28d-L1] 修法（结构重写）：按 ORIG 0x0808ca7a 还原「形参 len 作循环变量 + 单一 rem 累加 + 分步就地 op= + 逐字节 movzbl 组装 + 加法 lea」后 ndiff 147→70。残余 = callee-saved 寄存器分配（data[1] 落 %edx vs %eax）与 tmp 槽位（ORIG -0x4 vs -0x8），N7/N82 封顶。P167 CLEAR（0/0）语义等价。
#include "PIHash/DoSuperFastHash_1.inc"

// ---- MakeHash 前置（P210 callee 定义顺序定律：调用者定义须在 DoSuperFastHash
// 定义之前，否则 4.4 按 callee 帧精确继承 outgoing，0x38 保守大帧塌缩为 0x20）----
unsigned char MakeHash(unsigned char* data, unsigned int len, unsigned char seed)
{
    unsigned int hash;   // ORIG 无 init store（DoSuperFastHash 出参槽直传）
    DoSuperFastHash(data, len, hash);
    unsigned char* digest = reinterpret_cast<unsigned char*>(&hash);
    return static_cast<unsigned char>(digest[0] ^ digest[1] ^ digest[2] ^
                                      digest[3] ^ seed);
}

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

// ---- DoSuperFastHash ORIG @ 0x0808ca7a ----
// [b28d-L1] 修法：ORIG 用**形参 len 本身**作循环变量（`shrl $0x2,0xc(%ebp)` 就地
// 改形参槽）、累加只有 rem 一个局部（无独立 h/i），循环体与尾分支均为分步
// `rem op= …` 就地写（`add %eax,-0xc(%ebp)` / `xor %eax,-0xc(%ebp)` 实证），
// 且字节按 `movzbl (%eax); movzbl %al,%eax` 逐字节组装（P241 窄化形，非 movzwl）。
// 原单表达式形使 4.4 多 3 个局部槽（-0x10/-0xc/-0x8/-0x4）、循环尾 setne 形、
// 字节读 movzwl，ndiff 121（r48）。按 ORIG 分步形重写后逐条对齐。
// [SweepPIHash] 续修（139/138，gap 1）：
//   ① 中段改 `(data[3]<<8) + data[2]`（ORIG `lea (%edx,%eax,1)`）；旧写 `|` 产 `or`；
//   ② t 声明前置于 tmp（ORIG t@-0x8 / tmp@-0x4；4.4 函数头标量按声明序先深）；
//   ③ 尾改 `rem += rem>>6; hash = rem;` 两步（ORIG `add %eax,-0xc(%ebp)` 就地写；
//      单表达式 `hash = rem + (rem>>6)` 4.4 恒产 edx 中转）。
//   残余 N：尾部 `if(tmp==2/3/1)` 链 4.4 恒 `cmpl $K,mem; jne`×3（6 条）vs ORIG
//   `mov tmp,%eax; cmp $2,%eax; je; cmp $3,%eax; je; cmp $1,%eax; jne`（4.1 装载一次，
//   7 条）；switch 形 4.4 产 mov+cmp/je×3+jmp（8 条，多 1 条 default jmp）⇒ 两形均不闭合。
//   [P1T5 probe sw1] 已实测真 switch：ours 140（ORIG 139）、444/446 后端同 140 ⇒ 非后端差异，
//   是 4.4 decision-tree 布局不做末 case fall-through（末位恒 `je`+`jmp default`，ORIG 用 `jne`），
//   源码不可翻，回滚。
#include "PIHash/DoSuperFastHash_2.inc"
{
    hash = 0;
    unsigned int rem = len;
    if (len == 0 || data == 0)
        return 0;

    // [SweepPIHash] ORIG t 槽 -0x8 深于 tmp 槽 -0x4 ⇒ t 须声明在 tmp 之前
    // （4.4 函数头标量按声明序「先声明深」，与 ORIG 一致）。
    unsigned int t;
    unsigned int tmp = len & 3;
    for (len >>= 2; len != 0; --len)
    {
        rem += ((unsigned int)(unsigned char)data[1] << 8) +
               (unsigned int)(unsigned char)data[0];
        // ORIG 用 `(data[3]<<8) + data[2]`（lea 合并），非 `|`（or 形）。
        t = (((unsigned int)(unsigned char)data[3] << 8) +
             (unsigned int)(unsigned char)data[2]) << 11 ^ rem;
        rem = (rem << 16) ^ t;
        data += 4;
        rem += rem >> 11;
    }
    // [P1T5 probe sw1 · 回滚] switch 形实测 ours=140（ORIG 139，更差 1 条）：
    // 4.4 产 `mov tmp,%eax; cmp $2/je; cmp $3/je; cmp $1/je; jmp Lend`（8 条），
    // 比 ORIG 的 7 条（末位改 jne 的 fall-through 布局）多一条 default jmp；
    // 444/446 后端同形（各 140）⇒ 4.4 系布局不做末 case fall-through，源码无解。
    if (tmp == 2)
    {
        rem += ((unsigned int)(unsigned char)data[1] << 8) +
               (unsigned int)(unsigned char)data[0];
        rem ^= rem << 11;
        rem += rem >> 17;
    }
    else if (tmp == 3)
    {
        rem += ((unsigned int)(unsigned char)data[1] << 8) +
               (unsigned int)(unsigned char)data[0];
        rem ^= rem << 16;
        rem ^= (unsigned int)(int)(signed char)data[2] << 18;
        rem += rem >> 11;
    }
    else if (tmp == 1)
    {
        // ORIG movsx（signed char）——与 tmp==3 的 data[2] 一致。
        rem += (unsigned int)(int)(signed char)data[0];
        rem ^= rem << 10;
        rem += rem >> 1;
    }
    rem ^= rem << 3;
    rem += rem >> 5;
    rem ^= rem << 4;
    rem += rem >> 17;
    rem ^= rem << 25;
    // ORIG 分两步（`add %eax,-0xc(%ebp)` 就地更新 rem 再读入 hash），
    // 单表达式 `hash = rem + (rem>>6)` 4.4 恒产 edx 中转形。
    rem += rem >> 6;
    hash = rem;
    return 1;
}

// ORIG W 0x0808c174：unsigned char* 重载（ORIG 中与 char* 版同为独立展开）。
#include "PIHash/DoSuperFastHash_3.inc"
{
    // [b28d-L1] 同 char* 版修法（ORIG W 0x0808c174）：len 作循环变量、单一 rem
    // 累加、分步就地 `rem op=`、逐字节 movzbl 组装。
    // [SweepPIHash] 续修同 char* 版（139/138）：`(data[3]<<8)+data[2]`、t 前置声明、
    // 尾部 `rem += rem>>6; hash = rem;` 分两步；残余为尾部 if 链内存直比（N）。
    hash = 0;
    unsigned int rem = len;
    if (len == 0 || data == 0)
        return 0;

    // [SweepPIHash] 同 char* 版：t 声明前置于 tmp（ORIG t@-0x8 / tmp@-0x4）。
    unsigned int t;
    unsigned int tmp = len & 3;
    for (len >>= 2; len != 0; --len)
    {
        rem += ((unsigned int)data[1] << 8) + (unsigned int)data[0];
        // ORIG `(data[3]<<8) + data[2]`（lea），非 `|`。
        t = (((unsigned int)data[3] << 8) + (unsigned int)data[2]) << 11 ^ rem;
        rem = (rem << 16) ^ t;
        data += 4;
        rem += rem >> 11;
    }
    if (tmp == 2)
    {
        rem += ((unsigned int)data[1] << 8) + (unsigned int)data[0];
        rem ^= rem << 11;
        rem += rem >> 17;
    }
    else if (tmp == 3)
    {
        rem += ((unsigned int)data[1] << 8) + (unsigned int)data[0];
        rem ^= rem << 16;
        rem ^= (unsigned int)(int)(signed char)data[2] << 18;
        rem += rem >> 11;
    }
    else if (tmp == 1)
    {
        rem += (unsigned int)(int)(signed char)data[0];
        rem ^= rem << 10;
        rem += rem >> 1;
    }
    rem ^= rem << 3;
    rem += rem >> 5;
    rem ^= rem << 4;
    rem += rem >> 17;
    rem ^= rem << 25;
    // 同 char* 版：ORIG 分两步就地更新 rem 再读入 hash。
    rem += rem >> 6;
    hash = rem;
    return 1;
}

// ---- _NS_PI_2ND_GetPacketIndex ORIG @ 0x082772a9 ----
#include "PIHash/_NS_PI_2ND_GetPacketIndex.inc"
{
    return *reinterpret_cast<short*>(packet + 1);
}

// this+0x4 为当前 key 字节（ORIG asm：movb $0x27,0x4(%eax) / movzbl 0x4(%eax)）；
// bNewKey 时先写固定种子 0x27，再以 rand() 刷新。
// [SweepPIHash] 修法（ORIG 0x0808be6e 79 条逐条对齐，79/79）：
//   ① 原码 rnd 是 uchar 字节槽（ORIG -0x15，`mov %al,-0x15`），非 uint 具名局部；
//   ② 原码 nHash(-0x14)/ret(-0x10)/h(-0x9) 三个具名局部声明即初始化 0
//      （ORIG 三条 `movl $0x0,-0x14`/`movl $0x0,-0x10`/`movb $0x0,-0x9` 实证）；
//   ③ 合成式书写序 = ORIG 计算序：h 项在前、rnd 项在后，末项 `(rnd & 0xc0) >> 6`
//      复现 ORIG `shr $0x6,%al`（`(rnd>>6)` 4.4 恒产 sar）；
//   ④ `ret = (nHash << 8) | (rand() & 0xff)`（左操作数先算）复现 ORIG
//      `mov nHash,%ebx; shl $8,%ebx; call rand; and $0xff,%eax; or %ebx,%eax`
//      的 callee-saved 跨 call 保活（右操作数先写则 rand 先 call，无 %ebx）；
//   ⑤ 原码第 8 项是 rnd>>6（非 key>>6）——旧源码误写 key>>6（真语义 bug，已修）；
//   ⑥ m_pad 为 char[4]，`char = uchar` 符号性异型 ⇒ 4.4 RHS-first；
//      三处改 uchar 左值视图 `*reinterpret_cast<unsigned char*>(m_pad)` 复现
//      ORIG LHS-first（P211 档③）。
// 残余仅 `and $0xc0,%ax`(ORIG 16 位) vs `and $0xc0,%eax`(4.4 32 位) 1 行 ⇒ NEAR。
#include "PIHash/MakeHashForSending.inc"
                                                 unsigned int len,
                                                 bool bNewKey)
{
    if (bNewKey)
        reinterpret_cast<PISenderPadView*>(this)->m_pad[0] = 0x27;
    unsigned char key =
        reinterpret_cast<PISenderPadView*>(this)->m_pad[0];
    unsigned char rnd = static_cast<unsigned char>(rand());
    reinterpret_cast<PISenderPadView*>(this)->m_pad[0] = rnd;

    unsigned int nHash = 0;
    unsigned int ret = 0;
    unsigned char h = 0;
    if (len == 0)
        h = static_cast<unsigned char>(rand());
    else
        h = MakeHash(data, len, key);

    nHash = static_cast<unsigned short>(
        ((h & 3) << 6) | ((h & 0xc) << 12) | ((h & 0x30) >> 2) |
        ((h & 0xc0) << 4) | ((rnd & 3) << 12) | ((rnd & 0xc) << 2) |
        ((rnd & 0x30) << 4) | ((rnd & 0xc0) >> 6));
    ret = (nHash << 8) | (rand() & 0xff);
    return ret;
}

// ---- _NS_PI_2ND_ReconstructCheckSumRandomkey ORIG @ 0x0808ca60 ----
#include "PIHash/_NS_PI_2ND_ReconstructCheckSumRandomkey.inc"
                                             unsigned int randomKey)
{
    checksum = (checksum & 0xff) | (randomKey << 8);
}

// ---- _NS_PI_2ND_SendHash_SC ORIG @ 0x082751ee ----
// [SweepPIHash] 修法（ORIG 0x082751ee 51 条逐条对齐）：
//   ① ORIG 具名局部按槽深序：bodyLen(-0x20,int)、body(-0x1c,uchar*)、
//      first(-0x15,char)、index(-0x14,short)、bNonClient(-0x11,bool)、
//      hash(-0x10,uint)、pCksum(-0xc,uint*)——旧版把 body/bodyLen/pCksum
//      内联进调用（少 3 个槽、帧 0x38 vs 0x44）；
//   ② bNonClient 用**单表达式 `bool b = (a && b)`** 物化（[P1T5 已翻 AE]）：
//      ORIG 0x082751fd 起 `mov $1,%eax; jmp; mov $0,%eax; mov %al,-0x11(%ebp)`
//      = 单一 store 点的 store_flag 形；旧 if/else 形 4.4 产 `movb $1,-0x11`×2
//      （探针 t1/t2/t3 即该形，故误判封顶）；改单表达式后 4.4 逐条同形 ⇒ 51/51 AE。
//   ③ receiver 走链式 `user->GetPISenderManager()->MakeHashForSending(...)`
//      （ORIG 无 sender 具名局部，GetPISenderManager 结果直接作 this）。
// ⇒ 本件已无残余（n1_v AE 51/51）。
#include "PIHash/_NS_PI_2ND_SendHash_SC.inc"
{
    unsigned int bodyLen = len - 0xf;
    unsigned char* body = reinterpret_cast<unsigned char*>(data + 0xf);
    char first = data[0];
    short index = _NS_PI_2ND_GetPacketIndex(data);
    // [P1T5 probe s1] 试单表达式形（ORIG 24-26 = mov $1,%eax / jmp / mov $0,%eax
    // + 单一 store 点 ⇒ 疑为 4.1 对 `bool b = cond;` 的 store_flag 形）。
    bool bNonClient = (first == '\0' && index == 1);

    unsigned int hash =
        reinterpret_cast<PISenderManager*>(user->GetPISenderManager())
            ->MakeHashForSending(body, bodyLen, bNonClient);
    unsigned int* pCksum = reinterpret_cast<unsigned int*>(data + 0xb);
    _NS_PI_2ND_ReconstructCheckSumRandomkey(*pCksum, hash);
}

// ---- SetUserManagerToolIP ORIG @ 0x08655327 ----
// SIG_UPDATE_LOGOUT 布局（本函数实证）：+0x22 premium 管理工具标志(byte)、
// +0x23 ip 相关 dword、+0x27 ip[0x10]、+0x37 前段、+0x43 后段（'.' 拆分）。
// [b28d-L1] N103 封顶（工具链头/库差异）：ORIG 0x08655327 调 `_ZSt7strrchrPci`（libstdc++ C++ 重载，需 __CORRECT_ISO_CPP_STRING_H_PROTO）；本工具链 cstring 把 std::strrchr 映射为 ::strrchr（P167 MISSING×1/EXTRA×1）。源码不可控，语义等价。
// [SweepPIHash] 尾部 4 个 nop+jmp 岛已按 P223 多岛形（4 条独立 goto + 尾标签串）
// 复现（旧 4 条早返只产 3 岛）。
// [P1T5 修法 u1 · 已落] ORIG 0x08655375/0x0865538e（前缀/后缀长度比较）是
// `cmpl $0xc,-0x10(%ebp)`/`cmpl $0x4,-0xc(%ebp)` + `ja`（mem 直比），旧码把
// prefixLen/suffixLen 声明为 int 且每处 `static_cast<unsigned int>` ⇒ 4.4 产
// `mov -0x10(%ebp),%eax; cmp $0xc,%eax`（多 1 条载入 ×2，且整体错位）。改
// 局部本身为 `unsigned int`（赋值处 cast 同步）+ 去比较处 cast ⇒ 两处转直比，
// fdiff blocks 9→3、ours 111→109（ORIG 110），逐槽偏移同时对齐。
// 残余：①N102 m_ipBuf/m_pcRoomIp 读 ORIG 恒
// `mov base,%eax; add $0x27/$0x8e418,%eax; movzbl (%eax)` 两步，4.4 折单条 disp
// （七源形探针全折）；②N103 std::strrchr 重载；③`m_ipDword`(unsigned) ←
// `m_field24`(int) 异型 ⇒ 4.4 多 1 条 `mov %eax,%edx` 中转（P265；字段类型改动属
// P321-S2 共享头旋钮；[P1T5 probe c1] 显式 `static_cast<unsigned int>` 同形，已回滚）。
#include "PIHash/SetUserManagerToolIP.inc"
{
    if (user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xc))
        sig->m_pcbangFlag = 1;
    else
        sig->m_pcbangFlag = 0;

    user->m_network.GetPeerIP2(sig->m_ipBuf, 0x10);

    // peer IP 为空时回退用户 m_mcap.m_pcRoomIp（+0x8e418）记录的 IP。
    if (sig->m_ipBuf[0] == '\0' && user->m_mcap.m_pcRoomIp[0] != '\0')
    {
        std::memset(sig->m_ipBuf, 0, 0x10);
        std::strncpy(sig->m_ipBuf, user->m_mcap.m_pcRoomIp, 0x10);
    }

    // [SweepPIHash] ORIG 0x08655327 尾部为 4 个独立 nop+jmp 岛（落点 9d/a0/a3/a6，
    // 均 jmp 出口 a7）⇒ 按 P223 多岛形写 4 条独立 goto + 尾标签串（标签序=守卫序）。
    // 变量先声明（无初始化）以合法跨越 goto（P223 BM8）。
    char* dot;
    // [P1T5 probe u1] ORIG 67/77 直接 `cmpl $0xc,-0x10`/`cmpl $0x4,-0xc` + ja
    // （mem 直比无载入）⇒ 试局部本身为 unsigned（无符号比较 + 无 cast 中转）。
    unsigned int prefixLen;
    unsigned int suffixLen;
    if (std::strlen(sig->m_ipBuf) > 0x10)
        goto tool_ip_end1;
    // 以最后一个 '.' 把 a.b.c.d 拆为前缀(+0x37)/后缀(+0x43)。
    dot = std::strrchr(sig->m_ipBuf, '.');
    if (dot == 0)
        goto tool_ip_end2;
    prefixLen = static_cast<unsigned int>(dot - sig->m_ipBuf);
    if (prefixLen > 0xc)
        goto tool_ip_end3;
    // [b23S3C] P48：后缀起点用 ipBuf+prefixLen+1 表达式（ORIG 两使用点各自
    // `mov 0xc;lea 0x27;mov -0x10;add$1;lea(,%eax,1)` 重取 5 条；dot+1 缓存形
    // 少 3 条/处，非 ORIG 表达式）。
    suffixLen = static_cast<unsigned int>(std::strlen(sig->m_ipBuf + prefixLen + 1));
    if (suffixLen > 4)
        goto tool_ip_end4;
    std::strncpy(sig->m_ipPrefix, sig->m_ipBuf, prefixLen);
    std::strncpy(sig->m_ipSuffix, sig->m_ipBuf + prefixLen + 1, suffixLen);
    // [P1T5 probe c1 · 回滚] 显式 `static_cast<unsigned int>` 与隐式转换同形
    // （4.4 仍产 `mov %eax,%edx` 中转），ORIG 101 直入 edx 属 m_field24(int) →
    // m_ipDword(unsigned) 异型（P265/P321-S2 共享头旋钮），源码不可翻。
    sig->m_ipDword = user->m_mcap.m_field24;  // +0x8e414
tool_ip_end1:;
tool_ip_end2:;
tool_ip_end3:;
tool_ip_end4:;
}
// ============================================================================
// OnlyOrigBatch11 落地：_NS_PI_2ND_MakeHash / GetRandomHashKey /
// ReconstructCheckSum（报告 _lt_global_gt_ 三份，objdump 逐指令转写）
// ============================================================================

// ORIG T 0x0808ca1c：return rand() & 0xf。
unsigned char _NS_PI_2ND_GetRandomHashKey()
{
    return static_cast<unsigned char>(rand() & 0xf);
}

// ORIG T 0x0808ca2c：checksum = (cksum & 0xff) | ((k1<<8) & 0xffff00) |
// ((k2<<24) & 0xf000000) | (k3 << 28)。
// [b28d-L1] 修法：ORIG 0x0808ca2c 首项 `movzbl %al,%edx`（单段落 %edx）；`(unsigned char)checksum` cast 形使 4.4 产 `movzbl %al,%eax; mov k1,%edx; or %eax,%edx`（首项落 %eax，DIFF 4 行）。改 `checksum & 0xff` 掩码形后逐条一致（探针 /tmp/b28d_l1/probe/p_rc.cpp r1 vs r4）。
#include "PIHash/_NS_PI_2ND_ReconstructCheckSum.inc"
                                    unsigned int k2, unsigned int k3)
{
    // [b28c-lane24] ORIG 0x0808ca2c 无帧（sub 无）、累积在 %edx（`movzbl %al,%edx`
    // 起，逐 `or %eax,%edx`），末 `mov %edx,(%eax)` ⇒ 源码是单一赋值表达式
    //（P0：无具名局部槽）。
    // [b28d-L1] 修法：`(unsigned char)checksum` cast 形使 4.4 产
    // `movzbl %al,%eax; mov k1,%edx; …; or %eax,%edx`（首项目标 %eax，DIFF 4 行）；
    // 改 `checksum & 0xff` 掩码形后 4.4 直接 `movzbl %al,%edx` 与 ORIG
    // 0x0808ca35 逐条一致（探针 /tmp/b28d_l1/probe/p_rc.cpp r1 vs r4：cast 首项
    // 落 %eax、掩码形首项落 %edx）。语义同为取低 8 位。
    checksum = (checksum & 0xff) |
               ((k1 << 8) & 0xffff00) |
               ((k2 << 24) & 0xf000000) |
               (k3 << 28);
}
