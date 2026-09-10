bool Dispatcher_BuyCeraShopItem::useCountDownCoinInFreeCoinDungeon(
    CUser* user, MSG_BUY_CERASHOP_ITEM const& msg)
{
    if (user == 0)
    {
        return false;
    }
    if (msg.m_scalars.m_itemCount != 1)        // +0x36（movzbl 实证字段）
    {
        return false;
    }
    // [b28d-solo-Dispatchers] P211 档③/P254：ORIG 0x081fe4e7 `mov 0x39(%eax),%eax;
    //   mov %eax,%ebx`（两段）＝「unsigned 视图字段 + FindGoods(int) 形参异型」的
    //   转换中转形；+0x39 具名（P156）改经 unsigned 视图内联实参。
    struct __attribute__((packed)) BuyCeraScalarsView
    {
        char m_pad39[0x39];
        unsigned int m_goodsIdx;      // +0x39
    };
    CCeraShopGoods* goods = reinterpret_cast<CCeraShopGoods*>(
        G_CDataManager()->FindGoods(
            reinterpret_cast<const BuyCeraScalarsView&>(msg).m_goodsIdx));
    if (goods == 0)
    {
        return false;
    }
    if (goods->GetItemIdx() != 1)
    {
        return false;
    }
    CParty* party = static_cast<CParty*>(user->GetParty());
    // [b28c-32] ORIG 0x081fe53b/0x081fe54d/0x081fe563 三条件共享尾 `mov $0` 出口
    // （P222 正向包裹；平铺早返会各生独立 mov$0/jmp 岛 +2 条）。
    if (party != 0)
    {
        CDungeon* dungeon = party->m_battleField.m_pDungeon;
        if (dungeon != 0)
        {
            if (party->checkFreeRevivalCondition(dungeon))
                return true;
        }
    }
    return false;
