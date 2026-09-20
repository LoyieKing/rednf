// ORIG 0x081d6af4 T _ZN34Dispather_RegenerationRandomOption7processEP5CUserR8MSG_BASER9ParamBase
// （报告 Dispather_RegenerationRandomOption/process.md ## 反编译 C 全文映射）：
// 两装备槽逐槽校验（非空/可穿装/已有选项/IsRandomOption/Restrict(1,0x10)/安全保护
// 0x1b/CheckItemLock）→ 全部 m_field0==1 时可再封印 → reSealCount=ROUND(0.5+(r0+r1)/2)
// → RegenerationROI::Regenerate(usableLv0,usableLv1,opt0,opt1,&cost,newItem,user)
// → 统计 regen_cnt/regen_gold → use_money(cost,0x2f,1) → delete_item(1,槽,1,0x2e,1)×2
// → newItem 置封印标志与次数 → insertItemIntoInventory(reason 0x36,1,1) →
// SendUpdateItemList ×3 + cUserHistoryLog::RandomOption(reason 1)。
// [b28d-29] N82：帧 0x19c vs 0x1ac + 槽平移与参数副本顺序差，调用点全等  (ORIG 0x081d6af4 insn 479->466)
// 错误码写 param.m_errNo；插入槽位写 param.m_insertedSlot。
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 238（ORIG 479 / ours 465），残差=setcc-fold bool 物化岛/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
int Dispather_RegenerationRandomOption::process(CUser* user, MSG_BASE& msg,

                                                ParamBase& param)
{
    stRegenROIResult& out = static_cast<stRegenROIResult&>(param);
    MSG_REGENERATION_RANDOM_OPTION& req =
        *reinterpret_cast<MSG_REGENERATION_RANDOM_OPTION*>(&msg);

    out.m_errNo = 0;
    Inven_Item items[2];
    // [G5-ProcB] ORIG 0x081d6cec `mov %eax,-0x44(%ebp,%ebx,4)` 直接落数组槽，
    // 其前无 `movl $0x0` 清零 ⇒ 原码 `CItem* baseItems[2];` 无 `= {0,0}`
    // （旧文多 2 条 movl）。
    CItem* baseItems[2];
    // [G5-ProcB] ORIG 0x081d6b57 `mov -0x3c(%ebp),%eax; movzwl 0xd(%eax,%edx,2),%eax;
    // movswl %ax,%esi` ⇒ 两槽位不拷入本地数组，直接以 msg 为基址索引读
    // （0x0d + i*2）。旧文 `const unsigned short slotPos[2] = {...}` 多 1 条
    // movl 清零 + 2 条读存（数组落在 -0x48，索引变 `movzwl -0x48(%ebp,%eax,2)`）。
    #pragma pack(push, 1)
    #pragma pack(pop)
    bool allSealable = true;          // ORIG local_36（任一槽 m_field0!=1 即 false）
    unsigned char reSealCount = 0;

    for (int i = 0; i < 2; ++i)
    {
        // [G5-ProcB] ORIG 0x081d6b62 `call getCurCharacInvenR` → 结果留在 %eax 跨
        // imul/lea 直作 0x081d6b8e `mov %eax,0x4(%esp)`（GetInvenSlot this 实参），
        // 无 invenR 具名槽。
        items[i] = user->getCurCharacInvenR()->GetInvenSlot(
            (INVEN_TYPE)1,
            (int)(short)(i ? req.m_equipSlot1 : req.m_equipSlot0));
        if (items[i].isEmpty() || !items[i].isEquipableItemType() ||
            items[i].m_random.m_random.empty())
        {
            out.m_errNo = 0x13;
            return 0;
        }
        baseItems[i] = G_CDataManager()->find_item(items[i].m_addInfo);
        if (baseItems[i] == 0 || baseItems[i]->IsRandomOption() != true)
        {
            out.m_errNo = 0x13;
            return 0;
        }
        if (ARAD::Singleton<ServiceRestrictManager>::Get()->isRestricted(
                user, RestrictCategory::Enum_1, 0x10))
        {
            out.m_errNo = 0xd1;
            return 0;
        }
        int secuErr = GlobalData::s_pSecuProtectionField->Check(user,
                                                                (SECURITY_PROTCTION)0x1b);
        if (secuErr != 0)
        {
            out.m_errNo = secuErr;
            return 0;
        }
        if (user->CheckItemLock(
                1,
                (int)(short)(i ? req.m_equipSlot1 : req.m_equipSlot0)))
        {
            out.m_errNo = 0xd5;
            return 0;
        }
        if (items[i].m_field0 != 1)
        {
            allSealable = false;
        }
    }
    // ORIG：reSealCount=(uchar)(short)fistp(0.5f+(r0+r1)/2.0f)（浮点常量 0x3f000000/0x40000000）
    reSealCount = (unsigned char)(short)(
        // [G5-ProcB] ORIG 0x081d6e49 `mov %eax,-0x14c(%ebp)` / 0x081d6e4f
        // `fildl -0x14c(%ebp)`：ORIG 是 **signed int**→float（fildl）；旧文
        // `(unsigned int)` 使 4.4 走无符号修正路径（零扩展 64 位 + `fildll`，多
        // `mov $0x0,%edx; mov %edx,SLOT` 两条）。
        0.5f + (float)((int)items[0].GetReSealCount() +
                       (int)items[1].GetReSealCount()) / 2.0f);

    // [BigDispatchCore] **语义完整优先，+41 行形态代价，Main 2026-09-09 裁定** ——
    // 本件 ndiff 216 → 257 是**已裁定的正确结果**，后来泳道请勿以“数字变差”为由回滚。
    // 依据：ORIG local_4c 是原码真有的一个具名 std::string 局部（只构造、全函数不读取），
    // callee 证据 _ZNSsC1Ev@0x081d6e9d、_ZNSsD1Ev@0x081d724e/0x081d7265、
    // _Unwind_Resume@0x081d725a（landing pad 0x081d7244）；不补则 P167 恒有 4 项 MISSING。
    // 4.4 会把 dtor 集中到函数尾并改写 landing pad ⇒ 形态代价计入 **N1042**。
    std::string unusedSealName;
    Inven_Item newItem;
    int regenCost = 0;
    // ORIG RegenerationROI::Regenerate T 0x085f8646（声明 CDataManager.h，定义 blocked：
    // 依赖 random_option::CRandomOptionItemHandle::RegenVer_* 无任何 TU 提供）
    int regenErr = G_CDataManager()->m_regeneration.Regenerate(
        baseItems[0]->getUsableLevel(), baseItems[1]->getUsableLevel(),
        (int)req.m_srcOptIndex0, (int)req.m_srcOptIndex1, regenCost, newItem, user);
    if (regenErr != 0)
    {
        out.m_errNo = regenErr;
        return 0;
    }
    statistc_proxy::add(user, "log_random_option", "regen_cnt", 1, "%d,'%s'",
                        user->get_charac_10_level_section(),
                        GlobalData::s_systemTime_.getCurDate());
    if (regenCost > 0)
    {
        statistc_proxy::add(user, "log_random_option", "regen_gold",
                            (unsigned int)regenCost, "%d,'%s'",
                            user->get_charac_10_level_section(),
                            GlobalData::s_systemTime_.getCurDate());
    }
    if (!user->getCurCharacInvenW()->use_money(regenCost, (eMoneySubReason)0x2f, true))
    {
        out.m_errNo = 0xa;
        return 0;
    }
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, 0);
    for (int i = 0; i < 2; ++i)
    {
        if (user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1,
                (int)(short)(i ? req.m_equipSlot1 : req.m_equipSlot0), 1,
                (eItemDelReason)0x2e, true) != 1)
        {
            out.m_errNo = 0x13;
            return 0;
        }
    }
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, (int)req.m_equipSlot0);
    user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, (int)req.m_equipSlot1);
    newItem.m_field0 = allSealable ? 1 : 0;   // ORIG CONCAT31 低字节 ← local_36
    newItem.SetReSealCount(reSealCount);
    int insertedSlot = user->getCurCharacInvenW()->insertItemIntoInventory(
        newItem, (eItemAddReason)0x36, true, true);
    if (insertedSlot >= 0)
    {
        user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, insertedSlot);
        out.m_insertedSlot = (short)insertedSlot;
    }
    user->m_historyLog.RandomOption(newItem, randomOptionReason::T_REGENERATION);
    return 0;
}
