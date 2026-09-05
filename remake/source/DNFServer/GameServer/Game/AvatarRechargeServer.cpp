// df_game_r Game/ AvatarRechargeServer 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 依据 docs/class_func_reports/AvatarRechargeServer/、ARAD__SCRIPT__AvatarRecharge/
// 与 ORIG 反汇编转写。
#include "AvatarRechargeServer.h"
#include "Arad_InternalDispatcher.h"  // ARAD::DISPATCHER::make_internal_stream_jpn

#include <cstdio>
#include <string>
#include <string.h>

#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "GameTypes.h"        // cMyTrace
#include "MoreTypes.h"        // arad::SigArad*
#include "DNFLexWrapper.h"    // loadRDARScriptFile / unloadRDARScriptFile / Scan*
#include "CUser.h"            // CUser（含 eSendTarget/get_state/SendPacket 等）
#include "CInventory.h"       // CInventory::GetInvenRef/delete_item/GetAvatarItemMgrR
#include "CAvatarItemMgr.h"   // WongWork::CAvatarItemMgr::getJewelSocketData
#include "CParty.h"           // CParty::IsUseFatigueDungeon/CheckMapClear
#include "Inven_Item.h"       // Inven_Item::isEmpty/m_addInfo/m_addInfo2/m_fieldSplit
#include "GamePODTypes.h"     // stAvatarEmblemInfo_t
#include "PacketGuard.h"      // PacketGuard（InterfacePacketBuf put_*/finalize）
#include "LogManager.h"       // LogManager::logFormat

// make_internal_stream_jpn 定义在 Arad_InternalDispatcher.cpp（ORIG 0x081977b6，
// 签名 (CStreamGuard&, ENUM_INTERNALPACKET_JPN, int)）。

// ============================================================================
// getSocketCount —— ORIG l 0x08190699 _ZL14getSocketCountPK20stAvatarEmblemInfo_t
// （文件局部静态）。遍历 5 个徽章槽（每槽 6 字节，取首 ushort emblemSocketType_），
// 非零计数。ORIG 不判空（调用方经 getJewelSocketData 保证非空），忠实转写。
// ============================================================================
static int getSocketCount(const stAvatarEmblemInfo_t* emblem)
{
    int count = 0;
    for (int i = 0; i <= 4; ++i)
    {
        if (emblem->stEmblemSocket[i].emblemSocketType_ != 0)
            ++count;
    }
    return count;
}

// ============================================================================
// SendDurability —— ORIG T 0x08190416 _ZN...14SendDurabilityEP5CUsercsci
// ============================================================================
void AvatarRechargeServer::SendDurability(CUser* user, char param2, short param3,
                                          char param4, int param5)
{
    if (user == 0 || user->get_state() != 5)
        return;
    if (param2 != 1 || param4 != 3)
        return;
    // 目标：默认单人(1)，在队伍中改队伍(2)。
    CUser::eSendTarget target = (CUser::eSendTarget)1;
    if (user->GetParty() != 0)
        target = (CUser::eSendTarget)2;
    UpdateAvatarEndurance(target, user, (short)param5);
}

// ============================================================================
// CalcDurability —— ORIG T 0x0819049a _ZN...14CalcDurabilityEP5CUseri
// ============================================================================
void AvatarRechargeServer::CalcDurability(CUser* user, int count)
{
    for (int round = 0; round < count; ++round)
    {
        std::vector<int> rates;
        rates.reserve(10);
        for (int slot = 0; slot <= 9; ++slot)
            CalcDurabilityDetail(user, slot, rates);
        if (!rates.empty())
        {
            for (std::vector<int>::size_type i = 0; i < rates.size(); ++i)
                UpdateAvatarEndurance((CUser::eSendTarget)2, user,
                                      (short)rates[i]);
        }
    }
}

// CalcDurabilityChangeEquip —— ORIG T 0x081905b4：定义 UserCoreDepsB2.cpp（去重保留）。

