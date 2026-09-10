void ItemVendingMachine::_GetItemString(const CCeraShopGoods* goods, char* buf,
                                        unsigned int len)
{
    // [b23S3C] ORIG 0x0817645a：无 switch/jump table——七条 if-else 链，每判重 call
    // GetItemIdx（P48 去缓存），命中才 call GetStackCount 作 snprintf 实参直落 0xc(%esp)。
    if (goods->GetItemIdx() == 1) { OS_API::snprintf(buf, len, "Continue Coin %d", goods->GetStackCount()); return; }
    else if (goods->GetItemIdx() == 0xca) { OS_API::snprintf(buf, len, "Mystic Coin %d", goods->GetStackCount()); return; }
    else if (goods->GetItemIdx() == 8) { OS_API::snprintf(buf, len, "Hands of Remy %d", goods->GetStackCount()); return; }
    else if (goods->GetItemIdx() == 0xf) { OS_API::snprintf(buf, len, "Kaleido Box %d", goods->GetStackCount()); return; }
    else if (goods->GetItemIdx() == 0x43d) { OS_API::snprintf(buf, len, "Manaade %d", goods->GetStackCount()); return; }
    else if (goods->GetItemIdx() == 0xe) { OS_API::snprintf(buf, len, "Golden Wax Candle %d", goods->GetStackCount()); return; }
    else if (goods->GetItemIdx() == 0xd8) { OS_API::snprintf(buf, len, "Mystic Cube %d", goods->GetStackCount()); return; }
    CItem* related = goods->GetRelatedItem();
    if (related == 0)
    {
        LogManager::logFormat(1, "localjapan/AppendVendingMachine.cpp", kPrettyGetItemString,
                              0x18d, "CashShop Relation Item is NULL [ITEM:%d]",
                              goods->GetItemIdx());
        return;
    }
    if (related->is_stackable() && goods->GetStackCount() >= 2)
        OS_API::snprintf(buf, len, "%s Count %d", related->GetItemName(), goods->GetStackCount());
    else
        strncpy(buf, related->GetItemName(), len);
