// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CChattingEmoticonList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "CChattingEmoticonList/Factory.inc"


// ---- 本地 STScriptFileList（与 CItemShop.cpp / STScriptFileList.cpp 同布局：
//      +0x00 std::map<int,std::string>，大小 0x18）----


// ORIG 全局（B g_ChattingEmoticonScriptFileList 0x094f6cc0 = 0x18
// STScriptFileList 对象 / g_ChattingEmoticonScriptBaseDirectory
// 0x094f6bc0[0x100]）。由 initChattingEmoticonScriptList
// （InitScriptProducers_5.cpp）填充；Load_list 遍历使用（ORIG 0x80e54f2
// map begin 直取 0x94f6cc0，无空指针检查）。
STScriptFileList g_ChattingEmoticonScriptFileList;
extern char g_ChattingEmoticonScriptBaseDirectory[0x100];   // [SplitC batch5] 定义已迁出至 RDARScriptChattingEmoticon.cpp

#include "CChattingEmoticonList/ImportChattingEmoticon.inc"
                            const char* path)
{
    if (!loadRDARScriptFile(g_ChattingEmoticonScriptBaseDirectory, path))
        return false;                                   // +0x23 ebx=0
    script.m_index =
        g_ChattingEmoticonScriptFileList.FindIndexByFullScan(path);  // +0x34

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;      // ORIG -0x70（+0x45 构造）
    std::string value;      // ORIG -0x74（+0x50 构造）
    int dead20 = 0;         // ORIG -0x20 死写入，照抄

    for (;;)
    {
        if (!ScanType(token, true))    // +0x62（xor$1 物化）
        {
        scan_exit:                     // [P164] 回边落点：入口直落、出口共享
            goto scan_done;            // +0x45c：mov $1,%ebx → dtor 岛 → true
        }

    redo_lookup:                       // +0x80：两条重派发边（+0x23c/+0x451）落点
        static LookupMap __lookup;     // guard + __cxa_atexit(map::~map)
        static SpinLock __init_lock;   // guard + __cxa_atexit(SpinLock::~SpinLock)
        static bool __inited = false;  // 零初始化 plain bool（无 guard）

        int id;                        // ORIG -0x1c
        if (__inited)
        {
            LookupMap::const_iterator it = __lookup.find(token.c_str());  // -0x68 临时
            id = (it != __lookup.end()) ? it->second : -1;             // -0x60/-0x64
        }
        else
        {
            __init_lock.enter();                                       // +0x21f
            if (__inited)
            {
                __init_lock.leave();                                   // +0x236
                goto redo_lookup;                                      // +0x23c
            }
            id = 0;                                // +0x241 注册轮入口
        }

        switch (id)
        {
        case 0:
        case 0x2f:                      // [type]（+0x275 与 case 0 共 leaf）
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[type]", 0x2f));
            }
            else
            {
                if (!ScanStr(&value))              // +0x2cc
                    return false;
                if (value == "EVENT")              // +0x2f0
                    script.m_type = 1;
                else if (value == "CASH")          // +0x313
                    script.m_type = 2;
                else
                    script.m_type = 4;
                if (!ScanInt(&script.m_param))     // +0x33b
                    return false;
                break;                             // → 公共尾 +0x430
            }
            /* fall through（+0x2c1 jmp +0x355） */
        case 0x40:                      // [ani]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[ani]", 0x40));
            }
            else
            {
                if (!ScanStr(&script.m_field0c))   // +0x3ac
                    return false;
                break;                             // → 公共尾
            }
            /* fall through（+0x39e jmp +0x3c2） */
        case 0x46:                      // [macro]
            if (!__inited)
            {
                __lookup.insert(LookupMap::value_type("[macro]", 0x46));
            }
            else
            {
                if (!ScanStr(&script.m_field10))   // +0x419
                    return false;
                break;                             // → 公共尾
            }
        }

        if (!__inited)                 // 公共尾 +0x430（xor$1 物化）：
        {                              // inited → je 落尾部回跳岛（ORIG +0x456
            __init_lock.leave();                   //   nop;jmp +0x62，nop 剥离
            __inited = true;                       // +0x44e   后为 je;jmp 形）
            goto redo_lookup;                      // +0x451 → guard 重派发
        }
        // [V331n00 SEMANTIC-DIFF 修复] 公共尾回边 = **回循环头**（ORIG 0x88e360b
        // `jmp 0x88e3216 <+0x62>`，即 `movl $0x1,0x4(%esp); call ScanType` 处）：
        // 原写 `goto scan_exit;` 会被折进 if 体内的 `goto scan_done` ⇒ 变 return true
        // （我方入口集合 #28=∅/#283=[282]，ORIG #28=[282]/#283=[34]），使循环只跑一轮、
        // 后续 [ani]/[macro] 段不再解析。
        // 控制流自然到达循环体末尾 ⇒ 回边到循环头（ScanType 检查）。**勿**写
        // `goto scan_exit;`（会被折进 if 体内的 goto scan_done ⇒ 变 return true）。
    }
scan_done:
    return true;                                   // +0x4ac ebx=1
}

// ============================================================================
// ChattingEmoticonBase 派生类（ctor 置 vptr + 调基类；GetStart/End/语义见报告）
// ============================================================================

#include "CChattingEmoticonList/ChattingEmoticon_byEvent.inc"


#include "CChattingEmoticonList/isUsable.inc"


#include "CChattingEmoticonList/GetStartArrayIndex.inc"
#include "CChattingEmoticonList/GetEndArrayIndex.inc"

#include "CChattingEmoticonList/ChattingEmoticon_byPremium.inc"


#include "CChattingEmoticonList/isUsable_1.inc"


#include "CChattingEmoticonList/GetStartArrayIndex_1.inc"
#include "CChattingEmoticonList/GetEndArrayIndex_1.inc"

#include "CChattingEmoticonList/ChattingEmoticon_byLevel.inc"


#include "CChattingEmoticonList/isUsable_2.inc"


#include "CChattingEmoticonList/GetStartArrayIndex_2.inc"
#include "CChattingEmoticonList/GetEndArrayIndex_2.inc"

#include "CChattingEmoticonList/isCommonEmoticon.inc"


#include "CChattingEmoticonList/AddPremiumIndex.inc"


#include "CChattingEmoticonList/Load_list.inc"