// ============================================================================
// CheckConditionUseRecharge —— ORIG T 0x081906e2
// 充值石(区1)/头像(区2)存在性、IsEmblemRechargeStone、CheckItemLock×2 校验；
// 通过时 sig->maxEndurance = m_rechargeLimit × socket 数，返 true。
// ============================================================================
bool AvatarRechargeServer::CheckConditionUseRecharge(
    CUser* user, arad::SigAradUseAvatarRecharge* sig)
{
    const CInventory* invenR = user->getCurCharacInvenR();
    const Inven_Item* stone =
        invenR->GetInvenRef(1, (int)sig->stoneSlot);
    if (stone == 0 || stone->isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x16);
        ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                   sig->stoneSlot);
        return false;
    }
    if (!m_script.IsEmblemRechargeStone(stone->m_addInfo))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x11);
        LogManager::logFormat(
            1, "localjapan/Arad_EmblemReformServer.cpp",
            "bool AvatarRechargeServer::CheckConditionUseRecharge(CUser*, "
            "arad::SigAradUseAvatarRecharge*)",
            0xad,
            "USE AVATAR RECHANGE : [item_id:%d] isn't RechargeStone [charac_no:%d]",
            stone->m_addInfo, user->getCurCharacNo());
        return false;
    }
    if (user->CheckItemLock(1, (int)sig->stoneSlot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0xd5);
        ARAD::PRINT_ERROR::CheckItemLock((char*)"USE AVATAR RECHANGE", user,
                                         sig->stoneSlot);
        return false;
    }

    const Inven_Item* avatar =
        invenR->GetInvenRef(2, (int)sig->avatarSlot);
    if (avatar == 0 || avatar->isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x16);
        ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                   sig->avatarSlot);
        return false;
    }
    if (user->CheckItemLock(1, (int)sig->avatarSlot))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0xd5);
        ARAD::PRINT_ERROR::CheckItemLock((char*)"USE AVATAR RECHANGE", user,
                                         sig->avatarSlot);
        return false;
    }

    const stAvatarEmblemInfo_t* socketData =
        (const stAvatarEmblemInfo_t*)invenR->GetAvatarItemMgrR()->getJewelSocketData(
            avatar->m_addInfo2);
    sig->maxEndurance = *(int*)((char*)this + 4) * getSocketCount(socketData);
    if ((int)(unsigned int)(unsigned short)avatar->m_fieldSplit.m_avatarEndur <
        sig->maxEndurance)
    {
        return true;
    }

    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0xd8);
    LogManager::logFormat(
        1, "localjapan/Arad_EmblemReformServer.cpp",
        "bool AvatarRechargeServer::CheckConditionUseRecharge(CUser*, "
        "arad::SigAradUseAvatarRecharge*)",
        0xcd,
        "USE AVATAR RECHANGE : already max endurance [ui_id:%d] [charac_no:%d]",
        avatar->m_addInfo2, user->getCurCharacNo());
    return false;
}

