// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ProxySecurityDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ProxySecurityDeps.cpp —— checkDenyIP / cutIPdclass / getSha256UsingSalt 真实定义。
// 全部按 ORIG objdump 逐指令转写（报告缺失，地址见 ProxySecurityDeps.h）。

#include "ProxySecurityDeps.h"
#include "ChecksumAlgo.h"        // CreateCryptoChecksum/DestroyCryptoChecksum/IChecksum
#include <string.h>              // strncmp/strcpy/strcat/memset

// ORIG T _Z11cutIPdclassPKc @0x081f440a sz 0xb1（sret）：
//   std::string s(ip); s.erase(s.rfind('.', npos), npos); return s;
// [b28d-N7 复验] fresh 58=58：ORIG 0x081f4416 `mov %ebx,%esi`（sret 指针提前入
// callee-saved，跨 2 次 call 保活）vs 4.4 在 call 后 `mov %ebx,%eax` 重载 ⇒
// 寄存器角色互换（P52/N7 族）。p167 调用点差集仅 C1↔C2/D1↔D2 同址别名对（P93，
// 非缺口）。语义逐条全等，N 封顶。
// [round5 路由实测] 本 TU 不在任何路由表；447 / 仅 446 后端 / 446 后端+446 头集
// 三态编译读数**全同**（identical=0 ae=4 near=0 diff=2）⇒ P230 补的「4.4.6 可翻」
// 在本 TU 不成立，路由变更零收益（勿再派单试路由）。
std::string cutIPdclass(const char* ip)
{
    std::string s(ip);                            // ORIG _ZNSsC1EPKcRKSaIcE
    s.erase(s.rfind('.'), std::string::npos);     // rfind '.'@0x2e pos=npos; erase(pos,-1)
    return s;
}

// ORIG T _Z11checkDenyIPPKcS0_ @0x081f44bb sz 0x1d0：
//   null/空串 → false；strncmp(a,b,0x10)==0 → false；
//   cutIPdclass 相等 → false；a=="0" → false；返回 (b == "0")。
// [b28d-N86] fresh 复验 132/117：差值全在 std::string 成员函数外联调用序列
// （4.1 保留 find/erase/operator= 中间结果寄存器链，4.4 多 15 条 sret/临时槽）
// 与多分支早返块布局（P47/N104）⇒ 跨版本 libstdc++ 内联差，语义逐分支全等。
#include "ProxySecurityDeps/checkDenyIP.inc"
}

// ORIG T _Z18getSha256UsingSaltPKhiPc @0x0808cc6a sz 0x18c：
//   null→false；首字节 '\0'→true（不处理）；
//   buf=new uchar[len+4]; memset(0); strcpy(src); strcat("nhs");
//   digest[0x20]={0}; CreateCryptoChecksum(CRYPTO_CHECKSUM_TYPE(1))；
//   虚槽 +0xc（仓库 ChecksumAlgo.h 槽序为 MakeChecksumFromMemory，直呼同方法——
//   槽位偏移差异属共享头排列豁免，行为一致）：
//     MakeChecksumFromMemory(buf, len+4, digest, 0x20)；==0x6fffffff → ok=true；
//   for i in 0..31: sprintf(out+i*2, "%02X", digest[i])；
//   DestroyCryptoChecksum(&ck); delete[] buf; return ok。
// [b28d-N7 复验 + round5 五修] ORIG 113 vs our 110：
//   本轮按报告逐段对齐后已修：①buf 清零改 `new[]`+显式 memset（ORIG 0x0808cccd
//   为 call memset，非 value-init 内联循环）；②digest 改 `= {0}` 内联 word 循环
//   （ORIG 0x0808cd11..0x0808cd34 `and $0xfffffffc` 循环，非 memset call）；
//   ③补 3 条原码死写入 int（0x20/0x41/0x4，全函数无读取点，P171 照抄）——
//   补齐后 ok/-0x1d、3×int/-0x1c..-0x11、salt/-0x21..-0x1e 与 ORIG 逐槽全等；
//   ④循环尾测改 `(unsigned int)i < 0x20` 复现 `mov slot;cmp;setbe`（P310）。
//   ⑤[round5] 补 `int i = 0;` + `for (i = 0; …)` 双初始化：ORIG 0x0808cd87/
//   0x0808cd8e 连发**两条** `movl $0x0,-0xc(%ebp)`（P191 配套）⇒ our 109→110。
//   剩余 3 条 = N7 族：ORIG 虚函数指针提前入 %esi（push/pop esi + 帧 0x60）且
//   `len+4` 走 mov+add，4.4.7 恒 `call *%eax` + lea 折叠（帧 0x64）——间接 call 目标
//   寄存器角色差。**round5 路由实测：447 / 仅 446 后端 / 446 后端+446 头集 三态
//   逐函数读数全同（diff=2）⇒ 路由变更无收益（P230 补的「4.4.6 可翻」本 TU 不成立）。**
//   p167 CALLSITE CLEAR。
#include "ProxySecurityDeps/getSha256UsingSalt.inc"
}
