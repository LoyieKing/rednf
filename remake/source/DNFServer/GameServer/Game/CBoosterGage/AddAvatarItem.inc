// [b28d-solo-CBoosterGage N82] 118/118 助记符全等（NEAR）、p167 13/13 CLEAR；残余 = 槽序：
// ORIG nSlot@-0x64（函数级标量紧接形参 spill 区）、invenItem sret@-0x5d；
// 4.4 恒「聚合对象优先占低槽、标量分区排布」（N82附）⇒ invenItem@-0x61、nSlot@-0x24，
// 帧两侧同为 0xa0。已试尽无旋钮：声明序交换（4.4 按首次使用序，N98 无效）、
// int[1] 数组化、struct 包裹、内层块收窄 invenItem 作用域、死局部 char[1]
// （仅把 nSlot 推深 4B 且破坏帧）。ORIG 0x080dcbd6。
// [M4FreeNext15C.M4S1 round5 复扫] 新钥匙 P339（全头部声明 + 声明序按 ORIG 槽深→浅）
// 已试并**回滚**：把 `Inven_Item invenItem;` 提到函数头 ⇒ Inven_Item 有非平凡
// ctor，报 118→153 条（+35：多 ctor/额外帧）、NEAR→DIFF、p167 亦劣 ⇒ 立即回滚
// （读数复原 118/118、near=1）。另核：ORIG 与本件的 nLoop@-0x14 / i@-0x10 /
// tNow@-0xc 三槽**完全一致**，仅 nSlot 与 invenItem 两槽互换分区，非 P331
// 「恒定常数额平移」形（两处差值 0x40 与 4 不同向）⇒ 维持 N82 封顶。
// 语义轴复核：`G_CDataManager()->m_boosterGage.m_cMultiboxCount`（movzbl 0x877e）、
// `eReason=0x16` 写回形参槽 0x30(%ebp)、CInventory::AddAvatarItem 十实参序
// （-1/0/0/pAgency/eReason/0/0）、GetRemainDate(invenItem.m_addInfo2,tNow)、
// push_back(make_pair(nSlot, GetItemSpace)) 逐字与 ORIG 相同；本件无字符串/日志。
int CBoosterGage::AddAvatarItem(std::vector<std::pair<int, int> >& itemList,
                                bool bIsFirst, CUser* pUser, const Inven_Item& item,
                                int nCount, bool bFlag, char cType, char cParam,
                                const char* pAgency, eAvatarItemAddReason eReason,
                                int& nRemainDate)
{
    int nSlot = 0;
    int nLoop = 1;
    if (bIsFirst)
    {
        nLoop = G_CDataManager()->m_boosterGage.m_cMultiboxCount;
        eReason = eAvatarItemAddReason_0x16;
    }
    for (int i = 0; i < nLoop; ++i)
    {
        nSlot = pUser->getCurCharacInvenW()->AddAvatarItem(
            item.m_addInfo, nCount, bFlag, 0, -1, pAgency, eReason, 0, 0);
        if (nSlot == -1)
        {
            make_postar(pUser, item, pAgency, nCount);
        }
        else
        {
            time_t tNow = time(0);
            Inven_Item invenItem = pUser->getCurCharacInvenR()->GetInvenSlot(2, nSlot);
            nRemainDate = pUser->getCurCharacInvenR()->GetAvatarItemMgrR()
                              ->GetRemainDate(invenItem.m_addInfo2, (int)tNow);
            itemList.push_back(std::make_pair(nSlot, item.GetItemSpace()));
        }
    }
    return nSlot;
}