// ============================================================================
// Recharge —— ORIG T 0x08190a70
// 删充值石 → 按 [emblem recharge stone info][itemId] 加耐久（clamp 上限）
// → 回传 addEndurance/enduranceAfter → DB 保存流。
// ============================================================================
bool AvatarRechargeServer::Recharge(CUser* user,
                                    arad::SigAradUseAvatarRecharge* sig)
{
    const CInventory* invenR = user->getCurCharacInvenR();
    const Inven_Item* stoneRef =
        invenR->GetInvenRef(1, (int)sig->stoneSlot);
    const int stoneItemId = stoneRef->m_addInfo;   // ORIG 判空前读取，忠实保序
    if (stoneRef == 0 || stoneRef->isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x16);
        ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                   sig->stoneSlot);
        return false;
    }

    CInventory* invenW = user->getCurCharacInvenW();
    if (invenW->delete_item((INVEN_TYPE)1, sig->stoneSlot, 1,
                            (eItemDelReason)3, true))
    {
        user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)0,
                                 sig->stoneSlot);
        cMyTrace trace(
            "bool AvatarRechargeServer::Recharge(CUser*, "
            "arad::SigAradUseAvatarRecharge*)",
            0xe8, 0);
        trace("USE AVATAR RECHANGE : delete_item() success. [slot:%d] "
              "[charac_no:%d]",
              (int)sig->stoneSlot, user->getCurCharacNo());

        Inven_Item* avatar = invenW->GetInvenRef(2, (int)sig->avatarSlot);
        if (avatar == 0 || avatar->isEmpty())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x16);
            ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                       sig->avatarSlot);
            return false;
        }

        int newEndur = (int)avatar->m_fieldSplit.m_avatarEndur +
                       m_script.m_emblemRechargeStoneMap[stoneItemId];
        if (newEndur > sig->maxEndurance)
            newEndur = sig->maxEndurance;
        sig->addEndurance =
            newEndur - (int)avatar->m_fieldSplit.m_avatarEndur;
        avatar->m_fieldSplit.m_avatarEndur = (unsigned short)newEndur;
        sig->enduranceAfter =
            (unsigned int)(unsigned short)avatar->m_fieldSplit.m_avatarEndur;

        user->SendUpdateItemList((CUser::eSendTarget)1, (ENUM_ITEMSPACE)1,
                                 sig->avatarSlot);
        cMyTrace trace2(
            "bool AvatarRechargeServer::Recharge(CUser*, "
            "arad::SigAradUseAvatarRecharge*)",
            0xfe, 0);
        trace2("USE AVATAR RECHARGE : success [slot:%d] [item_id:%d] "
               "[Endurance:%d] [charac_no:%d]",
               (int)sig->avatarSlot, avatar->m_addInfo,
               (int)(unsigned short)avatar->m_fieldSplit.m_avatarEndur,
               user->getCurCharacNo());

        arad::SigAradAvatarEnduranceSave save;   // ORIG ctor：清 +0/+4
        save.uiId = (unsigned int)avatar->m_addInfo2;
        save.endurance =
            (unsigned int)(unsigned short)avatar->m_fieldSplit.m_avatarEndur;
        SaveAvatarEnduranceUnit(save);
        return true;
    }

    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x13);
    ARAD::PRINT_ERROR::delete_item((char*)"USE AVATAR RECHANGE", user,
                                   sig->stoneSlot);
    return false;
}

// ============================================================================
// UseEmblem —— ORIG T 0x08190ed8
// 徽章合成加耐久：累加 grades 在 [recharge use emblem info] 的数值并 clamp。
// ============================================================================
bool AvatarRechargeServer::UseEmblem(CUser* user, std::vector<int>& grades,
                                     Inven_Item* emblemItem, short slot)
{
    const CInventory* invenR = user->getCurCharacInvenR();
    const stAvatarEmblemInfo_t* socketData =
        (const stAvatarEmblemInfo_t*)invenR->GetAvatarItemMgrR()->getJewelSocketData(
            emblemItem->m_addInfo2);
    const int maxEndurance =
        *(int*)((char*)this + 4) * getSocketCount(socketData);

    int addSum = 0;
    for (std::vector<int>::size_type i = 0; i < grades.size(); ++i)
    {
        std::map<int, int>::const_iterator it =
            m_script.m_useEmblemGradeRateMap.find(grades[i]);
        if (it != m_script.m_useEmblemGradeRateMap.end())
            addSum += it->second;
    }

    CInventory* invenW = user->getCurCharacInvenW();
    Inven_Item* avatar = invenW->GetInvenRef(2, (int)slot);
    if (avatar == 0 || avatar->isEmpty())
    {
        ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                   (unsigned short)slot);
        return false;
    }

    int newEndur = (int)avatar->m_fieldSplit.m_avatarEndur + addSum;
    if (newEndur > maxEndurance)
        newEndur = maxEndurance;
    const int added = newEndur - (int)avatar->m_fieldSplit.m_avatarEndur;
    avatar->m_fieldSplit.m_avatarEndur = (unsigned short)newEndur;

    UpdateAvatarEndurance((CUser::eSendTarget)1, user, slot);

    cMyTrace trace(
        "bool AvatarRechargeServer::UseEmblem(CUser*, "
        "std::vector<int, std::allocator<int> >&, Inven_Item*, short int)",
        0x142, 0);
    trace("USE EMBLEM + RECHARGE : success [slot:%d] [item_id:%d] "
          "[Endurance:%d] [charac_no:%d]",
          (int)slot, avatar->m_addInfo,
          (int)(unsigned short)emblemItem->m_fieldSplit.m_avatarEndur,
          user->getCurCharacNo());

    arad::SigAradAvatarEnduranceSave save;   // ORIG ctor：清 +0/+4
    save.uiId = (unsigned int)avatar->m_addInfo2;
    save.endurance =
        (unsigned int)(unsigned short)avatar->m_fieldSplit.m_avatarEndur;
    SaveAvatarEnduranceUnit(save);
    return true;
}

