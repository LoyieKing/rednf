// df_game_r Game/ ARAD::Arad_DataManager（批3：从 GameStubs.cpp 迁移，批4：TU 粒度转写）。
//
// ORIG（df_game_r, elf32-i386）：
//   class ARAD::Arad_DataManager（namespace ARAD）  size = 0xbc
//   mangled 符号见本文件各方法注释。ctor _ZN4ARAD16Arad_DataManagerC1Ev @0x08188030。
//
// ORIG 布局（ctor 反汇编 0x8188030）：
//   +0x00  vptr（Arad_DataManager vtable @0x8b91d88）
//   +0x04  std::map<std::string,ARAD::Arad_InterfaceGameScript*> m_scriptMap
//   +0x1c  ExceptionItem        m_exceptionItem
//   +0x38  PurchaseLimitItem    m_purchaseLimitItem
//   +0x54  AradBonusItem        m_bonusItem
//   +0x70  Arad_Mileage_Data    m_mileageData
//   +0x84  Arad_OnePlusCardEvent_ItemList m_onePlusCard
//   +0xa0  Arad_VillageAttackRewardItem   m_villageAttackReward
//   （合计 0xa0 + sizeof(Arad_VillageAttackRewardItem)=0x1c → 0xbc）
//
// Arad_DataManager 直接派生自 ScriptInterface（vptr 槽0=Load 0x81882cc、槽1=Unload 0x81882f8 之外
// 还有 Destroy 0x81882e0，但 ORIG vtable @0x8b91d88 槽0=Load、槽1=Destroy、槽2=destroyAradData——见
// ScriptInterface 虚槽约定下方注释）。

#ifndef DNF_ARAD_DATAMANAGER_H
#define DNF_ARAD_DATAMANAGER_H

#include <string>
#include <map>
#include <vector>
#include <list>
#include <set>

#include "Arad_ScriptLoader.h"

class CUser;
class AradServerStateMessage;

namespace ARAD
{

// ============================================================================
// ScriptInterface —— Arad_DataManager / 各脚本服务类的虚基接口。
// ORIG ctor _ZN4ARAD15ScriptInterfaceC1Ev @0x080e2454（仅写 vptr @0x8b2db78）；
// 槽0=Load（虚，返回 bool）、槽1=Destroy（虚）。Arad_DataManager 从本类派生。
// 注意：Arad_DataManager 的 vtable @0x8b91d88 槽0=Load 0x81882cc、槽1=Destroy
// 0x81882e0（mangled _ZN4ARAD16Arad_DataManager7DestroyEv）——派生类 override
// 的槽1 虚方法必须命名为 Destroy（同名覆盖槽1），否则产生 Unload 符号而无法
// 匹配 ORIG。ScriptManager::DestroyScript 经槽1 调用（见 DestroyScript.md）。
// ============================================================================
class ScriptInterface
{
public:
    // ORIG nm 无 _ZN4ARAD15ScriptInterfaceD*Ev：析构为隐式平凡（勿声明/定义，
    // 否则各派生类 dtor 会多出 ORIG 没有的基析构调用）。
    virtual bool Load() = 0;          // vtable 槽 0（Arad_DataManager::Load -> initAradData）
    virtual void Destroy() = 0;       // vtable 槽 1（Arad_DataManager::Destroy -> destroyAradData）
};

// ============================================================================
// ScriptManager —— ARAD 脚本服务注册/装载器（ORIG LoadScript 0x81a11dc /
// DestroyScript 0x81a12f8 / RegistScript 0x81a136c / Init 0x81a13f6）。
// 持有 set<ScriptInterface*>；LoadScript 依次调用各脚本 Load()，全成功返回 1。
// ============================================================================
class ScriptManager
{
public:
    ScriptManager() {}

    // ORIG 0x81a11dc：Init() 后遍历 set 调用每个脚本的 Load()；全部成功返回 1，否则 0。
    bool LoadScript();
    // ORIG 0x81a12f8：遍历 set 逐个 Unload()，返回 1。
    bool DestroyScript();
    // ORIG 0x81a136c：插入脚本；重复插入时 cMyTrace 报错，Returns success/fail。
    bool RegistScript(ScriptInterface* s, int priority);
    // ORIG 0x81a13f6：注册各 ARAD 脚本单例（AvatarFixedHiddenOptionServer 等，未建模）。
    void Init();

