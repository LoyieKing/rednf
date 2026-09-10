bool CParty::CheckHellDungeonFreepassItemHaveAndDel(CUser* user, bool del)
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS 非 const normal_iterator 遍历 6 项
    //（ptEv/begin/end/pp/ne）+ EXTRA at()/size() ⇒ ORIG 为迭代器遍历而非下标形；
    // 改 iterator 实测 identical 全 TU 回归，暂封 N。@0x85bc86e
    int slotResult = 0;                       // ORIG 0x085bc87c：local_14=0 在函数最前
    if (m_hellWorldMap == 0)
    {
        int worldIdx = G_GameWorld()->GetWorldMapIndex(user);
        if (0 < worldIdx)
            m_hellWorldMap = G_CDataManager()->m_worldMapList.find_world_map(worldIdx);
        if (m_hellWorldMap == 0)
            return false;
    }
    // b28d-solo-CParty：P167 调用点差集实证 ORIG 为 const_iterator 遍历
    // （08193912 = __normal_iterator<const pair*>::operator->() const、
    //  0823dcd0 = operator++()、080dd5b6 = end()、0838793c = operator!=<const*,int*>；
    //  我们下标形多 at()/size()）。P28 增量侧：ORIG 用前置 ppEv ⇒ 写 ++it。
    for (std::vector<std::pair<int, int> >::const_iterator it =
             m_hellWorldMap->m_hellPassItemList.begin();
         it != m_hellWorldMap->m_hellPassItemList.end(); ++it)
    {
        // P0/P183：ORIG 每处重新 it.operator->()（0x85bc922/95d/9a8 三次），无具名缓存。
        slotResult = user->getCurCharacInvenR()->check_item_exist(it->first);
        if (slotResult == -1)
            continue;
        // ORIG 0x085bc94d `cmpb $0,-0x3c; je <末尾>` ⇒ 正极性 `if (del) {...} else break;`
        //（写 `if (!del) break;` 4.4 恒产 xor/test/jne，N122）。
        if (del)
        {
            // ORIG 0x085bc99b `xor $1; test %al; je <成功>`：delete_item 返回 bool，
            // 失败打 trace、成功才 SendUpdateItem（旧码丢弃返回值、trace 无条件前置 —— 真 bug）。
            // [G7 语义修正·ORIG 直证 0x085bc9a1/0x085bca25] 失败支 `jmp 85bca27` = ++it
            // （continue 继续遍历）；成功支 SendUpdateItem 后 `jmp 85bca6d` = 循环收尾
            // ⇒ 原码删除成功后 **break 退出循环**（旧码成功支落回 ++it 继续遍历，
            //   slotResult 被后续项覆盖 ⇒ 返回值与后续删除行为均不同，真语义 bug）。
            if (!user->getCurCharacInvenW()->delete_item(
                    static_cast<INVEN_TYPE>(1), slotResult, it->second,
                    static_cast<eItemDelReason>(0x17), true))
            {
                // P227/P37：ORIG 0x085bc9a2..0x085bc9df 先求 op() 实参（itemId→%esi、
                // characNo→%ebx）再构造 cMyTrace ⇒ 全表达式临时链形。
                cMyTrace(__PRETTY_FUNCTION__, 0x44fb, 5)(
                    "CParty::UseSealDoorItems, freepass delete_item failed , User ch=%d , %d %d",
                    user->getCurCharacNo(), slotResult, it->first);
                continue;
            }
            user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, slotResult);
            break;
        }
        else
            break;   // 仅检查存在性
    }
    // P56：ORIG 0x085bca71 `jle; mov $1; jmp; mov $0` ⇒ 分支形，非 setg。
    if (0 < slotResult)
        return true;
    return false;
