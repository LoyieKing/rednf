// ORIG T 0x0864d84e sz0x2a：param!=0 时 push_back 到受限商品列表(+0x711ec)。
void CUser::SetRestrictedGoods(int itemIdx)
{
    if (itemIdx != 0)
    {
        m_list711ec.push_back(itemIdx);
    }
    return;
}