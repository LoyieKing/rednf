void CUser::setRecipeEqiupmentOption(Inven_Item& outItem, stEquipCondition cond,
                                     int matUpgrade, const CStackableItem* stackable,
                                     int invenSpace)
{
    // [P0/P162] ORIG 0x0867306e+0x9：`mov 0x2(%eax),%eax; mov %eax,%ebx; call
    // G_CDataManager; mov %ebx,0x4(%esp); ...` —— 4 字节直读直传、**无槽往返**
    // ⇒ 原码无 outId 局部（旧 `unsigned short` 截断生 `mov %ax,-0x16; movzwl`），
    // trace 实参处 ORIG 亦 4 字节直传 ⇒ 两处都内联 `outItem.m_addInfo`（unsigned int）。
    CItem* outDef = G_CDataManager()->find_item(outItem.m_addInfo);

    if (outDef == 0)
    {
        LogManager::logFormat(
            1, "user.cpp",
            __PRETTY_FUNCTION__,
            0x5fbe, "RECIPE ERROR : Not Exist Output Item");
        return;
    }
    if (outItem.m_field1 != 1)                                               // ITEM_TYPE_EQUIP
    {
        LogManager::logFormat(
            1, "user.cpp",
            __PRETTY_FUNCTION__,
            0x5fc4, "RECIPE ERROR : Not Equipment Item");
        return;
    }
    if ((unsigned char)cond.m_upgrade > 0x1f)
    {
        LogManager::logFormat(
            1, "user.cpp",
            __PRETTY_FUNCTION__,
            0x5fca, "RECIPE ERROR : Impossible Upgrade Value");
        return;
    }

    // [CUserOnlyOrig 2026-09-09 P321] ORIG 0x08673143-0x0867316c：`je L; call
    // getCompoundUpgrade; jmp L2; L: mov $0x0,%eax; L2: mov %eax,-0x1c(%ebp)` ——
    // 三目形 + **4 字节直存**；trace 实参处（O71）亦 4 字节直传，仅 SetUpgrade
    // 两处（O82-O84/O103-O105）显式 `movzbl` ⇒ 原码是
    // `int outUpgrade = (matUpgrade == 0xb) ? 0 : getCompoundUpgrade(...)`。
    // 旧 `unsigned char` 局部使 trace 实参多一次 movzbl 且分支形不同。
    int outUpgrade = (matUpgrade == 0xb)
        ? 0 : getCompoundUpgrade(stackable, cond);

    cMyTrace trace(
        __PRETTY_FUNCTION__,
        0x5fdb, 0);
    trace("RECIPE LOG : out_item_id-%u, mat_up_val-%d, out_up_val-%d, mat_seal-%d",
          (unsigned)outItem.m_addInfo, (unsigned)(unsigned char)cond.m_upgrade,
          (unsigned)outUpgrade, (unsigned)(unsigned char)cond.m_flag6);

    if (cond.m_tableIdx == 0)                                                // cond+1 非0 → amplify
    {
        outItem.SetUpgrade(static_cast<unsigned char>(outUpgrade));
    }
    else
    {
        stAmplifyOption_t& amp = outItem.m_amp;                              // Inven_Item+0x11
        amp.assign(cond.m_amplifySelect, cond.m_field4);                     // type=cond+2, value=cond+4w
        amp.identify();
        outItem.SetUpgrade(static_cast<unsigned char>(outUpgrade));
    }

    // bind 标记：m_flag6==1 且 GetAttachType∈{3,4} → item+0=1，否则 0
    // [CUserOnlyOrig 2026-09-09 P0/N96] ORIG 0x086731xx 直接 `movb $0x1,(%eax)` /
    // `movb $0x0,(%eax)`（无 bool 局部槽、无 movzbl 回读）⇒ 原码是 if/else 常量直写；
    // 旧 `bool bind` + 三目形多 1 槽（-0xd）与 3 条物化。
    if ((unsigned char)cond.m_flag6 == 1 &&
        (outDef->GetAttachType() == 3 || outDef->GetAttachType() == 4))
        outItem.m_field0 = 1;
    else
        outItem.m_field0 = 0;

    if (outDef->GetAttachType() == 3)
        outItem.SetReSealCount((unsigned char)cond.m_pad7);                  // cond+7 reseal

    getCurCharacInvenW()->update_item((INVEN_TYPE)1, invenSpace, outItem);
