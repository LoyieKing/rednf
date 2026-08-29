// df_game_r Game/ ARAD 命名空间类实现（批3：从 GameStubs.cpp 迁移，批4：Arad 脚本生命周期）。
// 依据 docs/class_func_reports/ARAD__Arad_DataManager/* + 各子对象报告 + ORIG elf 反汇编。
#include "AradServerStateMessage.h"
#include "Arad_EventPeriodDataManager.h"
#include "Arad_StatisticsEventItemList.h"
#include "AvatarFixedHiddenOptionServer.h"  // #1 注册项（本 TU 新建）
#include "AvatarRechargeServer.h"            // #2 注册项
#include "EmblemCompoundServer.h"            // #3 注册项（FixAradEmblemConvert 新建 TU）
#include "AvatarConvertServer.h"             // #4 注册项（FixAradEmblemConvert 新建 TU）
#include "ARAD_FN_DeleteGameScript.h"   // FN_DeleteGameScript 唯一类声明（Arad_DataManager.h 不含）
#include "Arad_Script.h"
#include "Arad_Script.h"
#include "CUser.h"          // CerashopAddRestrict::Manager（Init 注册）
#include "PacketGuard.h"    // notifyOpenMessageDialog 组包
#include <algorithm>
#include <cstring>
#include <cstring>

namespace ARAD
{

// ============================================================================
// ScriptInterface（虚基接口）
// （ORIG nm 无 ScriptInterface 析构符号：析构隐式平凡，不提供定义——
//  见 Arad_DataManager.h ScriptInterface 注释。）

// ============================================================================
// ScriptManager（ORIG LoadScript 0x81a11dc / DestroyScript 0x81a12f8 /
// RegistScript 0x81a136c / Init 0x81a13f6）
// ============================================================================
bool ScriptManager::LoadScript()
{
    // ORIG 0x81a11dc：Init() 后遍历 set 调用各脚本 Load()；全成功返回 1，否则 cMyTrace "Fail" 返回 0。
    Init();
    for (std::set<ScriptInterface*>::const_iterator it = m_scripts.begin();
         it != m_scripts.end(); ++it)
    {
        if (!(*it)->Load())
            return false;
    }
    return true;
}

bool ScriptManager::DestroyScript()
{
    // ORIG 0x81a12f8：遍历 set 逐个 Unload()，返回 1。
    for (std::set<ScriptInterface*>::const_iterator it = m_scripts.begin();
         it != m_scripts.end(); ++it)
        (*it)->Destroy();
    return true;
}

bool ScriptManager::RegistScript(ScriptInterface* s, int /*priority*/)
{
    // ORIG 0x81a136c：insert 成功返回 true；重复则返回 false。
    return m_scripts.insert(s).second;
}

void ScriptManager::Init()
{
    // ORIG 0x81a13f6（59 指令）：按序 RegistScript(Singleton<X>::Get(), 0x13) 共 9 个：
    //   AvatarFixedHiddenOptionServer → AvatarRechargeServer → EmblemCompoundServer →
    //   AvatarConvertServer → CerashopAddRestrict::Manager → LevelupSupportEventManger →
    //   Arad_DataManager → CreateDnfEventSciprtServer → LevelupSupport2ndEventManger。
    // 当前依赖闭包完整的六个（保持 ORIG 相对次序；#3/#4 由 FixAradEmblemConvert 落地）：
    RegistScript(Singleton<AvatarFixedHiddenOptionServer>::Get(), 0x13);
    RegistScript(Singleton<AvatarRechargeServer>::Get(), 0x13);
    RegistScript(Singleton<EmblemCompoundServer>::Get(), 0x13);   // #3（ORIG 次序）
    RegistScript(Singleton<AvatarConvertServer>::Get(), 0x13);    // #4（ORIG 次序）
    RegistScript(Singleton<CerashopAddRestrict::Manager>::Get(), 0x13);
    RegistScript(Singleton<Arad_DataManager>::Get(), 0x13);
    // 剩余注册项按 ORIG 次序待各自批次建模后补入：
    // #6 LevelupSupportEventManger / #8 CreateDnfEventSciprtServer /
    // #9 LevelupSupport2ndEventManger（LevelupSupport/CreateDnf 批次）。
}

Arad_InterfaceGameScript::Arad_InterfaceGameScript(std::string name)
    : m_scriptDataName(name)
{
}

Arad_InterfaceGameScript::~Arad_InterfaceGameScript() {}

std::string Arad_InterfaceGameScript::scriptDataName() const
{
    // ORIG 0x8188b18：return m_scriptDataName
    return m_scriptDataName;
}

}  // namespace ARAD

