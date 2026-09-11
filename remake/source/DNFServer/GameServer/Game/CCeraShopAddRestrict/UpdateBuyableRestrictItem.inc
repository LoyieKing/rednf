// ORIG T 0x080e0d14（LinkE2.JpnExImpl 批次修正）：static 成员；定义体内经
// ARAD::Singleton<Manager>::Get() 调 FindIpgNo/InputBuyInfo/SendSyncPacket。
void Manager::UpdateBuyableRestrictItem(CUser* user, unsigned int ipgNo,
                                        unsigned int itemIdx)
{
    if (!ARAD::Singleton<Manager>::Get()->FindIpgNo(ipgNo))
        return;
    DBInputData data;
    // [b28c-4 P0] 存量 `register unsigned char ok asm("eax")` 已清除（全域禁绑，
    // N96 登记）；ORIG 调用点是 int callee 返回值的 al 直测（P3），改表达式直读
    // 复测。data 槽基：ORIG -0x10（sizeof 已实证 0x8，见 CUser.h b28c 代落注）。
    if (ARAD::Singleton<Manager>::Get()->InputBuyInfo(user, ipgNo, itemIdx, data)) {
        // DB 请求经 DB_CerashopAddRestrict.cpp 的真实成员函数投递（ORIG 0x080e17ba）。
        DB_SaveCerashopAddRestrictType::makeRequest(user, data);
        // ORIG 直接调 static SendSyncPacket（无 this、无 Get()；注释 0x817915c 实证）
        SendSyncPacket(user);
    }
}