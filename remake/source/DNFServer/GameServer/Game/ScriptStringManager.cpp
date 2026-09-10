// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ScriptStringManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// ScriptStringManager 底层引擎（G4 词法批次；ORIG 地址与汇编证据见
// docs/class_func_reports/ScriptStringManager.md）
// ----------------------------------------------------------------------------
// 本 TU 提供（符号名与 ORIG ABI 一致，由真实 C++ 类成员函数定义生成）：
//   ScriptStringManager::ScriptStringManager()   _ZN19ScriptStringManagerC1Ev
//                                                          ORIG 0x8acf110
//   ScriptStringManager::~ScriptStringManager()  _ZN19ScriptStringManagerD1Ev
//                                                          ORIG 0x8acf5f0
//                                                _ZN19ScriptStringManagerD0Ev
//                                                          ORIG 0x8acf700
//   ScriptStringManager::importFromBuffer(...)   _ZN19ScriptStringManager16importFromBufferEPKhi
//                                                          ORIG 0x8acf970
//   ScriptStringManager::importFromFile(...)     _ZN19ScriptStringManager14importFromFileEPKc
//                                                          ORIG 0x8acfd00
//   ScriptStringManager::clear()                 _ZN19ScriptStringManager5clearEv
//                                                          ORIG 0x8acf550
//   ScriptStringManager::exportToBuffer(...)     _ZNK19ScriptStringManager14exportToBufferEPhi
//                                                          ORIG 0x8acf420
//   ScriptStringManager::exportToFile(...)       _ZNK19ScriptStringManager14exportToFileEPKc
//                                                          ORIG 0x8acf2a0
//   ScriptStringManager::getString(int,str*)     _ZNK19ScriptStringManager9getStringEiPSs
//                                                          ORIG 0x8acf160
//   ScriptStringManager::getString(int)          _ZNK19ScriptStringManager9getStringEi
//                                                          ORIG 0x8acf1f0
//   ScriptStringManager::insertString(...)       _ZN19ScriptStringManager12insertStringEPKc
//                                                          ORIG 0x8acf720
//
// 类布局依据 ORIG ScriptStringManager::ScriptStringManager（0x8acf110）：
//   +0x00 vptr（虚析构；CompiledDNFLex::destroy 0x8ad0370 经 vptr[1]=D0 释放）
//   +0x04 strings_    std::vector<std::string>（4.4 COW，元素 4 字节）
//   +0x10 string_map_ std::map<std::string,int>
//     （c6root 4.4.4 bits/stl_tree.h：_Rb_tree_impl{_Node_allocator(EBO),
//      _M_key_compare(4), _M_header(16), _M_node_count(4)} = 24 字节；
//      header 起于 map+4 → 对象 +0x14 color/+0x18 parent/+0x1c left/
//      +0x20 right/+0x24 node_count，与 ORIG 构造逐条吻合）
//   总大小 0x28，与 CompiledDNFLex::create（0x8ad03d0）
//   operator new(0x28) 一致。
//
// 字符串表二进制格式（exportToBuffer 0x8acf420 写出，本函数读回）：
//   [0:4]       count（字符串条数）
//   [4:4]       offsets[0] = 4 + 4*count（理论字符串区起点，仅差值有意义）
//   [4+4*i:4]   offsets[i] = offsets[i-1] + len(str[i-1])，i = 1..count-1
//   [4*count+4:4] offsets[count] = total（哨兵，= 4*count+4+Σlen，比文件
//                总大小 4*count+8+Σlen 小 4；import 只用相邻差值）
//   [4*count+8: ] 字符串数据：第 i 个字符串长 offsets[i+1]-offsets[i]，
//               数据区顺序排布（export 从 buffer+4*count+8 起写）
//
// importFromBuffer 语义（0x8acf970）：
//   1. buffer==NULL 或 buffSize<4 → false
//   2. 读 count；逐步校验 buffSize 后读入 count+1 个 offsets
//   3. reserve 字符串向量 count；i=1..count：
//        len = offsets[i]-offsets[i-1]；拷贝到 1024 栈缓冲并 NUL 终止
//        （len>1023 时原版只拷前 1023 字节——栈缓冲截断，行为保留）
//        构造 std::string push 到 strings_；缓冲越界 → false
//        （已入列部分保留，ORIG 如此）
//   4. 全部成功 → true
//
// importFromFile（0x8acfd00）：fopen "rb" → fseek END → ftell → fseek 0 →
//   new[size] → fread → fclose → importFromBuffer（结果忽略）→ delete[] →
//   true；任一步失败 → false（ORIG 在 importFromBuffer 后无条件 return
//   true，忠实保留）。
//
// clear（0x8acf550）：strings_ 各元素释放（COW refcount 递减）后 finish 重置
//   为 start，string_map_ 整树 _M_erase 后 header 复位。等价于标准
//   strings_.clear(); string_map_.clear();（二者均不释放 capacity）。
//
// exportToBuffer（0x8acf420，const）：按文件头格式写 count + count+1 个
//   offset 字 + 字符串数据到用户缓冲；返回写入总字节数，buffSize 不足
//   任一段时返回 -1（写前逐段检查：offsets 区每字、数据区每串 memcpy 后）。
//   注意 ORIG 无 buffer==NULL 检查（UB），此处以返回 -1 作护栏。
//
// exportToFile（0x8acf2a0，const）：fopen(fileName,"wb") → fwrite count →
//   count>0 时逐串 fwrite 累加后的 offset 字 → fwrite 最终 total 哨兵 →
//   count>0 时逐串 fwrite 数据（len!=0 才写）→ fclose，全程 true；任一
//   fwrite 失败 → fclose 后 false；fopen 失败 → false（无 fclose）。
//
// getString(int,string*)（0x8acf160，const，bool）：先 _M_mutate(0,len,0)
//   清空输出串，再 assign(strings_[index])，成功 true。ORIG 的负 index
//   检查是死代码（负 index 恒走越界 assign，UB），此处按 DWARF 意图以
//   越界返回 false 作护栏。
//
// getString(int)（0x8acf1f0，const，返回指针）：函数内静态 DEFAULT_STRING
//   （guard 0x9501a00 惰性初始化，构造自 ""）；index<0 或 index>=条数时
//   返回 &DEFAULT_STRING，否则返回 &strings_[index]。ORIG 返回非常量
//   string*（DWARF），const 成员下取成员地址为 const，此处按 const 返回。
//
// insertString（0x8acf720，int）：字符串表 intern：已在 string_map_ 中 →
//   返回既有 index；否则 push 到 strings_ 并 map 插入 {string,新 index}，
//   返回新 index。ORIG 的"已存在"路径返回树节点内字段（无意义值），
//   插入路径返回新 index（8acf891 计算）；本实现统一按 intern 语义
//   返回 index。ORIG 无 string==NULL 检查（UB），此处以空串作护栏。
// ============================================================================

