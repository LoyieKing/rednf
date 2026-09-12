// ===========================================================================
// Avenger 计划（复仇者转职前礼物）事件
// ORIG 0x0868b552：事件码 0x8e + 创建时间窗判定，job==4 且未成长一级角色首次
// 登录 → 发放 0x28d3af 物品邮件并置 flag/DB 记录。
// ===========================================================================
// b28d-W8(2026-09-09): **gap 2→0（340/340）** — 本波 5 修：
// ①P0/P183 删 period/period2 具名局部（findGameScript 结果 %eax 直传 receiver，
//   省 2 对 store/reload）；
// ②P28 声明序 body(0x100)先于 subject(0x15)（清零块序照 ORIG 0x867b7e2/82c）；
// ③P52/P134 ReqDBSendNewSystemMail 与 makeRequest 三值内联实参（%ebx/%esi/%eax
//   跨调用保活，删 6 对槽往返）；
// ④P227 trace 全表达式临时链 + itemIdx 去 const（复现 0x868b9ae 槽读 %edi）；
// ⑤getFlagPresentAvengerTitle 返回 uchar→bool（CUser.h:3240/CUser.cpp:11353，
//   SweepCUser2 代落）+ `!f()` ⇒ 0x868b68b `xor $1` 复现。
// p167 CLEAR（仅 C1/C2 别名）；余 isApplied 段 1 条 xor（P168a③b 跨版本 bool ABI）
// + 槽号整体平移。ORIG 0x0868b552。
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N18b/N7/N96，gap=+0/blocks=41）→ 试=①prett/q3_gap2
//   缺口 0、strall CLEAR ②n1_v 残差三类：accId/createTime→%esi/%ebx 角色互换（ORIG
//   `mov %eax,%ebx` 在前 vs 我方 `%eax,%esi`）、局部槽平移 -0x35/-0x3c（ORIG）vs
//   -0x39/-0x40（我方）、尾调用实参寄存器对调 ⇒ 全属 4.1/4.4 寄存器角色 + 槽分配（N7/N96）
//   ⇒ 维持 N（340/340，无源码旋钮）。
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
    // [R28wrt §34.2.2 字段写修复] ORIG 0x0868b77c `movl $0x28d3af,-0x8c(%ebp)` 是
    //   **item(@-0x8e)+0x02 = Inven_Item::m_addInfo** 的写入（判据 Inven_Item.h:269
    //   `unsigned int m_addInfo; // +0x02`），**不是**独立栈槽 —— 旧注释「ORIG 0x868b77c
    //   独立栈槽 / 非 Inven_Item 成员」为**误判（已删除）**：0x0868b76e
    //   `lea -0x8e(%ebp),%eax` 即 item 基址（同址实参 Inven_Item::C1Ev @0x0868b777 与
    //   create_item @0x0868b796），-0x8e + 0x02 ≡ -0x8c 精确重合。
    //   消费链（缺写 ⇒ 读未初始化值）：CItemList::create_item ORIG 0x08511fd2/0x08511ff3
    //   `mov 0x10(%ebp),%eax; mov 0x2(%eax),%eax` 直读 item+0x02 作 find_item(int)
    //   实参（0x08511fe2 call）；返回 0 时落失败/错误日志块（0x08511ff0 起）⇒ 本字段
    //   必须在 create_item 之前写入。
    item.m_addInfo = 0x28d3af;
    // ORIG 0x868b78b `mov 0xc(%eax),%eax` 直读 m_itemList 成员（getItemList
    // 转发 call orig=0，具名字段直访）。
    G_CDataManager()->m_itemList->create_item(
        static_cast<ENUM_ITEM_CREATE_TYPE>(0), item, 1);
    if (item.m_addInfo != 0 && item.m_addInfo != 0xffffffffu)
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
        // ctor 前右起求值 item+0x02(%edi)→characNo(%esi)→NumberToString(%ebx)，
        // 三值全程寄存器无槽 ⇒ 原码无具名局部（P0）。
        cMyTrace(__PRETTY_FUNCTION__, 0xa6eb, 0)(
            "avenger plan event item post m_id(%s),charac_no(%u), itemidx(%u)",
            NumberToString(get_acc_id(), 0), getCurCharacNo(), item.m_addInfo);
        return 1;
    }
    return 0;
}