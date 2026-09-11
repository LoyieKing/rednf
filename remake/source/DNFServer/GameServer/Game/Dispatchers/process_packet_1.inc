// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x0813fc06 orig/ours=188/190 ndiff-blocks=43
//   帧差 sub $5c vs $6c + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x5c,%esp | mov    %eax,-0x34(%ebp) | mov    %eax,-0x30(%ebp)
int Dispatcher_BuyItem::process_packet(CUser* user, CMDPacketStruct::STBaseRequest& req,
                                       CMDPacketStruct::STBaseResponse& resp) const
{
    // [objdump 0x0813fc06] pReq→-0x34 / pRes→-0x30 局部指针槽；5 条回显 →
    // check_error → !=0 提前 return；found 线性查找（迭代器后置++）；cost 累加循环。
    CMDPacketStruct::_ReqAdvanceAltarBuyItem* pReq =
        reinterpret_cast<CMDPacketStruct::_ReqAdvanceAltarBuyItem*>(&req);
    CMDPacketStruct::_ResAdvanceAltarBuyItem* pRes =
        reinterpret_cast<CMDPacketStruct::_ResAdvanceAltarBuyItem*>(&resp);
    pRes->m_nBuyIndex = pReq->m_nBuyIndex;                 // buyIndex 回显
    pRes->m_nShopType = pReq->m_nShopType;                 // shopType
    pRes->m_nLevel = pReq->m_nLevel;                       // level
    pRes->m_wFrom = pReq->m_wFrom;                         // fromSlot（ushort）
    pRes->m_wTo = pReq->m_wTo;                             // toSlot（ushort）
    pRes->m_nErrorCode = check_error(user, req);
    if (pRes->m_nErrorCode != 0)
        return 0;
    // ORIG 0x0813fc9c `call G_CDataManager; add $0x631c,%eax`：无具名 dm 槽（P183 直内联）。
    std::vector<advancealtar::BuyUpgradeData>* vec =
        G_CDataManager()->m_altarShop.getBuyUpgradeDataVec(
            pReq->m_nBuyIndex,
            static_cast<AdvanceAltarShopType::T>(pReq->m_nShopType));
    if (vec == 0)                                          // [objdump 0x813fcbf] err=3
    {
        pRes->m_nErrorCode = 3;
        return 0;
    }
    const BuyUpgradeData* found = 0;                       // 线性查找 item 匹配行
    for (std::vector<BuyUpgradeData>::iterator it = vec->begin(); it != vec->end(); it++)
    {
        if (it->m_nItemId == pRes->m_nLevel)               // [objdump 0x813fd04] cmp *it==resp+0x10
        {
            found = &(*it);
            break;
        }
    }
    if (found == 0)                                        // [objdump 0x813fd6b] err=3
    {
        pRes->m_nErrorCode = 3;
        return 0;
    }
    int totalCost = 0;
    for (int lv = (short)pReq->m_wFrom;
         lv <= (short)pReq->m_wTo; ++lv)
    {
        FieldData* fd = const_cast<BuyUpgradeData*>(found)->getFieldDataPoint(
            lv, static_cast<FieldType::T>(2));             // ORIG T 0x088a2e4c
        if (fd == 0)                                       // [objdump 0x813fdb6] err=3
        {
            pRes->m_nErrorCode = 3;
            return 0;
        }
        totalCost += fd->m_value;                          // [objdump 0x813fdca] add +4(int)
    }
    int rewardIdx = 0;
    // ORIG 0x0813fe5b：buyItem 返回值**直写** pRes->m_nErrorCode（无 rc 具名槽，P183），
    // 随后 0x0813fe6a 从字段读回比较（P92）。
    pRes->m_nErrorCode = user->m_advanceAltar.buyItem(     // ORIG T 0x081319ae
        static_cast<AdvanceAltarShopType::T>(pReq->m_nShopType),
        pReq->m_nLevel,
        (short)pReq->m_wFrom,
        (short)pReq->m_wTo,
        totalCost, rewardIdx);
    pRes->m_nTotalCost = rewardIdx;
    if (pRes->m_nErrorCode == 0)
    {
        HistoryLog::starSub(*user, static_cast<SubStarType::T>(0), totalCost);  // ORIG T 0x08133db0
    }
    return 0;
}