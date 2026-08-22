#include "CUser.h"
#include "CItem.h"
#include "CEquipItem.h"
#include "CSkill.h"
#include "CDataManager.h"
#include "GameWorld.h"
#include "SkillSlot.h"
#include "InterfacePacketBuf.h"

// ORIG _ZL21_S_CHARAC_JOB_MASTARY（file-local static）：checkEquipmentState 按
// (a*5+b)（a<9,b<5 → 最大 44）与 a*0x14（a<9 → 最大 160）索引，需 ≥161 项；
// 此处为全局定义（CInventory.h extern int[]），尺寸取 0xC0。
int _S_CHARAC_JOB_MASTARY[0xC0] = {
    3,2,0,3,4, 2,0,2,3,2, 1,1,3,0,1,
    0,0,0,1,1, 3,4,2,0,3, 1,1,3,0,1,
    1,1,2,1,1, 2,0,2,3,2, 0,0,1,0,0,
    0,0,0,0,0, 0,0,0,0,0,0,0
};

int IsEquipable(CUser* user, const CItem* item, int slot)
{
    if (!item->getUsableItemType(slot) && item->GetItemType() != slot) return 0x11;
    int foot = item->getFootControlRateLimit();
    if (foot >= 0) {
        unsigned int ratio = user->getCurCharacHelpAbuseComputedRatio();
        unsigned int scaled = (unsigned int)(((unsigned long long)ratio * 0xd1b71759ULL) >> 45);
        if (scaled > (unsigned int)foot) return 0x11;
    }
    int type = item->GetItemType();
    int over = user->GetPremiumInfo()->GetOverEquipableLevel((ENUM_EQUIPMENTTYPE)type);
    int diff = user->GetCurCharacMaxEquipLevel() - user->get_charac_level();
    if (diff > over) over = diff;
    if (!item->check_job_type((char)user->get_charac_job())) return 0x11;
    int level = user->get_charac_level() + over;
    if (!item->check_low_level(level)) {
        if (item->GetAttachType() == 5 && user->get_charac_level() > ((const CEquipItem*)item)->GetUsableMaxLevel())
            return G_GameWorld()->IsCharacterLevelRevisionChannel() ? 7 : 0xe;
    }
    int need = item->get_need_skill();
    if (need != -1) {
        CSkill* s = G_CDataManager()->find_skill(user->get_charac_job(), need);
        if (!s) return 0x11;
        if (((const SkillSlot*)user->getCurCharacSkillR())->get_skillslot_no(need, s->get_group(), (ENUM_SKILL_TREE_KIND)user->GetCurCharacSkillTreeIndex(), true) < 0) return 0x11;
    }
    if (!item->isExpertJobUsable((ENUM_EXPERT_JOB_TYPE)user->GetCurCharacExpertJobType(), user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp()))) return 0x11;
    return 0;
}