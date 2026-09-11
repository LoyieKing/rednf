// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 SimpleEncryption/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// SimpleEncryption 仅ORIG 11 符号真实定义（9 函数 + 无额外辅助）。
// 证据：docs/class_func_reports/SimpleEncryption/*.md（9 份报告全文已读）。
// 关键点：
//  - instance() 为函数局部 static 单例（P20），局部名 simpleEncryption 对应
//    _ZZN16SimpleEncryption8instanceEvE16simpleEncryption（16=名字长度实测）。
//  - 单参 decryptUINT 在 ORIG 中调用的是双参 encryptUINT（0x0808cfd8 call 0x0808cf24），
//    非双参 decryptUINT——照抄 ORIG 行为。
//  - C1 用函数体逐句赋值（P29：初始化列表会缓存 this 到 ecx，ORIG 三次重读 0x8(%ebp)）。
//  - 双参 encrypt/decryptUINT：ORIG 局部先 movl $0 清零再赋值（0x0808cf2a/0x0808cf6e）；
//    u 以 int 声明、使用处 (unsigned int)u ^ shift，复现 ORIG xor 左先物化形态。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN16SimpleEncryptionC2Ev`（ORIG 0x808d043）；ctor 已是首强符号。
#include <iostream>

#include "SimpleEncryption.h"

#include "SimpleEncryption/SimpleEncryption.inc"


#include "SimpleEncryption/dtor_SimpleEncryption.inc"


#include "SimpleEncryption/instance.inc"


#include "SimpleEncryption/initialize.inc"


#include "SimpleEncryption/uninitialize.inc"


#include "SimpleEncryption/encryptUINT.inc"


#include "SimpleEncryption/encryptUINT_1.inc"


#include "SimpleEncryption/decryptUINT.inc"


#include "SimpleEncryption/decryptUINT_1.inc"

