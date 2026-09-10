void Manager::SendSyncPacket(CUser* user)
{
    // ORIG：ARAD::Singleton<CerashopAddRestrict::Manager>::Get()->makeSyncPacket(user)
    // 符号 _ZN4ARAD9SingletonIN19CerashopAddRestrict7ManagerEE3GetEv（W @ 0x080e274d）
    ARAD::Singleton<CerashopAddRestrict::Manager>::Get()->makeSyncPacket(user);
