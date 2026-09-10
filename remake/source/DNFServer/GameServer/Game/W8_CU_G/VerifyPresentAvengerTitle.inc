bool CUser::VerifyPresentAvengerTitle()
{
    // ORIG T 0x868b552 全录（报告 docs/class_func_reports/CUser/VerifyPresentAvengerTitle.md
    // 汇编+Ghidra C 双段读完；调用序 Counter 48 项对齐重建 2026-09-05 CUserC8）：
    //  accId 先取 → createTime → 临时 string 两段（allocator C1/string C1/dtor）
    //  → Singleton Get → findGameScript → checkCreateTime(0x8e, createTime, accId)
    //  → 三 guard 短路 → getCurSec → flag!=1 才 **第二次独立** Get/findGameScript
    //    +isApplied（orig 各 2 次，勿复用 period）→ Inven_Item C1 → **先写
    //    item.m_random.m_rand4=0x28d3af 再 create_item**（判据读同一 local_90 槽，
    //    原 const 局部为语义误植）→ findString/strncpy 交替对 → ReqDBSend 系
    //    参数序 group→strlen→characNo → makeRequest 系 characNo→acc→GetUID →
    //    trace 段 idxVal→characNo→acc→NumberToString→C1→operator()。
    // 判据⑤（X 族边界补注 CUserC8）：ORIG 0x868b558-5f accId→%ebx、createTime→
    // %esi 双 callee-saved 跨 string/Get/findScript 四 call 存活（push esi/push ebx
    // prologue 0x868b559-560）。b28d-21：按 P0/N96 清除 `asm("reg")` 绑定，
    // 改纯 register 存储类提示（P231/P248，非绑定形）。
    // b28d-W8b 复测：P248 裸 register 位置旋钮无效 —— 仅 createTime 保留 register
    // 时 accId 被槽化（-0x24）+ createTime→%ebx，341/340 反劣已回退；双 register
    // 形 4.4 反配（accId→%esi/createTime→%ebx）⇒ N7 封顶。
    register unsigned int accId = (unsigned int)get_acc_id();
    register unsigned int createTime =
        static_cast<unsigned int>(getCurCharacCreateTime());
    // b28d-W8: P0/P183 —— ORIG 0x868b5ac findGameScript 结果 %eax 直传 checkCreateTime
    // receiver（无 period 槽往返）⇒ 内联表达式，不设具名局部。
    if (static_cast<ARAD::Arad_EventPeriodDataManager*>(
            ARAD::Singleton<ARAD::Arad_DataManager>::Get()->findGameScript(
                std::string("Arad_EventPeriodDataManager")))
            ->checkCreateTime(static_cast<ENUM_REPEAT_EVENT_CODE>(0x8e),
                              createTime, accId) != 1)
    {
        return 0;
    }

    if (get_charac_level() < 0x14 || get_charac_job() != 4 ||
        getCurCharFirstGrowType() != '\0')
    {
        return 0;
    }

    unsigned int curSec = GlobalData::s_systemTime_.getCurSec();

    // b28d-W8: ORIG 0x868b676..0x868b768 —— A 假/B 假同落 L_false（mov $0,%eax），
    // 条件值经 %edi 跨 string+allocator 析构存活后才 test ⇒ 原码是
    // `if (!(A && B)) return 0;`（条件临时无具名槽，P0；eventOn 具名局部是误建模）。
    if (!(!getFlagPresentAvengerTitle() &&
          static_cast<ARAD::Arad_EventPeriodDataManager*>(
              ARAD::Singleton<ARAD::Arad_DataManager>::Get()->findGameScript(
                  std::string("Arad_EventPeriodDataManager")))
              ->isApplied(static_cast<ENUM_REPEAT_EVENT_CODE>(0x8e), curSec)))
    {
        return 0;
    }

    Inven_Item item;
    int itemIdx = 0x28d3af;         // [b28d-C08 · C05② 旋钮**两态已试毕**，勿再烧] ORIG
                                    // 0x868b7b0/7ba
                                    // 为 `mov -0x8c(%ebp),%eax; test %eax,%eax; je` +
                                    // `cmp $0xffffffff,%eax`（寄存器比较），曾按 P209/
                                    // P218 档② 改 unsigned 期望翻正；fresh 实测 int/
                                    // unsigned **两态同形**（widthscan cmp_l 恒 our=2）
                                    // —— 本件是「test 零比较 + cmp $imm 常量比较」双比较点，
                                    // itemIdx 在零比较点后仍被复用（日志 `itemidx(%u)`，
                                    // ORIG 0x868b9ae `mov -0x8c(%ebp),%edi`）⇒ 按 A12 口径
                                    // 属「⚠ 可试一次」，但**已试尽**：4.4 恒折
                                    // `cmpl $0x0/-0x1c(%ebp)` 内存直比（同 A01 check_error、
                                    // D01 loadFromString 负例）⇒ **N18b 常量比较子型**
                                    // 封顶，回退 int 保持最小改动。
                                    // ORIG 0x868b77c 独立栈槽 -0x8c（非 Inven_Item
                                    // 成员——0x868b77c 与 item@-0x8e 尺寸不重叠实证；
                                    // ChronicleRandomOptionInfo 亦无 +1 偏移 int 成员）
                                    // b28d-W8: 去 const —— ORIG 0x868b9ae 从槽
                                    // `mov -0x8c(%ebp),%edi` 读回，4.4 对 const 常量
                                    // 传播成 `movl $0x28d3af,0x10(%esp)`（少 1 条 + 无槽）。
    // ORIG 0x868b78b `mov 0xc(%eax),%eax` 直读 m_itemList 成员（getItemList
    // 转发 call orig=0，具名字段直访）。
    G_CDataManager()->m_itemList->create_item(
        static_cast<ENUM_ITEM_CREATE_TYPE>(0), item, 1);
    if (itemIdx != 0 && itemIdx != -1)
    {
        setFlagPresentAvengerTitle(true);

        // ORIG 0x868b7e2/82c 两段 peel+rep stos 清零块 = `= {0}` 聚合初始化
        //（makeGuildItemUpgradMessage board 同形，4.4 产 memset 调用 orig=0）。
        // b28d-W8: 声明序照 ORIG —— body(0x100)@0x868b7e2 先清零、subject(0x15)
        // @0x868b82c 后清零（P28 声明序即发射序；原序反了）。
        char body[0x100] = {0};
        char subject[0x15] = {0};
        strncpy(subject,
                g_scriptStringManager_.findString(4, "ready_to_avenger_title", 0),
                0x14);
        strncpy(body,
                g_scriptStringManager_.findString(4, "ready_to_avenger_mail", 0),
                0xff);

        // b28d-W8: P52/P134 —— ORIG 0x868b906/91c/923 三值入 callee-saved
        //（group→%ebx、strlen→%esi、characNo→%eax）跨调用保活、装载段直读寄存器；
        // 具名局部引入 3 对 store/reload（P0 表达式直传）。
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            subject, item, 0, getCurCharacNo(), body,
            static_cast<unsigned int>(strlen(body)), 0,
            static_cast<ENUM_SERVER_GROUP>(GetServerGroup()), false, false);

        // b28d-W8: P52/P134 —— ORIG 0x868b979/986/993 三值 %esi/%ebx/%eax 直传，
        // 无 characNo2/accId2/uid 具名槽。
        DB_SaveGiveAvengerTitleFlag::makeRequest(
            (int)GetUID(), get_acc_id(), getCurCharacNo());

        // b28d-W8: P227 全表达式临时链 —— ORIG 0x868b9ae..0x868ba1b 在 cMyTrace
        // ctor 前右起求值 itemIdx(%edi)→characNo(%esi)→NumberToString(%ebx)，
        // 三值全程寄存器无槽 ⇒ 原码无具名局部（P0）。
        cMyTrace(__PRETTY_FUNCTION__, 0xa6eb, 0)(
            "avenger plan event item post m_id(%s),charac_no(%u), itemidx(%u)",
            NumberToString(get_acc_id(), 0), getCurCharacNo(), itemIdx);
        return 1;
    }
    return 0;
