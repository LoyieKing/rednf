#ifndef GAME_CHANDLEPREMIUM_H
#define GAME_CHANDLEPREMIUM_H

// ---------------------------------------------------------------------------
// df_game_r —— WongWork::CHandlePremium 权威声明（唯一）。
// ORIG 方法地址见 docs/class_func_reports/WongWork__CHandlePremium/README.md。
// 所有方法均以 CUser* 为第一实参（CHandlePremium 自身无状态成员，方法按
// ORIG 以静态/无状态语义实现）。由 respond_special_item_premium /
// CCeraShop::_ClosePremium / CEventCharacterHandler 等调用。
// 实现见 CHandlePremium.cpp。
// ---------------------------------------------------------------------------

class CUser;

#ifndef DNF_ENUM_ENUM_PREMIUM_TYPE_DEFINED
#define DNF_ENUM_ENUM_PREMIUM_TYPE_DEFINED
enum ENUM_PREMIUM_TYPE { ENUM_PREMIUM_TYPE_0 = 0 };
#endif

namespace WongWork
{

class CHandlePremium
{
public:
    // 0x086af8b4  _ZN8WongWork14CHandlePremium23handleNotifyPremiumInfoEP5CUserii
    static void handleNotifyPremiumInfo(CUser* user, int type, int remainSec);

    // 0x086afc32  _ZN8WongWork14CHandlePremium19handlePremiumEffectEP5CUser17ENUM_PREMIUM_TYPE
    static void handlePremiumEffect(CUser* user, ENUM_PREMIUM_TYPE type);

    // 0x086b02fe  _ZN8WongWork14CHandlePremium19handleSetUserPCRoomEP5CUserll
    static void handleSetUserPCRoom(CUser* user, long start, long end);

    // 0x086af7e8  _ZN8WongWork14CHandlePremium20handleSetUserPremiumEP5CUser17ENUM_PREMIUM_TYPEllib
    static void handleSetUserPremium(CUser* user, ENUM_PREMIUM_TYPE type,
                                     long start, long end, int flag, bool recalc);
};

}  // namespace WongWork

#endif  // GAME_CHANDLEPREMIUM_H
