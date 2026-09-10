bool DB_StatisticAdvanceAltar::dispatch(int /*uid*/, int /*uniqueId*/, Stream* stream)
{
    try
    {
        MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)4, (ENUM_SERVER_GROUP)0);
        struct tm now;
        __GetCurrentDNFDate(now);
        // ORIG 0x0814128c 段三条 `movl $0x0,-0x5c/-0x58/-0x54` = date[12] 预清。
        char date[12] = {0};
        std::sprintf(date, "%04d-%02d-%02d", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday);

        int entranceCount = 0;
        *stream >> entranceCount;                                   // [0x861c5c4 operator>>(int&)]
        for (int i = 0; i < entranceCount; ++i)
        {
            // [b28d-solo-advancealtar_DB] P89：ORIG 单槽 -0x28（GetOutBuffer 结果
            //   即视图指针），旧 rawE+e 双局部多一槽并致全帧平移 8。
            EntranceView* e = reinterpret_cast<EntranceView*>(
                stream->GetOutBuffer<STAdvanceAltarEntranceData>());
            if (e == 0)
                throw 0x624;
            // P118/P139：ORIG 0x081412d5 `movl $<rodata>,0x4(%esp)` 直接传函数签名串
            //   （_ZZ…E19__PRETTY_FUNCTION__），非 static 指针变量（多一条装载）。
            cMyTrace(__PRETTY_FUNCTION__, 0x626, 0)("[AdvanceAltar] %u,%u,%u,%u",
                                                     e->stageId, e->entranceCount, e->successCount, e->obtainStar);
            mysql->set_query(
                             " upDate log_advance_alter_entrance  set entrance_count = entrance_count + %u"
                             "  , success_count = success_count + %u  , obtain_star = obtain_star + %u"
                             "  where occ_date = '%s' and stage_id = %u",
                             e->entranceCount, e->successCount, e->obtainStar, date, e->stageId);
            if (!(mysql->exec(true) && mysql->getAffectedRowCount() != 0))
            {
                mysql->set_query(
                                 " inSert into log_advance_alter_entrance(occ_date,  stage_id,"
                                 " entrance_count, success_count, obtain_star)"
                                 "  values('%s', %u, %u, %u, %u) ",
                                 date, e->stageId, e->entranceCount, e->successCount, e->obtainStar);
                mysql->exec(true);
            }
        }

        int layerCount = 0;
        *stream >> layerCount;
        for (int i = 0; i < layerCount; ++i)
        {
            LayerView* l = reinterpret_cast<LayerView*>(
                stream->GetOutBuffer<STAdvanceAltarLayerStatData>());
            if (l == 0)
                throw 0x64d;
            cMyTrace(__PRETTY_FUNCTION__, 0x64f, 0)("[AdvanceAltar] %u,%u,%u,%u,%u",
                                                     static_cast<unsigned>(l->itemType), l->itemId,
                                                     static_cast<unsigned>(l->itemLevel),
                                                     static_cast<unsigned>(l->useCount),
                                                     static_cast<unsigned>(l->useStar));
            mysql->set_query(
                             " upDate log_advance_alter_layer_stat  set use_count = use_count + %u,"
                             " use_star = use_star + %u  where occ_date = '%s' and item_type = %u and"
                             " item_id = %u and item_level = %u",
                             static_cast<unsigned>(l->useCount), static_cast<unsigned>(l->useStar),
                             date, static_cast<unsigned>(l->itemType), l->itemId,
                             static_cast<unsigned>(l->itemLevel));
            if (!(mysql->exec(true) && mysql->getAffectedRowCount() != 0))
            {
                mysql->set_query(
                                 " inSert into log_advance_alter_layer_stat(occ_date,  item_type,"
                                 " item_id, item_level, use_count, use_star)"
                                 " values('%s', %u, %u, %u, %u, %u)",
                                 date, static_cast<unsigned>(l->itemType), l->itemId,
                                 static_cast<unsigned>(l->itemLevel),
                                 static_cast<unsigned>(l->useCount),
                                 static_cast<unsigned>(l->useStar));
                mysql->exec(true);
            }
        }
        return true;
    }
    catch (int err)
    {
        LogManager::logFormat(1, "localglobal/global_AdvanceAltar_Dispatcher.cpp", __PRETTY_FUNCTION__,
                              0x674, "[AdvanceAltar] query error line(%u)", err);   // [0x814174e]
        return false;
    }
