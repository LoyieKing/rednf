// ============================================================================
// eventReward::sendPostalReward —— ORIG T 0x080f0976
// ============================================================================
int eventReward::sendPostalReward(CUser& user, rewardKey key, int count,
                                  const char* title, const char* body)
{
    if (this->m_map0.empty())
        return 0;
    std::map<rewardKey, std::map<int, std::vector<itemData> > >::iterator it =
        this->m_map0.find(key);
    if (it == this->m_map0.end())
        return 0;
    // [L10-2] 同 rewardWriteUserInven：ORIG 单次解引用（消 p167 EXTRA const ptEv）。
    std::map<int, std::vector<itemData> >& subMap = it->second;
    std::map<int, std::vector<itemData> >::iterator fit = subMap.find(count);
    if (fit == subMap.end())
        return 0;
    std::vector<itemData>& vec = fit->second;
    if (vec.empty())
        return 0;
    // [L10-2] P28 增量侧：ORIG 调 `normal_iterator::ppEi`（后置）⇒ 源形 `vit++`；
    // `++vit` 走 ppEv 会缺 ppEi 并多发 ppEv（p167 MISSING/EXTRA 实证）。
    for (std::vector<itemData>::iterator vit = vec.begin(); vit != vec.end(); vit++)
    {
        const int itemIdx = vit->m_itemIdx;
        Inven_Item inven;
        CItem* citem = G_CDataManager()->find_item(itemIdx);
        // [本波] serverGroup 声明为 ENUM_SERVER_GROUP（而非 int + 实参位 cast）：
        // ORIG 0x080f0c8c `mov -0x20(%ebp),%edx; mov %edx,0x1c(%esp)` 直读槽、实参位
        // 无额外临时 ⇒ 261→259 条（=ORIG）。残余 1 条：我方 4.4 在 0x080f0c39 之后多发
        // `mov -0x1c(%ebp),%edi`（expDate 提前保活），而 ORIG 0x080f0c93 是实参段直读
        // `mov -0x1c(%ebp),%edx`——属 4.4 实参保活族；`({ expDate; })` 包裹与去 const
        // 两变体实测零变化（均 259）。
        ENUM_SERVER_GROUP serverGroup = static_cast<ENUM_SERVER_GROUP>(0);
                // ORIG +0x159..+0x185：IsIntegratedPvPBaseChannel 先测，test %al 无取反；
        // 真分支（集成 PvP）取 user.GetServerGroup，假分支取 G_CEnvironment+0x378。
        if (G_GameWorld()->IsIntegratedPvPBaseChannel())
                        serverGroup = static_cast<ENUM_SERVER_GROUP>(user.GetServerGroup());
        else
            serverGroup = static_cast<ENUM_SERVER_GROUP>(
                G_CEnvironment()->m_serverEnvir.m_serverGroup);
        if (citem != 0)
        {
            inven.m_addInfo = itemIdx;              // item+0x02
            citem->make_item(inven);                // vtable+0x8
            applyItemOption(inven, citem, *vit);
            char buf[256] = { '\0' };
            // ORIG +0x243：`call get_charac_level; mov %eax,%ebx`（无槽，跨 findString 保活）
            // ⇒ P226 语句表达式实参（值升格 callee-saved）。
            OS_API::snprintf(buf, 0xff,
                             g_scriptStringManager_.findString(4, body, 0),
                             ({ user.get_charac_level(); }));
            // ORIG +0x2ab：读的是 inven.m_addInfo（-0x87 = inven+0x02），非 itemIdx 形参副本；
            // 且短路带 bool 物化岛（mov $1/jmp/mov $0 + test）⇒ P226 语句表达式。
            if (({ inven.m_addInfo != 0 && citem->is_stackable(); }))
            {
                const int expDate = citem->getExpirationDate();
                // ORIG +0x2c2..+0x2e6：strlen→%esi、getCurCharacNo→%ebx、title 串→%eax
                // 均无栈槽 ⇒ 实参位内联 + P226 语句表达式升格。
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                    g_scriptStringManager_.findString(4, title, 0), inven, 0,
                    ({ user.getCurCharacNo(); }), buf, ({ (int)strlen(buf); }),
                    expDate, serverGroup, 0, 1);
            }
        }
    }
    return 1;
}