void CUser::processFatigueCharac(int usedMin)
{
    int job = get_charac_job();
    int giftId = 0;
    giftId = G_CDataManager()->get_useJobFatigueMatchGift(
        (char)job, (int)getCurCharacUsedFatigue(), usedMin);
    if (giftId == 0)
        return;

    std::string title;
    std::string fmtKey;
    G_CDataManager()->get_useJobFatigueMatchGiftMsg(job, title, fmtKey);

    Inven_Item item;
    CItem* def = G_CDataManager()->find_item(giftId);
    int srvGrp = GetServerGroupToPvP();
    // [Lane14C] ORIG 0x08692689 `cmpl $0,-0x14(%ebp); je <+0x21f 共享清理出口>`
    // = 正向包裹形（P222/P257）：def==0 直接落共享清理出口、不执行 snprintf/
    // findString（原「if (def == 0) return;」平铺形使 4.4 生成反向 jne + 独立
    // 出口块，帧/块数 168 vs ORIG 162）。
    if (def != 0)
    {
        int itemIndex = def->get_index();
        def->make_item(item);
        item.set_add_info(1);

        char msg[256];
        // ORIG 0x086926d8：`call getCurCharacUsedFatigue; movzwl %ax,%ebx`（无局部槽，
        // 值在 %ebx 跨 c_str/findString 保活）⇒ 变参位内联（P44/P0）。
        OS_API::snprintf(msg, 0xff,
                         g_scriptStringManager_.findString(4, fmtKey.c_str(), 0),
                         getCurCharacUsedFatigue());
        if (itemIndex != 0)
        {
            int expire = def->getExpirationDate();
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                g_scriptStringManager_.findString(4, title.c_str(), 0),
                item, 0, getCurCharacNo(), msg, (int)strlen(msg), expire,
                (ENUM_SERVER_GROUP)srvGrp, false, true);
        }
    }
