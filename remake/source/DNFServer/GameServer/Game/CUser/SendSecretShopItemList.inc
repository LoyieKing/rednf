// ORIG T 0x0868860a：转发 party->SendSecretShopItemList(this)（声明 CUser.h:3432）
void CUser::SendSecretShopItemList()
{
    CParty* party = (CParty*)GetParty();
    if (party != 0)
        party->SendSecretShopItemList(this);
}