    std::set<ScriptInterface*> m_scripts;   // +0x00（注册脚本集）
};

// ============================================================================
// Arad_InterfaceGameScript —— Arad_DataManager::m_scriptMap 的 value 类型。
// ORIG ctor _ZN4ARAD24Arad_InterfaceGameScriptC1ESs @0x08195c5c：
//   vptr @0x8b9ca68 + std::string m_scriptDataName @+0x04（typeinfo/vtable 共 0xc）。
// 派生类（Arad_EventPeriodDataManager/Arad_StatisticsEventItemList）会在 vptr 后
// 第一位放脚本名 string，然后才是各自成员。虚槽：槽2=loadScriptData、槽3=clearScriptData。
// ============================================================================
class Arad_InterfaceGameScript
{
public:
    Arad_InterfaceGameScript(const std::string& name);
    virtual ~Arad_InterfaceGameScript();   // vtable 槽0=D1、槽1=D0（ORIG 有虚析构）
    // ORIG 基类这两虚槽为纯虚（vtable @0x8b9ca68 槽2/3 = __cxa_pure_virtual）。
    // 派生类覆盖为 loadScriptData（槽2）/ clearScriptData（槽3），以匹配 ORIG
    // mangled 符号（如 _ZN4ARAD27Arad_EventPeriodDataManager14loadScriptDataEv）。
    virtual bool loadScriptData() = 0;    // vtable 槽 2（派生类装载脚本数据）
    virtual void clearScriptData() = 0;   // vtable 槽 3（派生类清理脚本数据）

    std::string scriptDataName();   // ORIG 0x8188b18：返回 m_scriptDataName

private:
    std::string m_scriptDataName;   // +0x04（vptr +0x00 之后）
};

// ============================================================================
// Singleton<T>::Get() —— Meyers 单例（ORIG _ZN4ARAD9SingletonIT_E...Get 模式，guard 变量见
// ORIG nm "guard variable for ARAD::Singleton<ARAD::Arad_DataManager>::Get()::obj"）。
// 模板定义唯一，各 TU 显式/隐式实例化；ARAD.cpp/CCeraShopAddRestrict.cpp 不得再定义。
// ============================================================================
template <class T>
class Singleton
{
public:
    static T* Get();
};

template <class T>
T* Singleton<T>::Get()
{
    static T instance;
    return &instance;
}

int notifyOpenMessageDialog(CUser* user, int a, int b,
                            const AradServerStateMessage& msg);  // ORIG 0x81978dd

// ORIG T 0x08197d00 _ZN4ARAD19notifySystemMessageEP5CUserPc：
// 系统提示消息（PacketGuard 组包 Send）。定义归 Arad 域 TU，此处仅声明。
void notifySystemMessage(CUser* user, char* msg);

// ============================================================================
// PRINT_ERROR —— ARAD 错误上报辅助（全静态成员）。
// ORIG W：isEmpty 0x08197ba4 / CheckItemLock 0x08197bfd / delete_item 0x08197c56。
// 定义归 Arad 域 TU，此处仅声明（AvatarRechargeServer 等调用方使用）。
// ============================================================================
class PRINT_ERROR
{
public:
    static void isEmpty(char* tag, CUser* user, unsigned short slot);
    static void CheckItemLock(char* tag, CUser* user, unsigned short slot);
    static void delete_item(char* tag, CUser* user, unsigned short slot);
    // ORIG W _ZN4ARAD11PRINT_ERROR9find_itemEPcP5CUseri 0x08197caf：
    // "%s : find_item() fail [item_id:%d] [charac_no:%d]"（定义归
    // Game/AradPeriodB.cpp，LinkClosureB.AradServers）。
    static void find_item(char* tag, CUser* user, int itemId);
};


}  // namespace ARAD

