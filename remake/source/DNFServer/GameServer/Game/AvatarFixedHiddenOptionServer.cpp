// df_game_r Game/ AvatarFixedHiddenOptionServer 实现（独立 TU）。
// 依据 docs/class_func_reports/AvatarFixedHiddenOptionServer/、
// ARAD__SCRIPT__AvatarFixedHiddenOption/（含 parseAbility.md 全量反编译）与 ORIG 反汇编转写。
#include "AvatarFixedHiddenOptionServer.h"

#include <cstdio>
#include <map>

#include "CEnvironment.h"    // less_TCHAR / SpinLock
#include "GameTypes.h"       // cMyTrace
#include "DNFLexWrapper.h"   // loadRDARScriptFile / ScanType / ScanInt / ScanFloat

// ---- 布局断言（ORIG elf32-i386 反汇编实证）----
typedef char avatar_hidden_ability_variation_size_assert[
    sizeof(STAvatarHiddenAbilityVariation) == 0x14 ? 1 : -1];
typedef char avatar_fixed_hidden_option_size_assert[
    sizeof(ARAD::SCRIPT::AvatarFixedHiddenOption) == 0x4c ? 1 : -1];
typedef char avatar_fixed_hidden_option_server_size_assert[
    sizeof(AvatarFixedHiddenOptionServer) == 0x98 ? 1 : -1];

// ============================================================================
// STAvatarHiddenAbilityVariation —— ORIG ctor W 0x088b01e8 / 0x088b021a、dtor W 0x088b0aae
// ============================================================================
STAvatarHiddenAbilityVariation::STAvatarHiddenAbilityVariation(
    AVATAR_HIDDEN_ABILITY_TYPE::T type, int value, int rate)
{
    m_type = type;          // +0x00
    m_bFloatValue = false;  // +0x04 = 0
    m_fValue = 0.0f;        // +0x08 清零
    m_nValue = value;       // +0x0c
    m_nRate = rate;         // +0x10
}

STAvatarHiddenAbilityVariation::STAvatarHiddenAbilityVariation(
    AVATAR_HIDDEN_ABILITY_TYPE::T type, float value, int rate)
{
    m_type = type;          // +0x00
    m_bFloatValue = true;   // +0x04 = 1
    m_fValue = value;       // +0x08（浮点值）
    m_nValue = 0;           // +0x0c 清零
    m_nRate = rate;         // +0x10
}

STAvatarHiddenAbilityVariation::~STAvatarHiddenAbilityVariation()
{
}

// ============================================================================
// ARAD::SCRIPT::AvatarFixedHiddenOption
// ============================================================================

// ORIG C1 0x088aecac：6 个 vector 顺序构造后调 clearScript()。
ARAD::SCRIPT::AvatarFixedHiddenOption::AvatarFixedHiddenOption()
{
    clearScript();
}

// ORIG D1 0x088aed5e：clearScript 后逆序析构 6 个 vector（成员析构自动完成）。
ARAD::SCRIPT::AvatarFixedHiddenOption::~AvatarFixedHiddenOption()
{
    clearScript();
}

// ORIG 0x088b00e0：clear 每桶并 push_back 一个全 0 默认元素（种子保证 Init 的
// reserve(count-1) 非负，与 ORIG 一致）。
void ARAD::SCRIPT::AvatarFixedHiddenOption::clearScript()
{
    for (int i = 0; i < 6; ++i)
    {
        m_abilities[i].clear();
        m_abilities[i].push_back(
            STAvatarHiddenAbilityVariation(AVATAR_HIDDEN_ABILITY_TYPE::NONE, 0, 0));
    }
}

// ORIG 0x088b0164：return *(int*)(this+0x48)
int ARAD::SCRIPT::AvatarFixedHiddenOption::getMaterialItemIndex()
{
    return m_materialItemIndex;
}

// ORIG 0x088b0170：return *(int*)(this+0x48) == itemIndex
bool ARAD::SCRIPT::AvatarFixedHiddenOption::isMaterialItemIndex(int itemIndex)
{
    if (m_materialItemIndex == itemIndex)
        return true;
    return false;
}

