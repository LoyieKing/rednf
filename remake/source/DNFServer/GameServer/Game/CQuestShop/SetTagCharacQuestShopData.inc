// ---- ORIG T 0x85efdc0 size 0x94：装入 tag 状态计数 ------------------------
bool CQuestShop::SetTagCharacQuestShopData(CHARAC_QUEST_SHOP_DATA* data)
{
    for (int i = 0; i <= 0x4c; ++i)                 // cmp $0x4c setle
    {
        // b28c-L10：完整类型具名成员直访问（+0x0c 计数视角，见 CHARAC_QUEST_SHOP_DATA.h
        // 匿名 union）。ORIG 0x85efdc0 装载序为「i 先、base 后」且 make_pair 实例
        // `IRiRt`（非 const ushort 左值）；reinterpret_cast 视图形在 4.4 反装为
        // 「base 先 + 多一条 add」（ndiff 17→0 见探针 /tmp/l10_probe6.cpp fH）。
	        // [FragB] ORIG nm 实证：全库只有 `_ZSt9make_pairIRiRtE` /
	        // `_ZNSt4pairIKhsEC[12]IitE`，**不存在** short 版（IRiRs/Iis）实例；
	        // 旧 short 视角（m_statusCounts）使 4.4 发射 IRiRs+C1Iis+C2Iis 一组
	        // ORIG 无的弱符号，且 make_pair 值经 short 符号扩展（>0x7fff 语义分叉）
	        // ⇒ 改用头内既有同址 ushort 视角 m_statusCountsU（零布局影响）。
	        if (data->m_statusCountsU[i] == 0)
            continue;                               // movzwl + test ax / je
        m_tagBuyingStatus.insert(std::make_pair(
	            i, data->m_statusCountsU[i]));
    }
    return 1;                                       // movl $0x1,%eax
}