#include "CUser.h"
#include "CItem.h"
#include "CEquipItem.h"
#include "CSkill.h"
#include "CDataManager.h"
#include "GameWorld.h"
#include "SkillSlot.h"
#include "InterfacePacketBuf.h"
#include <algorithm>   // std::max<int>(int const&, int const&) ORIG 808e218

// [B01 铁律1] 原 `int _S_CHARAC_JOB_MASTARY[0xC0]`（外部链接 D 768B）为发明符号：
// ORIG `nm -S --defined-only` 无裸名，真形是 489 份 TU-local `_ZL21_S_CHARAC_JOB_MASTARY`
// （r 220B = 55×int，如 0x8ae5b60 / 0x8b126e0 …），成因即 CInventory.h 内 file-scope
// static 被多 TU include。本 TU 的 IsEquipable()（ORIG 0x085003be）全程不引用该表
// （索引点 checkEquipmentState 在 ORIG 同 TU 0x0850cfbe/0x0850cfe6，我方落 CInventory.cpp）
// ⇒ 此处删除；TU-local 副本由 CInventory.h:580 的静态定义在引用者 CInventory.cpp 发射。

// [b28d-30] 复核 DIFF：帧/槽位跨版本差（N82/N93 族），ORIG 0x085003be。
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