#include "ScriptStringManager.h"

#include <cstdio>
#include <cstring>

// 类定义见 ScriptStringManager.h（唯一声明点）。

// clear @ 0x8acf550：逐元素释放 strings_（COW refcount 递减，8acf56a 检查
// empty rep 0x948ccf0 后 _M_destroy）→ finish 重置为 start（8acf57e）→
// string_map_ 整树 _M_erase（8acf58b）→ header left/right=自身、parent=0、
// node_count=0（8acf590-8acf5a0）。标准 strings_.clear(); string_map_.clear();
// 语义完全等价（二者均保留 capacity）。
#include "ScriptStringManager/clear.inc"
}

// getString(int,string*) @ 0x8acf160：先 _M_mutate(0,len,0) 清空输出串
// （8acf194），再 assign(strings_[index])（8acf1aa），成功返回 true。
// ORIG 的负 index 分支检查是死代码（负 index 恒满足 index<=size 而走
// 越界 assign，UB），此处按 DWARF 意图以越界返回 false 作护栏；
// outputString==NULL 亦为 ORIG UB，护栏返回 false。
// b28d-solo-ScriptStringManager 判词：DIFF 封顶 **N7**（44 vs 44 条，p167 CLEAR）。
// 残差 = ①装载寄存器角色互换（ORIG `mov 0x4(%edi),%eax; lea (%eax,%esi,4),%eax`
// vs 我方 %ecx 中转）；②`xor %eax,%eax`（false 常量）提前物化（ORIG 在 epilogue
// 前）。实测旋钮均不翻：纯 register 声明（P231）、const 引用局部、具名 count 中转。
#include "ScriptStringManager/getString.inc"
}

// getString(int) @ 0x8acf1f0：函数内静态 DEFAULT_STRING（guard 0x9501a00
// 惰性初始化，ORIG 经 _ZNSsC1EPKcRKSaIcE 自 "" 构造，对象 0x9501a08）。
// index<0 或 index>=条数 → &DEFAULT_STRING（8acf221），否则返回
// &strings_[index]（8acf226）。ORIG 返回非常量 string*（DWARF 原型），
// const 成员取成员地址为 const，此处按 const std::string* 返回。
// b28d-solo-ScriptStringManager 判词：**NEAR**（45 vs 45 条，p167 CLEAR）。
// 残差仅 .o 未解析重定位的 call/guard 标签显示差。关键：具名 `result` 单出口形
// 消掉 4.4 的双 epilogue（嵌套 if / 三目恒 53 条），与 ORIG 共享尾块同构。
#include "ScriptStringManager/getString_1.inc"
}

