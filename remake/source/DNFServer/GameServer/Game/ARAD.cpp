// df_game_r Game/ ARAD 命名空间类实现（批3：从 GameStubs.cpp 迁移，批4：Arad 脚本生命周期）。
// 依据 docs/class_func_reports/ARAD__Arad_DataManager/* + 各子对象报告 + ORIG elf 反汇编。
#include "AradServerStateMessage.h"
#include "Arad_EventPeriodDataManager.h"
#include "Arad_StatisticsEventItemList.h"
#include "AvatarFixedHiddenOptionServer.h"  // #1 注册项（本 TU 新建）
#include "AvatarRechargeServer.h"            // #2 注册项
#include "EmblemCompoundServer.h"            // #3 注册项（FixAradEmblemConvert 新建 TU）
#include "AvatarConvertServer.h"             // #4 注册项（FixAradEmblemConvert 新建 TU）
#include "LevelupSupportEventManger.h"      // #6 注册项
#include "CreateDnfEventSciprt.h"           // #8 注册项
#include "ARAD_FN_DeleteGameScript.h"   // FN_DeleteGameScript 唯一类声明（Arad_DataManager.h 不含）
#include "Arad_Script.h"
#include "CUser.h"          // CerashopAddRestrict::Manager（Init 注册）
#include "PacketGuard.h"    // notifyOpenMessageDialog 组包
#include "LogManager.h"
#include "DNFLexWrapper.h"  // parsing：limited_map 段 Load(g_dnfLex.getInputStreamBuffer())
#include <algorithm>
#include <cstdio>
#include <cstdlib>
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
    // ORIG 0x81a11dc：Init() 后遍历 set 调用各脚本 Load()；全成功返回 1，
    // 否则 cMyTrace "Fail" 返回 0；每次成功也记 "Success"。
    Init();
    // b28d-60：P13 end 缓存（ORIG end() 在循环外一次 → -0x30 槽）；
    // 成功路径 cMyTrace 第4/5参 ORIG 为 (0,1) ⇒ 源码 (false, true)（原 true,false 反）。
    for (std::set<ScriptInterface*>::const_iterator it = m_scripts.begin(),
                                                     e = m_scripts.end();
         it != e; ++it)
    {
        if (!(*it)->Load())
        {
            // ORIG 0x81a1276：失败记 cMyTrace(pretty, 0x12, 9, true, true) + "Fail"。
            cMyTrace trace(__PRETTY_FUNCTION__, 0x12, 9, true, true);
            trace("Fail");
            return false;
        }
        // ORIG 0x81a12f4：每次成功记 cMyTrace(pretty, 0x15, 9, true, false) + "Success"。
        cMyTrace trace(__PRETTY_FUNCTION__, 0x15, 9, false, true);
        trace("Success");
    }
    return true;
}

bool ScriptManager::DestroyScript()
{
    // ORIG 0x81a12f8：遍历 set 逐个 Unload()，返回 1。
    // ORIG end() 一次求值（P13 缓存）；it 槽浅 -0xc / e 槽深 -0x10 ⇒
    // it 先声明（P212 头部直构试探）。
    std::set<ScriptInterface*>::const_iterator it = m_scripts.begin();
    std::set<ScriptInterface*>::const_iterator e = m_scripts.end();
    for (; it != e; ++it)
        (*it)->Destroy();
    return true;
}

