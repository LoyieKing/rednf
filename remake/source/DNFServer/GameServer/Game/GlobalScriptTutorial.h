#ifndef DNF_GLOBAL_SCRIPT_TUTORIAL_H
#define DNF_GLOBAL_SCRIPT_TUTORIAL_H

#include "TutorialData.h"

#ifndef DNF_ENUM_CHARACTERJOB_DEFINED
#define DNF_ENUM_CHARACTERJOB_DEFINED
enum ENUM_CHARACTERJOB
{
    ENUM_CHARACTERJOB_0 = 0
};
#endif

class GlobalScriptTutorial
{
public:
    GlobalScriptTutorial();
    ~GlobalScriptTutorial();

    bool loadScript();
    bool loadScript(const char* directory, const char* path);

    void LoadDialogTag(ENUM_CHARACTERJOB&);
    void LoadAvatarTag(ENUM_CHARACTERJOB&);
    void LoadEquipmentTag(ENUM_CHARACTERJOB&);
    void LoadDungeonIndex(ENUM_CHARACTERJOB&);
    void LoadSkillTag(ENUM_CHARACTERJOB&);
    void LoadPassiveSkilltag(ENUM_CHARACTERJOB&);
    void LoadTutorialNotice(ENUM_CHARACTERJOB&);
    void LoadItem(ENUM_CHARACTERJOB&);

    void clearScript();
    TutorialData* getData(int);
    static bool IsGlobalTutorialDungeon(unsigned int);   // ORIG T 0x08ab976a：体不读 this，static 成员（dungeon==0xdb2）

private:
    TutorialData m_data[11];
};

#endif
