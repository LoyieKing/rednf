#ifndef DNF_GAME_CPREMIUM_LETHE_MANAGER_H_
#define DNF_GAME_CPREMIUM_LETHE_MANAGER_H_

#include "SkillSlot.h"

class CUser;

class CPremiumLetheManager
{
public:
    CPremiumLetheManager();
    ~CPremiumLetheManager();

    bool ResetSkillReq(CUser* user);
    bool ConfirmSkillReq(CUser* user);
    bool BackToPre(CUser* user, _Mastered_skill* skills,
                   ENUM_SKILL_TREE_KIND kind);
    bool UpdateBackupSkillFlag(CUser* user, ENUM_SKILL_TREE_KIND kind);
    bool InitLetheSkill(CUser* user, ENUM_SKILL_TREE_KIND kind);
    void _resetSkill(CUser* user);
    void InitSkill(CUser* user, bool flag, ENUM_SKILL_TREE_KIND kind);
    bool BackupSkill(CUser* user, ENUM_SKILL_TREE_KIND kind);
    void _update(CUser* user, unsigned char state, ENUM_SKILL_TREE_KIND kind);
    void _removeInitSkill(CUser* user, _Mastered_skill* skills);

    static unsigned char USE_LETHE_STATE;
    static unsigned char SKILL_CONFIRM_STATE;
    static unsigned char BACK_TO_PRE_STATE;
    static unsigned char INIT_LETHE_SKILL;
    static unsigned char NOT_USE_LETHE_STATE;
};

#endif
