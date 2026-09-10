bool CQuestShop::SetTagCharacQuestShopData(CHARAC_QUEST_SHOP_DATA* data)
{
    for (int i = 0; i <= 0x4c; ++i)                 // cmp $0x4c setle
    {
        // b28c-L10：完整类型具名成员直访问（+0x0c 计数视角，见 CHARAC_QUEST_SHOP_DATA.h
        // 匿名 union）。ORIG 0x85efdc0 装载序为「i 先、base 后」且 make_pair 实例
        // `IRiRt`（非 const ushort 左值）；reinterpret_cast 视图形在 4.4 反装为
        // 「base 先 + 多一条 add」（ndiff 17→0 见探针 /tmp/l10_probe6.cpp fH）。
        if (data->m_statusCounts[i] == 0)
            continue;                               // movzwl + test ax / je
        m_tagBuyingStatus.insert(std::make_pair(
            i, data->m_statusCounts[i]));
    }
    return 1;                                       // movl $0x1,%eax