void ScriptManager::RegistScript(ScriptInterface* s, int priority)
{
    // ORIG 0x81a136c：find 已存在（重复注册）→
    // printf("Arad_ScriptManager::RegistScript() Duplicate Class : Line(%d)\n", priority) + exit(0)；
    // 不存在 → insert，返回 true。
    // b28d-60：NEAR 残差=insert 后 `leave` 前一条对齐 nop（ORIG 无）——if/else
    // 双块形替代 guard-return 试探（消 4.4 的块对齐 nop）。
    if (m_scripts.find(s) == m_scripts.end())
    {
        m_scripts.insert(s);
    }
    else
    {
        printf("Arad_ScriptManager::RegistScript() Duplicate Class : Line(%d)", priority);
        exit(0);
    }
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
    RegistScript(Singleton<LevelupSupportEventManger>::Get(), 0x13);    // #6（ORIG 次序）
    RegistScript(Singleton<Arad_DataManager>::Get(), 0x13);
    RegistScript(Singleton<CreateDnfEventSciprtServer>::Get(), 0x13);   // #8（ORIG 次序）
    RegistScript(Singleton<LevelupSupport2ndEventManger>::Get(), 0x13); // #9（ORIG 次序）
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
    if (script.GetRowNum() == 0)
        return 0x2a;
    // ORIG 循环每轮重调 GetRowNum
    // b28d-60：P283 4.1「begin 后无条件 jmp 到底部条件块」布局——原 while(true)+
    // break 使 4.4 条件置顶（setle/jne end）；改 goto 尾标签复现 setg/jne body。
    int r = 0;
    goto check;
loop_body:
    {
        int idx = script.GetInt(r, "material_exception_item_index");
        if (!m_exceptionItemIndex.insert(std::make_pair(idx, idx)).second)
            return 0x33;   // ORIG insert 失败（.second==0）→ 0x33
        ++r;
    }
check:
    if (script.GetRowNum() > r)
        goto loop_body;
    return 0;
}

bool ExceptionItem::find_index(int index)
{
    // ORIG 0x81a210c：find 先于 end（求值序），且尾为 test/je/mov$1/jmp/mov$0
    // 分支归一化（P56）——具名迭代器 + if(cond)return true 复现。
    std::map<int, int>::iterator it = m_exceptionItemIndex.find(index);
    if (it != m_exceptionItemIndex.end())
        return true;
    return false;
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
        return 0x4c;                  // ORIG 0x81a21ab：Load 失败返 0x4c
    if (script.GetRowNum() == 0)
        return 0x4f;                  // ORIG 0x81a21cc：无行返 0x4f
    // b28d-60：P283 goto 尾标签（同 ExceptionItem::parsing）。
    int r = 0;
    goto check;
loop_body:
    {
        Info info;
        unsigned int idx = (unsigned int)script.GetInt(r, "Item_Index");
        info.purchaseType = (unsigned short)script.GetInt(r, "purchase_Type");
        info.limitCount = (unsigned short)script.GetInt(r, "puchase_limit_count");
        // ORIG 0x81a2297：insert 而非 operator[]（重复 key 保留首条），
        // 插入失败（已存在）返 0x5c。
        if (!m_purchaseLimitMap.insert(std::make_pair(idx, info)).second)
            return 0x5c;
        ++r;
    }
check:
    if (script.GetRowNum() > r)
        goto loop_body;
    return 0;
}

bool PurchaseLimitItem::find(unsigned int itemIndex, Info& out)
{
    // ORIG 0x81a2310：map<uint,Info> 命中写 out 返回 true。
    // P96：4.4 的 const_iterator 与 iterator 是不同类型，find 返回值赋 const_iterator
    // 会多一次转换构造（ORIG 无该 call）；ORIG 侧 cv 限定为非 const iterator。
    std::map<unsigned int, Info>::iterator it = m_purchaseLimitMap.find(itemIndex);
    // P222：ORIG 0x81a235f 为正向包裹（`test; je <mov $0 失败岛>; 命中体; mov $1`），
    // 否定早返 `if (it == end) return false;` 产反向块序。
    if (it != m_purchaseLimitMap.end())
    {
        out = it->second;
        return true;
    }
    return false;
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
        return 0x79;
    if (script.GetRowNum() == 0)
        return 0x7c;
    // b28d-60：P283 goto 尾标签（同 ExceptionItem::parsing）。
    int r = 0;
    goto check;
loop_body:
    {
        Data d;
        // P167：ORIG make_pair<unsigned&,Data&> 第一参为 unsigned int 局部，
        // 经 pair<unsigned,Data> 转换构造 pair<const int,Data> 插入 map<int,Data>。
        unsigned int itemIdx = (unsigned int)script.GetInt(r, "item_id");
        d.itemId = (int)itemIdx;
        d.bonusType = (unsigned short)script.GetInt(r, "bonus_type");
        d.bonusValue = script.GetInt(r, "bonus_value");
        if (!m_bonusMap.insert(std::make_pair(itemIdx, d)).second)
            return 0x88;   // ORIG insert 失败（.second==0）→ 0x88
        ++r;
    }
check:
    if (script.GetRowNum() > r)
        goto loop_body;
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
// [b22] ORIG ctor 0x08188cc0 不写 +4（m_defaultPercent）：仅基类 ctor+vptr+vector ctor，
// 16→14 条。默认值清零发生在 parsing()（`m_defaultPercent = 0` 行），ctor 侧删初始化。
Arad_Mileage_Data::Arad_Mileage_Data() {}
Arad_Mileage_Data::~Arad_Mileage_Data() {}

int Arad_Mileage_Data::parsing(char* data)
{
    Arad_Script script;
    if (!script.Load(data, "default"))
        return 0x18c;
    if (script.GetRowNum() == 0)
        return 0x18f;

    m_defaultPercent = 0;
    m_defaultPercent = script.GetInt(0, "percent");
    if (m_defaultPercent < 0 || m_defaultPercent > 0x3e8)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Script_Loader.cpp",
            "virtual int Arad_Mileage_Data::parsing(char*)",
            0x197, "[ERROR] invalid mileage default value. (%f)", m_defaultPercent);
        return 0x198;
    }

    if (!script.Load(data, "item"))
        return 0x19e;
    if (script.GetRowNum() == 0)
        return 0;

    for (int r = 0; r < script.GetRowNum(); ++r)
    {
        int ipg = script.GetInt(r, "ipg_no");
        int percent = script.GetInt(r, "percent");
        if (find(ipg) < 0)
        {
            Data d(ipg, percent);
            m_mileageList.push_back(d);
        }
        else
        {
            LogManager::logFormat(
                1, "localjapan/Arad_Script_Loader.cpp",
                "virtual int Arad_Mileage_Data::parsing(char*)",
                0x1b0, "[ERROR] already exist item index (IPG:%d, percent:%d)",
                ipg, percent);
            return 0x1b1;
        }
    }
    return 0;
}

