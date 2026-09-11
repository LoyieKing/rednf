// [b28d-solo-AvatarConvertServer] DIFF 封顶（ORIG 0x081931dc，115 vs 114，p167 CLEAR）。
// 修法：权威 Packet_Emblem_Create_Statistic C1 W 0x08110bdc（消原 char[0x36]+memset+
// 字节写）+ m_grades/m_values 具名数组（P311/P95）+ 语句位 `it++` 复现 ppEi（探针实证，
// N113 仅适用 for 增量段）+ int i 复现 cmpl $5/jg + 代理内联（消 TU-local helper）。
// 残差（语义完成、byte 层 N 封顶）：① find_item 实参求值序——ORIG 0x0819327b
// `mov -0x14(%ebp),%ebx; call G_CDataManager; mov %ebx,0x4(%esp)`，4.4 恒 receiver 先
// （P75/N82附）② `mov -0x18(%ebp),%ebx; call size; cmp %eax,%ebx`（N22）③ m_values 的
// disp/索引分解 -0x58+4(i+8) vs -0x50+4(i+6)（N94/N4 族）。
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
}