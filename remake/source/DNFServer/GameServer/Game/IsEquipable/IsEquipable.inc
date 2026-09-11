// [b28d-30] 复核 DIFF：帧/槽位跨版本差（N82/N93 族），ORIG 0x085003be。
int IsEquipable(CUser* user, const CItem* item, int slot)
{
    // [FragC 试形·P63 共享出口 · 负结论] ORIG 所有 `return 0x11` 共享尾部单块
    // （`je <tail>` 集中到 0x85005a2 附近 `mov $0x11,%eax; jmp epilogue`）；
    // 我方 4.4 逐处独立发射（+10 条）。试形 = 7 处 `goto fail` + 尾部
    // `fail: return 0x11;`（主体包内层复合块以避开 P223-BM8「goto 跨同层初始化」
    // 限制）⇒ **反劣**：our 232→**233**、ndiff 17→**24** 块（4.4 的跳转线程化
    // 与 4.1 共享块布局不可同时满足）⇒ 已回滚，维持逐处 `return 0x11;`。
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