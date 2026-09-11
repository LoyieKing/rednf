// ---- ORIG T 0x08132cb8（setDefaultBuyItem.md）：清对应 shop 表后全量重放。----
void CharacAdvanceAltarManager::setDefaultBuyItem(AdvanceAltarShopType::T type)
{
    if (m_currentAdvanceAltarData == 0)   // ORIG 内联 *(int*)this 判空，无 call W thunk
    {
        return;
    }
    std::map<int, _BuyItem>* items = m_currentAdvanceAltarData->getBuyItemMap(type);
    if (items == 0)
    {
        return;
    }
    items->clear();
    int stageIdx = 0;   // ORIG -0xc 槽：int 局部 = 0 传第 3 参（P0 有槽⇒有局部）
    setUnlockBuyItem(type, stageIdx, 0);
}