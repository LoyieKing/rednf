// ===========================================================================
// OnlyOrigBatch12 追加：procSpecialPassiveObjectVerifyError —— ORIG T
// 0x089e6f62。动作验证状态机：j==0 读动作类型名（[monster]/[trap]/[item]/
// [quest]/[hellparty]，未知记 CRITICAL_ERROR）；j==1 读索引并核对
// （monster/trap 查 checkValidIndex，item 查 assignItems 未用标记 0xffff0001）；
// j>=2 直接 ScanInt。[hellparty] 分支与 procSpecialPassiveObject 同构，
// 处理完立即返回 hasError（ORIG 控制流）。
// ===========================================================================
// [b28d-solo-STMapScript] _Z35procSpecialPassiveObjectVerifyError…：p167 CLEAR 74/74
//（语义无缺口）；verdict DIFF 残差＝N82 槽/寄存器分配（大函数帧伪影，P182）。
// 修法：①pool insert 改 std::make_pair；②`tok == "[hellparty]"` 正极性包裹（P222）；
// ③priority_queue 第二实参 static_cast<vector<int>&&>（ORIG C1ERKS4_OS2_ move 重载）；
// ④assignItems 步进改后置 it++（ORIG _List_iterator ppEi）；⑤rate 累加改 `+=`。
// [N3Free171to190.S1STMapScript 收尾判词·N] call 多重集 CLEAR（ORIG 74 / 我方 97 次，差集
// 仅 `.rodata` 假项）⇒ 无语义缺口；残差 = 帧差 0x10 + 同一栈布局族（同 procSpecialPassive
// Object）⇒ N（N82/N89）。
bool procSpecialPassiveObjectVerifyError(SPOBJ_ACTION_DATAS& action,
                                         const char* path,
                                         STScriptFileList& monsterList,
                                         STScriptFileList& trapList,
                                         std::list<STAssignItem>& assignItems,
                                         STAssignSpecialPassiveObj& owner,
                                         int fileIdx)
{
    bool hasError = false;                     // -0x35
    std::string tok;                           // -0xd4
    std::vector<int> heapVec;                  // -0xd0
    // P167/P234：ORIG 选 priority_queue(less&, vector&&)（MISSING C1ERKS4_OS2_），
    // heapVec 此后不再使用 ⇒ 用 static_cast<T&&> 选中 move 重载（零 out-of-line call）。
    std::priority_queue<int> groupQueue(std::less<int>(),
                                        static_cast<std::vector<int>&&>(heapVec));  // -0xe4
    int fieldIdx = 0;                          // -0x2c
    int slotIdx = 0;                           // -0x28
    int curTop = 0;                            // -0x34
    int lastTop = 0;                           // -0x30

    for (int j = 0; j <= 5; ++j)               // -0x24
    {
        if (j == 0)
        {
            if (!ScanStr(&tok))
                continue;
            if (tok == "[monster]")            // 0x8e1ad7b
            {
                action.m_type = 0;
                continue;
            }
            if (tok == "[trap]")               // 0x8e1c0de
            {
                action.m_type = 1;
                continue;
            }
            if (tok == "[item]")               // 0x8e1c0e5
            {
                action.m_type = 2;
                continue;
            }
            if (tok == "[quest]")              // 0x8e1c0ec
            {
                action.m_type = 3;
                continue;
            }
            // P222：ORIG 用正极性 `tok == "[hellparty]"`（ORIG 5 处 eq 之一）
            if (tok == "[hellparty]")          // 0x8e1c0f4
            {
            // ---- [hellparty] 分支（同 procSpecialPassiveObject，错误记 hasError）----
            std::map<unsigned short, STHellPartyGroupOrder> localOrders;  // -0x100
            for (;;)
            {
                bool ok = false;               // -0xe5
                int tvIdx = ScanInt(&ok);      // -0x104
                if (!ok)
                    break;
                groupQueue.push(tvIdx);

                stHellParty_t* hellParty =
                    reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t());
                std::map<unsigned short, stHellPartyMonsterGroupList>::iterator git =
                    hellParty->m_groupMap.find(static_cast<unsigned short>(tvIdx));

                int rate = ScanInt(&ok);       // -0x1c（复用 ok）
                if (!ok)
                {
                    hasError = true;
                    break;
                }
                int cnt = ScanInt(&ok);        // -0x10c
                if (!ok)
                {
                    hasError = true;
                    break;
                }
                if (cnt <= 0 || cnt > 9)       // 0x89e71da
                {
                    LogManager::logFormat(0, "../RDARScriptMap.cpp",
                                          __FUNCTION__, 0xca8,
                                          "HELLPARTY_LOG : GROUP ORDER SCRIPT ERROR : %d\n",
                                          cnt);
                    hasError = true;
                }

                STHellPartyGroupRate orderRate;   // -0x118/-0x114
                orderRate.m_groupIdx = tvIdx;
                orderRate.m_rate = rate;

                // [R4a06 ④ 符号性修复] 同 procSpecialPassiveObject：ORIG 0x089e72f1/0x089e72fb/
                //   0x089e7343/0x089e734d 同形（`movzbl` 读 + `movsbl` 符号扩展）⇒ signed char。
                const char diffSlot = git->second.m_field54;

                std::map<unsigned short, STHellPartyGroupOrder>::iterator mit =
                    localOrders.find(static_cast<unsigned short>(cnt));
                if (mit != localOrders.end())
                {
                    mit->second.m_rates.push_back(orderRate);
                    mit->second.m_rateByDifficulty[diffSlot] +=
                        static_cast<unsigned short>(rate);
                }
                else
                {
                    STHellPartyGroupOrder newOrder;
                    newOrder.m_rates.push_back(orderRate);
                    newOrder.m_rateByDifficulty[diffSlot] =
                        static_cast<unsigned short>(
                            newOrder.m_rateByDifficulty[diffSlot] + rate);
                    localOrders.insert(std::make_pair(cnt, newOrder));   // P167 同上
                }
            }
            Get_instance_groupOrder_map_pool()->insert(
                std::make_pair(fileIdx, localOrders));   // P167：ORIG make_pair<int&,map&>

            while (!groupQueue.empty())
            {
                curTop = groupQueue.top();
                if (curTop == lastTop || curTop == 0)
                {
                    groupQueue.pop();
                    continue;
                }
                lastTop = curTop;

                stHellParty_t* hellParty =
                    reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t());
                std::map<unsigned short, stHellPartyMonsterGroupList>::iterator git2 =
                    hellParty->m_groupMap.find(static_cast<unsigned short>(curTop));

                fieldIdx = 0;
                (&action.m_type)[fieldIdx++] = 5;
                while (git2->second.m_monsterIdx[slotIdx] > 0)
                {
                    (&action.m_type)[fieldIdx++] =
                        git2->second.m_monsterIdx[slotIdx];
                    (&action.m_type)[fieldIdx++] = 1;
                    (&action.m_type)[fieldIdx++] = 1;
                    (&action.m_type)[fieldIdx++] = 1;
                    (&action.m_type)[fieldIdx++] = -1;
                    owner.m_vec10.push_back(action);
                    fieldIdx = 1;
                    ++slotIdx;
                }
                fieldIdx = 0;
                slotIdx = 0;
            }
            return hasError;                   // 89e762c：hellparty 直接收尾
            }
            LogManager::logFormat(0, "../RDARScriptMap.cpp",
                                  __FUNCTION__, 0xced,
                                  "CRITICAL_ERROR : \xc0\xdf\xb8\xf8\xb5\xc8 [special passive object] \xc5\xc2\xb1\xd7 - \xbe\xf8\xb4\xc2 \xc4\xab\xc5\xd7\xb0\xed\xb8\xae - %s\n",
                                  path);
            hasError = true;
            continue;
        }
        else if (j == 1)
        {
            (&action.m_type)[j] = ScanInt(static_cast<bool*>(0));
            switch (action.m_type)
            {
            case 0:
                if (!checkValidIndex((&action.m_type)[j], monsterList))   // 0x10(%ebp)
                {
                    LogManager::logFormat(0, "../RDARScriptMap.cpp",
                                          __FUNCTION__, 0xcfc,
                                          "CRITICAL_ERROR : \xc0\xdf\xb8\xf8\xb5\xc8 [special passive object] \xc5\xc2\xb1\xd7 `\xb8\xf3\xbd\xba\xc5\xcd` - %s\n",
                                          path);
                    hasError = true;
                }
                break;
            case 1:
                if (!checkValidIndex((&action.m_type)[j], trapList))      // 0x14(%ebp)
                {
                    LogManager::logFormat(0, "../RDARScriptMap.cpp",
                                          __FUNCTION__, 0xd04,
                                          "CRITICAL_ERROR : \xc0\xdf\xb8\xf8\xb5\xc8 [special passive object] \xc5\xc2\xb1\xd7 `\xc6\xae\xb7\xa6` - %s\n",
                                          path);
                    hasError = true;
                }
                break;
            case 2:
                if ((&action.m_type)[j] == -1)
                    break;
                if (assignItems.size() >
                    static_cast<unsigned int>((&action.m_type)[j]))
                {
                    int target = (&action.m_type)[j];
                    std::list<STAssignItem>::iterator it = assignItems.begin();
                    for (int k = 0; k < target; ++k)
                        it++;                         // P167：ORIG ppEi（后置）
                    if (it->m_field00 != 0xffff0001)
                    {
                        it->m_field00 = 0xffff0001;
                    }
                    else
                    {
                        hasError = true;
                        LogManager::logFormat(0, "../RDARScriptMap.cpp",
                                              __FUNCTION__, 0xd1d,
                                              "CRITICAL_ERROR : \xc0\xdf\xb8\xf8\xb5\xc8 [special passive object] \xc5\xc2\xb1\xd7 `\xbe\xc6\xc0\xcc\xc5\xdb` - \xc0\xcc\xb9\xcc \xbb\xe7\xbf\xeb\xc7\xd1 \xbe\xc6\xc0\xcc\xc5\xdb\xb8\xae\xbd\xba\xc6\xae - %s\n",
                                              path);
                    }
                }
                else
                {
                    hasError = true;
                    LogManager::logFormat(0, "../RDARScriptMap.cpp",
                                          __FUNCTION__, 0xd23,
                                          "CRITICAL_ERROR : \xc0\xdf\xb8\xf8\xb5\xc8 [special passive object] \xc5\xc2\xb1\xd7 `\xbe\xc6\xc0\xcc\xc5\xdb` - \xbe\xf8\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb\xb8\xae\xbd\xba\xc6\xae \xbb\xe7\xbf\xeb\xc7\xcf\xb7\xc1 \xc7\xd4- %s\n",
                                          path);
                }
                break;
            }
        }
        else
        {
            (&action.m_type)[j] = ScanInt(static_cast<bool*>(0));
        }
    }
    return hasError;                           // 89e78f2
}