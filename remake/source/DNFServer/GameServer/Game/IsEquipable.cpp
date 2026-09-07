#include "CUser.h"
#include "CItem.h"
#include "CEquipItem.h"
#include "CSkill.h"
#include "CDataManager.h"
#include "GameWorld.h"
#include "SkillSlot.h"
#include "InterfacePacketBuf.h"
#include <algorithm>   // std::max<int>(int const&, int const&) ORIG 808e218

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
    // ORIG 0x85003be：虚表槽5 IsCreatureItem()（无参）判定，非 getUsableItemType(slot)。
    // ORIG 两个独立 if 各自提前返回 0x11。
    if (!item->IsCreatureItem())
        return 0x11;
    if (item->GetItemType() != slot)
        return 0x11;
    // ORIG 8500406/8500438：getFootControlRateLimit 两处各自调用（P48 去缓存）
    if (item->getFootControlRateLimit() >= 0) {
        unsigned int ratio = user->getCurCharacHelpAbuseComputedRatio();
        unsigned int scaled = (unsigned int)(((unsigned long long)ratio * 0xd1b71759ULL) >> 45);
        if (scaled > (unsigned int)item->getFootControlRateLimit()) return 0x11;
    }
    int type = item->GetItemType();
    int over = user->GetPremiumInfo()->GetOverEquipableLevel((ENUM_EQUIPMENTTYPE)type);
    int diff = user->GetCurCharacMaxEquipLevel() - user->get_charac_level();
    over = std::max(over, diff);        // ORIG 808e218 std::max<int>(const&, const&)
    if (!item->check_job_type((char)user->get_charac_job())) return 0x11;
    int level = user->get_charac_level() + over;
    if (!item->check_low_level(level)) {
        if (item->GetAttachType() == 5 && user->get_charac_level() > ((const CEquipItem*)item)->GetUsableMaxLevel())
            return G_GameWorld()->IsCharacterLevelRevisionChannel() ? 7 : 0xe;
    }
    // ORIG 8500582/850059b/85005f7：get_need_skill 三处各自调用（P48 去缓存）
    if (item->get_need_skill() != -1) {
        CSkill* s = G_CDataManager()->find_skill(user->get_charac_job(), item->get_need_skill());
        if (!s) return 0x11;
        if (((const SkillSlot*)user->getCurCharacSkillR())->get_skillslot_no(item->get_need_skill(), s->get_group(), (ENUM_SKILL_TREE_KIND)user->GetCurCharacSkillTreeIndex(), true) < 0) return 0x11;
    }
    if (!item->isExpertJobUsable((ENUM_EXPERT_JOB_TYPE)user->GetCurCharacExpertJobType(), user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp()))) return 0x11;
    return 0;
}