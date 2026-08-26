#ifndef DNF_GAME_CAVATARITEMMGR_H_
#define DNF_GAME_CAVATARITEMMGR_H_

// df_game_r Game/ WongWork::CAvatarItemMgr（独立 TU，从 GameStubs.cpp 拆出）。
// 布局：m_registeredItems(+0x00) / m_waitAssignItems(+0x18) / m_tempKeyCounter(+0x30)
//       / m_emblemInfo(+0x34) / m_emblemEnduranceMap(+0x54) / m_enduranceUpdateMap(+0x6c)。
// 语义字段：m_registeredItems/m_waitAssignItems/m_tempKeyCounter/m_emblemEnduranceMap
//          /m_enduranceUpdateMap 替代原 m_items1/m_items2/m_field30/m_field54/m_field6c。

#include <map>
#include "GamePODTypes.h"
#include "AvatarItem.h"

class Inven_Item;

namespace WongWork
{

class CAvatarItemMgr
{
public:
    CAvatarItemMgr();   // ORIG W 0x0822d22c（三 map 隐式成员构造）
    // G7 miscB 追加：ORIG _ZN8WongWork14CAvatarItemMgrC1ERKS0_ W 0x0869937c
    // （合成式逐成员拷贝，定义 WongWorkMiscB_A.cpp）。
    CAvatarItemMgr(const CAvatarItemMgr& other);
    void Reset();
    static bool IsTempKey(int key);   // ORIG W 0x0822d20d（_ZN...9IsTempKeyEi；
                                      //   ORIG 调用无 this，函数体不读对象 → static，
                                      //   mangled 不变；既有 mgr->IsTempKey 调用仍合法）
    int _AssignTempKey();
    int _MakeTempKey(int key) const;
    Avatar_Item* _FindItem(int key) const;
    Avatar_Item* _FindItemAll(int key) const;
    Avatar_Item* _FindWaitAssignItem(int key) const;
    void AddAvatarItem(int key, Avatar_Item* item);
    void AddEmblemEndurance(int key, const Avatar_Item* item);
    bool AssignAvatarItemInfo(int key, int newKey, int expireDate, int itemIdx,
                              const char* ipgAgency);
    bool AssignAvatarItemKey(int oldKey, int newKey);
    unsigned long RegistNotAssignedAvatarItem(int itemIdx, int tick,
                                              const char* ipgAgency, bool b,
                                              const stAvatarEmblemInfo_t& emblem);
    unsigned long RegistNotAssignedAvatarItem(const Inven_Item& item);
    // ORIG T 0x082f95f4 按值版 _ZN8WongWork14CAvatarItemMgr27RegistNotAssignedAvatarItemE10Inven_Item
    // （与 const 引用重载语义一致：SetData(item.m_addInfo2,0,"") 后入 m_waitAssignItems，
    //   返回 tempKey；定义 CAvatarItemMgr.cpp）。
    unsigned long RegistNotAssignedAvatarItem(Inven_Item item);
    int RegistItem(int key, int expireDate, int registTime, const char* ipgAgency,
                   const stAvatarEmblemInfo_t& emblem,
                   const stAvatarExpansionInfo_t& expansion, bool b);
    void RegistItem(const AvatarDetailInfo& info, bool b);
    int GetExpireDate(int avatarUid) const;
    int GetRegistTime(int avatarUid) const;
    const char* GetIPGAgencyNo(int addInfo) const;
    int GetRemainDate(int nAvatarUid, int nTick) const;
    int GetEmblemReduceRate(int key);   // ORIG T 0x082f9f9e 非 const（_ZN...19GetEmblemReduceRateEi）
    const char* getJewelSocketData(int key) const;
    void* GetExpansionInfo(int key) const;
    bool SetAvatarColor(int key, short color1, short color2);
    Avatar_Item* GetAvatarInfo(int key) const;
    int GetItem(AvatarDetailInfo* info, int* count) const;
    bool UnRegistItem(int key);
    Avatar_Item* EraseAvatarItem(int key);
    bool isSocketAvatar(int key);       // ORIG T 0x082f9228 非 const（_ZN...14isSocketAvatarEi）
    int setEmblemSocketData(int key, const stAvatarEmblemInfo_t& emblem);
    void UpdateEndurance(int key, int endurance);
    void SaveEndurance();
    int updateEmblemSocket(int key, unsigned int count, char* grades, unsigned long* itemIds);

    std::map<int, Avatar_Item*> m_registeredItems;     // +0x00 (原 m_items1)
    std::map<int, Avatar_Item*> m_waitAssignItems;     // +0x18 (原 m_items2)
    int m_tempKeyCounter;                              // +0x30 (原 m_field30)
    stAvatarEmblemInfo_t m_emblemInfo;                 // +0x34
    std::map<int, int> m_emblemEnduranceMap;           // +0x54 (原 m_field54)
    std::map<int, int> m_enduranceUpdateMap;           // +0x6c (原 m_field6c)
};

}  // namespace WongWork

#endif  // DNF_GAME_CAVATARITEMMGR_H_