// insertString @ 0x8acf720：字符串表 intern。已存在（map 树中找到，
// 8acf7f5 后 8acf802 分支）→ 返回既有 index；否则 push 到 strings_
// （8acf8f2 _M_insert_aux / 8acf85c 原地拷贝构造）→ map _M_insert_unique
// 插入 {string, 新 index}（8acf8b7）→ 返回新 index（8acf891 计算，
// (finish-start)>>2 - 1）。ORIG"已存在"路径返回树节点内字段（无意义
// 值），本实现统一按 intern 语义返回 index。
// b28d-solo-ScriptStringManager 判词：DIFF 封顶 **N89**（259 vs 196 条；
// p167: 2 MISSING 右值模板实例 + 4 EXTRA）。ORIG 3×`std::string(string)` 临时 +
// out-of-line `~string`（_ZNSsD1Ev×4）；4.4.7 头在 -O2 把 `~basic_string` 内联
// 展开（4× `_M_destroy` + pthread_cancel 段）净 +~60 条。MISSING
// `_M_insert_auxIISsEE…DpOT_`（C++0x 右值版）来自 ORIG 归档 4.4 libstdc++；
// gnu++0x 实测更差（282 条 / TU diff 12 vs 10）⇒ 不可翻。
#include "ScriptStringManager/insertString.inc"
}

// exportToBuffer @ 0x8acf420：count + (count+1) 个 offset 字 + 字符串数据；
// 返回写入总字节数，任一段缓冲不足返回 -1。检查时机与 ORIG 逐点一致：
// offsets[i] 写前查 4+4*i<=buffSize（8acf480/8acf491）、数据区前查
// 总大小 offset<=buffSize（8acf4bd）、每串 memcpy 后查游标<=buffSize
// （8acf528）。ORIG 无 buffer==NULL 检查（UB），此处护栏返回 -1。
// b28d-solo-ScriptStringManager 判词：DIFF 封顶 **N82**（92 vs 102 条，
// p167 CLEAR）。字节游标 `p`/`pos` 具名形已复现 ORIG 的 strength-reduced 形
// （`mov -0x8(%edi,%eax,1)`，非 scaled index）+ `buffSize>7` 入口门（P171：
// 原码 buffSize<=7 时不越界写 buffer+8）；残差 = 槽序（ORIG
// count@-0x1c/p@-0x20/pos@-0x24 vs 我方 p@-0x1c/count@-0x20/finish@-0x24，
// 4.4 槽按首次使用分配、声明序不可翻 = N98）＋ %esi/%edi 角色互换。
#include "ScriptStringManager/exportToBuffer.inc"
}

// exportToFile @ 0x8acf2a0：fopen(fileName,"wb") → fwrite count(4B) →
// count>0 时逐串 fwrite 累加 offset 字（每串后 total+=len）→ fwrite 最终
// total 哨兵 → count>0 时逐串 fwrite 数据（len!=0 才写）→ fclose 后 true；
// 任一 fwrite 失败 → fclose 后 false；fopen 失败 → false（ORIG 无 fclose）。
// 布局与 exportToBuffer 完全一致。ORIG 无 fileName==NULL 检查（P171 照抄）。
// b28d-solo-ScriptStringManager 判词：DIFF 封顶 **N7**（112 vs 115 条，
// p167 CLEAR）。已复现：无 NULL 护栏、count/offset 预清零（P198①）、正向 if +
// 共享 fail 尾（P222）、第二循环 do-while 消 4.4 旋转（P283）。残差 = 第三段
// 循环 this/file/finish 的寄存器角色（ORIG file spill@-0x2c、this 保活 %ebx；
// 我方 this spill@-0x2c、file 保活 %esi）。
#include "ScriptStringManager/exportToFile.inc"
}
// 构造 @ 0x8acf110：vptr 赋值 + strings_ 三指针清零 + string_map_ header
// 初始化（_M_key_compare 默认、_M_header{red,0,&header,&header}、
// _M_node_count=0）。标准成员初始化即等价。
#include "ScriptStringManager/ScriptStringManager.inc"
#include "ScriptStringManager/dtor_ScriptStringManager.inc"
#include "ScriptStringManager/importFromBuffer.inc"
}

// importFromFile @ 0x8acfd00
#include "ScriptStringManager/importFromFile.inc"
}
