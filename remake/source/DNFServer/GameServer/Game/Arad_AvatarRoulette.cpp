// Arad_AvatarRoulette — 原始工程 localjapan/Arad_AvatarRoulette.cpp 的复刻 TU。
// 收录 AvatarCoin 的「使用侧」方法（ORIG 参照 docs/class_func_reports/AvatarCoin/）：
//   Use（前批次）/ SaveToDB / SendSyncPacket / GetPackage / HistoryLog::AddLog
//   以及 CUser::_GetSaveInvenBase（SaveToDB 的内联依赖，ORIG 独立 T 符号）。
// 另含 AvatarRouletteServer::isAvatarPottery（0x817fb0a）。
// 注意：Use 调用 IsUsable 必须跨 TU（PLT 调用 → ORIG 帧 sub $0x18），
// 与 AvatarCoin.cpp 保持分 TU。

#include "AvatarCoin.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "LogManager.h"
#include "PacketGuard.h"
#include "CDataManager.h"             // DataManagerInitDepends.h 前置依赖
#include "DataManagerInitDepends.h"   // AvatarRoulette 权威布局（成员声明）
#include <cstring>

// isAvatarPottery 为 AvatarRouletteServer 成员（ORIG T 0x0817fb0a，定义
// CtorProducers_1.cpp；本 TU 经 DataManagerInitDepends.h 类声明调用）。

// ---- AvatarRoulette::getAvatarList ORIG T 0x08911db0 ----
// 汇编形状（range 0x4d）：*isOk = 0；job<0 或 job>10 直接返回；
// 否则 out = m_avatars[job]（vector::operator=，PLT call）后 *isOk = 1。
// 地址计算 = this + job*0xc + 0x14（P30 两步寻址：*3 shl$2 后 +0x10 再 +0x4）。
void AvatarRoulette::getAvatarList(
    bool& isOk, ENUM_CHARACTERJOB job,
    std::vector<std::vector<std::pair<unsigned long, int> > >& out)
{
    isOk = false;
    if (job >= 0 && job <= 10)
    {
        out = m_avatars[job];
        isOk = true;
    }
    return;
}

// ---- AvatarRoulette::getStikerList ORIG T 0x08911dfe ----
// 同 getAvatarList 结构；out = m_stikers[job]（+0x90 再 +0x8 → +0x98）。
void AvatarRoulette::getStikerList(bool& isOk, ENUM_CHARACTERJOB job,
                                   std::vector<int>& out)
{
    isOk = false;
    if (job >= 0 && job <= 10)
    {
        out = m_stikers[job];
        isOk = true;
    }
    return;
}

unsigned int AvatarCoin::Use()
{
    // register 绑定使 -O0 下分支条件直接用 test %al 取字节（ORIG 形态），
    // 不产生 setne 提升。
    register char r __asm__("al") = IsUsable();
    if (r != 0)
    {
        m_coin = m_coin + -1;
        return 1;
    }
    return 0;
}

// ---- AvatarCoin 对象位于角色背包基址 +0x658（ORIG 全局约定） ----
static inline AvatarCoin* coinOf(char* invenBase)
{
    return reinterpret_cast<AvatarCoin*>(invenBase + 0x658);
}

// ---- CUser::_GetSaveInvenBase ORIG @ 0x0864fd80 ----
// SIG_SAVE_INVENTORY 权威尺寸约 0xada0（写入 +0xad95 实证）；本仓库
// SigTypes.h 中该结构仍为 0x100 占位，故全部按原始偏移裸写。
void CUser::_GetSaveInvenBase(SIG_SAVE_INVENTORY* out)
{
    out->m_accId = get_acc_id();
    out->m_characNo = getCurCharacNo();
    out->m_money = getCurCharacMoney();
    out->m_coin = getCurCharacCoin();
    out->m_payCoin = getCurCharacPayCoin();
    out->m_eventCoin = getCurCharacEventCoin();
    if (getCurCharacR() == 0)
    {
        out->m_avatarCoin = 0;
    }
    else
    {
        out->m_avatarCoin = coinOf(const_cast<char*>(
            reinterpret_cast<const char*>(getCurCharacInvenR())))->Get();
    }
    setCurCharacLastPlayTick(
        static_cast<long>(GlobalData::s_systemTime_.getCurSec()));
}

