// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CChattingEmoticonList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// [本波] ImportChattingEmoticon DIFF→AE（p167 CLEAR 42/42 语义完成基础上）：
//  ① P164 显式回边 `scan_exit:` + `goto scan_done` / 公共尾 `goto scan_exit`
//     复现 ORIG 入口直落（原 for(;;)+break 令 4.4 旋转出多余 `jmp/nop`）；
//  ② `const char* key` 具名局部内联进 `find(token.c_str())`（ORIG 该 char* 是
//     匿名实参临时，槽 -0x68；具名局部会整体加深 string/迭代器槽 4 字节）。
//  改后 TU near 1→0、主函数 AE。
// df_game_r 还原 —— CChattingEmoticonList（聊天表情列表，G5 批次）。
// 语义依据 docs/class_func_reports/CChattingEmoticonList/*.md 与
// ChattingEmoticon_{Base,byEvent,byPremium,byLevel}/*.md 反汇编。
// Factory（0x80e53a8）/ Load_list（0x80e54d2）。基类与 CChattingEmoticonList
// 声明见 CDataManager.h；派生类/EmoticonScript 见 CChattingEmoticonList.h。
// ============================================================================

#include "CChattingEmoticonList.h"

#include "CUser.h"
#include "DNFLexWrapper.h"   // loadRDARScriptFile / ScanType / ScanInt
#include <stdio.h>
#include "NpcScript.h"

// class SpinLock / struct less_TCHAR（ImportChattingEmoticon 函数级静态查找表）
#include "CEnvironment.h"
#include <map>
#include <string>
#include <vector>

// ============================================================================
// Factory（ORIG 0x80e53a8）
// 依 script.m_type 分派构造对应派生类；SetIndex 在**每个 case 内**执行
// （ORIG +0x73/+0xc3/+0x11a 三处独立 call，非共享尾），default 仅保持
// result=0（ORIG cmp 链尾 `cmp $1; jne end` 直落出口，无额外写）。
// ============================================================================
#include "CChattingEmoticonList/Factory.inc"
}

// ---- 本地 STScriptFileList（与 CItemShop.cpp / STScriptFileList.cpp 同布局：
//      +0x00 std::map<int,std::string>，大小 0x18）----


// ORIG 全局（B g_ChattingEmoticonScriptFileList 0x094f6cc0 = 0x18
// STScriptFileList 对象 / g_ChattingEmoticonScriptBaseDirectory
// 0x094f6bc0[0x100]）。由 initChattingEmoticonScriptList
// （InitScriptProducers_5.cpp）填充；Load_list 遍历使用（ORIG 0x80e54f2
// map begin 直取 0x94f6cc0，无空指针检查）。
STScriptFileList g_ChattingEmoticonScriptFileList;
char g_ChattingEmoticonScriptBaseDirectory[0x100];

// ============================================================================
// ImportChattingEmoticon —— ORIG T 0x88e31b4（报告
// docs/class_func_reports/_lt_global_gt_/ImportChattingEmoticon.md）。
// RDAR 双相派发族（P163/P164/P165/P177，与 importCraneMinigameScript
// 同构，见 CraneMinigameManager.cpp）：函数级静态 __lookup
// (map<const char*,int,less_TCHAR>，guard+atexit)/__init_lock(SpinLock)/
// __inited(bool) 三件套位于重派发标签后，每轮穿 guard；注册轮持锁 id=0 →
// case 0 与 case 0x2f 共 leaf（+0x275）→ 贯穿 insert "[type]"0x2f →
// "[ani]"0x40 → "[macro]"0x46（+0x2c1/+0x39e jmp），公共尾 +0x430
// leave+置位+jmp +0x80 以当前 token 重查派发。建表后派发：
// [type]：ScanStr(value) → "EVENT"→m_type=1 / "CASH"→2 / 其它→4，随后
//         ScanInt(&m_param)（失败 return false）；
// [ani]/[macro]：ScanStr 直写 script+0xc / +0x10。
// +0x5b `movl $0x0,-0x20(%ebp)` 为只写死局部，照抄（P171）。
// 收敛口径（§6.2 三件套）：帧 `sub $0x7c` + push edi/esi/ebx 全等；id/死槽
// -0x1c/-0x20 与 ORIG 同槽；P167 调用点差集 42/42 全命中（唯一差集为
// __cxa_atexit @plt 显示）。**本波已归零**（DIFF→AE）：P164 显式回边标签
// 取代 for(;;)+break，消掉 4.4 的入口旋转岛；`token.c_str()` 内联进 find
// 调用（去掉具名 key 局部）使 string/迭代器槽与 ORIG 全等。
// 全函数逐条全等（仅 .o 未重定位的数据地址呈 AE）。
// ============================================================================
#include "CChattingEmoticonList/ImportChattingEmoticon.inc"
}

// ============================================================================
// ChattingEmoticonBase 派生类（ctor 置 vptr + 调基类；GetStart/End/语义见报告）
// ============================================================================

#include "CChattingEmoticonList/ChattingEmoticon_byEvent.inc"

#include "CChattingEmoticonList/isUsable.inc"
}

#include "CChattingEmoticonList/GetStartArrayIndex.inc"
#include "CChattingEmoticonList/GetEndArrayIndex.inc"

#include "CChattingEmoticonList/ChattingEmoticon_byPremium.inc"

#include "CChattingEmoticonList/isUsable_1.inc"
}

#include "CChattingEmoticonList/GetStartArrayIndex_1.inc"
#include "CChattingEmoticonList/GetEndArrayIndex_1.inc"

#include "CChattingEmoticonList/ChattingEmoticon_byLevel.inc"

#include "CChattingEmoticonList/isUsable_2.inc"
}

#include "CChattingEmoticonList/GetStartArrayIndex_2.inc"
#include "CChattingEmoticonList/GetEndArrayIndex_2.inc"

#include "CChattingEmoticonList/isCommonEmoticon.inc"
}

// ORIG W 0x080e5fe0：空体（push ebp/mov esp,ebp/pop ebp/ret）。
#include "CChattingEmoticonList/AddPremiumIndex.inc"
}

// ============================================================================
// Load_list（ORIG 0x80e54d2）
// 遍历 g_ChattingEmoticonScriptFileList.m_fileMap，逐条 ImportChattingEmoticon；
// Factory 成功→ m_map.find(GetIndex()) 判重（重复时虚析构 delete 返回失败）→
// m_map.insert(make_pair(GetIndex()【第二次独立调用，P48】, emo))；
// **m_mapVec 按 script.m_type(+0x4) 入桶**（ORIG +0x187/+0x2ac 传 &script+4，
// 非 GetIndex）；桶缺失时显式 vector 临时+insert+逆序析构，最后统一下标 push_back。
// path==NULL 时 break → return true（+0x310 尾岛 mov $1）。
// 收敛口径：ORIG/ours 245/245，助记符序列全等（NEAR），栈槽逐条一致
// （-0xc/-0x68/-0x64/-0x7c/-0x80/-0x60/-0x58/-0x5c/-0x44/-0x4c/-0x54/
// -0x84/-0x38/-0x90/-0x1c/-0x2c/-0x34 全等）。残余文本差仅两类：
// ① call/数据地址 .o 未重定位显示（链接后 AE 核销）；② 判重 delete 的
// 虚调用 callee 寄存器 %edx↔%eax 互换（范式库 N7，勿再试）。
// ============================================================================
#include "CChattingEmoticonList/Load_list.inc"
}
