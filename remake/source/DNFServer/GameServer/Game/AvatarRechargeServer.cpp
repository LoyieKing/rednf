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
// [b24F] DONE：改 break 结构复现 ORIG 双出口循环（指令序列一致）。
// ============================================================================
static int getSocketCount(const stAvatarEmblemInfo_t* emblem)
{
    int count = 0;
    for (int i = 0; i <= 4; ++i)
    {
        if (emblem->stEmblemSocket[i].emblemSocketType_ == 0)
            break;
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
    // [b28c-L9] 残差 2 = 4.1 为前两级各发射一枚空 else 中继岛
    // （0x08190494 `nop; jmp exit`、0x08190497 `nop`）；4.4 三形实测均不闭合
    // （否定早返 +2 jmp、正向嵌套 −2 jmp、P223 尾 goto 标签串 +2 jmp）
    // ⇒ 空块条数跨版本不可择（P223/P184 负例），保持可读早返形。
    if (user == 0)
        return;
    if (user->get_state() != 5)
        return;
    if (param2 != 1)
        return;
    if (param4 != 3)
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
    const Inven_Item* stone =
        user->getCurCharacInvenR()->GetInvenRef(1, (int)sig->stoneSlot);
    if (stone == 0 || stone->isEmpty())
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x16);
        ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                   sig->stoneSlot);
        return false;
    }
    // b28d-lane25：P167 差集实证 ORIG 0x081907b8/0x081907cd 直接内联
    // map find/end + eq（end() 在左控实参序，P74 补充），不调 IsEmblemRechargeStone。
    std::map<int, int>::iterator it = m_emblemRechargeStoneMap.find(stone->m_addInfo);
    if (m_emblemRechargeStoneMap.end() == it)
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
        user->getCurCharacInvenR()->GetInvenRef(2, (int)sig->avatarSlot);
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
        (const stAvatarEmblemInfo_t*)user->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
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

    if (user->getCurCharacInvenW()->delete_item((INVEN_TYPE)1, sig->stoneSlot, 1,
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

        Inven_Item* avatar = user->getCurCharacInvenW()->GetInvenRef(2, (int)sig->avatarSlot);
        if (avatar == 0 || avatar->isEmpty())
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0x24d, 0x16);
            ARAD::PRINT_ERROR::isEmpty((char*)"USE AVATAR RECHANGE", user,
                                       sig->avatarSlot);
            return false;
        }

        int newEndur = (int)avatar->m_fieldSplit.m_avatarEndur +
                       m_emblemRechargeStoneMap[stoneItemId];
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
        // b28d-lane25：P167 差集实证 ORIG 非 const iterator + 先声明后赋值（P50/P96）。
        std::map<int, int>::iterator it;
        it = m_useEmblemGradeRateMap.find(grades[i]);
        if (it != m_useEmblemGradeRateMap.end())
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
    // [b23S3C] P48 去缓存：ORIG getCurCharacInvenW 结果 eax 直落实参槽
    // （0x32 mov %eax,(%esp); call GetInvenRef），无 invenW 槽 ⇒ 右值直调。
    Inven_Item* item = user->getCurCharacInvenW()->GetInvenRef(0, (int)slot);
    if (item == 0 || item->isEmpty())
        return;

    const stAvatarEmblemInfo_t* socketData =
        (const stAvatarEmblemInfo_t*)
            user->getCurCharacInvenR()->GetAvatarItemMgrR()->getJewelSocketData(
                item->m_addInfo2);
    // [b23S3C] ORIG 无 sockets/limit 槽（getSocketCount→%ebx 跨 call 缓存 +
    // getLimit 返回 eax 直 imul），仅乘积落 -0x14 ⇒ 合并为单局部；调用顺序
    // getSocketCount 先、Singleton::Get 后（左→右求值同 ORIG）。
    const int maxEndurance =
        getSocketCount(socketData) *
        ARAD::Singleton<AvatarRechargeServer>::Get()
            ->getAvatarRecharageLimit();

    int newEndur = (int)item->m_fieldSplit.m_avatarEndur + (int)delta;
    if (newEndur < 0)
        newEndur = 0;
    else if (newEndur > maxEndurance)
        newEndur = maxEndurance;
    item->m_fieldSplit.m_avatarEndur = (unsigned short)newEndur;

    // [b23S3C] ORIG 0x111 直压 3 参（target/user/slot）无 Get() call ⇒ static 调。
    UpdateAvatarEndurance((CUser::eSendTarget)1, user, slot);
    SaveAvatarEnduranceUnit(item->m_addInfo2,
                            (int)(unsigned short)item->m_fieldSplit.m_avatarEndur);

    // [b23S3C] ORIG -0xc 槽存串指针两条（movl $str; mov →0x4 esp）⇒ char*
    // 具名局部（P0 槽往返），非 char 数组逐字节初始化。
    char* message = (char*)"succ avatar Endurance change";
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
    // [b28c-L9] ORIG 0x0819136a `mov 0x10(%ebp),%eax; mov %al,-0x1c(%ebp)` 是 bool
    // 形参的单次截断 spill（非具名局部——再加 `bool bEn` 会多一对
    // `movzbl -0x1c,%eax; mov %al,-0x11` 中转）。
    m_bUseEnduranceReduceRate = enable;
    if (enable)
    {
        if (rate < 0)
            rate = 0;
        else if (rate > 100)
            rate = 1000;
        else
            rate *= 10;
        m_enduranceReduceRate = rate;

        // [b28c-L9] 旧「字面量直填 N 封顶」判词作废：ORIG 0x081913b6
        // `movl $str,-0x10(%ebp); mov -0x10(%ebp),%eax; mov %eax,0x4(%esp)`
        // 与 0x081913db 的 -0xc 同形 ⇒ 原码是具名 char* 局部（P0），
        // 两臂各一枚（-0x10 深 / -0xc 浅）。
        char* msgOn = (char*)"succ avatar Endurance reduce rate mode on! change rate";
        ARAD::notifySystemMessage(user, msgOn);
    }
    else
    {
        m_enduranceReduceRate = 0;
        char* msgOff = (char*)"avatar Endurance reduce rate mode off";
        ARAD::notifySystemMessage(user, msgOff);
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
    // [b28c-L9] 旧「构造序跨版本 N」判词作废：ORIG C1 0x08190200 的
    // `mov this; add $0x4,%eax; call ARAD::SCRIPT::AvatarRecharge::C1` 先于 vptr、
    // 且 vptr 在三个 call 之后才写 ⇒ 脚本子对象是**基类**而非成员。
    // 按 Itanium primary-base 规则把 AvatarRecharge 提为第一基类（非动态 ⇒ 不占
    // vptr；ScriptInterface 动态 ⇒ primary base，vptr@+0x00），
    // sizeof 0x5c/全偏移不变（头内 static_assert 自证）⇒ 21=21 AE。
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
    if (loadScript())
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

// ORIG 0x0819114c：非静态成员，find 于 this+0x38（= m_useEmblemGradeRateMap）。
int AvatarRechargeServer::GetRateFromEmblemGrade(int grade)
{
    // [b28c-L9] ORIG 0x0819114c：find→-0x10、end→-0xc 两匿名槽 + 外链
    // `_Rb_tree_iterator::operator!=`（非 const 迭代器，P96）+ 正极性
    // `it != end` → `it->second`（P220/P47）。旧「const 迭代器 + ==end」形
    // 多生成转换构造与 operator== 外链（44 vs 34 insn）。
    std::map<int, int>::iterator it = m_useEmblemGradeRateMap.find(grade);
    if (it != m_useEmblemGradeRateMap.end())
        return it->second;
    return 0;
}

// ORIG 0x08190dea（静态）：Acquire 流 → make_internal_stream_jpn(0x31, -1) → put_binary 8 字节
// → DB_Q。不触碰 this，与 ORIG 一致保持静态。
void AvatarRechargeServer::SaveAvatarEnduranceUnit(arad::SigAradAvatarEnduranceSave& save)
{
    // ORIG（idx66 报告）：Acquire 结果直传 CStreamGuard ctor 出参槽（P0 无具名
    // 局部）；make_internal_stream_jpn 后经 operator->() 再 put_binary。
    // [b23S3C] 在途复验：fresh .o 52=52（norm），仅 put_binary 实参装载寄存器/时刻
    // 差（P75 跨版本形），方向正确保留。
    // [b28c-L9] 复核定稿 ndiff=3：ORIG `mov 0x8(%ebp),%edx; mov %edx,0x4(%esp)`，
    // 4.4 提前把同一值升格 %ebx 跨 call 保活（N7 角色轮换 + N18a 单段直落不可造）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("localjapan/Arad_EmblemReformServer.cpp", 0x10f),
        true);
    ARAD::DISPATCHER::make_internal_stream_jpn(guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x31, 0xffffffff);
    guard->put_binary((char*)&save, 8);
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
    // P56：ORIG 尾部 choose 岛（test;je;mov$1;jmp;mov$0）⇒ if-return 形。
    if (m_emblemRechargeStoneMap.find(itemIndex) != m_emblemRechargeStoneMap.end())
        return true;
    return false;
}

