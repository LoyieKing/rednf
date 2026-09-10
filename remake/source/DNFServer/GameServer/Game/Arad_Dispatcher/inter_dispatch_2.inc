int Arad_Dispatcher_BuyItemCheck_Impl::inter_dispatch(CUser* user,
                                                      char* body, int /*len*/)
{
    // P82/P99：本地 pack(1) 尾部视图使 4.4 折叠 `mov 0x3a(%eax),%eax`
    // （裸 (b+0x3a) cast 恒 add 分解，探针 /tmp/l18p/p.cpp）。
    #pragma pack(push, 1)
    struct ItemIdxView { char m_pad[0x3a]; unsigned int m_idx; };
    #pragma pack(pop)
    // ORIG 0x0818ea3e：
    //   body[0]==1 → ItemVendingMachine::BuyItem(user, *(MSG*)(body+1))；
    //               成功返回 0，失败 logFormat(…0x40…) 返回 -1。
    //   body[0]!=1 → CCeraShop::ProcessError(s_pCeraShop,user,0x74,0,0,1,1,0)，返回 0。
    // ORIG 0x0818ea44：body 形参 spill 至 -0xc 槽（P0 具名局部）；0x0818ea4a
    // `movzbl (%eax),%eax; xor $0x1,%eax; test; je`＝ `!*(bool*)b` 直判形
    // （xor 精确判 b==1，P171 原码 bool 视角照抄）。
    char* b = body;
    if (!*(bool*)b)
    {
        GlobalData::s_pCeraShop->ProcessError(user, 0x74, 0, 0, 1, 1, '\0');
        return 0;
    }
    if (!GlobalData::s_pItemVendingMachine->BuyItem(
            user, *(const MSG_BUY_CERASHOP_ITEM*)(b + 1)))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Dispatcher_BuyItemCheck_Impl.cpp",
            "virtual int ARAD::Arad_Dispatcher_BuyItemCheck_Impl::inter_dispatch("
            "CUser*, char*, int)", 0x40,
            "[ERROR] BuyItem() Result Error ItemIdx(%d)",
            ((const ItemIdxView*)b)->m_idx);
        return -1;
    }
    return 0;
