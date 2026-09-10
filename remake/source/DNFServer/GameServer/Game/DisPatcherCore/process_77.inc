int Dispather_RegenerationRandomOption::process(CUser* user, MSG_BASE& msg,
                                                ParamBase& param)
{
    stRegenROIResult& out = static_cast<stRegenROIResult&>(param);
    MSG_REGENERATION_RANDOM_OPTION& req =
        *reinterpret_cast<MSG_REGENERATION_RANDOM_OPTION*>(&msg);

    out.m_errNo = 0;
    Inven_Item items[2];
    CItem* baseItems[2] = { 0, 0 };
    const unsigned short slotPos[2] = { req.m_equipSlot0, req.m_equipSlot1 };
    bool allSealable = true;          // ORIG local_36（任一槽 m_field0!=1 即 false）
    unsigned char reSealCount = 0;

    for (int i = 0; i < 2; ++i)
    {
        const CInventory* invenR = user->getCurCharacInvenR();
        items[i] = invenR->GetInvenSlot((INVEN_TYPE)1, (int)slotPos[i]);
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
        if (user->CheckItemLock(1, (int)(short)slotPos[i]))
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
        0.5f + (float)((unsigned int)items[0].GetReSealCount() +
                       (unsigned int)items[1].GetReSealCount()) / 2.0f);

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
                (INVEN_TYPE)1, (int)slotPos[i], 1, (eItemDelReason)0x2e, true) != 1)
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