// ============================================================================
// 6 个脚本子对象位于【全局命名空间】（ORIG mangled _ZN13ExceptionItem... 等，
// 无 ARAD:: 前缀）——见 Arad_DataManager.h 说明。
// ============================================================================
// Arad_ScriptLoader（open/parsing 见 ScriptExtras.cpp）
// ============================================================================

// ============================================================================
// ExceptionItem（+0x1c）—— ORIG ctor 0x8188b50 / dtor 0x8188b7a / parsing 0x81a1fce /
// find_index 0x81a210c。解析 "material_exception_item" 节，行读 material_exception_item_index。
// ============================================================================
ExceptionItem::ExceptionItem() {}
ExceptionItem::~ExceptionItem() {}

int ExceptionItem::parsing(char* data)
{
    Arad_Script script;
    if (!script.Load(data, "material_exception_item"))
        return 0x27;
    int rows = script.GetRowNum();
    if (rows == 0)
        return 0x2a;
    for (int r = 0; r < rows; ++r)
    {
        int idx = script.GetInt(r, "material_exception_item_index");
        m_exceptionItemIndex[idx] = idx;   // map<int,int> 置索引
    }
    return 0;
}

bool ExceptionItem::find_index(int index)
{
    // ORIG 0x81a210c：map<int,int> 命中返回 true。
    return m_exceptionItemIndex.find(index) != m_exceptionItemIndex.end();
}

// ============================================================================
// PurchaseLimitItem（+0x38）—— ORIG ctor 0x8188bae / dtor 0x8188bd8 / parsing 0x81a216e /
// find 0x81a2310。解析 "purchase_limit_item" 节，读 Item_Index/purchase_Type/puchase_limit_count。
// ============================================================================
PurchaseLimitItem::PurchaseLimitItem() {}
PurchaseLimitItem::~PurchaseLimitItem() {}

int PurchaseLimitItem::parsing(char* data)
{
    Arad_Script script;
    if (!script.Load(data, "purchase_limit_item"))
        return 0x27;
    int rows = script.GetRowNum();
    for (int r = 0; r < rows; ++r)
    {
        Info info;
        unsigned int idx = (unsigned int)script.GetInt(r, "Item_Index");
        info.purchaseType = (unsigned short)script.GetInt(r, "purchase_Type");
        info.limitCount = (unsigned short)script.GetInt(r, "puchase_limit_count");
        m_purchaseLimitMap[idx] = info;
    }
    return 0;
}

bool PurchaseLimitItem::find(unsigned int itemIndex, Info& out)
{
    // ORIG 0x81a2310：map<uint,Info> 命中写 out 返回 true。
    std::map<unsigned int, Info>::const_iterator it = m_purchaseLimitMap.find(itemIndex);
    if (it == m_purchaseLimitMap.end())
        return false;
    out = it->second;
    return true;
}

// ============================================================================
// AradBonusItem（+0x54）—— ORIG ctor 0x8188c0c / dtor 0x8188c36 / parsing 0x81a238a /
// find 0x81a252c。解析 "bonus_item" 节，读 item_id/bonus_type/bonus_value。
// ============================================================================
AradBonusItem::AradBonusItem() {}
AradBonusItem::~AradBonusItem() {}

