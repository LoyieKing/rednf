// ============================================================================
// SaveFeaturedIdx —— ORIG 0x08325a30 (T)
// ============================================================================
void CCeraShop::SaveFeaturedIdx(CUser* user, int restrictCode, int curSec, int d)
{
    if (restrictCode == 0)
        return;
    // ORIG 0x8325a50：characNo 跨 call 存 %ebx、accId eax 直传出参槽（P24/P52
    // 单表达式实参形，无具名槽）。
    DB_SaveFeaturedGoodsIdx::makeRequest(user->get_acc_id(), user->get_charac_no(-1),
                                         restrictCode, curSec, d);
}