// ============================================================================
// GmReduceEndurance —— ORIG T 0x081911b6 _ZN...17GmReduceEnduranceEP5CUserss
// ============================================================================
void AvatarRechargeServer::GmReduceEndurance(CUser* user, short slot,
                                             short delta)
{
    if (slot < 0 || slot > 9)
        return;
    CInventory* invenW = user->getCurCharacInvenW();
    Inven_Item* item = invenW->GetInvenRef(0, (int)slot);
    if (item == 0 || item->isEmpty())
        return;

    const stAvatarEmblemInfo_t* socketData =
        (const stAvatarEmblemInfo_t*)
            user->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
                item->m_addInfo2);
    const int sockets = getSocketCount(socketData);
    const int limit = ARAD::Singleton<AvatarRechargeServer>::Get()
                          ->m_script.getAvatarRecharageLimit();
    const int maxEndurance = limit * sockets;

    int newEndur = (int)item->m_fieldSplit.m_avatarEndur + (int)delta;
    if (newEndur < 0)
        newEndur = 0;
    else if (newEndur > maxEndurance)
        newEndur = maxEndurance;
    item->m_fieldSplit.m_avatarEndur = (unsigned short)newEndur;

    ARAD::Singleton<AvatarRechargeServer>::Get()->UpdateAvatarEndurance((CUser::eSendTarget)1, user, slot);   // static 化后无 this，经单例调成员
    SaveAvatarEnduranceUnit(item->m_addInfo2,
                            (int)(unsigned short)item->m_fieldSplit.m_avatarEndur);

    char message[] = "succ avatar Endurance change";
    ARAD::notifySystemMessage(user, message);
}

// ============================================================================
// GmChangeEnduranceReduceRate —— ORIG T 0x08191318
// GM 前置校验后转发 GmSetEnduranceReduceRate（ORIG 经 Singleton::Get 取 this，
// Meyers 单例同一对象，语义一致）。
// ============================================================================
void AvatarRechargeServer::GmChangeEnduranceReduceRate(CUser* user, bool enable,
                                                       int rate)
{
    if (!user->isGMUser())
        return;
    // ORIG 0x08191318：静态（无 this），经 Singleton::Get 取实例后转发
    ARAD::Singleton<AvatarRechargeServer>::Get()->GmSetEnduranceReduceRate(
        user, enable, rate);
}

// ============================================================================
// GmSetEnduranceReduceRate —— ORIG T 0x08191364
// enable 写 +0x58；rate<0→0 / ≤100→×10 / >100→1000 写 +0x54；系统提示。
// ============================================================================
void AvatarRechargeServer::GmSetEnduranceReduceRate(CUser* user, bool enable,
                                                    int rate)
{
    m_bUseEnduranceReduceRate = enable;
    if (enable)
    {
        if (rate < 0)
            rate = 0;
        else if (rate <= 100)
            rate *= 10;
        else
            rate = 1000;
        m_enduranceReduceRate = rate;

        // ORIG 0x08191364：const char* 字面量直传（movl $0x8b9a544）
        ARAD::notifySystemMessage(
            user, (char*)"succ avatar Endurance reduce rate mode on! change rate");
    }
    else
    {
        m_enduranceReduceRate = 0;

        // ORIG 0x08191364：const char* 字面量直传（movl $0x8b9a57c）
        ARAD::notifySystemMessage(user, (char*)"avatar Endurance reduce rate mode off");
    }
}