int AradBonusItem::parsing(char* data)
{
    Arad_Script script;
    if (!script.Load(data, "bonus_item"))
        return 0x27;
    int rows = script.GetRowNum();
    for (int r = 0; r < rows; ++r)
    {
        Data d;
        d.itemId = script.GetInt(r, "item_id");
        d.bonusType = (unsigned short)script.GetInt(r, "bonus_type");
        d.bonusValue = script.GetInt(r, "bonus_value");
        m_bonusMap[d.itemId] = d;
    }
    return 0;
}

bool AradBonusItem::find(unsigned int itemId, Data& out)
{
    // ORIG 0x81a252c：map<int,Data> 命中写 out（拷 10 字节）返回 true。
    std::map<int, Data>::const_iterator it = m_bonusMap.find((int)itemId);
    if (it == m_bonusMap.end())
        return false;
    out = it->second;
    return true;
}

// ============================================================================
// Arad_Mileage_Data（+0x70）—— ORIG ctor 0x8188cc0 / dtor 0x8188cea / parsing 0x81a3416 /
// find 0x81a3668 / default_value 0x81a36f0。解析 "default"（percent）与 "item"（ipg_no/percent）。
// ============================================================================
Arad_Mileage_Data::Arad_Mileage_Data() : m_defaultPercent(0) {}
Arad_Mileage_Data::~Arad_Mileage_Data() {}

int Arad_Mileage_Data::parsing(char* data)
{
    Arad_Script script;
    if (script.Load(data, "default"))
    {
        if (script.GetRowNum() > 0)
            m_defaultPercent = script.GetInt(0, "percent");
    }
    if (script.Load(data, "item"))
    {
        int rows = script.GetRowNum();
        for (int r = 0; r < rows; ++r)
        {
            Data d(script.GetInt(r, "ipg_no"), script.GetInt(r, "percent"));
            m_mileageList.push_back(d);
        }
    }
    return 0;
}

int Arad_Mileage_Data::find(int ipgNo)
{
    // ORIG 0x81a3668：按 ipgNo 找 Data，命中返回 percent，未命中返回 -1。
    for (std::vector<Data>::const_iterator it = m_mileageList.begin();
         it != m_mileageList.end(); ++it)
        if (it->ipgNo == ipgNo)
            return it->percent;
    return -1;
}

int Arad_Mileage_Data::default_value()
{
    // ORIG 0x81a36f0：return *(int*)(this+0x04)
    return m_defaultPercent;
}

// ============================================================================
// Arad_OnePlusCardEvent_ItemList（+0x84）—— ORIG ctor 0x8188d0a / dtor 0x8188d6a /
// parsing 0x81a36fc / find_item x2 / is_limited_map。解析 "item"（web_id/item_index）与
// "limited_map"（index）。
// ============================================================================
Arad_OnePlusCardEvent_ItemList::Arad_OnePlusCardEvent_ItemList() {}
Arad_OnePlusCardEvent_ItemList::~Arad_OnePlusCardEvent_ItemList() {}

int Arad_OnePlusCardEvent_ItemList::parsing(char* data)
{
    Arad_Script script;
    if (script.Load(data, "item"))
    {
        int rows = script.GetRowNum();
        for (int r = 0; r < rows; ++r)
        {
            Data d((unsigned short)script.GetInt(r, "web_id"), (unsigned long)script.GetInt(r, "item_index"));
            m_cardList.push_back(d);
        }
    }
    if (script.Load(data, "limited_map"))
    {
        int rows = script.GetRowNum();
        for (int r = 0; r < rows; ++r)
            m_limitedMap.push_back((unsigned int)script.GetInt(r, "index"));
    }
    return 0;
}

