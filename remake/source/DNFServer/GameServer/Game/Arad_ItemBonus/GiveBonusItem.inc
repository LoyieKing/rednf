// ---------------------------------------------------------------------------
// GiveBonusItem —— ORIG 0x081abf44（GiveBonusItem.md）：
// keyCheck(goodsIdx) 命中后 find(key)->second 上做阈值匹配再发放；
// itemIdx 形参在 ORIG 中未使用，保持签名一致。
// ---------------------------------------------------------------------------
void CeraShopPurcahseCountBonus::GiveBonusItem(CUser* user,
                                               unsigned long itemIdx,
                                               int goodsIdx,
                                               unsigned short threshold)
{
    (void)itemIdx;
    if (!CheckBonusItem(goodsIdx))
        return;

    PurcahseBonus local;
    std::map<int, std::vector<PurcahseBonus> >::iterator it =
        m_bonusMap.find(goodsIdx);
    // ORIG 0x081abf8x：`call _findBonusItem; xor $0x1,%eax; test; jne 尾`＝
    // !boolcall() 负形早退（P168a①），正向 if 落 je 形。
    if (!_findBonusItem(it->second, local, threshold))
        return;
    _processBonusItem(user, goodsIdx, local);
}