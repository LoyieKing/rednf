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
STEventCharacterInfo::STEventCharacterInfo() : m_growType(0), m_secondGrowType(0)
{
    m_skillList.clear();
    m_itemList.clear();
}
// ORIG 083ce3ee：先析构 m_itemList(+0x10) 再 m_skillList(+0x04)（成员声明逆序，编译器自动生成）
STEventCharacterInfo::~STEventCharacterInfo() {}

// ORIG 0x895b890：character_info map 默认构造后调 clear()。
STEventCharacterInfo_level::STEventCharacterInfo_level()
{
    character_info.clear();
}

// ORIG C1 0x8371fde：map 构造 + clear()。
EventCharacterParameterScript::EventCharacterParameterScript() : m_levels()
{
    clear();
}

EventCharacterParameterScript::~EventCharacterParameterScript() {}

// ORIG clear W 0x8371fb6：m_levels.clear()。
void EventCharacterParameterScript::clear()
{
    m_levels.clear();
}

bool EventCharacterParameterScript::importEventCharacterParameterScript(const char* file_name)
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

bool EventCharacterParameterScript::load_event_character_list()
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

bool EventCharacterParameterScript::GetEventCharacterInfo(int job, int level,
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