// ORIG 0x088aef52 parseAbility：首次调用构建 21 项静态映射（SpinLock 双检的语义等价：
// 函数局部静态守卫），随后查 tag → 内部 id，读数值入桶。未知标签返回 false。
bool ARAD::SCRIPT::AvatarFixedHiddenOption::parseAbility(
    std::vector<STAvatarHiddenAbilityVariation>& variations, std::string& tag)
{
    // ORIG：__lookup 为 map<const char*,int,less_TCHAR>，__init_lock 为 SpinLock，
    // __inited 在锁保护下置位；使用真实公共类型以保留跨 TU 依赖和初始化语义。
    static std::map<const char*, int, less_TCHAR> lookup;
    static SpinLock initLock;
    static bool inited = false;

    if (!inited)
    {
        initLock.enter();
        if (!inited)
        {
            // 插入次序与 ORIG 初始化链一致（0x46..0xac）。
            lookup.insert(std::make_pair("[physical attack]", 0x46));
            lookup.insert(std::make_pair("[magical attack]", 0x4b));
            lookup.insert(std::make_pair("[physical defense]", 0x50));
            lookup.insert(std::make_pair("[magical defense]", 0x55));
            lookup.insert(std::make_pair("[HP MAX]", 0x5a));
            lookup.insert(std::make_pair("[MP MAX]", 0x5f));
            lookup.insert(std::make_pair("[HP regen speed]", 100));
            lookup.insert(std::make_pair("[MP Regen speed]", 0x69));
            lookup.insert(std::make_pair("[attack speed]", 0x6e));
            lookup.insert(std::make_pair("[move speed]", 0x73));
            lookup.insert(std::make_pair("[cast speed]", 0x78));
            lookup.insert(std::make_pair("[inventory limit]", 0x7d));
            lookup.insert(std::make_pair("[stuck]", 0x82));
            lookup.insert(std::make_pair("[stuck resistance]", 0x87));
            lookup.insert(std::make_pair("[all activestatus resistance]", 0x8c));
            lookup.insert(std::make_pair("[hit recovery]", 0x91));
            lookup.insert(std::make_pair("[equipment magical defence]", 0x96));
            lookup.insert(std::make_pair("[equipment physical defence]", 0x9c));
            lookup.insert(std::make_pair("[jump power]", 0xa2));
            lookup.insert(std::make_pair("[physical critical hit]", 0xa7));
            lookup.insert(std::make_pair("[magical critical hit]", 0xac));
            inited = true;
        }
        initLock.leave();
    }

    std::map<const char*, int, less_TCHAR>::const_iterator it =
        lookup.find(tag.c_str());
    if (it == lookup.end())
        return false;

    switch (it->second)
    {
    case 0x46:   // [physical attack] → type 1
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::PHYSICAL_ATTACK, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x4b:   // [magical attack] → type 2
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::MAGICAL_ATTACK, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x50:   // [physical defense] → type 3
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::PHYSICAL_DEFENSE, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x55:   // [magical defense] → type 4
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::MAGICAL_DEFENSE, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x5a:   // [HP MAX] → type 5
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::HP_MAX, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x5f:   // [MP MAX] → type 6
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::MP_MAX, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 100:    // [HP regen speed] → type 7
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::HP_REGEN_SPEED, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x69:   // [MP Regen speed] → type 8
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::MP_REGEN_SPEED, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x6e:   // [attack speed] → type 9
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::ATTACK_SPEED, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x73:   // [move speed] → type 10
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::MOVE_SPEED, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x78:   // [cast speed] → type 11
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::CAST_SPEED, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x7d:   // [inventory limit] → type 12
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::INVENTORY_LIMIT, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x82:   // [stuck] → type 13（第一个数值为浮点）
    {
        float value = ScanFloat((bool*)NULL);
        int rate = ScanInt((bool*)NULL);
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::STUCK, value, rate));
        break;
    }
    case 0x87:   // [stuck resistance] → type 14
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::STUCK_RESISTANCE, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x8c:   // [all activestatus resistance] → type 15
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::ALL_ACTIVESTATUS_RESISTANCE,
            ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x91:   // [hit recovery] → type 16
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::HIT_RECOVERY, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0x96:   // [equipment magical defence] → type 17（首个 ScanInt 弃读）
    {
        ScanInt((bool*)NULL);
        int value = ScanInt((bool*)NULL);
        int rate = ScanInt((bool*)NULL);
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::EQUIPMENT_MAGICAL_DEFENCE, value, rate));
        break;
    }
    case 0x9c:   // [equipment physical defence] → type 18（首个 ScanInt 弃读）
    {
        ScanInt((bool*)NULL);
        int value = ScanInt((bool*)NULL);
        int rate = ScanInt((bool*)NULL);
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::EQUIPMENT_PHYSICAL_DEFENCE, value, rate));
        break;
    }
    case 0xa2:   // [jump power] → type 19
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::JUMP_POWER, ScanInt((bool*)NULL), ScanInt((bool*)NULL)));
        break;
    case 0xa7:   // [physical critical hit] → type 20（浮点）
    {
        float value = ScanFloat((bool*)NULL);
        int rate = ScanInt((bool*)NULL);
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::PHYSICAL_CRITICAL_HIT, value, rate));
        break;
    }
    case 0xac:   // [magical critical hit] → type 21（浮点）
    {
        float value = ScanFloat((bool*)NULL);
        int rate = ScanInt((bool*)NULL);
        variations.push_back(STAvatarHiddenAbilityVariation(
            AVATAR_HIDDEN_ABILITY_TYPE::MAGICAL_CRITICAL_HIT, value, rate));
        break;
    }
    default:
        return false;
    }
    return true;
}

