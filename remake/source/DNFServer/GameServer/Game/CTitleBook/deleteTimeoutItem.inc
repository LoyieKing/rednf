void CTitleBook::deleteTimeoutItem(
    const std::vector<std::pair<int, int> >& list)
{
    // b28d-solo-CTitleBook 终判（本波归零，175=175 AE，p167 差集 CLEAR 33=33）：
    // 旧「N121 封顶」判词作废——残差根因 = `continue`。
    // 4.4 为 continue/goto 到循环 latch 的跳转在目标（`addl`）前插 1 字节对齐 nop
    // ⇒ 体尾 `jmp .+2; nop` 恒在（N121 假象）。ORIG 4.1 是第二分支体尾 fall-through
    // ⇒ 改正向包裹（P257）`if (item->m_addInfo != 0) { 体 }`（语义等价，消 continue）
    // 后逐条全等；探针 /tmp/ctb_probe.cpp p1(有 continue, 177) vs p3(正向包裹, 175)。
    std::vector<std::pair<int, int> > toDelete;
    for (int category = 0; category < 4; ++category)
    {
        for (int i = 0; i < TITLE_BOOK_MAX[category]; ++i)
        {
            // b28d-solo-CTitleBook：ORIG 0x08641285 缓存 `const Inven_Item* item`
            //（-0xc）——make_pair 实参取 `lea 0x2(%eax)`（const 左值 ⇒ 模板参
            // `<unsigned long const&, int const&>`）；reset 则重新完整展开
            // `m_category[category][i]`（P183，故 item 必须为 const 指针）。
            const Inven_Item* item = m_category[category] + i;
            if (item->m_addInfo != 0)
            {
                if (_checkTimeoutItem(*item) ||
                    (_isMatchedItem(item->m_addInfo, list) != -1))
                {
                    m_category[category][i].reset();
                    alter();
                }
                // §4.3 跨TU隔离例外：本 TU CDataManager 为最小镜像类（仅 find_item）；
                // 真定义 CDataManager.h:3665 `DeleteInvalidItemScript m_deleteInvalidItem; // +0xb658`。
                // 镜像内补该字段会把其落 +0x00 而非 +0xb658 → 真实地址漂移 bug，故保留
                // 具名视图 + 文档化基址访问（同 CItemLock/CMailBoxHelper 判例）。
                else if (G_CDataManager()->m_deleteInvalidItem.IsInvalidItem2Delete(
                             item->m_addInfo))
                {
                    // ORIG make_pair<unsigned long const&, int const&>：实参直接取
                    // m_addInfo 地址（+0x02）⇒ 用同尺寸 unsigned long 左值视图
                    //（P266 先例；布局/取值不变）。
                    toDelete.push_back(
                        std::make_pair(
                            *reinterpret_cast<const unsigned long*>(
                                &item->m_addInfo),
                            item->m_addInfo2));
                    m_category[category][i].reset();
                    alter();
                }
            }
        }
    }
    if (m_user)
    {
        m_user->RewardItem2DeleteInvalidItem(
            std::string("TitleBook"), toDelete);
    }
}