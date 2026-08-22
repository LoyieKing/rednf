#include "EventCharacterParameterScript.h"
#include <fstream>
#include <cstdlib>

namespace {
EventCharacterParameterScript::ScriptFileList g_files;
std::string g_base_directory;

bool integer(const std::string& s, int& value)
{
    char* end = 0;
    long v = ::strtol(s.c_str(), &end, 10);
    if (!end || *end != '\0') return false;
    value = static_cast<int>(v);
    return true;
}

bool next_int(std::istream& in, int& value)
{
    std::string token;
    return static_cast<bool>(in >> token) && integer(token, value);
}
}

// ORIG 0848ecce：m_growType/m_secondGrowType 清零 + 两个 vector 默认构造
// ORIG 083ce3ee：先析构 m_itemList(+0x10) 再 m_skillList(+0x04)
STEventCharacterInfo::STEventCharacterInfo() : m_growType(0), m_secondGrowType(0) {}
STEventCharacterInfo::~STEventCharacterInfo() {}

EventCharacterParameterScript::EventCharacterParameterScript() {}
EventCharacterParameterScript::~EventCharacterParameterScript() {}
EventCharacterParameterScript::ScriptFileList& EventCharacterParameterScript::script_file_list() { return g_files; }
std::string& EventCharacterParameterScript::base_directory() { return g_base_directory; }

bool EventCharacterParameterScript::importEventCharacterParameterScript(const char* file_name)
{
    if (!file_name) return false;
    std::string path = g_base_directory;
    if (!path.empty() && path[path.size() - 1] != '/') path += '/';
    path += file_name;
    std::ifstream in(path.c_str());
    if (!in) return false;

    int level = 0, job = 0;
    bool have_level = false, have_job = false;
    STEventCharacterInfo current;
    std::string token, pending;
    while (true) {
        if (!pending.empty()) { token = pending; pending.clear(); }
        else if (!(in >> token)) break;
        if (token == "[level]") {
            if (!next_int(in, level)) return false;
            have_level = true;
        } else if (token == "[job const]") {
            if (!have_level || !next_int(in, job)) return false;
            current = STEventCharacterInfo(); have_job = true;
        } else if (token == "[grow type]") {
            if (!have_job || !next_int(in, reinterpret_cast<int&>(current.m_growType))) return false;
        } else if (token == "[awakening type]") {
            if (!have_job || !next_int(in, reinterpret_cast<int&>(current.m_secondGrowType))) return false;
        } else if (token == "[skill]") {
            while (in >> token && token[0] != '[') {
                int skill = 0, rank = 0;
                if (!integer(token, skill) || !next_int(in, rank)) return false;
                current.m_skillList.push_back(std::make_pair(skill, rank));
            }
            if (in && !token.empty() && token[0] == '[') pending = token;
        } else if (token == "[item]") {
            while (in >> token && token[0] != '[') {
                STEventCharacterItem item;
                int itemIdx = 0;
                if (!integer(token, itemIdx) || !next_int(in, item.m_count) ||
                    !next_int(in, item.m_reason) || item.m_reason < 0 || item.m_reason > 31) return false;
                item.m_itemIdx = (unsigned long)itemIdx;
                current.m_itemList.push_back(item);
            }
            if (in && !token.empty() && token[0] == '[') pending = token;
        } else if (token == "[/job const]") {
            if (!have_job) return false;
            if (!m_levels[level].character_info.insert(std::make_pair(job, current)).second) return false;
            have_job = false;
        } else if (token == "[/level]") {
            if (have_job || !have_level) return false;
            have_level = false;
        } else return false;
    }
    return !in.bad() && !have_job;
}

bool EventCharacterParameterScript::load_event_character_list()
{
    for (ScriptFileList::const_iterator it = g_files.begin(); it != g_files.end(); ++it)
        if (!importEventCharacterParameterScript(it->second.c_str())) return false;
    return true;
}

bool EventCharacterParameterScript::GetEventCharacterInfo(int job, int level,
                                                            STEventCharacterInfo& out) const
{
    if (level <= 0 || level >= 0x47) return false;
    std::map<int, STEventCharacterInfo_level>::const_iterator level_it = m_levels.find(level);
    if (level_it == m_levels.end()) return false;
    std::map<int, STEventCharacterInfo>::const_iterator job_it = level_it->second.character_info.find(job);
    if (job_it == level_it->second.character_info.end()) return false;
    out = job_it->second;
    return true;
}