// ORIG 0x088aedce loadScript。控制流忠实转写：外层 ScanType 失败即返回 0；
// "[upper]" / "[rare]" 分支内先 ScanType 再 parseAbility，parse 未命中回落外层；
// "[mystic circle]" 读一个整数到材料索引。打开脚本失败返回 -1。
int ARAD::SCRIPT::AvatarFixedHiddenOption::loadScript()
{
    if (!loadRDARScriptFile("Etc/Avatar_Roulette/", "AvatarFixedHiddenOptionList.etc"))
        return -1;

    std::string token;
    for (;;)
    {
        if (!ScanType(token, true))
            return 0;
        if (token == "[upper]")
        {
            for (;;)
            {
                if (!ScanType(token, true))
                    break;
                if (!parseAbility(m_abilities[2], token))   // ORIG this+0x18
                    break;
            }
        }
        else if (token == "[rare]")
        {
            for (;;)
            {
                if (!ScanType(token, true))
                    break;
                if (!parseAbility(m_abilities[3], token))   // ORIG this+0x24
                    break;
            }
        }
        else if (token == "[mystic circle]")
        {
            m_materialItemIndex = ScanInt((bool*)NULL);           // ORIG *(int*)(this+0x48)
        }
    }
}

// ============================================================================
// AvatarFixedHiddenOptionServer —— ORIG C1 0x0817e02c / D1 0x0817e0e6 /
// Load 0x0817e154 / Destroy 0x0817e1e2 / Init 0x0817e3e8
// ============================================================================

AvatarFixedHiddenOptionServer::AvatarFixedHiddenOptionServer()
{
}

AvatarFixedHiddenOptionServer::~AvatarFixedHiddenOptionServer()
{
}

// ORIG 0x0817e154：cMyTrace(__PRETTY_FUNCTION__,0x1a,9,true,false) +
// "\t- Loading Avatar Hidden Option - "；loadScript==0 且 Init()!=0 才算成功。
bool AvatarFixedHiddenOptionServer::Load()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x1a, 9, true, false);
    trace("\t- Loading Avatar Hidden Option - ");
    return m_script.loadScript() == 0 && Init() != 0;
}

// ORIG 0x0817e1e2：恒返回 1（接口 void 化同 CerashopAddRestrict::Manager 先例）。
int AvatarFixedHiddenOptionServer::Destroy()
{
    return 1;
}

// ORIG 0x0817e3e8：6 个桶各自把下标 1..count-1 的 m_nRate（元素 +0x10）累加成前缀和
// push 入 m_cumulativeRate[i]，之前 reserve(count-1)。恒返回 1。
int AvatarFixedHiddenOptionServer::Init()
{
    for (int i = 0; i < 6; ++i)
    {
        int cumulative = 0;
        int count = (int)m_script.m_abilities[i].size();
        m_cumulativeRate[i].reserve((std::vector<int>::size_type)(count - 1));
        for (int k = 1; k < count; ++k)
        {
            cumulative += m_script.m_abilities[i][k].m_nRate;
            m_cumulativeRate[i].push_back(cumulative);
        }
    }
    return 1;
}

