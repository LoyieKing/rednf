// df_game_r Game/ advancealtar::StageControl 专属 TU（从 GameStubs.cpp 拆出）。
// 布局依据 CGameManager.h（m_pad[0xc0]；ctor/dtor 由 CGameManager.cpp 提供）。
// getIndex/setIndex 依 ORIG 访问 +0 的 index；leaveUser/onTimerStageTick/reset
// 依赖 ProcStage/CharacData 完整布局（当前 CGameManager.h 仅为 pad 占位），
// 按任务约定保留为签名匹配的空体桩（符号 T），后续批次补全语义。
#include "CGameManager.h"
#include "CInventory.h"  // PacketGuard 完整定义 / CInventory::SendItemLockListInven
#include "GameWorld.h"
#include <cstring>        // std::memset / std::memcpy

namespace advancealtar
{

// ORIG 0x81348ba：(const) return *(int*)this;
int StageControl::getIndex() const
{
    return *(const int*)(const void*)&m_pad[0];
}

// ORIG 0x82a669c：*(int*)this = idx;
void StageControl::setIndex(int idx)
{
    *(int*)(void*)&m_pad[0] = idx;
}

// ORIG 0x812fc4e：leaveUser —— 用户离开 advancealtar 舞台。
// 布局：+0x04 ProcStage；+0x98 CharacData（+0x00 m_pUser）。
//   user 为空/无当前角色 → 0；
//   HistoryLog::leaveDungeon（未建模，推断跳过）→
//   GameWorld::out_from_dungeon(user) → 广播 basic_info(0,2) →
//   send_itemspace(0) → SendItemLockListInven() → setAdvanceAltarIndex(-1)
//   （CUser+0x8d012）→ StageControl::destroy（未建模，推断跳过）→ 1。
void StageControl::leaveUser()
{
    char* cd = (char*)this + 0x98;
    CUser* user = *(CUser**)cd;  // CharacData+0x00 m_pUser
    if (user == 0)
    {
        return;
    }
    if (user->getCurCharacR() == 0)
    {
        return;
    }
    // [推断] ORIG 调 HistoryLog::leaveDungeon(user, getProcStage(this))
    // （HistoryLog 未建模）与 StageControl::destroy()（CGameManager.h 未声明），
    // 暂不执行。
    G_GameWorld()->out_from_dungeon(user);
    PacketGuard guard;
    guard.put_header(0, 2);
    guard.put_byte(0);
    guard.put_short(1);
    user->make_basic_info((char*)&guard, 0);
    guard.finalize(true);
    G_GameWorld()->send_all(guard, user);
    user->send_itemspace(0);
    user->getCurCharacInvenW()->SendItemLockListInven();
    user->m_field8d012 = -1;  // ORIG CUser::setAdvanceAltarIndex(user,-1)
}

// ORIG 0x812fb98：onTimerStageTick —— ProcStage+0x04 state 为 3/4 时推进
//   CharacData::upGage（+0x98 处）与 ProcStage::procTimeLine（+0x04 处）。
//   [推断] sendTestCheckGage（upGage 尾部发包）与 procTimeLine 依赖
//   ProcSummonObjectMs 内部（未建模），暂不执行；state 检查与 upGage 真实。
void StageControl::onTimerStageTick()
{
    char* procStage = (char*)this + 0x04;
    int state = *(int*)(procStage + 0x04);
    if (state == 3 || state == 4)
    {
        // ORIG CharacData::upGage @ 0x812d0b6（真实偏移实现）
        char* cd = (char*)this + 0x98;
        CDataManager* dm = G_CDataManager();
        int gageLevel = *(int*)(cd + 0x08);
        int gage = *(int*)(cd + 0x0c);
        char* cfg = (char*)dm + 0x6364;
        int base = **(int**)(cfg + 0x18);
        int maxGage = base * *(int*)(cfg + 0x0c + (gageLevel + 4) * 4);
        if (gage < maxGage)
        {
            float inc = (float)*(int*)(cfg + 0x08) *
                        ((float)*(int*)(cfg + 0x08 + gageLevel * 4) / (float)base);
            *(int*)(cd + 0x0c) = gage + (int)(inc + 0.5f);
        }
        else
        {
            *(int*)(cd + 0x0c) = maxGage;
        }
        // [推断] ORIG 此处调 CharacData::sendTestCheckGage() 与
        // ProcStage::procTimeLine()（均依赖未建模内部），暂不执行。
    }
}

void StageControl::reset()
{
}

// TimeLine 定义（ORIG：清零/拷贝 0x1c 字节；STMapScript 等使用）。
// 权威声明在 advancealtar.h；此处为构建内唯一定义（advancealtar.cpp 未入 CMake）。
TimeLine::TimeLine()
{
    std::memset(this, 0, sizeof(*this));
}

TimeLine::~TimeLine()
{
}

TimeLine& TimeLine::operator=(const TimeLine& other)
{
    if (this != &other)
        std::memcpy(this, &other, sizeof(*this));
    return *this;
}

}  // namespace advancealtar