// ---- respond_special_item_premium —— ORIG 0x0827d3dc ----
// g_map_premium_item_data[itemIdx] 命中后按 premium type (STPremiumItemData.m_value0)
// jump-table 分发：term/flag/item 取自 m_value4(低2字节)/m_value8。各 case 处理
// 限制码/日志/技能备份/表情/疲劳/金币等；收尾统一走
// cal_end_time → send_db → handleSetUserPremium → handlePremiumEffect →
// is_notify/_sendPremiumPacket → write_log。未命中记 PREMIUM_ERROR 日志返回。
// 注：报告汇编中 "ItemVendingMachine::BuyAuctionItem" 标签为 report_resolve 误标，
// 实际地址 0x0817a10c 为 CItemList::GetRestrictCode（nm 核实），本函数不使用 BuyAuctionItem。
// b28d-D42 verdict=DIFF [N82/N89] ORIG 0x0827d3dc orig/ours=672/703 ndiff=680。
// 巨件帧/槽平移 + STL/字符串内联差（N89）；语义按报告逐条对齐。
// b28d-solo-CCeraShop_Special verdict=DIFF [N82/N89] 672/703 ndiff 890→863；lethe
// 获取前置到 param 字段读取之前（ORIG 0x0827d3dc 序）；P167 CLEAR 101/101；余帧
// 0xc0/0xf4 + 槽平移封 N82。
// [CeraShop12Run 2026-09-11 本轮] 680/701→680/693（-8；TU 三数 176/319/3/11 与
//   rule4 逐函数 verdict 全一致，仅本件 COUNT 改善）：
//   ① m_char1f 按 bool 左值视图读（ORIG 0x0827d527..0x0827d531 `movzbl 0x1f(%eax);
//      movzbl %al` 无 bool 归一化）⇒ 消 test/setne/movzbl 三条；
//   ② needGain 三态赋值改单条 `if (A != 0 && B)` ⇒ 复现 ORIG 0x0827dcf5..0x0827dd17
//      的 && 物化岛（mov $1/mov $0/test/je，%eax 单寄存器零槽），消 movb×2+cmpb（-7）。
//   ③ 旋钮D 负结论：`(IsEndLetheAtLogin() ^ 1) != 0`（与 `!= 1` 对 uchar 等价）被
//      4.4 规范化回 `cmp $1; setne`（693→693 逐条不变）⇒ 已回滚。
// 余 gap +13 = N82 槽号平移 + 下述 .h 待办三处（各 +1 条）：
//   · premium_helper_function::is_notify 返回 uchar（.h:26）⇒ ORIG 0x0827ddf5
//     `call; xor $1; test` 直证应为 bool；我方出 `cmp $1; setne; test`。
//   · CUserCharacInfo::IsEndLetheAtLogin 返回 uchar（.h:1115）⇒ ORIG 0x0827d905
//     `call; xor $1; test` 直证应为 bool。
//   · STSpecailItem_Param::m_char1f 声明 uchar（CCeraShop.h:223）⇒ ORIG 传 bool
//     形参无 test/setne；本件已用 bool 左值视图规避（若改 bool 可去视图）。
void SpecialItemHandler::respond_special_item_premium(int type,
                                                      const STSpecailItem_Param& param,
                                                      STSpecailItem_Result&)
{
    // ORIG 0x0827d3dc+：lethe 管理器函数头单次获取入槽（local_48），先于
    // param 字段读取。
    CPremiumLetheManager* lethe =
        G_CGameManager()->GetPremiumLetheManager();

    CUser* user = static_cast<CUser*>(param.m_user);
    // [G10 实验] ORIG 0x0827d3f7..0x0827d405：`mov 0x4(%eax),%eax; mov %eax,-0x9c(%ebp)`
    // 单条 4B store 后 **直接** `lea -0x9c(%ebp),%edx` 作 map::find 的 key 地址
    // （0x0827d43c）⇒ 该局部与 map key 同型（32 位下 unsigned long=4B），
    // 无 int→ulong 转换临时槽。旧 `int itemIdx` + 显式 cast 使 4.4 多出
    // 转换临时（我方 -0x48→-0xa8 两次 store）。
    // 实测（fresh .o，同一 ORIG 基线）：diff 段 +538/-519（3 hunks）→
    // +524/-501（3 hunks）；TU 四数 176/319(合计 495)/3/11/104 逐值不变 ⇒ 不劣
    // ⇒ 保留（帧 0xd0 未变，槽号由 -0x50/-0x4c... 上移到 -0x4c/-0x48...，
    // 与 ORIG -0x44/-0x40... 之差收敛到 8 = P332 版本首槽常数）。
    unsigned long itemIdx = param.m_itemIdx;
    int count = param.m_count;
    // ORIG 0x0827d414：`movzbl 0x1c(%eax),%eax; mov %al,-0x35`（字节槽，9 处
    // `movb $0x1,-0x35`、1 处 `movzbl -0x35,%eax`）+ 传 cal_end_time 第 6 形参
    // （bool）时零扩展直传无 setcc ⇒ 按 P292/T3 payload 侧 bool（m_flag 已 bool 化）。
    bool flag = param.m_flag;                   // -0x35
    // ORIG 槽：timeVal@-0x34（SaveFeaturedIdx 用，case 0xd 不改）、timeA@-0x30（base）、
    // start@-0xa0（cal_end_time 的 start 出参，初值 timeA，0827d430）、end@-0xa8。
    long timeVal = param.m_time;                // -0x34
    long timeA = timeVal;                       // -0x30
    long start = timeA;                         // -0xa0

    std::map<unsigned long, STPremiumItemData>::iterator it =
        g_map_premium_item_data.find(itemIdx);
    if (it == g_map_premium_item_data.end())
    {
        LogManager::logFormat(
            1, "SpecialItemHandler.cpp",
            __PRETTY_FUNCTION__,
            0x2ff, "PREMIUM_ERROR : NO PREMIUM ITEM!! : ITEM_ID (%u)",
            (unsigned int)itemIdx);
        return;
    }

    // ORIG 四字段各发一次 _Rb_tree_iterator::operator->（P157 禁缓存引用）。
    int ptype = it->second.m_value0;            // 商品类型（jump 键）
    int term = (int)it->second.m_c4;            // 期限（signed char）
    unsigned int flag2 =
        (unsigned int)(unsigned char)it->second.m_c5;  // 次字节
    int item = it->second.m_value8;             // 限制物品 idx
    // [H9MgrItemLoginCera.CCeraShop 回滚] `(bool)param.m_char1f` 实测 codegen 与
    // `!= 0` 完全相同（672/695/ndiff 396 逐值不变）⇒ 复原原形减噪。
    // [CeraShop12Run 2026-09-11 旋钮A] ORIG 0x0827d527..0x0827d531：
    // `movzbl 0x1f(%eax),%eax; movzbl %al,%eax` 直接零扩展作 bool 形参，
    // 无 test/setne 归一 ⇒ 原码按 bool 读该字节（字段声明 uchar 属视图问题）；
    // 用 bool 左值视图复现（`!= 0` 会多发 test/setne/movzbl 三条）。
    int resultVal = premium_helper_function::cal_premium_term(
        term, *reinterpret_cast<const bool*>(&param.m_char1f), param.m_extra);


    switch (ptype)
    {
    case 8:
        flag = 1;
        break;
    case 0xf:
        if (itemIdx == 200)
            resultVal = 0x1e;
        else if (itemIdx == 0xcb)
            resultVal = 3;
        else if (itemIdx == 0xcc)
            resultVal = 7;
        else if (itemIdx == 0xcd)
            resultVal = 0xe;
        if (user->getCurCharacInvenR()->GetCoin() < 3)
        {
            // ORIG 0x0827d5f2：三次 getter 结果经 %ebx 累加（无 coin/payCoin 具名槽）
            // ⇒ 单表达式（P0/P157：每次重新取得 invenR）。
            int sum = user->getCurCharacInvenR()->GetCoin() +
                      user->getCurCharacInvenR()->GetPayCoin() +
                      user->getCurCharacInvenR()->GetEventCoin();
            int amt = 3;
            if (100000 < sum + amt)
                amt = 100000 - sum;
            if (0 < amt)
            {
                user->getCurCharacInvenW()->UpdateDailyCoin(amt);
                user->m_historyLog.CoinAdd(
                    user->getCurCharacInvenR()->GetCoin(), amt,
                    (eCoinAddReason)4);
            }
        }
        break;
    // ORIG 0x16/0x1b/0x1d 三 case 各含独立同形调用块（-O0 不合并，×3 静态点）。
    case 1:
    case 3:
        resultVal = WongWork::CEventMgr::GetPremiumServiceBonus() + resultVal + 1;
        break;
    case 0xd:
    {
        // ORIG 0x0827d6ea：短路结果零槽物化（mov$1/jmp/mov$0/test;je，值只在 %eax）
        // ⇒ P226 扩展：语句表达式包住整条条件（内不含具名变量）。
        if (({ user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xd) != 1 &&
                user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xe) == 1; }))
        {
            WongWork::SUserPremiumInfo* info = user->GetPremiumInfo()->GetPremiumInfo(0xe);
            if (info->m_type == 0xe)
            {
                // ORIG 0x0827d730/733/736：只更新 timeA(-0x30) 与 start(-0xa0)，
                // 不动 timeVal(-0x34)（SaveFeaturedIdx 仍用原始 param.m_time）。
                timeA = info->m_end;
                start = timeA;
            }
        }
        break;
    }
    case 0xe:
        if (user->isAffectedPremium((ENUM_PREMIUM_TYPE)0xd) != 0)
        {
            count = G_CDataManager()->m_itemList->GetRestrictCode(itemIdx);
            ptype = 0xd;
        }
        break;
    case 0x16:
        count = G_CDataManager()->m_itemList->GetRestrictCode(item);
        flag = 1;
        break;
    case 0x1b:
        count = G_CDataManager()->m_itemList->GetRestrictCode(item);
        flag = 1;
        break;
    case 0x1d:
        count = G_CDataManager()->m_itemList->GetRestrictCode(item);
        flag = 1;
        break;
    case 0x21:
    {
        if (flag2 == 0 && count != 0 &&
            !user->IsRestrictedGoods(count))
        {
            user->SetRestrictedGoods(count);
            int serverCode = 0;
            if (g_SPremiumInfo[ptype].m_useTargetServer == 1)
                serverCode = G_CEnvironment()->m_serverEnvir.m_serverGroup;  // +0x378
            m_owner->SaveFeaturedIdx(user, count, (int)timeVal, serverCode);
        }
        count = G_CDataManager()->m_itemList->GetRestrictCode(item);
        flag = 1;
        // ORIG 0x0827d8ac：varargs（no→%esi、name→%ebx）在 cMyTrace ctor 之前求值并跨
        // ctor 保活 ⇒ P227 全表达式临时链（禁具名临时）。
        cMyTrace(__PRETTY_FUNCTION__, 0x3a5, 0)(
            "ONE_DAY_LETHE : BUY_RESTRICTED_PREMIUM, char(%s), char_no(%d)",
            user->getCurCharacName(), user->getCurCharacNo());
        // ORIG：lethe 判空用函数头 local_48 槽；SkillSave 两条各自重发
        // get_charac_job/getCurCharacSkillR（P48），技能块 = SkillR 视图 +0x46/
        // +0x1de（SkillSlot.h m_skillSlot/m_skillSlot2 权威偏移，直加数形态）。
        if (lethe != 0)
        {
            // [CeraShop12Run 2026-09-11 旋钮D 负结论] ORIG 0x0827d900..0x0827d90a：
            // `call; xor $0x1,%eax; test %al,%al; je`（4.1 视其返回为 bool）；
            // 我方声明 unsigned char（CUserCharacInfo.h:1115）⇒ 4.4 出
            // `cmp $1; setne; test`（+1 条）。试 `(x ^ 1) != 0`（与 `x != 1`
            // 对任意 uchar 等价、xor 只翻 bit0）实测 **4.4 同样规范化成 cmp+setne**
            // （our 693→693 逐条不变）⇒ 回滚。根治需 .h 类型改 bool（见待办）。
            if (user->IsEndLetheAtLogin() != 1)
            {
                lethe->BackupSkill(user, SKILL_TREE_1);
                lethe->BackupSkill(user, SKILL_TREE_2);
            }
            user->m_historyLog.SkillSPInfoSave(user, (eSkillSPInfoSaveReason)0);
            user->m_historyLog.SkillSave(
                reinterpret_cast<const _Mastered_skill*>(
                    user->getCurCharacSkillR() + 0x46),
                (unsigned int)user->get_charac_job(), SKILL_TREE_1,
                (eSkillSaveReason)0);
            user->m_historyLog.SkillSave(
                reinterpret_cast<const _Mastered_skill*>(
                    user->getCurCharacSkillR() + 0x1de),
                (unsigned int)user->get_charac_job(), SKILL_TREE_2,
                (eSkillSaveReason)0);
        }
        break;
    }
    case 0x4f:
    {
        count = G_CDataManager()->m_itemList->GetRestrictCode(item);
        flag = 1;
        cMyTrace(__PRETTY_FUNCTION__, 0x3cb, 0)(
            "GROWTH_CONTRACT : BUY_PREMIUM, char(%s), char_no(%d)",
            user->getCurCharacName(), user->getCurCharacNo());
        break;
    }
    case 0x53:
    {
        count = G_CDataManager()->m_itemList->GetRestrictCode(item);
        flag = 1;
        cMyTrace(__PRETTY_FUNCTION__, 0x3d3, 0)(
            "EMOTICON_CONTRACT : BUY_PREMIUM, char(%s), char_no(%d)",
            user->getCurCharacName(), user->getCurCharacNo());
        if (user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x57) != 0)
            m_owner->_ClosePremium(user, 0x57);
        break;
    }
    case 0x57:
    {
        count = G_CDataManager()->m_itemList->GetRestrictCode(item);
        flag = 1;
        cMyTrace(__PRETTY_FUNCTION__, 0x3db, 0)(
            "EMOTICON_CONTRACT : BUY_PREMIUM, char(%s), char_no(%d)",
            user->getCurCharacName(), user->getCurCharacNo());
        if (user->isAffectedPremium((ENUM_PREMIUM_TYPE)0x53) != 0)
            m_owner->_ClosePremium(user, 0x53);
        break;
    }
    case 0x5c:
    {
        count = G_CDataManager()->m_itemList->GetRestrictCode(item);
        flag = 1;
        cMyTrace(__PRETTY_FUNCTION__, 0x3e5, 0)(
            "CUBE_CONTRACT : BUY_PREMIUM, char(%s), char_no(%d)",
            user->getCurCharacName(), user->getCurCharacNo());
        break;
    }
    default:
        // ORIG default：count==0 时按 itemIdx 现取 restrict code。
        if (count == 0)
{
            count = G_CDataManager()->m_itemList->GetRestrictCode(itemIdx);
    }
        break;
    }

    // ---- 公共收尾（switch 之后）----
    if (count != 0 && user->IsRestrictedGoods(count) != 1)
    {
        user->SetRestrictedGoods(count);
        int ts = premium_helper_function::get_target_server(ptype);
        m_owner->SaveFeaturedIdx(user, count, (int)timeVal, ts);
    }

    // [CeraShop12Run 2026-09-11 旋钮C] ORIG 0x0827dcf5..0x0827dd17 为
    // `test al; je; call CheckFatigue; xor $1; test; je; mov $1,%eax; jmp;
    //  mov $0,%eax; test %al,%al; je` —— 值为单寄存器 %eax 的 && 物化岛，
    // **无 bool 具名槽**（我方旧形 movb ×2 + cmpb + je 多 4 条）。
    // [R3a04 第三轮 SEMANTIC-DIFF 修正] 真值表：ORIG `field==0` 直接跳 false 块；
    //   `call CheckFatigue; xor $1; test; je false` ⇒ 仅当 CheckFatigue()==0(**未**疲劳)
    //   时落穿 true 块（0x0827dd0b `mov $0x1`）⇒ 原码为 `field!=0 && !CheckFatigue()`
    //   （旧注释把 Ghidra 的 `cVar2 == '\x01'` 误读成正极性，实为取反）。
    if (g_SPremiumInfo[ptype].m_needDailyCheck != 0 &&
        !user->CheckFatigue())
        user->gainCurCharacFatigue(1);

    long end = 0;
    premium_helper_function::cal_end_time(user, start, end, timeA, ptype,
                                          flag, resultVal);
    premium_helper_function::send_db(user, ptype, start, end, count);
    WongWork::CHandlePremium::handleSetUserPremium(user, (ENUM_PREMIUM_TYPE)ptype,
                                                   start, end, count, true);
    WongWork::CHandlePremium::handlePremiumEffect(user, (ENUM_PREMIUM_TYPE)ptype);
    if (premium_helper_function::is_notify(user, ptype) != 1)
        m_owner->_sendPremiumPacket(user, type, ptype, (int)(end - timeA));
    premium_helper_function::write_log(user, ptype);
}