std::map<int, int>* ARAD::SCRIPT::AvatarRecharge::getEmblemRechargeStoneInfo()
{
    return &m_emblemRechargeStoneMap;   // ORIG return this+4
}

int ARAD::SCRIPT::AvatarRecharge::getEmblemRecharge(int grade)
{
    // ORIG 0x088b132a：+0x1c map 值或 0。
    // b28d-lane25：P167 差集实证 ORIG 用非 const iterator + 先声明后赋值（C1Ev）
    // + `!=` 正极性（P50 反例复活 / P96）。
    std::map<int, int>::iterator it;
    it = m_emblemRechargeMap.find(grade);
    if (it != m_emblemRechargeMap.end())
        return it->second;
    return 0;
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
                // b28d-lane25：P167 差集实证 ORIG 三段均为
                // `if (find==end) insert(value_type(k,v)); else {printf;return}`（eq 形）。
                if (m_emblemRechargeStoneMap.find(idx) == m_emblemRechargeStoneMap.end())
                {
                    m_emblemRechargeStoneMap.insert(
                        std::map<int, int>::value_type(idx, value));
                }
                else
                {
                    printf("[emblem recharge stone info] duplicate item_index:%d", idx);
                    return -2;
                }
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
                if (m_emblemRechargeMap.find(grade) == m_emblemRechargeMap.end())
                {
                    m_emblemRechargeMap.insert(
                        std::map<int, int>::value_type(grade, value));
                }
                else
                {
                    printf("[emblem recharge] duplicate grade:%d", grade);
                    return -3;
                }
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
                if (m_useEmblemGradeRateMap.find(grade) == m_useEmblemGradeRateMap.end())
                {
                    m_useEmblemGradeRateMap.insert(
                        std::map<int, int>::value_type(grade, value));
                }
                else
                {
                    printf("[recharge use emblem info] duplicate grade:%d", grade);
                    return -4;
                }
            }
        }
    }

    unloadRDARScriptFile();
    m_loaded = true;
    return 0;
}
