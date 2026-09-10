int Inter_UpdateServerSnapShot::dispatch_sig(CUser* /*user*/, char* /*data*/, int /*len*/)
{
    G_GameWorld()->UpdateServerSnapShot();
    return 0;
