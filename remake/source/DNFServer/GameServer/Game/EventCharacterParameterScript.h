#ifndef GAME_EVENTCHARACTERPARAMETERSCRIPT_H_
#define GAME_EVENTCHARACTERPARAMETERSCRIPT_H_

#include <map>
#include <string>
#include <utility>
#include <vector>

struct STEventCharacterItem
{
    unsigned long m_itemIdx;
    int m_count;
    int m_reason;
    STEventCharacterItem() : m_itemIdx(0), m_count(0), m_reason(0) {}
};

struct STEventCharacterInfo
{
    unsigned char m_growType;
    unsigned char m_secondGrowType;
    char m_pad2[2];
    std::vector<std::pair<int, int> > m_skillList;
    std::vector<STEventCharacterItem> m_itemList;
    STEventCharacterInfo() : m_growType(0), m_secondGrowType(0) {}
};

struct STEventCharacterInfo_level {
    std::map<int, STEventCharacterInfo> character_info;
};

class EventCharacterParameterScript {
public:
    typedef std::map<int, std::string> ScriptFileList;
    EventCharacterParameterScript();
    ~EventCharacterParameterScript();
    bool importEventCharacterParameterScript(const char* file_name);
    bool load_event_character_list();
    bool GetEventCharacterInfo(int job, int level, STEventCharacterInfo& out) const;
    static ScriptFileList& script_file_list();
    static std::string& base_directory();
private:
    std::map<int, STEventCharacterInfo_level> m_levels;
};

#endif
