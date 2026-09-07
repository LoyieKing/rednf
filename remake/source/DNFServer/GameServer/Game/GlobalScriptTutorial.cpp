#include "GlobalScriptTutorial.h"
#include "DNFLexWrapper.h"

#include <string>
#include <vector>

int getCharacterJob(const char* name);

GlobalScriptTutorial::GlobalScriptTutorial()
    : m_data()
{
    clearScript();
}

GlobalScriptTutorial::~GlobalScriptTutorial()
{
    clearScript();
}

bool GlobalScriptTutorial::loadScript()
{
    if (!loadRDARScriptFile("Etc/GlobalTutorialInfo/", "CharacterList.lst"))
        return false;

    std::vector<std::string> files;
    std::string token;
    while (ScanType(token, true))
    {
        if (token != "[list]")
            continue;
        while (ScanStr(&token))
            files.push_back(token);
        break;
    }
    unloadRDARScriptFile();

    for (std::vector<std::string>::const_iterator it = files.begin();
         it != files.end(); ++it)
    {
        if (!loadScript("Etc/GlobalTutorialInfo/", it->c_str()))
            return false;
    }
    return true;
}

bool GlobalScriptTutorial::loadScript(const char* directory, const char* path)
{
    if (!loadRDARScriptFile(directory, path))
        return false;

    ENUM_CHARACTERJOB job = static_cast<ENUM_CHARACTERJOB>(11);
    std::string token;
    while (ScanType(token, true))
    {
        if (token == "[dialog]")
            LoadDialogTag(job);
        else if (token == "[avatar]")
            LoadAvatarTag(job);
        else if (token == "[equipment]")
            LoadEquipmentTag(job);
        else if (token == "[dungeonindex]")
            LoadDungeonIndex(job);
        else if (token == "[skill]")
            LoadSkillTag(job);
        else if (token == "[passive skill]")
            LoadPassiveSkilltag(job);
        else if (token == "[tutorial notice]")
            LoadTutorialNotice(job);
        else if (token == "[item]")
            LoadItem(job);
        else
            job = static_cast<ENUM_CHARACTERJOB>(getCharacterJob(token.c_str()));
    }
    unloadRDARScriptFile();
    return true;
}

void GlobalScriptTutorial::LoadDialogTag(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int key, face;
    std::string text;
    while (ScanInt(&key) && ScanInt(&face) && ScanStr(&text))
    {
        m_data[static_cast<int>(job)].addDialogData(key, face, text.c_str());
    }
}

void GlobalScriptTutorial::LoadAvatarTag(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int type, index;
    while (ScanInt(&type) && ScanInt(&index))
    {
        m_data[static_cast<int>(job)].addAvatarIndex(type, index);
    }
}

void GlobalScriptTutorial::LoadEquipmentTag(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int type, index, slot;
    while (ScanInt(&type) && ScanInt(&index) && ScanInt(&slot))
    {
        m_data[static_cast<int>(job)].addEquipmentIndex(type, index, slot);
    }
}

void GlobalScriptTutorial::LoadDungeonIndex(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    {
    int index;
        for (;;)
{
            if (!ScanInt(&index))
                goto done;
            m_data[static_cast<int>(job)].setDungeonIndex(
                static_cast<unsigned int>(index));
    }
    }
done:
    ;
}

void GlobalScriptTutorial::LoadSkillTag(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int slot, hotkey, skill, field4, field8;
    while (ScanInt(&slot) && ScanInt(&hotkey) && ScanInt(&skill) &&
           ScanInt(&field4) && ScanInt(&field8))
    {
        m_data[static_cast<int>(job)].addSkillIndex(slot, hotkey, skill, field4, field8);
    }
}

void GlobalScriptTutorial::LoadPassiveSkilltag(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int slot, skill, field4;
    while (ScanInt(&slot) && ScanInt(&skill) && ScanInt(&field4))
    {
        m_data[static_cast<int>(job)].addPassiveSkillData(slot, skill, field4);
    }
}

void GlobalScriptTutorial::LoadTutorialNotice(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int key, field8, fieldc;
    std::string path;
    while (ScanInt(&key) && ScanInt(&field8) && ScanInt(&fieldc) && ScanStr(&path))
    {
        m_data[static_cast<int>(job)].addTutorialNotice(key, field8, fieldc, path.c_str());
    }
}

void GlobalScriptTutorial::LoadItem(ENUM_CHARACTERJOB& job)
{
    if (static_cast<int>(job) == 11) return;
    int item, count;
    while (ScanInt(&item) && ScanInt(&count))
    {
        m_data[static_cast<int>(job)].addItem(item, count);
    }
}

void GlobalScriptTutorial::clearScript()
{
}


bool GlobalScriptTutorial::IsGlobalTutorialDungeon(
    register unsigned int dungeon)
{
    if (dungeon == 0xdb2)
        return true;
    return false;
}
