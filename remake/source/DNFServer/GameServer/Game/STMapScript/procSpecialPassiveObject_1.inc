// [b28d-solo-STMapScript] _Z24procSpecialPassiveObject…：p167 CLEAR 64/64（语义无缺口）；
// verdict DIFF 残差＝N82 槽/寄存器分配（大函数帧伪影，P182）。
// 修法：①make_pair 键用 int 形参 cnt（ORIG 0x089e9db8 make_pair<int&,…>，勿收窄 ushort）；
// ②pool insert 改 std::make_pair(fileIdx,localOrders)（ORIG 走 pair&& move ctor）；
// ③`tok == "[hellparty]"` 正极性包裹（P222，ORIG _ZSteqIcSt…）；④rate 累加改 `+=`。
// [N3Free171to190.S1STMapScript 收尾判词·N] call 多重集 CLEAR（ORIG 64 / 我方 75 次，唯一
// 差集项是 `.rodata` 数据引用假项）⇒ 无缺调用/无语义缺口。残差 = 帧 0x150(我方) vs 0x140
// (ORIG) + 栈布局不可平移：深区 -0x12c/-0x128 vs ORIG -0x124/-0x120（差 8）、浅区 -0xc8/
// -0xd8 vs -0xc4/-0xd4（差 4）⇒ 局部对象排列不同（非单槽平移），P331/P339 声明序旋钮
// 在此形下无效 ⇒ N（N82 栈布局/寄存器分配族）。
bool procSpecialPassiveObject(SPOBJ_ACTION_DATAS& action,
                              STAssignSpecialPassiveObj& owner, int fileIdx)
{
    std::string tok;                                      // -0xc4(%ebp)
    std::priority_queue<int> groupQueue;                  // less<int> 大顶（-0xd4）
    int curTop = 0;                                       // -0x28
    int lastTop = 0;                                      // -0x24
    int fieldIdx = 0;                                     // -0x20
    int slotIdx = 0;                                      // -0x1c

    for (int j = 0; j <= 5; ++j)                          // -0x18；action 六个 int 字段
    {
        if (j != 0)
        {
            (&action.m_type)[j] = ScanInt(static_cast<bool*>(0));   // 0x89e80ac
            continue;
        }

        // j == 0：动作类型名
        if (!ScanStr(&tok))                               // 0x88bc7a2
            continue;
        if (tok == "[monster]")                           // 0x08e1ad7b
        {
            action.m_type = 0;
            continue;
        }
        if (tok == "[trap]")                              // 0x08e1c0de
        {
            action.m_type = 1;
            continue;
        }
        if (tok == "[item]")                              // 0x08e1c0e5
        {
            action.m_type = 2;
            continue;
        }
        if (tok == "[quest]")                             // 0x08e1c0ec
        {
            action.m_type = 3;
            continue;
        }
        // P222：ORIG 用正极性 `tok == "[hellparty]"` 包裹（p167 MISSING _ZSteqIcSt…）
        if (tok == "[hellparty]")                         // 0x08e1c0f4
        {

        // ---- [hellparty] 分支（0x89e7ae3..）----
        std::map<unsigned short, STHellPartyGroupOrder> localOrders;   // -0xf0
        for (;;)
        {
            bool ok = false;                              // -0xd5
            int tvIdx = ScanInt(&ok);                     // -0xf4
            if (!ok)
                break;
            groupQueue.push(tvIdx);

            stHellParty_t* hellParty =
                reinterpret_cast<stHellParty_t*>(Get_instance_stHellParty_t());
            std::map<unsigned short, stHellPartyMonsterGroupList>::iterator git =
                hellParty->m_groupMap.find(static_cast<unsigned short>(tvIdx));
            if (git == hellParty->m_groupMap.end())
            {
                LogManager::logFormat(0, "../RDARScriptMap.cpp", __FUNCTION__,
                                      0xd5c,
                                      "HELLPARTY_LOG : GROUP INDEX NOT FOUND : %d\n",
                                      tvIdx);
                return false;
            }

            // pair 视图 +0x58 = second.m_field54（难度槽位）
            const unsigned char diffSlot = git->second.m_field54;

            bool ok2 = false;
            int rate = ScanInt(&ok2);                     // -0x10 组序号
            if (!ok2)
                return false;
            bool ok3 = false;
            int cnt = ScanInt(&ok3);                      // -0xfc 数量
            if (!ok3)
                return false;
            if (cnt <= 0 || cnt > 9)
            {
                LogManager::logFormat(0, "../RDARScriptMap.cpp", __FUNCTION__,
                                      0xd69,
                                      "HELLPARTY_LOG : GROUP ORDER SCRIPT ERROR : %d\n",
                                      cnt);
                return false;
            }

            STHellPartyGroupRate orderRate;               // -0x108（前两 int 赋值）
            orderRate.m_groupIdx = tvIdx;
            orderRate.m_rate = rate;

            std::map<unsigned short, STHellPartyGroupOrder>::iterator mit =
                localOrders.find(static_cast<unsigned short>(cnt));
            if (mit != localOrders.end())                 // 0x89e7d64 前分支
            {
                // P167：ORIG 调 _Rb_tree_iterator::ptEv（operator->）×2
                mit->second.m_rates.push_back(orderRate);
                mit->second.m_rateByDifficulty[diffSlot] +=
                    static_cast<unsigned short>(rate);
            }
                                            // 0x89e7d64 未命中分支
            else
            {
                STHellPartyGroupOrder newOrder;           // ctor memset 0xc + list@+0xc
                newOrder.m_rates.push_back(orderRate);
                newOrder.m_rateByDifficulty[diffSlot] = static_cast<unsigned short>(
                    newOrder.m_rateByDifficulty[diffSlot] + rate);
                // P167：ORIG 调 make_pair<int&, STHellPartyGroupOrder&>
                //（0x089e9db8）；键必须是 int 形参 cnt，勿加 (unsigned short) 收窄。
                localOrders.insert(std::make_pair(cnt, newOrder));
            }
        }

        // 组序表入池（key=fileIdx；ORIG make_pair(0x10(%ebp), localOrders)）
        Get_instance_groupOrder_map_pool()->insert(
            std::make_pair(fileIdx, localOrders));   // P167/P315：ORIG 走 move ctor（pair&&）

        // 大顶队列去重排空：每个唯一非零 top 处理一轮（0x89e7f31..）
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
            std::map<unsigned short, stHellPartyMonsterGroupList>::iterator git =
                hellParty->m_groupMap.find(static_cast<unsigned short>(curTop));
            // ORIG 此处未检查 end()（保持原样）

            fieldIdx = 0;                                 // 89e8056/89e805d 等价复位
            slotIdx = 0;
            (&action.m_type)[fieldIdx++] = 5;             // 动作类型 5
            while (git->second.m_monsterIdx[slotIdx] > 0) // pair+0x4 数组逐槽展开
            {
                (&action.m_type)[fieldIdx++] = git->second.m_monsterIdx[slotIdx];
                (&action.m_type)[fieldIdx++] = 1;
                (&action.m_type)[fieldIdx++] = 1;
                (&action.m_type)[fieldIdx++] = 1;
                (&action.m_type)[fieldIdx++] = -1;
                owner.m_vec10.push_back(action);
                fieldIdx = 1;                             // 保留 action[0]=5
                ++slotIdx;
            }
            fieldIdx = 0;
            slotIdx = 0;
        }
        return false;                                     // 89e8097 ebx=0
        }
        continue;
    }
    return true;                                          // 89e80da ebx=1
}