// ---- AvatarCoin::SaveToDB ORIG @ 0x081800d6 ----
int AvatarCoin::SaveToDB(CUser* user)
{
    Stream* stream = GlobalData::s_big_stream_pool->Acquire(
        "localjapan/Arad_AvatarRoulette.cpp", 0x184);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x23);              // SIG_SAVE_INVENTORY
    guard.operator*()->operator<<((int)user->GetUID());

    SIG_SAVE_INVENTORY* sig = guard.GetInBuffer<SIG_SAVE_INVENTORY>();
    user->_GetSaveInvenBase(sig);
    sig->m_saveFlag = 1;  // ORIG local_10[8]=1（仅 AvatarCoin 保存标志）

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}

// ---- AvatarCoin::SendSyncPacket ORIG @ 0x0817ffe4 ----
// bool 返回：ORIG 调用点 0x0819f1cd `xor $0x1,%eax` 判据（Dispatchers 多处）。
bool AvatarCoin::SendSyncPacket(CUser* user)
{
    if (user == 0)
        return false;
    if (user->getCurCharacR() == 0)
        return false;

    PacketGuard packet;
    packet.put_header(1, 0x24b);
    packet.put_byte(1);   // ORIG 0x0817ffe4 头部后追加 1 字节
    // ORIG 0x818005f：inven 基址 add $0x658 → Get()（无 coinOf 调用、无 coin 槽）
    packet.put_int(reinterpret_cast<AvatarCoin*>(
                       const_cast<char*>(reinterpret_cast<const char*>(
                           user->getCurCharacInvenR())) + 0x658)->Get());
    packet.finalize(true);
    user->Send(packet);
    return true;
}

// ---- AvatarCoin::HistoryLog::AddLog ORIG @ 0x817ff9c ----
void AvatarCoin::HistoryLog::AddLog(CUser* user, unsigned int amount)
{
    if (user == 0)
        return;
    // P0：ORIG 内联 `call getCurCharacInvenRefR; add $0x658; call AvatarCoin::Get`
    // （无 coinOf 调用、无 coin 局部槽），同 GetPackage L129 内联形。
    user->LogHistory("AvatarCoin+,%d,%d", amount,
        reinterpret_cast<AvatarCoin*>(
            const_cast<char*>(reinterpret_cast<const char*>(
                user->getCurCharacInvenRefR())) + 0x658)->Get());
}

// ---- AvatarCoin::GetPackage ORIG @ 0x0818028a ----
bool AvatarCoin::GetPackage(CUser* user, int avatarUid, bool bReset)
{
    if (!bReset)
    {
        coinOf(reinterpret_cast<char*>(user->getCurCharacInvenW()))
            ->Add(static_cast<unsigned int>(avatarUid));
        if (SaveToDB(user) != 1)
        {
            LogManager::logFormat(
                1, "localjapan/Arad_AvatarRoulette.cpp",
                __PRETTY_FUNCTION__, 0x1b0,
                "SaveAvatarCoin failed");
        }
        HistoryLog::AddLog(user, static_cast<unsigned int>(avatarUid));
        SendSyncPacket(user);
    }
    return 1;
}

// ---- AvatarCoin::BuyProcess ORIG @ 0x081801d0 ----
// mangled _ZN10AvatarCoin10BuyProcessEP5CUserR10Inven_Item
// 陶罐（avatar pottery）商品购买：isAvatarPottery(item.m_addInfo) 为假直接
// 返 0；否则取 m_addInfo2（+0x07）为币值，加到背包基址 +0x658 的 AvatarCoin，
// SaveToDB 失败打 ORIG 同文日志（__PRETTY_FUNCTION__ 文本），随后
// HistoryLog::AddLog + SendSyncPacket，返 1。
bool AvatarCoin::BuyProcess(CUser* user, Inven_Item& item)
{
    // register 绑定使 -O0 下分支条件直接 test %al（ORIG 形态，同 Use()）；
    // 成功体直落、失败跳转，与 ORIG je 布局一致。
    register char pottery __asm__("al") =
        reinterpret_cast<AvatarRouletteServer*>(GlobalData::s_pAvatarRoulette)
            ->isAvatarPottery(static_cast<unsigned long>(item.m_addInfo));
    if (pottery != 0)
    {
        unsigned int amount = static_cast<unsigned int>(item.m_addInfo2);
        coinOf(reinterpret_cast<char*>(user->getCurCharacInvenW()))
            ->Add(amount);
        if (SaveToDB(user) != 1)
        {
            LogManager::logFormat(
                1, "localjapan/Arad_AvatarRoulette.cpp",
                __PRETTY_FUNCTION__,
                0x19d, "SaveAvatarCoin failed");
        }
        HistoryLog::AddLog(user,
                           static_cast<unsigned int>(item.m_addInfo2));
        SendSyncPacket(user);
        return true;
    }
    return false;
}

