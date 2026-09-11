void CInventory::MoveClearAvatar(int a, int b, int c, int d, int e, int f)
{
    // b28d-solo-CInventory：268/273，N7 间接 call 目标寄存器封顶（4.4.7 后端固有）。
    // b28d-37 [N7 封顶，ORIG 0x850?]：间接 call 目标寄存器 %eax↔%edx 互换（P230 换后端可翻）；268/273。
    // [J8InvenPvPWorld.CInventoryWriter 2026-09-11 · 本轮] ①**语义轴·字段错位**：color2 错误
    // 日志实参原写 `expInfo->m_color[1]`，ORIG 0x08509226..0x08509232 为
    // `mov -0x2c(%ebp),%eax; movzwl (%eax),%eax; movswl %ax,%edx` = m_color[0]（与 color1
    // 日志同字段）⇒ 已改回 m_color[0]；②color1 的 `find(...) == end()` 改具名 iterator
    // （ORIG 0x08509145..0x0850916e 为 find(-0x48)→end(-0x40)，4.4 对 `==` 右操作数先求值）
    // ⇒ our 268 = ORIG 268，gap 0，差异块 5→2。残余 2 块 = color2 段 find/end 求值序
    // （复用 color1 具名变量实测 our→270 劣化已回滚，见 color2 处注释）⇒ N 类残差。
    // [K8CInventory 2026-09-11 复核] 268/268、结构/调用序/结构偏移直方图全同；残余 10 条
    // 槽差 = color1/color2 两块的临时槽基**互换**：ORIG color1 组 -0x54/-0x50/-0x4c
    // （0x08509145/0x0850913e/0x08509161）vs 我方 -0x4c/-0x48/-0x44（0xa7ab/a7a8/a7c7）；
    // ORIG color2 组 -0x48/-0x44/-0x40 vs 我方 -0x54/-0x50/-0x40 ⇒ 4.1 先见块占更深槽、
    // 4.4 反是，源码层不可达 ⇒ N 定谳。
    CItem* pClearItem = 0;
    CItem* pTargetItem = 0;
    CItem* localDummy = 0;   // ORIG -0x34 槽：第三个具名局部（初值 0，无使用点，P232）
    // [K8CInventory 2026-09-11 · P331/P-L11d 双试形回退] ①把 colorId1/colorId2 提到函数级
    // （同尺寸标量区，追 ORIG 先声明更深）实测 268/268 但槽差**条数不变**（10→10，仅从
    // (-8)×5+(+12)×3 摊成 (-8)×4+(+8)×2+(-4)+(-16)+(+4)+(+12)）；②P-L11d「同名局部共用
    // 促复用」不适用：ORIG 两组本就不复用（find sret -0x54 vs -0x48），互换的是**块分配
    // 方向**（4.1 先见块占深槽 / 4.4 后见块占深槽）⇒ 源码层不可达，N 定谳。
    if (c != 0)
    {
        pClearItem = G_CDataManager()->find_item(c);
        if (pClearItem != 0 && pClearItem->IsClearAvatar())
        {
            if (f != 0)
            {
                pTargetItem = G_CDataManager()->find_item(f);
                if (pTargetItem != 0 && !pTargetItem->IsClearAvatar())
                {
                    SetClearAvatar(d, f);
                    if (d < 0 || d > 0x19)
                    {
                        return;
                    }
                    Inven_Item* pSrcItem = &m_invenItem[d];
                    stAvatarExpansionInfo_t* expInfo =
                        (stAvatarExpansionInfo_t*)GetAvatarItemMgrW()->GetExpansionInfo(
                            pSrcItem->m_addInfo2);
                    if (expInfo == 0)
                    {
                        return;
                    }
                    int avatarIdx = a - 10;
                    if (avatarIdx < 0 || avatarIdx > 0x68)
                    {
                        return;
                    }
                    Inven_Item* slotItem = &m_pAvatarSlot[avatarIdx];
                    std::map<int, AvatarVariation::colorRGB>& colorMap =
                        G_CDataManager()->m_avatarColor.m_colorMap;
                    int colorId1 = (int)expInfo->m_color[0];
                    // 求值序轴：ORIG 0x08509145..0x0850916e 为 find(-0x48 sret) → end(-0x40)，
                    // 我方 `find(...) == end()` 被 4.4 右操作数先求值（end → find）。具名
                    // iterator 固定左先求值（P227/P221 全表达式临时链反转）。
                    std::map<int, AvatarVariation::colorRGB>::iterator colorIt =
                        colorMap.find(colorId1);
                    if (colorIt == colorMap.end())
                    {
                        LogManager::logFormat(
                            1, "inventory.cpp", __PRETTY_FUNCTION__, 0x21db,
                            "Avatar color1 error. ItemID(%d), colorId(%d)",
                            pSrcItem->m_addInfo2, (int)expInfo->m_color[0]);
                        return;
                    }
                    int colorId2 = (int)expInfo->m_color[1];
                    // [求值序轴 · 本轮定版] ORIG 0x085091df..0x08509208 为 find(-0x48)→end(-0x40)；
                    // 4.4 对 `find(...) == end()` 右操作数先求值（end→find）⇒ 须具名 iterator 固定左先。
                    // 负结论：复用 color1 的 colorIt 实测 our 268→270（槽间
                    // `mov -x(%ebp),%eax; mov %eax,-y(%ebp)` 复制 2 条，4.4 对二次赋值另配 sret 槽）
                    // ⇒ 已回滚；改用**独立具名 colorIt2**（各自 sret 槽，同 ORIG -0x48 布局）⇒
                    // our 268 = ORIG 268，指令序列逐条一致。
                    std::map<int, AvatarVariation::colorRGB>::iterator colorIt2 =
                        colorMap.find(colorId2);
                    if (colorIt2 == colorMap.end())
                    {
                        LogManager::logFormat(
                            1, "inventory.cpp", __PRETTY_FUNCTION__, 0x21e2,
                            "Avatar color2 error. ItemID(%d), colorId(%d)",
                            // 字段错位轴：ORIG 0x08509226..0x0850922c 该 log 实参为
                            // `mov -0x2c(%ebp),%eax; movzwl (%eax),%eax; movswl %ax,%edx`
                            // = expInfo->m_color[0]（ORIG 侧与 color1 log 同字段），
                            // 我方原写 m_color[1] ⇒ 运行期日志打印错误色号。
                            pSrcItem->m_addInfo2, (int)expInfo->m_color[0]);
                        return;
                    }
                    bool colorOk = GetAvatarItemMgrW()->SetAvatarColor(
                        slotItem->m_addInfo2, expInfo->m_color[0], expInfo->m_color[1]);
                    if (!colorOk)
                    {
                        return;
                    }
                    SendAvatarEvent(0x31a, slotItem->m_addInfo2, expInfo->m_color[0],
                                    expInfo->m_color[1], 0);
                }
            }
        }
    }
    if (f != 0)
    {
        if (pTargetItem == 0)
        {
            pTargetItem = G_CDataManager()->find_item(f);
        }
        if (pTargetItem != 0 && pTargetItem->IsClearAvatar())
        {
            SetClearAvatar(d, 0);
        }
    }
}