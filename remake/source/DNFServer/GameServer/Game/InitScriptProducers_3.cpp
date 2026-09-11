// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 InitScriptProducers_3/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CDataManager.h"
#include "sync_script.h"         // CSyncScript/STEquipmentPartSetPiece（须先于 deps 头，与 CDataManager.cpp 口径一致）
#include "DataManagerInitDepends.h"
#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt/ScanFloat/ScanStr/ScanTypeOrStr
#include "LogManager.h"          // LogManager::logFormat

#include <map>
#include <string>
#include <string.h>              // memcpy（ORIG importQuestParameterScript [difficulty] 段）
#include <utility>
#include <vector>

// ---- 外部依赖声明（ORIG T 符号，定义归属各自批次；此处仅引用）----

#include "InitScriptProducers_3/importRewards.inc"
#include "InitScriptProducers_3/importTitleSection.inc"
// _Z18getTitleCollectionP15stTitleBookInfoi 0x08aa94e1
std::vector<stTitleElement>* getTitleCollection(stTitleBookInfo* book, int tab);
#include "InitScriptProducers_3/convertTabTypeStringToEnum.inc"
#include "InitScriptProducers_3/loadAppendageType.inc"
#include "InitScriptProducers_3/loadValueType.inc"

// g_equipmentStatInfoMap —— ORIG 未修饰名 B 0x095000a0（RDARScriptOldEquip 域
// 全局，本 TU 为其唯一定义点；消费方经同名 extern "C" 引用）。
// 类型证据：报告 _lt_global_gt_/__static_initialization_and_destruction_0_
// 08a588b8.md —— ORIG 静态初始化为 map<unsigned int, STEquipmentStatInfo>
// 默认构造并 __cxa_atexit 注册析构（本定义的动态初始化与之等价）。
extern "C"
{
std::map<unsigned int, STEquipmentStatInfo> g_equipmentStatInfoMap;
}

#include "InitScriptProducers_3/importOldEquipmentStatInfo.inc"


#include "InitScriptProducers_3/importPVPChannelList.inc"


// [SplitC 2026-09-11 batch4 · TU 迁移] importGuildParameterScript 已迁出至 RDARScriptGuildParameter.cpp（ORIG GI 键对齐）。



// [SplitC 2026-09-11 batch4 · TU 迁移] importPowerParameterScript 已迁出至 RDARScriptPowerParameter.cpp（ORIG GI 键对齐）。



// [SplitC 2026-09-11 batch4 · TU 迁移] importQuestParameterScript 已迁出至 RDARScriptQuestParameter.cpp（ORIG GI 键对齐）。