// ============================================================================
// UpdateAvatarEndurance —— ORIG T 0x081913f6
// 组 0x220 包：uid 低 16 位 / slot / item_idx / 耐久计数，SendPacket 上报。
// ============================================================================
void AvatarRechargeServer::UpdateAvatarEndurance(CUser::eSendTarget target,
                                                 CUser* user, short slot)
{
    if (slot < 0 || slot > 9)
        return;
    CInventory* invenW = user->getCurCharacInvenW();
    Inven_Item* item = invenW->GetInvenRef(0, (int)slot);
    if (item == 0 || item->isEmpty())
        return;

    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0x220);
    packet.put_short(user->get_unique_id() & 0xffff);
    packet.put_short((int)slot);
    packet.put_int(item->m_addInfo);
    packet.put_short(
        (int)(unsigned short)item->m_fieldSplit.m_avatarEndur);
    packet.finalize(true);
    user->SendPacket(target, packet);
}

// ============================================================================
// AvatarRechargeServer —— ORIG C1 0x08190200 / D1 0x08190246 /
// Load 0x0819026c / Destroy 0x081902de / Test 0x08190266
// ============================================================================

// ORIG C1：SCRIPT 子对象构造 + 清 +0x54 与字节 +0x58（+0x00 上限字段不初始化，忠实 ORIG）。
AvatarRechargeServer::AvatarRechargeServer()
    : m_enduranceReduceRate(0),
      m_bUseEnduranceReduceRate(false)
{
}

AvatarRechargeServer::~AvatarRechargeServer()
{
}

// ORIG 0x0819026c：cMyTrace(__PRETTY_FUNCTION__,0x22,9,true,false) +
// "\t- Loading Avatar Recharge script - "；loadScript()==0 即成功。
bool AvatarRechargeServer::Load()
{
    cMyTrace trace(__PRETTY_FUNCTION__, 0x22, 9, true, false);
    trace("\t- Loading Avatar Recharge script - ");
    if (m_script.loadScript())
        return false;
    return true;
}

// ORIG 0x081902de：恒返回 1（接口 void 化同 CerashopAddRestrict::Manager 先例）。
int AvatarRechargeServer::Destroy()
{
    return 1;
}

// ORIG 0x08190266：纯 ret。
void AvatarRechargeServer::Test()
{
}

// ORIG 0x0819114c：非静态成员，find 于 this+0x38（= m_script.m_useEmblemGradeRateMap）。
int AvatarRechargeServer::GetRateFromEmblemGrade(int grade)
{
    std::map<int, int>::const_iterator it = m_script.m_useEmblemGradeRateMap.find(grade);
    if (it == m_script.m_useEmblemGradeRateMap.end())
        return 0;
    return it->second;
}

// ORIG 0x08190dea（静态）：Acquire 流 → make_internal_stream_jpn(0x31, -1) → put_binary 8 字节
// → DB_Q。不触碰 this，与 ORIG 一致保持静态。
void AvatarRechargeServer::SaveAvatarEnduranceUnit(arad::SigAradAvatarEnduranceSave& save)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("localjapan/Arad_EmblemReformServer.cpp", 0x10f);
    CStreamGuard guard(stream, true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x31, 0xffffffff);
    guard.put_binary((char*)&save, 8);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ORIG 0x08190eae（静态）：SigAradAvatarEnduranceSave ctor（清 +0/+4）后
// 写 uiId/endurance 两字段，走引用重载。
void AvatarRechargeServer::SaveAvatarEnduranceUnit(int key, int value)
{
    arad::SigAradAvatarEnduranceSave save;   // ORIG ctor：清 +0/+4
    save.uiId = (unsigned int)key;
    save.endurance = (unsigned int)value;
    SaveAvatarEnduranceUnit(save);
}

// ============================================================================
// ARAD::SCRIPT::AvatarRecharge —— ORIG C1 0x088b0dd0 / D1 0x088b0e46 /
// loadScript 0x088b0eba / clearScript 0x088b12d8 / 访问器 0x88b1314..0x88b13a6
// ============================================================================

// ORIG C1：三个 map 默认构造 + 清 +0x4c 已装载标志（+0x00 上限不初始化）。
ARAD::SCRIPT::AvatarRecharge::AvatarRecharge()
    : m_loaded(false)
{
}

