// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ProxySecurityDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "ProxySecurityDeps/checkDenyIP.inc"


#include "ProxySecurityDeps/getSha256UsingSalt.inc"

