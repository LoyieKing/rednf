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
    // [V331b14 2026-09-12 · SEMANTIC-DIFF 修复] ORIG 实为**短路与**单 return：
    //   Ghidra C `if ((A!=1) && (B!=slot)) return 0x11;`（A=vtable+0x14、B=vtable+0xc）；
    //   汇编 0x085003d9 `xor $1,%eax;test %al,%al;je +0x48` 与 0x085003f2
    //   `cmp 0x10(%ebp),%eax;setne %al;test %al,%al;je +0x48` 两 je **同落 +0x48（主流程）**
    //   ⇒ A 为真时不评 B；A 为假且 B==slot 时也不返回。
    //   旧写两个独立 if（`if(!A()) return 0x11; if(B()!=slot) return 0x11;`）在两处多返回：
    //   ①A 真 & B!=slot ②A 假 & B==slot ⇒ 已按 ORIG 合并为短路与。
    // [R6a10 2026-09-12 P360 形态对齐] ORIG 0x085003d9 与 0x085003f2 两条 je **同落 +0x48**
    // （短路与语义不变），且第二条件独立物化 `cmp 0x10(%ebp),%eax; setne %al; test %al,%al; je`。
    // 写成 `&&` 链时 4.4 恒折成链尾 mov $1/mov $0 岛（无 setne）；拆成嵌套 if 即逐条件复现 ORIG。
    if (!item->IsCreatureItem())
    {
        if (item->GetItemType() != slot)
            return 0x11;
    }
    // ORIG 8500406/8500438：getFootControlRateLimit 两处各自调用（P48 去缓存）
    if (item->getFootControlRateLimit() >= 0) {
        // [SemA06 试形 C] 去具名 ratio 局部（ORIG 侧该槽为 4.1 的表达式临时）
        // [SemA06 2026-09-12 · 常量除法还原] ORIG 08500423..0850042f 为
        //   `mov $0xd1b71759,%edx; mov ratio,%eax; mul %edx; mov %edx,%ebx; shr $0xd,%ebx`
        //   = 32 位无符号常量除法的 GCC 魔数形；0xd1b71759 = ceil(2^45 / 10000)
        //   ⇒ 原码是 `ratio / 10000`。旧写法 `(ull)ratio * 0xd1b71759ULL >> 45`
        //   数学等价，但 4.4 对 64 位常量乘法发 12 条
        //   （imul/imul/add/mov $imm/mul/add/xor/shr…），比 ORIG 多 10 条 = 本件 gap 全部来源。
        //   改回 32 位常量除法后 4.4 发与 ORIG 同形的 mul+shr 序列。
        // [SemA06 试形 A] 去具名 scaled 局部，比值内联进比较（ORIG 侧 scaled 无槽、值留 %ebx）
        if (user->getCurCharacHelpAbuseComputedRatio() / 10000u >
            (unsigned int)item->getFootControlRateLimit())
            return 0x11;
    }
    // [SemA06 试形 B] 去具名 type 局部（ORIG 侧 GetItemType 结果直入 %ebx 作实参、无槽）
    int over = user->GetPremiumInfo()->GetOverEquipableLevel(
        (ENUM_EQUIPMENTTYPE)item->GetItemType());
    int diff = user->GetCurCharacMaxEquipLevel() - user->get_charac_level();
    over = std::max(over, diff);        // ORIG 808e218 std::max<int>(const&, const&)
    if (!item->check_job_type((char)user->get_charac_job())) return 0x11;
    int level = user->get_charac_level() + over;
    // [R11a20 2026-09-12 · SEMANTIC-DIFF 修复] ORIG 0x0850050f..0x08500555 是**单条 `||` 短路**：
    //   0x08500514 `xor $1,%eax` / 0x08500519 `test %al,%al; jne 0x8500547`（L_true = `mov $1,%eax`）
    //   ⇒ `!check_low_level(level)` 为真**直接**进错误出口；否则再评内层 `&&`
    //   （0x08500529 `jne 0x850054e`(L_false=0)、0x08500545 `jge 0x850054e`）落 L_true；
    //   两路共享 0x08500553 `test %al,%al; je 0x850057c`(正常段)。
    //   旧写「嵌套 if」= `&&`：!f() 为真时仍要求内层为真才返回 ⇒ 漏早退（真语义差异）。
    // [AuditFix 2026-09-12] ORIG 0x8500531..0x8500545：先求 GetUsableMaxLevel()
    // 后求 get_charac_level()（`cmp %eax,%ebx; jge`）⇒ 原码是
    // `GetUsableMaxLevel() < get_charac_level()`；镜像写法使两侧求值序互换（+1 insn）。
    if (!item->check_low_level(level) ||
        (item->GetAttachType() == 5 && ((const CEquipItem*)item)->GetUsableMaxLevel() < user->get_charac_level()))
        return G_GameWorld()->IsCharacterLevelRevisionChannel() ? 7 : 0xe;
    // ORIG 8500582/850059b/85005f7：get_need_skill 三处各自调用（P48 去缓存）
    if (item->get_need_skill() != -1) {
        CSkill* s = G_CDataManager()->find_skill(user->get_charac_job(), item->get_need_skill());
        if (!s) return 0x11;
        if (((const SkillSlot*)user->getCurCharacSkillR())->get_skillslot_no(item->get_need_skill(), s->get_group(), (ENUM_SKILL_TREE_KIND)user->GetCurCharacSkillTreeIndex(), true) < 0) return 0x11;
    }
    if (!item->isExpertJobUsable((ENUM_EXPERT_JOB_TYPE)user->GetCurCharacExpertJobType(), user->GetCurExpertJobLevel(user->GetCurCharacExpertJobExp()))) return 0x11;
    return 0;
}