// ============================================================================
// Arad_ScriptLoader —— 6 个脚本子对象的虚基类（完整定义见 Arad_ScriptLoader.h）。
// 注意：以下 6 个子对象（ExceptionItem/PurchaseLimitItem/AradBonusItem/
// Arad_Mileage_Data/Arad_OnePlusCardEvent_ItemList/Arad_VillageAttackRewardItem）
// 在 ORIG 中位于【全局命名空间】（mangled _ZN13ExceptionItem... 等，无 ARAD::
// 前缀），故必须放在 namespace ARAD 之外，否则生成 _ZN4ARAD13ExceptionItem...
// 无法匹配 ORIG。仅 Arad_DataManager 本身在 ARAD 命名空间。
// ============================================================================

// ============================================================================
// ExceptionItem —— Arad_DataManager::m_exceptionItem（+0x1c）。
// ORIG ctor @0x08188b50、dtor @0x08188b7a；size = 0x1c（Arad_ScriptLoader 基 0x4 + map 0x14 +
// 对齐 0x4 → 0x1c，见下方 find_index/parsing 报告）。
// 成员：+0x04 std::map<int,int> m_exceptionItemIndex（base vptr 0x4）。
// 方法：parsing / find_index。
// ============================================================================
class ExceptionItem : public Arad_ScriptLoader
{
public:
    ExceptionItem();
    ~ExceptionItem();

    // ORIG 0x081a1fce：装载 "material_exception_item" 节，逐行读
    // material_exception_item_index → map<int,int>。返回 0=成功。
    int parsing(char* data);
    // ORIG 0x081a210c：索引存在则返回 true。
    bool find_index(int index);

private:
    std::map<int, int> m_exceptionItemIndex;   // +0x04
};

// ============================================================================
// PurchaseLimitItem —— Arad_DataManager::m_purchaseLimitItem（+0x38）。
// ORIG ctor @0x08188bae、dtor @0x08188bd8；size = 0x1c。
// 成员：+0x04 std::map<unsigned int,PurchaseLimitItem::Info> m_purchaseLimitMap。
// 内嵌 Info：+0x00 ushort purchaseType +0x02 ushort limitCount（size 0x4）。
// ============================================================================
class PurchaseLimitItem : public Arad_ScriptLoader
{
public:
    // ORIG Info ctor _ZN17PurchaseLimitItem4InfoC1Ev @0x081a405e 清零 8 字节：
    //   +0x00 ushort purchaseType、+0x02 ushort limitCount、+0x04 int（未用/保留）。
    // find@0x081a2310 拷贝 8 字节（+4 与 +8 处两 dword），故 Info 为 8 字节宽。
    struct Info
    {
        Info() { purchaseType = 0; limitCount = 0; reserved = 0; }
        unsigned short purchaseType;   // +0x00
        unsigned short limitCount;     // +0x02
        int reserved;                  // +0x04（ORIG Info::Info 清零，parsing 不填）
    };

    PurchaseLimitItem();
    ~PurchaseLimitItem();

    // ORIG 0x081a216e：装载 "purchase_limit_item" 节，读 Item_Index/purchase_Type/
    // puchase_limit_count → map<uint,Info>。返回 0=成功。
    int parsing(char* data);
    // ORIG 0x081a2310：按 itemIndex 查 Info，命中写 out 并返回 true。
    bool find(unsigned int itemIndex, Info& out);

private:
    std::map<unsigned int, Info> m_purchaseLimitMap;   // +0x04
};

// ============================================================================
// AradBonusItem —— Arad_DataManager::m_bonusItem（+0x54）。
// ORIG ctor @0x08188c0c、dtor @0x08188c36；size = 0x1c。
// 成员：+0x04 std::map<int,AradBonusItem::Data> m_bonusMap。
// 内嵌 Data：+0x00 int itemId +0x04 int bonusValue +0x08 ushort bonusType（size 0xa，pad→0xc）。
// ============================================================================
class AradBonusItem : public Arad_ScriptLoader
{
public:
    struct Data
    {
        Data() { itemId = 0; bonusValue = 0; bonusType = 0; }
        int itemId;           // +0x00
        int bonusValue;       // +0x04
        unsigned short bonusType;  // +0x08
    };

    AradBonusItem();
    ~AradBonusItem();