bool Arad_OnePlusCardEvent_ItemList::find_item(unsigned short webId, Data& out)
{
    // ORIG 0x081a398e：Data(webId,0) + std::find → operator==（0x081a420a）按 webId 匹配。
    Data tmp(webId, 0);
    std::vector<Data>::iterator it = std::find(m_cardList.begin(), m_cardList.end(), tmp);
    if (it == m_cardList.end())
        return false;
    out.webId = it->webId;
    out.itemIndex = it->itemIndex;
    return true;
}

bool Arad_OnePlusCardEvent_ItemList::find_item(unsigned short webId)
{
    // ORIG 0x081a3a6e：Data(0,0) 委托两参 find_item，返回值透传。
    Data tmp(0, 0);
    return find_item(webId, tmp);
}

bool Arad_OnePlusCardEvent_ItemList::is_limited_map(unsigned int mapIndex)
{
    for (std::vector<unsigned int>::const_iterator it = m_limitedMap.begin();
         it != m_limitedMap.end(); ++it)
        if (*it == mapIndex) return true;
    return false;
}

// ============================================================================
// Arad_VillageAttackRewardItem（+0xa0）—— ORIG ctor 0x8188dbe / dtor 0x8188e1e /
// parsing 0x81a3b48 / find_reward_item 0x81a3e3c / find_ranking_reward_item 0x81a3ef0。
// 解析 "reward_item"（win_point/item_index/item_count）与 "reward_ranker"
// （item_index/item_count/item_index2/item_count2）。
// ============================================================================
Arad_VillageAttackRewardItem::Arad_VillageAttackRewardItem() {}
Arad_VillageAttackRewardItem::~Arad_VillageAttackRewardItem() {}

int Arad_VillageAttackRewardItem::parsing(char* data)
{
    Arad_Script script;
    if (script.Load(data, "reward_item"))
    {
        int rows = script.GetRowNum();
        for (int r = 0; r < rows; ++r)
        {
            Data d;
            d.winPoint = script.GetInt(r, "win_point");
            d.itemIndex = script.GetInt(r, "item_index");
            d.itemCount = script.GetInt(r, "item_count");
            m_rewardList.push_back(d);
        }
    }
    if (script.Load(data, "reward_ranker"))
    {
        int rows = script.GetRowNum();
        for (int r = 0; r < rows; ++r)
        {
            RankData d;
            d.itemIndex = script.GetInt(r, "item_index");
            d.itemCount = script.GetInt(r, "item_count");
            d.itemIndex2 = script.GetInt(r, "item_index2");
            d.itemCount2 = script.GetInt(r, "item_count2");
            m_rankList.push_back(d);
        }
    }
    return 0;
}

bool Arad_VillageAttackRewardItem::find_reward_item(int winPoint, Data& out)
{
    for (std::vector<Data>::const_iterator it = m_rewardList.begin();
         it != m_rewardList.end(); ++it)
        if (it->winPoint == winPoint) { out = *it; return true; }
    return false;
}

bool Arad_VillageAttackRewardItem::find_ranking_reward_item(short rank, RankData& out)
{
    if (rank < 0 || (size_t)rank >= m_rankList.size())
        return false;
    out = m_rankList[rank];
    return true;
}

