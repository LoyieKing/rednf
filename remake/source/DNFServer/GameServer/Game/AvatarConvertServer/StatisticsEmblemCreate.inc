void AvatarConvertServer::StatisticsEmblemCreate(
    const std::vector<std::pair<int, int> >& created)
{
    Packet_Emblem_Create_Statistic packet;
    if (created.size() > 5)
        return;

    // P198③/P212：i 先声明占最浅槽、赋值留到 begin/end 之后（ORIG i=0 在
    // end() 调用后发射）。
    int i;
    std::vector<std::pair<int, int> >::const_iterator it = created.begin();
    std::vector<std::pair<int, int> >::const_iterator endIt = created.end();
    i = 0;
    while (it != endIt)
    {
        // [b28d-E03 2026-09-10 试形回退] ORIG 0x0819327b `mov -0x14(%ebp),%ebx;
        //   call G_CDataManager; mov %ebx,0x4(%esp)`（实参先升格 callee-saved）；
        //   4.4 恒 receiver 先、实参后载 %edx。P231 纯 register 提示实测**无效**
        //   （112 条与基线同、TU 四数不变）⇒ 随判词①封顶（P75/N82附 装载时刻）。
        // [b28d-E03 2026-09-10 轴 A 命中] Main 新配方（const 常量传播）实测有效：
        //   原 `const int itemId/addInfo` ⇒ ours 112 条；**去 const ⇒ 114 条**
        //   （ORIG 115，gap −3→−1）；再加 `register` 反落回 112 ⇒ 保留 `int` 形。
        int itemId = it->first;
        int addInfo = it->second;
        it++;                                    // 语句位后置：4.4 保留 ppEi（探针实证）
        CItem* item = G_CDataManager()->find_item(itemId);
        if (item == 0)
            return;                              // ORIG 中途 return
        if (item->GetItemType() != 0x14)
            continue;                            // 非头像物品不计入
        packet.m_grades[i] = item->get_grade();
        packet.m_slots[i + 6] = addInfo;
        ++i;
        if (i > 5)
            break;                               // ORIG 计数上限 cmp $5
    }
    if (i != (int)created.size())
        return;
    packet.m_count = i;
    // [本波实测] P52 cast 形对照（reinterpret_cast<char*> vs (char*)）重编后读数
    // 逐条相同（110/109、ndiff 13）⇒ 非 cast 选择；残余两条归 N94/N4
    // （`-0x58+4(i+8)` vs `-0x50+4(i+6)` 同址不同分解 + callee-saved 装载时点）。
    GlobalData::s_statistic_proxy_mgr
        ->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x36);