// ============================================================================
// LinkClosureB.AradServers 追加段：
//   GetSkillIndex(rarity)       ORIG T 0x0817e1ec（GetSkillIndex.md 转写）
//   GetSkillIndex(rarity, idx)  ORIG T 0x0817e23e（GetSkillIndex_0817e23e.md）
//   GetRandomInput              ORIG T 0x0817e334（GetRandomInput.md）
//   IsVaildSkillIndex           ORIG T 0x0817e392（IsVaildSkillIndex.md）
//   CheckCondition              ORIG T 0x0817e4c6（CheckCondition.md 全文转写）
//   ChangeHiddenOption          ORIG T 0x0817e8d6（ChangeHiddenOption.md 全文转写）
// ============================================================================

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "CDataManager.h"       // G_CDataManager / find_item
#include "LogManager.h"
#include "GlobalData.h"
#include "MoreTypes.h"          // arad::SigAvatarHiddenOptionChange / SigAradAvatarHiddenOptionSave
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "Arad_InternalDispatcher.h"   // DISPATCHER::make_internal_stream_jpn

int AvatarFixedHiddenOptionServer::GetRandomInput(ENUM_RARITY rarity)
{
    if (rarity >= 6)
        return -1;
    if (m_cumulativeRate[rarity].empty())
        return -1;
    return m_cumulativeRate[rarity].back();
}

bool AvatarFixedHiddenOptionServer::IsVaildSkillIndex(ENUM_RARITY rarity,
                                                      int idx)
{
    if (rarity >= 6)
        return false;
    int size = (int)m_script.m_abilities[rarity].size();
    if (size == 0 || idx >= size)
        return false;
    return true;
}

int AvatarFixedHiddenOptionServer::GetSkillIndex(ENUM_RARITY rarity, int randIdx)
{
    std::vector<int>& rates = m_cumulativeRate[rarity];
    // ORIG 0x0817e23e：无 empty() 前置检查，直接 back()（GetRandomInput 保证非空进入）。
    if (!(randIdx < rates.back() && 0 <= randIdx))
        return -1;
    std::vector<int>::iterator up =
        std::upper_bound(rates.begin(), rates.end(), randIdx);
    return (int)(up - rates.begin()) + 1;
}

int AvatarFixedHiddenOptionServer::GetSkillIndex(ENUM_RARITY rarity)
{
    int total = GetRandomInput(rarity);
    if (total == -1)
        return -1;
    int randIdx = get_rand_int(total);
    return GetSkillIndex(rarity, randIdx);
}

bool AvatarFixedHiddenOptionServer::CheckCondition(
    CUser* user, arad::SigAvatarHiddenOptionChange* sig)
{
    CInventory* invW = user->getCurCharacInvenW();
    // 材料槽（INVEN 1）—— ORIG 以 sret 形式调用 GetInvenSlot（按值返回）
    Inven_Item material = invW->GetInvenSlot(1, (int)sig->m_materialSlot);
    if (material.isEmpty())
    {
        LogManager::logFormat(
            1, "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp",
            __PRETTY_FUNCTION__,
            0x71,
            "AVATAR HIDDEN OPTION CHANGE : Material GetInvenSlot() failed. [slot:%d] [charac_no:%d]",
            (int)sig->m_materialSlot, (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 0x16);
        return false;
    }
    if (m_script.isMaterialItemIndex((int)material.m_addInfo))
    {
        Inven_Item avatar = invW->GetInvenSlot(2, (int)sig->m_avatarSlot);
        if (avatar.isEmpty())
        {
            LogManager::logFormat(
                1, "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp",
                __PRETTY_FUNCTION__,
                0x83,
                "AVATAR HIDDEN OPTION CHANGE : Avatar GetInvenSlot() failed. [slot:%d] [charac_no:%d]",
                (int)sig->m_avatarSlot, (unsigned int)user->getCurCharacNo());
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 0x16);
            return false;
        }
        CItem* item = G_CDataManager()->find_item((int)avatar.m_addInfo);
        if (item == 0 || !item->IsAvatarItem())
            return false;
        if (!item->IsHiddenOption())
        {
            LogManager::logFormat(
                1, "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp",
                __PRETTY_FUNCTION__,
                0x94,
                "AVATAR HIDDEN OPTION CHANGE : IsHiddenOption() failed. [index:%d] [charac_no:%d]",
                (int)avatar.m_addInfo, (unsigned int)user->getCurCharacNo());
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 0x11);
            return false;
        }
        return true;
    }
    LogManager::logFormat(
        1, "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp",
        __PRETTY_FUNCTION__,
        0x78,
        "AVATAR HIDDEN OPTION CHANGE : isMaterial() failed [index:%d] [charac_no:%d]",
        (int)material.m_addInfo, (unsigned int)user->getCurCharacNo());
    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 0x11);
    return false;
}