// ============================================================================
// Arad_DataManager（ORIG ctor 0x8188030；位于 ARAD 命名空间）
// ============================================================================
namespace ARAD
{
Arad_DataManager::Arad_DataManager()
    : m_scriptMap(), m_exceptionItem(), m_purchaseLimitItem(), m_bonusItem(),
      m_mileageData(), m_onePlusCard(), m_villageAttackReward()
{
}

Arad_DataManager::~Arad_DataManager() {}

bool Arad_DataManager::Load()
{
    // ORIG 0x81882cc：转发 initAradData()
    return initAradData();
}

void Arad_DataManager::Destroy()
{
    // ORIG 0x81882e0：转发 destroyAradData()，返回 true（vtable 槽1）。
    destroyAradData();
}

bool Arad_DataManager::initAradData()
{
    // ORIG 0x81882f8：三段装载——① "Event Period Script"：new Arad_EventPeriodDataManager
    // -> loadScriptData -> __insertGameScript；② "Statistics Item List"：new
    // Arad_StatisticsEventItemList（operator new(0x14)）-> loadScriptData ->
    // __insertGameScript；③ "NexonFolder Script"：逐子对象 Arad_ScriptLoader::open
    // （+0x1c/+0x38/+0x54 均 "Nexon/AradSpecialUseItems.txt"、+0x70
    // "Nexon/CeraShopMileage.txt"、+0xa0 "Nexon/VillageAttackRewardItem.txt"）。
    // 任一 Load/open 失败 → cMyTrace "Fail" 并返回 0。
    Arad_EventPeriodDataManager* ep = new Arad_EventPeriodDataManager();
    if (ep == 0 || !ep->loadScriptData())
    {
        delete ep;
        return false;
    }
    if (!__insertGameScript(ep))
    {
        return false;
    }
    Arad_StatisticsEventItemList* st = new Arad_StatisticsEventItemList();
    if (st == 0 || !st->loadScriptData())
    {
        delete st;
        return false;
    }
    if (!__insertGameScript(st))
    {
        return false;
    }
    if (m_exceptionItem.open("Nexon/AradSpecialUseItems.txt") != 0)
        return false;
    if (m_purchaseLimitItem.open("Nexon/AradSpecialUseItems.txt") != 0)
        return false;
    if (m_bonusItem.open("Nexon/AradSpecialUseItems.txt") != 0)
        return false;

    if (m_mileageData.open("Nexon/CeraShopMileage.txt") != 0)
        return false;
    if (m_villageAttackReward.open("Nexon/VillageAttackRewardItem.txt") != 0)
        return false;
    return true;
}

void Arad_DataManager::destroyAradData()
{
    // ORIG 0x8188952：std::for_each(begin,end,FN_DeleteGameScript()) 逐项删除，clear。
    // FN_DeleteGameScript::operator() 定义归 Arad_Verify.cpp（ORIG 0x8188c56），
    // 类声明见 ARAD_FN_DeleteGameScript.h（唯一声明点）。
    std::for_each(m_scriptMap.begin(), m_scriptMap.end(), FN_DeleteGameScript());
    m_scriptMap.clear();
}

bool Arad_DataManager::__insertGameScript(Arad_InterfaceGameScript* script)
{
    // ORIG 0x81889bc：scriptDataName 未注册则 m_scriptMap[name]=script 返回 true；否则 false。
    std::map<std::string, Arad_InterfaceGameScript*>::iterator it =
        m_scriptMap.find(script->scriptDataName());
    if (it != m_scriptMap.end())
        return false;
    m_scriptMap[script->scriptDataName()] = script;
    return true;
}

Arad_InterfaceGameScript* Arad_DataManager::findGameScript(std::string name)
{
    // ORIG 0x81882ac：return m_scriptMap[name]（未命中插入空指针，返回 0）。
    return m_scriptMap[name];
}

// Singleton<T>::Get() 模板定义已在 Arad_DataManager.h 提供；此处显式实例化。
template class Singleton<Arad_DataManager>;
template class Singleton<ScriptManager>;

int notifyOpenMessageDialog(CUser* user, int a, int b,
                            const AradServerStateMessage& msg)
{
    // ORIG 0x81978dd：user 判空返回 0；PacketGuard 组 0x21a 头 + 两 int + 12 字节
    // AradServerStateMessage，finalize(true) 后 Send。ORIG 另读一次
    // CSystemTime::getCurSec() 存栈但未使用（死存储），此处省略不影响可观察行为。
    if (user == 0)
        return 0;
    PacketGuard guard;
    guard.put_header(0, 0x21a);
    guard.put_int(a);
    guard.put_int(b);
    guard.put_binary((const char*)&msg, 0xc);
    guard.finalize(true);
    return user->Send(guard);
}

}  // namespace ARAD
