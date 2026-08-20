#include "CUser.h"
#include "CItem.h"
#include "CDataManager.h"
#include "GameWorld.h"
#include "SkillSlot.h"
#include "InterfacePacketBuf.h"

int _S_CHARAC_JOB_MASTARY[57] = {
    3,2,0,3,4, 2,0,2,3,2, 1,1,3,0,1,
    0,0,0,1,1, 3,4,2,0,3, 1,1,3,0,1,
    1,1,2,1,1, 2,0,2,3,2, 0,0,1,0,0,
    0,0,0,0,0, 0,0,0,0,0,0,0
};

extern "C" int item_foot(const CItem* p) asm("_ZNK5CItem23getFootControlRateLimitEv");
extern "C" int equip_usable_max(const void* p) asm("_ZNK10CEquipItem17GetUsableMaxLevelEv");
extern "C" int item_need_skill(const CItem* p) asm("_ZNK5CItem14get_need_skillEv");
extern "C" bool item_low(const CItem* p, int n) asm("_ZNK5CItem15check_low_levelEi");
extern "C" bool item_expert(const CItem* p, int t, int n) asm("_ZNK5CItem17isExpertJobUsableE20ENUM_EXPERT_JOB_TYPEi");
extern "C" void* user_premium(const CUser* p) asm("_ZNK5CUser14GetPremiumInfoEv");
extern "C" int premium_over(const void* p, int t) asm("_ZNK8WongWork12CUserPremium21GetOverEquipableLevelE18ENUM_EQUIPMENTTYPE");
extern "C" int user_max(const CUser* p) asm("_ZNK15CUserCharacInfo25GetCurCharacMaxEquipLevelEv");
extern "C" int user_skill_tree(const CUser* p) asm("_ZNK15CUserCharacInfo26GetCurCharacSkillTreeIndexEv");
extern "C" int user_expert_type(const CUser* p) asm("_ZNK15CUserCharacInfo25GetCurCharacExpertJobTypeEv");
extern "C" int user_expert_level(const CUser* p, int exp) asm("_ZN5CUser20GetCurExpertJobLevelEi");
extern "C" int user_expert_exp(const CUser* p) asm("_ZNK15CUserCharacInfo24GetCurCharacExpertJobExpEv");
extern "C" SkillSlot* user_skill(const CUser* p) asm("_ZNK15CUserCharacInfo18getCurCharacSkillREv");
extern "C" int skill_group(const void* p) asm("_ZNK6CSkill9get_groupEv");
extern "C" int skill_slot(const SkillSlot* p,int a,int b,int c,bool d) asm("_ZNK9SkillSlot16get_skillslot_noEii20ENUM_SKILL_TREE_KINDb");
extern "C" bool world_revision(const GameWorld* p) asm("_ZNK9GameWorld31IsCharacterLevelRevisionChannelEv");
extern GameWorld* G_GameWorld();

int IsEquipable(CUser* user, const CItem* item, int slot)
{
    if (!item->getUsableItemType(slot) && item->GetItemType() != slot) return 0x11;
    int foot = item_foot(item);
    if (foot >= 0) {
        unsigned int ratio = user->getCurCharacHelpAbuseComputedRatio();
        unsigned int scaled = (unsigned int)(((unsigned long long)ratio * 0xd1b71759ULL) >> 45);
        if (scaled > (unsigned int)foot) return 0x11;
    }
    int type = item->GetItemType();
    int over = premium_over(user_premium(user), type);
    int diff = user_max(user) - user->get_charac_level();
    if (diff > over) over = diff;
    if (!item->check_job_type((char)user->get_charac_job())) return 0x11;
    int level = user->get_charac_level() + over;
    if (!item_low(item, level)) {
        if (item->GetAttachType() == 5 && user->get_charac_level() > equip_usable_max(item))
            return world_revision(G_GameWorld()) ? 7 : 0xe;
    }
    int need = item_need_skill(item);
    if (need != -1) {
        CSkill* s = G_CDataManager()->find_skill(user->get_charac_job(), need);
        if (!s) return 0x11;
        if (skill_slot(user_skill(user), need, skill_group(s), user_skill_tree(user), true) < 0) return 0x11;
    }
    if (!item_expert(item, user_expert_type(user), user_expert_level(user, user_expert_exp(user)))) return 0x11;
    return 0;
}