ARAD::SCRIPT::AvatarRecharge::~AvatarRecharge()
{
}

// ORIG 0x088b12d8：上限清零 + 三 map clear。
void ARAD::SCRIPT::AvatarRecharge::clearScript()
{
    m_rechargeLimit = 0;
    m_emblemRechargeStoneMap.clear();
    m_emblemRechargeMap.clear();
    m_useEmblemGradeRateMap.clear();
}

int ARAD::SCRIPT::AvatarRecharge::getAvatarRecharageLimit()
{
    return m_rechargeLimit;   // ORIG return *(int*)this
}

bool ARAD::SCRIPT::AvatarRecharge::IsEmblemRechargeStone(int itemIndex)
{
    // ORIG 0x088b13a6：+0x04 map find 命中返回 true。
    return m_emblemRechargeStoneMap.find(itemIndex) != m_emblemRechargeStoneMap.end();
}

std::map<int, int>* ARAD::SCRIPT::AvatarRecharge::getEmblemRechargeStoneInfo()
{
    return &m_emblemRechargeStoneMap;   // ORIG return this+4
}

int ARAD::SCRIPT::AvatarRecharge::getEmblemRecharge(int grade)
{
    // ORIG 0x088b132a：+0x1c map 值或 0。
    std::map<int, int>::const_iterator it = m_emblemRechargeMap.find(grade);
    if (it == m_emblemRechargeMap.end())
        return 0;
    return it->second;
}

// ORIG 0x088b0eba loadScript。忠实转写：
//   已装载→0；打开失败→-1；重复键 printf（ORIG 无换行符）后分别 -2/-3/-4 且不卸载脚本；
//   主循环 ScanType 失败即 unloadRDARScriptFile + 置已装载 + 返回 0。
// [recharge use emblem info] 段 grade>=8 的行不消费数值直接退出内层（ORIG 同）。
int ARAD::SCRIPT::AvatarRecharge::loadScript()
{
    if (m_loaded)
        return 0;

    if (!loadRDARScriptFile("Etc/EmblemRenewal/", "avatar_recharge_jpn.etc"))
        return -1;

    std::string token;
    bool isType = false;
    bool isInt = false;

    for (;;)
    {
        if (!ScanType(token, true))
            break;

        if (token == "[avatar recharge limit]")
        {
            m_rechargeLimit = ScanInt((bool*)NULL);
        }
        else if (token == "[emblem recharge stone info]")
        {
            for (;;)
            {
                int idx = ScanTypeOrInt(isType, token, isInt, true);
                if (!(!isType && isInt))
                    break;
                int value = ScanInt((bool*)NULL);
                if (m_emblemRechargeStoneMap.find(idx) != m_emblemRechargeStoneMap.end())
                {
                    printf("[emblem recharge stone info] duplicate item_index:%d", idx);
                    return -2;
                }
                m_emblemRechargeStoneMap.insert(std::make_pair(idx, value));
            }
        }
        else if (token == "[emblem recharge]")
        {
            for (;;)
            {
                int grade = ScanTypeOrInt(isType, token, isInt, true);
                if (!(!isType && isInt))
                    break;
                int value = ScanInt((bool*)NULL);
                if (m_emblemRechargeMap.find(grade) != m_emblemRechargeMap.end())
                {
                    printf("[emblem recharge] duplicate grade:%d", grade);
                    return -3;
                }
                m_emblemRechargeMap.insert(std::make_pair(grade, value));
            }
        }
        else if (token == "[recharge use emblem info]")
        {
            for (;;)
            {
                int grade = ScanTypeOrInt(isType, token, isInt, true);
                if (!(!isType && isInt))
                    break;
                if (grade >= 8)
                    break;
                int value = ScanInt((bool*)NULL);
                if (m_useEmblemGradeRateMap.find(grade) != m_useEmblemGradeRateMap.end())
                {
                    printf("[recharge use emblem info] duplicate grade:%d", grade);
                    return -4;
                }
                m_useEmblemGradeRateMap.insert(std::make_pair(grade, value));
            }
        }
    }

    unloadRDARScriptFile();
    m_loaded = true;
    return 0;
}
