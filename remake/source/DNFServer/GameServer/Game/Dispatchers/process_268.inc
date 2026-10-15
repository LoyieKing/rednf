// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081db60c orig/ours=410/389 ndiff-blocks=55
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: mov    %eax,-0x30(%ebp) | cmpl   $0x0,-0x30(%ebp) | mov    -0x30(%ebp),%ebx
int Dispatcher_Select_Item_Grwoth_Power::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err = check_error(user, msg, param);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }

    MSG_GROWTH_POWER& req = reinterpret_cast<MSG_GROWTH_POWER&>(msg);
    stGrowthPowerParam& out = static_cast<stGrowthPowerParam&>(param);
    // [b28d-solo-Dispatchers] P183：ORIG 4 处各自完整调 G_CDataManager()
    //   （p167 MISSING `_Z14G_CDataManagerv` ×3 = 我方 dm 具名缓存省掉的 3 次），
    //   旧码 `CDataManager* dm = G_CDataManager();` 缓存 ⇒ 删除。
    STBreakAwaySection section =
        G_CDataManager()->m_growthPower.get_break_away_section_data(
            user->get_charac_level());
    int key = makeFairPVPEquipmentPackageKey(
        user->get_charac_job(),
        static_cast<int>(static_cast<unsigned char>(section.m_field0)));
    // [G1-Dispatchers 回滚判词] 曾试 `const_iterator`（p167 MISS const 版 ptEv 似为直证）：
    //   实测 TU identical **1009→1007**（丢 `__normal_iterator<PSt6vector<STStackablePackageData>*>
    //   ::C1ERKS5_` 与 `::base()` 两个 identical 实例 —— 它们由本使用点发射），
    //   而本件 verdict 未升级（仍 DIFF）⇒ 净负，回滚为非 const `iterator`（P96 原形）。
    //   ORIG 那两个实例由 STL 方法链（push_back/_M_insert_aux/aSERKS_ → begin/end）ODR-use。
    // [Q4DispatchBig 2026-09-11 负结论·复测回滚] 本件「it 改 const_iterator」与
    //   「it const + at()」两种改法都实测劣化：
    //   旋钮1（只改 it）TU identical 1011→1009、ae 2394→2392；
    //   旋钮1+2（再叠 `oit->at(sel)`）identical 1011→1008 ⇒ 净负（本件仍 DIFF，
    //   gap +15 不变）。机制：const 化后本使用点不再发射非 const
    //   `__normal_iterator<PSt6vector<STStackablePackageData>*,...>::C1ERKS5_` 与
    //   `::base()`（ORIG 有，0x0839xxxx 族）⇒ TU 少 2~3 个 identical 实例。
    //   ⇒ 维持 P96 原形（非 const iterator + `(*oit)[sel]`）。
    std::map<int, std::vector<std::vector<STStackablePackageData> > >::iterator it =
        G_CDataManager()->m_growthPower.m_map18.find(key);
    if (it == G_CDataManager()->m_growthPower.m_map18.end())
    {
        return 0;
    }

    std::vector<Inven_Item> items;
    int idx = 0;
    for (std::vector<std::vector<STStackablePackageData> >::const_iterator oit = it->second.begin();
         oit != it->second.end(); ++oit)
    {
        unsigned char sel = *reinterpret_cast<unsigned char*>(&req.m_0e[idx]);
        if (oit->size() <= static_cast<unsigned int>(sel))
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x4a8b, 0);
            trace("[GrowthPower](%d)(j:%d)(l:%d)(s:%d)(p:%d)",
                  user->get_acc_id(), user->get_charac_job(), user->get_charac_level(),
                  static_cast<int>(oit->size()), static_cast<int>(sel));
            return LineFunc(0x4a8c,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        }
        if (idx > 4)
        {
            return LineFunc(0x4a8f,
                            __PRETTY_FUNCTION__,
                            0, user->get_acc_id());
        }

        Inven_Item item;
        // ORIG 用 vector::at(sel)（_ZNKSt6vectorI22STStackablePackageDataSaIS0_EE2atEj）。
        item.m_addInfo = oit->at(sel).m_field0;
        CItem* info = G_CDataManager()->find_item(item.m_addInfo);
        if (info == 0)
        {
            out.m_4 = 0x11;
            return 0;
        }
        info->make_item(item);
        if (item.m_itemType != Inven_Item::ITEM_TYPE_EQUIP)
        {
            // [P4 负结论 2026-09-11 实测回滚] `oit->at(sel)` 使 `_ZNK9__gnu_cxx17__normal_iteratorIPK
            //   St6vectorI22STStackablePackageDataSaIS2_EES1_IS4_SaIS4_EEEdeEv` 不再发射，而该符号
            //   **ORIG 有**（0x083947b2 W, size 0xa）⇒ TU identical −1（1009→1008），EXTRA 收益
            //   不足以抵偿 ⇒ 维持 `(*oit)[sel]` 形。
            // [Q4DispatchBig 2026-09-11 复测确认] 「it const + at」合并改法同样劣化
            //   （identical 1011→1008、ae −2），负结论维持。
            item.m_addInfo2 = (*oit)[sel].m_field4;
        }
        items.push_back(item);
        ++idx;
    }

    for (std::vector<Inven_Item>::const_iterator iit = items.begin(); iit != items.end(); ++iit)
    {
        int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            *iit, static_cast<eItemAddReason>(8), true, true);
        if (slot < 0)
        {
            char title[0x15];
            char body[0x100];
            strncpy(title, g_scriptStringManager_.findString(4, "game_server_msg_213", 0), 0x14);
            strncpy(body, g_scriptStringManager_.findString(4, "game_server_msg_214", 0), 0xff);
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                title, *iit, 0, user->getCurCharacNo(), body,
                static_cast<int>(strlen(body)), 0, user->GetServerGroup(), false, false);
        }
        else
        {
            user->SendUpdateItemList(static_cast<CUser::eSendTarget>(1),
                                     static_cast<ENUM_ITEMSPACE>(0), slot);
        }
    }
    return 0;
}