    // ORIG 0x081a238a：装载 "bonus_item" 节，读 item_id/bonus_value/bonus_type。
    int parsing(char* data);
    // ORIG 0x081a252c：按 itemId 查 Data，命中写 out 并返回 true。
    bool find(unsigned int itemId, Data& out);

private:
    std::map<int, Data> m_bonusMap;   // +0x04
};

// ============================================================================
// Arad_Mileage_Data —— Arad_DataManager::m_mileageData（+0x70）。
// ORIG ctor @0x08188cc0、dtor @0x08188cea；size = 0x20。
// 成员：+0x00 vptr +0x04 int m_defaultPercent +0x08 std::vector<Data> m_mileageList。
// 内嵌 Data：+0x00 int ipgNo +0x04 int percent（size 0x8）。
// ============================================================================
class Arad_Mileage_Data : public Arad_ScriptLoader
{
public:
    struct Data
    {
        Data() : ipgNo(0), percent(0) {}
        Data(int ipg, int pct) : ipgNo(ipg), percent(pct) {}
        int ipgNo;      // +0x00
        int percent;    // +0x04
    };

    Arad_Mileage_Data();
    ~Arad_Mileage_Data();

    // ORIG 0x081a3416：装载 "default"/"item" 两节；默认百分百落在 +0x04，item 逐行 push。返回 0=成功。
    int parsing(char* data);
    // ORIG 0x081a3668：按 ipgNo 找 Data，命中返回 percent，未命中返回 -1。
    int find(int ipgNo);
    // ORIG 0x081a36f0：返回 +0x04 默认百分百。
    int default_value();

private:
    int m_defaultPercent;   // +0x04
    std::vector<Data> m_mileageList;   // +0x08
};

// ============================================================================
// Arad_OnePlusCardEvent_ItemList —— Arad_DataManager::m_onePlusCard（+0x84）。
// ORIG ctor @0x08188d0a、dtor @0x08188d6a；size = 0x1c。
// 成员：+0x04 std::vector<Data> m_cardList；+0x10 std::vector<unsigned int> m_limitedMap。
// 内嵌 Data：+0x00 ushort webId +0x04 int itemIndex（size 0x8）。
// ============================================================================
class Arad_OnePlusCardEvent_ItemList : public Arad_ScriptLoader
{
public:
    struct Data
    {
        Data() : webId(0), itemIndex(0) {}
        Data(unsigned short w, unsigned int i) : webId(w), itemIndex(i) {}
        bool operator==(const Data& rhs) const { return webId == rhs.webId; }
        unsigned short webId;     // +0x00
        int itemIndex;            // +0x04
    };

    Arad_OnePlusCardEvent_ItemList();
    ~Arad_OnePlusCardEvent_ItemList();

    // ORIG 0x081a36fc：装载 "item"/"limited_map" 两节。返回 0=成功。
    int parsing(char* data);
    // ORIG 0x081a398e / 0x081a3a6e：按 webId 查 Data。
    bool find_item(unsigned short webId, Data& out);
    bool find_item(unsigned short webId);
    // ORIG 0x081a3ab2：map 索引在 limited 列表中则返回 true。
    bool is_limited_map(unsigned int mapIndex);

private:
    std::vector<Data> m_cardList;             // +0x04
    std::vector<unsigned int> m_limitedMap;   // +0x10
};

// ============================================================================
// Arad_VillageAttackRewardItem —— Arad_DataManager::m_villageAttackReward（+0xa0）。
// ORIG ctor @0x08188dbe、dtor @0x08188e1e；size = 0x1c。
// 成员：+0x04 std::vector<Data> m_rewardList；+0x10 std::vector<RankData> m_rankList。
// 内嵌 Data：+0x00 int winPoint +0x04 int itemIndex +0x08 int itemCount（size 0xc）。
// 内嵌 RankData：+0x00 int itemIndex +0x04 int itemCount +0x08 int itemIndex2
//              +0x0c int itemCount2（size 0x10）。
// ============================================================================
class Arad_VillageAttackRewardItem : public Arad_ScriptLoader
{
public:
    struct Data
    {
        Data() : winPoint(0), itemIndex(0), itemCount(0) {}
        bool operator==(const Data& rhs) const { return winPoint == rhs.winPoint; }
        int winPoint;     // +0x00
        int itemIndex;    // +0x04
        int itemCount;    // +0x08
    };