#include "InitScriptProducers_3/importTitleBookEtcScript.inc"
{
    if (!loadRDARScriptFile("", path))
        return false;

    // ORIG 每条键注册只有两次 call：pair<const char* const,int>::pair<char const(&)[N],int>
    // （即 map::value_type 就地构造，符号 _ZNSt4pairIKPKciEC1IRA<N>_S0_iEEOT_OT0_）
    // 再 map::insert(value_type const&)。用 make_pair 会多出
    // pair<const char*,int> → value_type 的转换临时（第三次 call），与 ORIG 不符。
    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    std::string tok2;   // ORIG [ebp-0xdc]：构造/析构却从未使用的空串槽
    std::string tok3;
    bool ok = false;

    // ORIG +0x61 处条件在循环顶部（call ScanType; xor eax,1; test; jne +0x9d8），
    // 退出标号 +0x9d8 是一个 nop（break 出口）后接 return true。
    // [2026-09-10 D06] 外层同样改 goto 形（P164/P283）：`for (;;)` 会让 4.4 在
    // 循环头前多插一条入口 `jmp <head>; nop`（实测 241b `jmp 241e`），ORIG 是
    // `movb $0x0,-0xe1` 后**顺序流入** +0x61。
outer_retry:;
    if (!ScanType(token, true))
        goto outer_done;

        // ORIG 的重派发跳回 +0x82（__lookup 的 __cxa_guard 检查），
        // 说明三个函数级静态声明位于外层 token 循环体内部。
        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;   // ORIG !__inited 用 xor eax,1 → bool

        // ORIG 重派发是一个内层循环：continue 回到 +0x82（__lookup guard）。
        // [2026-09-10 D06] 内层循环用 goto 形（P164/P283）：`for (;;)` 会让 4.4 在
        // 循环头前多插一条入口 `jmp`（实测 241b `jmp 241e`），goto 形则顺序流入 guard。
    titlebook_retry:;                       // ORIG +0x82（__lookup guard 头）
        {
            int id;
            if (__inited)
            {
                std::map<const char*, int, less_TCHAR>::const_iterator it =
                    __lookup.find(token.c_str());
                id = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    goto titlebook_retry;
                }
                id = 0;
            }

            // ORIG +0x26e 起是 cmp/je 判定树：case 0（未初始化态）与首个
            // case 0x39 合并；每个 case 在 !__inited 时登记一条键值后**贯穿**
            // 到下一 case 继续登记，最终落到 default（=switch 末尾 +0x9ac）；
            // 已初始化态执行段体后 break 退出 switch。
            switch (id)
            {
            case 0x39:                           // [default reward]
            case 0:
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[default reward]", 0x39));
                }
                else
                {
                    while (true)
                    {
                        if (!ScanType(tok3, true))
                            return true;
                        if (tok3 == "[/default reward]")
                            break;
                        importRewards(&out->m_vec48, tok3);
                    }
                    break;
                }
            }
            /* fall through */
            case 0x4d:                           // [title collection info]
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[title collection info]", 0x4d));
                }
                else
                {
                    ScanStr(&tok3);
                    int tab = convertTabTypeStringToEnum(tok3.c_str());
                    int count = ScanInt((bool*)0);
                    for (int i = 0; i < count; ++i)
                    {
                        ScanInt((bool*)0);              // ORIG 丢弃值（asm +0x468）
                        stTitleElement element;
                        element.m_index = ScanInt((bool*)0);
                        if (element.m_index != -1)
                        {
                            element.m_achievementIdx = ScanInt((bool*)0);
                            int jobCount = ScanInt((bool*)0);
                            for (int j = 0; j < jobCount; ++j)
                                element.m_fitJobs.push_back(ScanInt((bool*)0));
                        }
                        out->m_map18[tab].push_back(element);
                    }
                    break;
                }
            }
            /* fall through */
            case 0x7f:                           // [section]
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[section]", 0x7f));
                }
                else
                {
                    ScanStr(&tok3);
                    int tab = convertTabTypeStringToEnum(tok3.c_str());
                    for (;;)
                    {
                        stTitleSection section;
                        section.m_category = tab;

                        // 统计同 tab 已有章节数作为段内索引（asm +0x622..+0x668）。
                        int sameCount = 0;
                        for (std::vector<stTitleSection>::size_type k = 0;
                             k < out->m_vecC.size(); ++k)
                        {
                            if (out->m_vecC[k].m_category == tab)
                                ++sameCount;
                        }

                        // ORIG +0x5f5 是 call/test al,al/je（无 keepGoing 中间变量），
                        // 且 then 分支内联、break 落到循环之后 → if(...){段体} else break。
                        if (importTitleSection(&section, sameCount))
                        {
                            // 收缩区间到首个空槽（asm +0x69d..+0x70b）：
                            // endIdx(+0x10) > i 且 coll[i].m_index==-1 时 endIdx=i。
                            std::vector<stTitleElement>* collection =
                                getTitleCollection(out, tab);
                            if (collection != 0)
                            {
                                for (int i = section.m_startIdx;
                                     section.m_endIdx > i; ++i)
                                {
                                    // ORIG 两个条件各自 setg/sete + test al,al（嵌套 if），非 &&。
                                    if ((int)collection->size() > i)
                                    {
                                        if ((*collection)[i].m_index == -1)
                                        {
                                            section.m_endIdx = i;
                                        }
                                    }
                                }
                            }
                            out->m_vecC.push_back(section);
                        }
                        else
                        {
                            break;
                        }
                    }
                    break;
                }
            }
            /* fall through */
            case 0xc6:                           // [custom condition message]
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[custom condition message]", 0xc6));
                }
                else
                {
                    stCustomConditionMsg entry;
                    out->m_vec30.clear();
                    for (;;)
                    {
                        // ORIG +0x6cd：条件在循环顶部（call ScanStr; xor eax,1; test; jne）。
                        if (!ScanStr(&tok3))
                            break;
                        int tab = convertTabTypeStringToEnum(tok3.c_str());
                        entry.m_tabType = tab;
                        entry.m_value = ScanInt((bool*)0);
                        ScanStr(&entry.m_msg);
                        out->m_vec30.push_back(entry);
                    }
                    break;
                }
            }
            /* fall through */
            case 0xe4:                           // [adjust list]
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[adjust list]", 0xe4));
                }
                else
                {
                    stAdjust entry;
                    for (;;)
                    {
                        // ORIG +0x8c2：顶部取值后判 ok；循环体内外均无 ok = false。
                        entry.m_questIdx = ScanInt(&ok);
                        if (!ok)
                            break;
                        entry.m_field4 = ScanInt((bool*)0);
                        entry.m_achievementIdx = ScanInt((bool*)0);
                        out->m_vec3c.push_back(entry);
                    }
                    break;
                }
            }
            /* fall through */
            case 0xf4:                           // [view titlebook eventmark]
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[view titlebook eventmark]", 0xf4));
                }
                else
                {
                    int value = 0;
                    // ORIG +0x98e：xor eax,1 / test / jne → if (!ScanInt(...)) break。
                    if (!ScanInt(&value))        // ORIG 使用 ScanInt(int*) 重载
                        break;
                    out->m_viewEventMark = (value != 0);
                    break;
                }
            }
            /* fall through */
            default:
                break;
            }

            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
                goto titlebook_retry;            // 以当前令牌重新派发（ORIG `jmp +0x82`）
            }
            // [2026-09-10 D06] ORIG 0x08aa8ae3 `jmp 08aa8171 <+0x61>`：已初始化路径
            // **一跳直落外层循环头**；写 `break`（跳出内层）会多出一条内层出口 jmp
            // （4.4 把内层出口与外层回边分成两条，实测 ours 643 vs orig 642 = +1）。
            goto outer_retry;
        }
outer_done:;

    return true;
}

#include "InitScriptProducers_3/importSlangName.inc"


#include "InitScriptProducers_3/importItmeGroupTable.inc"


#include "InitScriptProducers_3/stCustomConditionMsg.inc"

