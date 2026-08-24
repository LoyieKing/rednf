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
    // ORIG: _ZN20STEventCharacterInfoC1Ev @ 0848ecce / _ZN20STEventCharacterInfoD1Ev @ 083ce3ee
    // 非内联定义于 EventCharacterParameterScript.cpp，产生真实 mangled 符号（替代 GameStubs asm 桥接）。
    STEventCharacterInfo();
    ~STEventCharacterInfo();
};

struct STEventCharacterInfo_level
{
    std::map<int, STEventCharacterInfo> character_info;

    // ORIG _ZN26STEventCharacterInfo_levelC1Ev W 0x895b890（map 默认构造）
    STEventCharacterInfo_level();
};

class EventCharacterParameterScript {
public:
    typedef std::map<int, std::string> ScriptFileList;
    EventCharacterParameterScript();  // ORIG _ZN29EventCharacterParameterScriptC1Ev W 0x8371fde
    ~EventCharacterParameterScript();
    void clear();                     // ORIG _ZN29EventCharacterParameterScript5clearEv W 0x8371fb6
    bool importEventCharacterParameterScript(const char* file_name);
    bool load_event_character_list();
    bool GetEventCharacterInfo(int job, int level, STEventCharacterInfo& out) const;
    static ScriptFileList& script_file_list();
    static std::string& base_directory();
private:
    std::map<int, STEventCharacterInfo_level> m_levels;
};

#endif
