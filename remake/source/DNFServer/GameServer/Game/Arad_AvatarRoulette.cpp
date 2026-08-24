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
#include <cstring>

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
int AvatarCoin::SendSyncPacket(CUser* user)
{
    if (user == 0)
        return 0;
    if (user->getCurCharacR() == 0)
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0x24b);
    int coin = coinOf(const_cast<char*>(
                  reinterpret_cast<const char*>(user->getCurCharacInvenR())))
                  ->Get();
    packet.put_int(coin);
    packet.finalize(true);
    user->Send(packet);
    return 1;
}

// ---- AvatarCoin::HistoryLog::AddLog ORIG @ 0x817ff9c ----
void AvatarCoin::HistoryLog::AddLog(CUser* user, unsigned int amount)
{
    if (user == 0)
        return;
    unsigned int cur = coinOf(
        const_cast<char*>(reinterpret_cast<const char*>(
            user->getCurCharacInvenRefR())))
        ->Get();
    user->LogHistory("AvatarCoin+,%d,%d", amount, cur);
}

// ---- AvatarCoin::GetPackage ORIG @ 0x0818028a ----
int AvatarCoin::GetPackage(CUser* user, int avatarUid, bool bReset)
{
    if (!bReset)
    {
        coinOf(reinterpret_cast<char*>(user->getCurCharacInvenW()))
            ->Add(static_cast<unsigned int>(avatarUid));
        if (SaveToDB(user) != 1)
        {
            LogManager::logFormat(
                1, "localjapan/Arad_AvatarRoulette.cpp",
                "static bool AvatarCoin::GetPackage(CUser*, int, bool)", 0x1b0,
                "SaveAvatarCoin failed");
        }
        HistoryLog::AddLog(user, static_cast<unsigned int>(avatarUid));
        SendSyncPacket(user);
    }
    return 1;
}

// ============================================================================
// AvatarRouletteServer（类布局未完整建模；isAvatarPottery 仅依赖 +0x18 处的
// 指向 unsigned long 的指针字段）。
// ============================================================================
class AvatarRouletteServer
{
public:
    bool isAvatarPottery(unsigned long itemIdx);  // ORIG 0x0817fb0a

    char m_pad[0x18];            // +0x00
    unsigned long* m_pCompare;   // +0x18（ORIG **(ulong**)(this+0x18)）
};

bool AvatarRouletteServer::isAvatarPottery(unsigned long itemIdx)
{
    return *m_pCompare == itemIdx;
}
