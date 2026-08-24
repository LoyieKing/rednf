// df_game_r Game/ Arad_ScriptLoader（ORIG 0x081a1eec open / 0x081a4054 parsing）。
//
// ORIG vtable（_ZTV17Arad_ScriptLoader @0x8bad828）：
//   槽0 = bool open(char const*)    （T 0x081a1eec）
//   槽1 = bool parsing(char*)       （W 0x081a4054，默认返回 true）
//   无析构符号；C1（0x8188b42）仅写 vptr。
//
// 本类被 Arad_DataManager 的 6 个脚本子对象（ExceptionItem/PurchaseLimitItem/
// AradBonusItem/Arad_Mileage_Data/Arad_OnePlusCardEvent_ItemList/
// Arad_VillageAttackRewardItem）作为基类（虚基链 via vptr），它们各自覆盖 parsing()。
// 从 ScriptExtras.cpp 的局部类提升为共享头，供各子对象派生。

#ifndef DNF_ARAD_SCRIPTLOADER_H
#define DNF_ARAD_SCRIPTLOADER_H

class Arad_ScriptLoader
{
public:
    Arad_ScriptLoader() {}                 // ORIG C1 0x8188b42：仅写 vptr
    virtual int open(const char* path);    // vtable +8（槽2，T 0x081a1eec，返回解析结果）
    virtual int parsing(char* line);       // vtable +12（槽3，W 0x081a4054，返回 1=成功；派生类
                                           //  返回 0=成功、非零=错误码）
    // ORIG vtable 无虚析构（槽4=0）；析构方用值成员/非虚析构，不删除 Arad_ScriptLoader*。
};

#endif  // DNF_ARAD_SCRIPTLOADER_H