int Arad_Mileage_Data::find(int ipgNo)
{
    // ORIG 0x81a3668：按 ipgNo 找 Data，命中返回 percent，未命中返回 -1。
    for (std::vector<Data>::iterator it = m_mileageList.begin();
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
// ORIG ctor @0x08188d0a / dtor @0x08188d6a（vector 成员自动构造/析构）。
Arad_OnePlusCardEvent_ItemList::Arad_OnePlusCardEvent_ItemList()
{
}

Arad_OnePlusCardEvent_ItemList::~Arad_OnePlusCardEvent_ItemList()
{
}

int Arad_OnePlusCardEvent_ItemList::parsing(char* data)
{
    Arad_Script script;
    // [b23判词·AE假阳性] 167/167 归一化逐条相等（含两段 Load 的实参来源、循环体内
    // 默认值先行 store、logFormat 五个 %esp 实参槽序）；残余 diff 全为未 relink .o 的
    // call/jmp 重定位占位与 .rodata/g_dnfLex 数据地址（立即数为节内偏移 + R_386_32，
    // 已逐处核对其字符串内容与 ORIG 0x8bac8xx 一致，§6.3 假阳性），relink 后转
    // IDENTICAL_AE。
    if (!script.Load(data, "item"))
        return 0x1db;
    if (script.GetRowNum() == 0)
        return 0;
    for (int r = 0; r < script.GetRowNum(); ++r)
    {
        // ORIG 0x081a3767/0x081a376d：循环体先写默认值（movw $0x0 到 -0x12、
        // movl $0xffffffff 到 -0x10），再 GetInt 覆盖（P0：具名默认初始化局部）。
        unsigned short webId = 0;
        unsigned long itemIndex = 0xffffffff;
        webId = (unsigned short)script.GetInt(r, "web_id");
        itemIndex = (unsigned long)script.GetInt(r, "item_index");
        if (find_item(webId))
        {
            LogManager::logFormat(
                1, "localjapan/Arad_Script_Loader.cpp",
                "virtual int Arad_OnePlusCardEvent_ItemList::parsing(char*)",
                0x1ea,
                "[ERROR] already exist data id OnePlusCardEvent(id:%d, item:%u)",
                (int)webId, (unsigned int)itemIndex);
            return 0x1eb;
        }
        m_cardList.push_back(Data(webId, itemIndex));
    }
    // ORIG 0x081a3857：第二段 Load 用 g_dnfLex.getInputStreamBuffer() 当前输入缓冲，
    // 非形参 data（item 段 Load 消耗后 lexer 流位置）。
    if (!script.Load(g_dnfLex.getInputStreamBuffer(), "limited_map"))
        return 0x1f3;
    if (script.GetRowNum() == 0)
        return 0;
    for (int r = 0; r < script.GetRowNum(); ++r)
    {
        unsigned int map = (unsigned int)script.GetInt(r, "index");
        if (is_limited_map(map))
        {
            LogManager::logFormat(
                1, "localjapan/Arad_Script_Loader.cpp",
                "virtual int Arad_OnePlusCardEvent_ItemList::parsing(char*)",
                0x1fe,
                "[ERROR] already exist limited map index (map:%u)",
                map);
            return 0x1ff;
        }
        m_limitedMap.push_back(map);
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
    std::vector<unsigned int>::iterator it = std::find(
        m_limitedMap.begin(), m_limitedMap.end(), mapIndex);
    if (it == m_limitedMap.end())
    {
        return 0;
    }
    return 1;
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
    // ORIG 0x81a3b48：Load 失败分别返回 0x232/0x24c；GetRowNum()==0 提前返回 0；
    // 循环条件每轮重调 GetRowNum()。原 if 包裹写法吞掉了错误码（真语义差）。
    Arad_Script script;
    if (!script.Load(data, "reward_item"))
        return 0x232;
    if (script.GetRowNum() == 0)
        return 0;
    for (int r = 0; r < script.GetRowNum(); ++r)
    {
        // ORIG 0x81a3bbc：先构造 d（-0x1c）再构造 out（-0x28），GetInt 填 out 后拷 d，
        // find 用 d、logFormat/push_back 用 out。按此形态书写以匹配字节。
        Data d;
        Data out;
        out.winPoint = script.GetInt(r, "win_point");
        out.itemIndex = script.GetInt(r, "item_index");
        out.itemCount = script.GetInt(r, "item_count");
        d = out;
        if (find_reward_item(d.winPoint, d))
        {
            LogManager::logFormat(
                1, "localjapan/Arad_Script_Loader.cpp",
                "virtual int Arad_VillageAttackRewardItem::parsing(char*)",
                0x244,
                "[ERROR] Already exist data. (point:%d, item:%u, count:%d)",
                out.winPoint, out.itemIndex, out.itemCount);
            return 0x245;
        }
        m_rewardList.push_back(out);
    }
    if (!script.Load(data, "reward_ranker"))
        return 0x24c;
    if (script.GetRowNum() == 0)
        return 0;
    for (int r = 0; r < script.GetRowNum(); ++r)
    {
        RankData d;
        d.itemIndex = script.GetInt(r, "item_index");
        d.itemCount = script.GetInt(r, "item_count");
        d.itemIndex2 = script.GetInt(r, "item_index2");
        d.itemCount2 = script.GetInt(r, "item_count2");
        m_rankList.push_back(d);
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
    // ORIG 0x81a3ef0：cmpw rank + js（rank<0）+ cmp 16 位 size + jle（rank>=size）
    // —— 有符号 16 位比较；合并 || 以匹配 js/jle 前置 false 布局。
    if (rank >= 0 && rank < (short)m_rankList.size())
    {
        out = m_rankList[rank];
        return true;
    }
    return false;
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

Arad_DataManager::~Arad_DataManager()
{
    Destroy();    // ORIG 0x08188170：D1 写 vptr 后调 Destroy()（成员随后自动析构）
}

bool Arad_DataManager::Load()
{
    // ORIG 0x81882cc：转发 initAradData()
    return initAradData();
}

int Arad_DataManager::Destroy()
{
    // ORIG 0x81882e0：转发 destroyAradData()，返回 true（vtable 槽1）。
    destroyAradData();
    return 1;
}

bool Arad_DataManager::initAradData()
{

    // 各段起始/失败/成功均按 ORIG 记 cMyTrace（line/flag/bC/bD 逐点核对）：
    // 段①起(0x48,9,1,0)、Fail(0x4c/0x52,9,1,1)、Success(0x55,9,0,1)；
    // 段②起(0x59,9,0,1)、Fail(0x5d/0x63,9,1,1)、Success(0x66,9,0,1)；
    // 段③起(0x94,9,1,0)、open 失败(0x97/0x9d/0xa3/0xb2/0xbb,9,1,1) 带
    // "…script load fail. (line:%d)" 参数为 open 返回值。
    cMyTrace trace0("initAradData", 0x48, 9, true, false);
    trace0("\t- Loading Event Period Script - ");
    Arad_EventPeriodDataManager* ep = new Arad_EventPeriodDataManager();
    if (ep == 0 || !ep->loadScriptData())
    {
        cMyTrace trace("initAradData", 0x4c, 9, true, true);
        trace("Fail");
        return false;
    }
    if (!__insertGameScript(ep))
    {
        cMyTrace trace("initAradData", 0x52, 9, true, true);
        trace("Fail");
        return false;
    }
    cMyTrace trace1("initAradData", 0x55, 9, false, true);
    trace1("Success");

    cMyTrace trace2("initAradData", 0x59, 9, false, true);
    trace2("\t- Loading Statistics Item List - ");
    Arad_StatisticsEventItemList* st = new Arad_StatisticsEventItemList();
    if (st == 0 || !st->loadScriptData())
    {
        cMyTrace trace("initAradData", 0x5d, 9, true, true);
        trace("Fail");
        return false;
    }
    if (!__insertGameScript(st))
    {
        cMyTrace trace("initAradData", 0x63, 9, true, true);
        trace("Fail");
        return false;
    }
    cMyTrace trace3("initAradData", 0x66, 9, false, true);
    trace3("Success");

    cMyTrace trace4("initAradData", 0x94, 9, true, false);
    trace4("\t- Loading NexonFolder Script - ");
    int openResult = m_exceptionItem.open("Nexon/AradSpecialUseItems.txt");
    if (openResult != 0)
    {
        cMyTrace trace("initAradData", 0x97, 9, true, true);
        trace("AradSpecialUseItems script load fail. (line:%d)", openResult);
        return false;
    }
    openResult = m_purchaseLimitItem.open("Nexon/AradSpecialUseItems.txt");
    if (openResult != 0)
    {
        cMyTrace trace("initAradData", 0x9d, 9, true, true);
        trace("AradSpecialUseItems script load fail. (line:%d)", openResult);
        return false;
    }
    openResult = m_bonusItem.open("Nexon/AradSpecialUseItems.txt");
    if (openResult != 0)
    {
        cMyTrace trace("initAradData", 0xa3, 9, true, true);
        trace("AradSpecialUseItems script load fail. (line:%d)", openResult);
        return false;
    }
    openResult = m_mileageData.open("Nexon/CeraShopMileage.txt");
    if (openResult != 0)
    {
        cMyTrace trace("initAradData", 0xb2, 9, true, true);
        trace("CeraShopMileage script load fail. (line:%d)", openResult);
        return false;
    }
    openResult = m_villageAttackReward.open("Nexon/VillageAttackRewardItem.txt");
    if (openResult != 0)
    {
        cMyTrace trace("initAradData", 0xbb, 9, true, true);
        trace("VillageAttackRewardItem script load fail. (line:%d)", openResult);
        return false;
    }
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
    // 【b26 NL10】ORIG 以 (end结果槽, find结果槽) 序调 iterator::operator!=，
    // 源码写 end() != it 复现该实参序（4.1 实参求值 R→L）。
    std::map<std::string, Arad_InterfaceGameScript*>::iterator it =
        m_scriptMap.find(script->scriptDataName());
    if (m_scriptMap.end() != it)
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
    // [b23判词·AE假阳性] 65/65 归一化逐条相等（含 guard 局部 -0x18 寻址、put_binary
    // 的 `movl $0xc` 长度实参、异常清理路径的 %ebx/%esi 交换序）；残余 diff 全为未
    // relink .o 的 call 重定位占位 + GlobalData::s_systemTime_ 数据地址显示 0x0
    // （§6.3 假阳性），relink 后转 IDENTICAL_AE。
    if (user == 0)
        return 0;
    PacketGuard guard;
    // ORIG 0x8197907：getCurSec() 结果存 -0xc(%ebp) 栈槽后未再读（P0 具名死局部，
    // ORIG 原码含该存储语句；-O0 下保留槽与 store，与 ORIG 一致）。
    int curSec = GlobalData::s_systemTime_.getCurSec();
    (void)curSec;
    guard.put_header(0, 0x21a);
    guard.put_int(a);
    guard.put_int(b);
    guard.put_binary((const char*)&msg, 0xc);
    guard.finalize(true);
    return user->Send(guard);
}

}  // namespace ARAD