bool AvatarFixedHiddenOptionServer::ChangeHiddenOption(
    CUser* user, arad::SigAvatarHiddenOptionChange* sig)
{
    CInventory* invW = user->getCurCharacInvenW();
    if (invW->delete_item(INVEN_TYPE(1), (int)sig->m_materialSlot, 1,
                          eItemDelReason(3), true) != 1)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp",
            __PRETTY_FUNCTION__,
            0xa1,
            "AVATAR HIDDEN OPTION CHANGE : delete_item() failed. [slot:%d] [charac_no:%d]",
            (int)sig->m_materialSlot, (unsigned int)user->getCurCharacNo());
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24c, 0x13);
        return false;
    }
    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                             (int)sig->m_materialSlot);
    cMyTrace traceDel(
        __PRETTY_FUNCTION__,
        0xa6, 0);
    traceDel("AVATAR HIDDEN OPTION CHANGE : delete_item() success. [slot:%d] "
             "[charac_no:%d]",
             (int)sig->m_materialSlot, (unsigned int)user->getCurCharacNo());

    Inven_Item* ref = invW->GetInvenRef(2, (int)sig->m_avatarSlot);
    if (ref == 0)
        return false;
    CItem* item = G_CDataManager()->find_item((int)ref->m_addInfo);
    if (item == 0)
        return false;
    if (!item->IsAvatarItem())
        return false;

    int skillIdx = GetSkillIndex((ENUM_RARITY)item->get_grade());
    ref->m_fieldSplit.m_fielddLo = (short)skillIdx;      // +0x0d word 写入
    sig->m_hiddenOption = (unsigned short)(short)ref->m_fieldSplit.m_fielddLo;

    arad::SigAradAvatarHiddenOptionSave save;             // +0/…清零语义同 ORIG W ctor
    save.m_field0 = user->get_acc_id();
    save.m_characNo = (unsigned int)user->getCurCharacR()->m_characNo;
    save.m_itId = (unsigned int)ref->m_addInfo;
    save.m_slot = (short)((int)sig->m_avatarSlot + 10);
    save.m_hiddenOption = (short)ref->m_fieldSplit.m_fielddLo;

    cMyTrace traceOk(
        __PRETTY_FUNCTION__,
        0xc2, 0);
    traceOk("AVATAR HIDDEN OPTION CHANGE : success [slot:%d] [item_id:%d] "
            "[hidden_option:%d] [charac_no:%d]",
            (int)((int)sig->m_avatarSlot + 10), save.m_itId,
            (int)(short)save.m_hiddenOption,
            (unsigned int)user->getCurCharacNo());

    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localjapan/Arad_AvatarFixedHiddenOptionServer.cpp", 0xc6);
    CStreamGuard guard(stream, true);
    ARAD::DISPATCHER::make_internal_stream_jpn(
        guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2f, -1);
    guard.put_binary(&save, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), guard);
    user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)1,
                             (int)sig->m_avatarSlot);
    return true;
}
// ============================================================================
// UnitTest / Test1 —— ORIG UnitTest T 0x0817ecd8、Test1 t 0x0817ec67
// （docs/class_func_reports/AvatarFixedHiddenOptionServer/UnitTest.md、
//  docs/class_func_reports/_lt_global_gt_/Test1.md 全文转写）
// ============================================================================

// ORIG 0x0817ec67（文件内 static，_ZL 局部符号）：调 GetSkillIndex(rarity=3, input)
// 并把结果与期望值 printf 比对。
static void Test1(AvatarFixedHiddenOptionServer* server, int input, int expect)
{
    int output = server->GetSkillIndex((ENUM_RARITY)3, input);
    if (expect != output) {
        printf("input==%d, output!=%d/%d\n", input, output, expect);
    } else {
        printf("input==%d, output==%d/%d\n", input, output, expect);
    }
}

// ORIG 0x0817ecd8：非空 server 时依次跑 6 组 (input, expect) 用例。
void AvatarFixedHiddenOptionServer::UnitTest(AvatarFixedHiddenOptionServer* server)
{
    if (server == 0)
        return;
    Test1(server, 0, 1);
    Test1(server, 0x95, 1);
    Test1(server, 0x96, 2);
    Test1(server, 0x26dd, 0x3e);
    Test1(server, 0x26de, 0x3f);
    Test1(server, 9999, 0x3f);
}
