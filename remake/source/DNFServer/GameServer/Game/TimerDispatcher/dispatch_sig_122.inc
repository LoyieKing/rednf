int Timer_SecretShopStatistic::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x86397ce: secretshop::CSecretShop::SendSecretShopStatistic(s_secret_shop); RegistNextTimer(); return 1
    GlobalData::s_secret_shop->SendSecretShopStatistic();
    RegistNextTimer();
    return 1;
