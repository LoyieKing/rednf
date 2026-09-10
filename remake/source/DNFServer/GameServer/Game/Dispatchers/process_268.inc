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
    // [b28d-solo-Dispatchers] P96/P270：ORIG 用 const_iterator（p167 MISSING
    //   `_ZNKSt23_Rb_tree_const_iterator…ptEv`×2 + `C2ERKSt17_Rb_tree_iterator`×2 +
    //   `_ZNKSt6vector…5beginEv/3endEv`，我方旧 iterator 全走非 const 版）。
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
        if (item.m_field1 != Inven_Item::ITEM_TYPE_EQUIP)
        {
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
