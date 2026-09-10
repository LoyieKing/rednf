// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 EventCharacterParameterScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "EventCharacterParameterScript.h"
#include "DNFLexWrapper.h"   // loadRDARScriptFile / ScanType / ScanInt
#include "LogManager.h"
#include "NpcScript.h"       // STScriptFileList（g_eventCharacterScriptFileList 权威类型）

// ORIG 全局（定义于 InitScriptProducers_5.cpp；本 TU 仅 extern 引用）：
//   g_eventCharacterScriptBaseDirectory  B 0x094f73c0 (char[0x100])
//   g_eventCharacterScriptFileList      B 0x094f74c0 (STScriptFileList, 0x18)
extern char g_eventCharacterScriptBaseDirectory[0x100];
extern STScriptFileList g_eventCharacterScriptFileList;

// ORIG 0848ecce：m_growType/m_secondGrowType 清零 + 两个 vector 构造后各自 clear()
#include "EventCharacterParameterScript/STEventCharacterInfo.inc"
{
    m_skillList.clear();
    m_itemList.clear();
}
// ORIG 083ce3ee：先析构 m_itemList(+0x10) 再 m_skillList(+0x04)（成员声明逆序，编译器自动生成）
#include "EventCharacterParameterScript/dtor_STEventCharacterInfo.inc"

// ORIG 0x895b890：character_info map 默认构造后调 clear()。
#include "EventCharacterParameterScript/STEventCharacterInfo_level.inc"
{
    character_info.clear();
}

// ORIG C1 0x8371fde：map 构造 + clear()。
#include "EventCharacterParameterScript/EventCharacterParameterScript.inc"
{
    clear();
}

#include "EventCharacterParameterScript/dtor_EventCharacterParameterScript.inc"

// ORIG clear W 0x8371fb6：m_levels.clear()。
#include "EventCharacterParameterScript/clear.inc"
{
    m_levels.clear();
}

// b28d-D42 verdict=DIFF [N82/N89] ORIG 0x0895b006 orig/ours=423/421 ndiff=376。
// 残差为巨件帧/槽号平移（局部表/缓冲）与 std::map/string 内联差（N89）；
// P167 调用点已核，语义按报告逐条对齐。
#include "EventCharacterParameterScript/importEventCharacterParameterScript.inc"
{
    if (!loadRDARScriptFile(g_eventCharacterScriptBaseDirectory, file_name))
        return false;

    std::string token;
    int job = 0;
    int level = 0;
    STEventCharacterInfo_level level_info;
    bool ok;

    if (!ScanType(token, true)) return false;
    if (token == "[level]") {
        level = ScanInt(&ok);
        if (!ok) {
            LogManager::logFormat(0, "../RDARScriptEventCharacterParameter.cpp",
                __FUNCTION__, 0x5f, "file name(%s) job name [%d]", file_name, job);
            return false;
        }
    }
    for (;;) {
        STEventCharacterInfo current;
        for (;;) {
            if (!ScanType(token, true)) break;
            if (token == "[job const]") {
                job = ScanInt(&ok);
                if (ok) continue;
                break;
            }
            if (token == "[grow type]") {
                current.m_growType = (unsigned char)ScanInt(&ok);
                if (ok) continue;
                LogManager::logFormat(0, "../RDARScriptEventCharacterParameter.cpp",
                    __FUNCTION__, 0x79, "file name(%s) job name [%d]", file_name, job);
                return false;
            }
            if (token == "[awakening type]") {
                current.m_secondGrowType = (unsigned char)ScanInt(&ok);
                if (ok) continue;
                LogManager::logFormat(0, "../RDARScriptEventCharacterParameter.cpp",
                    __FUNCTION__, 0x82, "file name(%s) job name [%d]", file_name, job);
                return false;
            }
            if (token == "[skill]") {
                while (true) {
                    std::pair<int, int> p;
                    p.first = ScanInt(&ok);
                    if (!ok) break;
                    p.second = ScanInt(&ok);
                    if (!ok) break;
                    current.m_skillList.push_back(p);
                }
                continue;
            }
            if (token == "[item]") {
                while (true) {
                    STEventCharacterItem item;
                    item.m_itemIdx = ScanInt(&ok);
                    if (!ok) break;
                    item.m_count = ScanInt(&ok);
                    if (!ok) break;
                    item.m_reason = ScanInt(&ok);
                    if (item.m_reason < 0 || item.m_reason > 31) {
                        LogManager::logFormat(0, "../RDARScriptEventCharacterParameter.cpp",
                            __FUNCTION__, 0xa4,
                            "file name(%s) upgrade script error [%d] upgrade < 0 || upgrade > 31",
                            file_name, item.m_reason);
                        return false;
                    }
                    if (!ok) break;
                    current.m_itemList.push_back(item);
                }
                continue;
            }
            if (token == "[/job const]") {
                if (level_info.character_info.insert(std::make_pair(job, current)).second == 0) {
                    LogManager::logFormat(0, "../RDARScriptEventCharacterParameter.cpp",
                        __FUNCTION__, 0xb3, "file name(%s) job name [%d]", file_name, job);
                    return false;
                }
                break;
            }
            if (token == "[/level]") {
                if (m_levels.insert(std::make_pair(level, level_info)).second == 0) {
                    LogManager::logFormat(0, "../RDARScriptEventCharacterParameter.cpp",
                        __FUNCTION__, 0xbd, "file name(%s) same level exist[%d]", file_name, level);
                    return false;
                }
                return true;
            }
        }
    }
    return false;
}

#include "EventCharacterParameterScript/load_event_character_list.inc"
{
    std::map<int, std::string>::iterator it;
    it = g_eventCharacterScriptFileList.m_fileMap.begin();
    while (it != g_eventCharacterScriptFileList.m_fileMap.end()) {
        const char* name = it->second.c_str();
        if (name == 0) break;
        if (!importEventCharacterParameterScript(name)) {
            LogManager::logFormat(1, "../RDARScriptEventCharacterParameter.cpp",
                __FUNCTION__, 0x38,
                "EventCharacterParameterScript File Script Error: %s\n", name);
            return false;
        }
        ++it;
    }
    return true;
}

#include "EventCharacterParameterScript/GetEventCharacterInfo.inc"
                                                          STEventCharacterInfo& out) const
{
    if (level <= 0 || level >= 0x47) return false;
    std::map<int, STEventCharacterInfo_level>::const_iterator lit = m_levels.find(level);
    if (lit != m_levels.end()) {
        // b28c-36 AE（fresh ndiff 8→0，71=71）：ORIG 0x0895b799/0x0895b7b5 段
        // this(-0xc) 先载 %edx 再 lea key/sret。原 `const map& job_map = ...;
        // job_map.find(job)` 与指针形 `job_map->find` 两形均使 4.4 先算
        // key/sret 后载 this（残差 2 段/4 行）；改 **level 对象引用 +
        // 首成员访问** `lv.character_info.find(job)` 后 4.4 复现 ORIG 的
        // this-first 装载序（探针族 p2/p3 + 两形实测对照）。
        const STEventCharacterInfo_level& lv = (*lit).second;
        std::map<int, STEventCharacterInfo>::const_iterator jit =
            lv.character_info.find(job);
        if (jit != lv.character_info.end()) {
            out = (*jit).second;
            return 1;
        }
    }
    return 0;
}
