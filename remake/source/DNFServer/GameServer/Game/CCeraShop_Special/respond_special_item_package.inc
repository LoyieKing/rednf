// ---- respond_special_item_package —— ORIG 0x0827c204 ----
// 完整还原。此前 BLOCKED 判定的"匿名 static helper 0x827e06c..0x827e320"实为 STL 模板实例
// （std::vector<tagPackageItemSelection> / std::vector<STStackablePackageData> / iterator 操作），
// 由编译器依本函数 STL 用法自动生成，无需手写。依赖的 CCeraShop 方法
// (_onBuySpecialItem/_processAdditionalItem) 声明于 CCeraShop.h，实现由 CCeraShop 还原批次提供。
// 包字段/随机箱统计调用链：CStackableItem::m_pkg21c(+0x21c) 包列表、m_pkg234(+0x234)
// 逐选择包表，ceraParam 的 vector<tagPackageItemSelection>(+0xc)/vector<pair<int,int>>(+0x0)，
// 逐包 _processAdditionalItem（子物品 vtable+0x60 槽按 CItem::set_parent_booster_item_id
// 真实虚方法直调）+ CUser::SendRandomBoxStatistic(0x380/0x28982d/0x289749)。
// b28d-D42 verdict=DIFF [N82/N89] ORIG 0x0827c204 orig/ours=557/547 ndiff=497。
// 巨件帧/槽平移 + tagPackageItemSelection STL 内联差（N89）；P167 调用点已核。
// b28d-solo-CCeraShop_Special verdict=DIFF [N82/N89] 557/547；P167 CLEAR 90/90，余 STL 内联/帧槽（§6.2 语义完成封顶）
// b28d-sweepCCeraShop（本轮）557/557 gap=0：①switch 三 case 按 ORIG 块址序重排（P324，
// 见下）；②两处 error 日志的 `accStr` 具名局部删去、内联回实参位（P329，复现 ORIG
// characNo→ebx 跨调用保活）；③守卫改德摩根正极性 `outer>=size || inner>=size`（P258，
// 复现 ORIG `jae→mov $1 / jb→mov $0` 失败位物化）。剩余残差 = 槽号分配（ORIG deadSlot
// 独立 -0x34 vs 4.4 复用 count 槽 -0x40，N82 不复用）+ 尾部 1 条对齐 nop（§6.1 豁免），
// opcode 多重集差异仅此 nop；p167 CLEAR、虚槽 vtable+0x60 两侧一致。
// + ④pkgTable 声明移入 `if(!m_sel.empty())` 块内（ORIG 0x0827c3c9 在 empty() 检查后
// 赋值，P0 槽序）；⑤pairs 循环缓存 `pitEnd`（P13：ORIG 0x0827c7bd 循环外 begin/end
// 各一次，我方原在条件里重发 end()）⇒ 修后 **jcc 目标下标与 ORIG 逐位全等（32/32）**、
// 同长函数 jcc 目标比对全库 0 不匹配（Main 高优先级排查）。
void SpecialItemHandler::respond_special_item_package(
    int, const STSpecailItem_Param& param, STSpecailItem_Result&)
{
    CUser* user = static_cast<CUser*>(param.m_user);
    int itemIdx = param.m_itemIdx;
    std::vector<Inven_Item>* items = static_cast<std::vector<Inven_Item>*>(param.m_items);
    WongWork::CCeraShop::stCeraShopItemParam_t* cera =
        static_cast<WongWork::CCeraShop::stCeraShopItemParam_t*>(param.m_ceraParam);
    // ORIG 0x0827c236 `mov 0x14(%eax),%eax; mov %eax,-0x3c(%ebp)`：count 读入槽后
    // 不再被读（-0x3c 随后被 restrictCode 覆盖）——4.1 保留的死读取（P171）。
    int count = param.m_count;
    (void)count;
    long time = param.m_time;

    if (itemIdx == 0x1b)
    {
        // 疯狂升级事件包：header 0xb4 + body "0612\0"+accId+characNo 入队列 4。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("SpecialItemHandler.cpp", 0xc3),
                               true);
        guard.operator*()->operator<<((int)0xb4);
        guard.operator*()->operator<<(-1);
        // ORIG 0x0827c2f5/0x0827c30e 为 `mov -0x2c(%ebp),%edx; mov %eax,0x5(%edx)`
        // 具名结构字段写（裸指针 buf+5 会退化成 lea+间接 store）⇒ 用已有
        // SaveCrazyLevelupPayloadView 具名成员（P82/P211）。
        SaveCrazyLevelupPayloadView* buf = reinterpret_cast<SaveCrazyLevelupPayloadView*>(
            guard->GetInBuffer<SIG_SAVE_CRAZY_LEVELUP_EVENT>());
        memcpy(buf, "0612\0", 5);                       // DAT_08c07e5a
        buf->m_accId = user->get_acc_id();
        buf->m_characNo = user->get_charac_no(-1);
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(4), guard);
    }

    m_owner->_onBuySpecialItem(user, (ENUM_CERA_SPECIALITEM)5,
                               static_cast<unsigned long>(itemIdx));

    // ORIG 0x0827c37b `movl $0x0,-0x34(%ebp)`：写后不读的死局部（P171 照抄）。
    int deadSlot = 0;
    (void)deadSlot;

    // ORIG 实证元素类型（respond_special_item_package 反编译）：
    //   +0x21c = std::vector<STStackablePackageData>（包基础物表）
    //   +0x234 = std::vector<std::vector<STStackablePackageData>>（每选择包组表）
    // 头文件 m_pkg21c/m_pkg234 当前为 vector<char> 占位（元素类型修正涉共享头
    // CStackableItem.h 的多 TU 验证，另行批次）；此处以具名字段锚定偏移、按
    // ORIG 类型取视图。
    CStackableItem* stackItem =
        static_cast<CStackableItem*>(G_CDataManager()->find_item(itemIdx));
    std::vector<STStackablePackageData> pkgList(
        *reinterpret_cast<std::vector<STStackablePackageData>*>(&stackItem->m_pkg21c));
    // ORIG 无 selections/pairs 具名引用槽：每处按 `mov -0x40(%ebp),%eax;
    // lea 0xc(%eax),%edx`（4 次）重算 cera->m_sel / cera->m_pairs（P48 无 CSE）。
    if (!cera->m_sel.empty())
    {
        // ORIG 0x0827c3c9..0x0827c3d1：pkgTable 赋值在 `m_sel.empty()` 检查
        // **之后**（index 128-130）⇒ 原码声明在 if 块内，非函数级（P0 槽序）。
        // ORIG operator[] 为 const 版（local_2c const 视图）+ inner.at() const。
        const std::vector<std::vector<STStackablePackageData> >* pkgTable =
            reinterpret_cast<const std::vector<std::vector<STStackablePackageData> >*>(
                &stackItem->m_pkg234);
        if (cera->m_sel.size() > 1)
        {
            // 多选择：逐项记录 error 日志（kdd372418Log 前缀，ORIG 原样）。
            for (std::vector<tagPackageItemSelection>::iterator it = cera->m_sel.begin();
                 it != cera->m_sel.end(); ++it)
            {
                // P329：ORIG 0x0827c5da..0x0827c605 右到左求值（characNo 先于
                // NumberToString），accStr 无具名槽 ⇒ 表达式内联回实参位。
                LogManager::logFormat(
                    1, "SpecialItemHandler.cpp",
                    __PRETTY_FUNCTION__,
                    0xdb, "kdd372418Log::Package Selection item error (%s,%d,%d,%d,%d)",
                    NumberToString(user->get_acc_id(), 0), user->get_charac_no(-1),
                    it->m_itemIdx, (int)it->m_outer, (int)it->m_inner);
            }
        }
        // 主选择循环：找 itemIdx 匹配项，校验 pkgTable 索引，push 到 pkgList 并 erase。
        for (std::vector<tagPackageItemSelection>::iterator it = cera->m_sel.begin();
             it != cera->m_sel.end(); )
        {
            if (it->m_itemIdx == itemIdx)
            {
                // ORIG 0x0827c53a..0x0827c59d：`jae→mov $1 / jb→mov $0 / test;je 成功`
                // = 失败位 1 的德摩根物化（P258：`A||B` 形而非 `!(a&&b)`）⇒ 写
                // 正极性 `outer>=size || inner>=size` 失败臂，成功臂直落。
                if (it->m_outer >= pkgTable->size() ||
                    it->m_inner >= (*pkgTable)[it->m_outer].size())
                {
                    // P329：右到左内联求值（ORIG 0x0827c5e8/0x0827c5ef）。
                    LogManager::logFormat(
                        1, "SpecialItemHandler.cpp",
                        __PRETTY_FUNCTION__,
                        0xe7, "Package Selection error (%s,%d,%d,%d,%d)",
                        NumberToString(user->get_acc_id(), 0), user->get_charac_no(-1),
                        it->m_itemIdx, (int)it->m_outer, (int)it->m_inner);
                    break;
                }
                pkgList.push_back((*pkgTable)[it->m_outer].at(it->m_inner));
                it = cera->m_sel.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    // 逐包处理：配对 itemIdx→count 消费、set_parent_booster_item_id（vtable+0x60 槽）、
    // _processAdditionalItem、SendRandomBoxStatistic。
    // ORIG：pkgList 循环用 const_iterator 对（begin/end 各一次转换构造，
    // it-> 逐点重发 operator->const，P157/P183）；pairs 消费前先 empty() 短路。
    std::vector<STStackablePackageData>::const_iterator itBeg = pkgList.begin();
    std::vector<STStackablePackageData>::const_iterator itEnd = pkgList.end();
    for (; itBeg != itEnd; ++itBeg)
    {
        unsigned char local25 = 0;
        if (!cera->m_pairs.empty())
        {
            // P13：ORIG 0x0827c7bd..0x0827c7d5 在循环**外**各求一次 begin()/end()
            // （sret -0x88/-0x8c），循环条件只做 operator!=；我方原形在条件里
            // 每次重发 end() ⇒ 循环入口块错位。缓存 pitEnd。
            std::vector<std::pair<int, int> >::iterator pit = cera->m_pairs.begin();
            std::vector<std::pair<int, int> >::iterator pitEnd = cera->m_pairs.end();
            for (; pit != pitEnd; )
            {
                if (pit->first == itBeg->m_field0)
                {
                    local25 = static_cast<unsigned char>(pit->second);
                    cera->m_pairs.erase(pit);
                    break;
                }
                else
                {
                    ++pit;
                }
            }
        }

        CItem* sub = G_CDataManager()->find_item(itBeg->m_field0);
        // ORIG (**(code**)(*sub + 0x60))(sub, itemIdx)：CItem vtable 序核对槽 +0x60
        // = set_parent_booster_item_id(unsigned long)，按真实虚方法直调。
        if (sub != 0)
            sub->set_parent_booster_item_id(static_cast<unsigned long>(itemIdx));

        m_owner->_processAdditionalItem(
            user, static_cast<unsigned long>(itBeg->m_field0),
            static_cast<unsigned short>(itBeg->m_field4), local25, 0xff, *items, *cera, 0);

        if (sub != 0)
            sub->set_parent_booster_item_id(0);

        // 随机箱统计调用链（ORIG 0x0827c8fc..0x0827c976）：`movl $0x0,-0x1c(%ebp)`
        // + 单次 operator-> 取值 + 顺序 cmp 链（switch 形）；每个 case 把常量赋回
        // 具名 statKind 槽后传参（`movl $N,-0x1c; mov -0x1c,%eax; mov %eax,(%esp)`）。
        // P324：case 叶块物理序 = 书写序；ORIG 块序 0x380(+0x720)→0x28982d(+0x73d)
        // →0x289749(+0x759)，末块 fall through 到循环增量（无 jmp）⇒ 按块址序书写。
        int statKind = 0;
        switch (itBeg->m_field0)
        {
        case 0x380:
            statKind = 0;
            user->SendRandomBoxStatistic(statKind, (ENUM_RANDOMBOX_ACTION_KIND)0);
            break;
        case 0x28982d:
            statKind = 1;
            user->SendRandomBoxStatistic(statKind, (ENUM_RANDOMBOX_ACTION_KIND)0);
            break;
        case 0x289749:
            statKind = 2;
            user->SendRandomBoxStatistic(statKind, (ENUM_RANDOMBOX_ACTION_KIND)0);
            break;
        }
    }

    // 收尾：GetRestrictCode → SetRestrictedGoods/SaveFeaturedIdx → SaveInventory。
    // P0/P171（与 _processBoosterSelect/respond_special_item_booster 同型）：ORIG
    // 0x0827c9xx `local_40`（= param.m_count 的具名槽）被 GetRestrictCode 返回值
    // 覆写（Ghidra C `local_40 = CItemList::GetRestrictCode(...)`）⇒ 写回 count。
    count = G_CDataManager()->m_itemList->GetRestrictCode(itemIdx);
    if (count != 0)
    {
        if (!user->IsRestrictedGoods(count))
        {
            user->SetRestrictedGoods(count);
            m_owner->SaveFeaturedIdx(user, count, static_cast<int>(time), 0);
        }
    }
    user->SaveInventory();
}