    struct RankData
    {
        RankData() : itemIndex(0), itemCount(0), itemIndex2(0), itemCount2(0) {}
        int itemIndex;    // +0x00
        int itemCount;    // +0x04
        int itemIndex2;   // +0x08
        int itemCount2;   // +0x0c
    };

    Arad_VillageAttackRewardItem();
    ~Arad_VillageAttackRewardItem();

    // ORIG 0x081a3b48：装载 "reward_item"/"reward_ranker" 两节。返回 0=成功。
    int parsing(char* data);
    // ORIG 0x081a3e3c：按 winPoint 查 Data，命中写 out 返回 true。
    bool find_reward_item(int winPoint, Data& out);
    // ORIG 0x081a3ef0：rank 索引合法则写 out 返回 true。
    bool find_ranking_reward_item(short rank, RankData& out);

private:
    std::vector<Data> m_rewardList;      // +0x04
    std::vector<RankData> m_rankList;    // +0x10
};

// ============================================================================
// Arad_DataManager —— ARAD 数据管理器单例（位于 ARAD 命名空间）。
// ORIG ctor 8 个构造：Singleton 基 + ScriptInterface 基 + vptr + map + 6 子对象。
// ============================================================================
namespace ARAD
{
class Arad_DataManager : public ScriptInterface
{
public:
    // ORIG 0x08188030：先构造 Singleton 基（空）、ScriptInterface 基（vptr 0x8b2db78）、
    // 再写自身 vptr（0x8b91d88），然后逐成员构造 m_scriptMap / 6 个子对象。
    Arad_DataManager();
    ~Arad_DataManager();

    // ORIG 0x081882cc 槽0 Load：转发 initAradData()。
    virtual bool Load();
    // ORIG 0x081882e0 槽1 Destroy：转发 destroyAradData()，返回 true。
    virtual void Destroy();

    // ORIG 0x081882f8：加载"Event Period Script"、"Statistics Item List"、"NexonFolder Script"。
    // 在内部 new 各脚本单例并 __insertGameScript，成功后逐子对象 Arad_ScriptLoader::open。
    bool initAradData();

    // ORIG 0x08188952：遍历 m_scriptMap 逐项 delete 脚本（FN_DeleteGameScript），clear map。
    void destroyAradData();

    // ORIG 0x081889bc：若脚本名未注册则 m_scriptMap[scriptDataName()]=script 并返回 true；
    // 已存在返回 false。
    bool __insertGameScript(Arad_InterfaceGameScript* script);

    // ORIG 0x081882ac：return m_scriptMap[name]（未命中 map 插入空指针，返回 0）。
    Arad_InterfaceGameScript* findGameScript(std::string name);

    // ORIG weak getter（W）：返回各子对象地址。
    ExceptionItem* getExceptionItem() { return &m_exceptionItem; }
    PurchaseLimitItem* getPurchaseLimitItem() { return &m_purchaseLimitItem; }
    AradBonusItem* getAradBonusItem() { return &m_bonusItem; }
    Arad_Mileage_Data* getAradMileageData() { return &m_mileageData; }
    Arad_OnePlusCardEvent_ItemList* getAradOnePlusCardEventItemList() { return &m_onePlusCard; }
    Arad_VillageAttackRewardItem* getAradVillageAttackReward() { return &m_villageAttackReward; }

private:
    std::map<std::string, Arad_InterfaceGameScript*> m_scriptMap;  // +0x04
    ExceptionItem m_exceptionItem;                       // +0x1c
    PurchaseLimitItem m_purchaseLimitItem;               // +0x38
    AradBonusItem m_bonusItem;                           // +0x54
    Arad_Mileage_Data m_mileageData;                     // +0x70
    Arad_OnePlusCardEvent_ItemList m_onePlusCard;        // +0x84
    Arad_VillageAttackRewardItem m_villageAttackReward;  // +0xa0
};

}  // namespace ARAD

#endif  // DNF_ARAD_DATAMANAGER_H
