// ============================================================================
// df_game_r 还原 —— 匹配备份域批次 B（LinkClosureB.MatchRooms 独占 TU）
// 承载符号（ORIG /home/wangyilei/下载/df_game_r nm/objdump 实证）：
//   1. CLeagueMatchRoomList::CLeagueMatchRoomList()
//        W _ZN20CLeagueMatchRoomListC1Ev @0x08568fb0
//        （vtable 写入 0x8ca3bb0，基类 IRoomList ctor 0x8568fa2）
//   2. CMultiMatchRoomList::CMultiMatchRoomList()
//        W _ZN19CMultiMatchRoomListC1Ev @0x08566efc
//        （vtable 写入 0x8ca3bc8；+0x04 起 std::map<int,CWaitingRoomList>，
//          按 GetPlayCountPerOneTeam()&0xff 预建等待列表）
//   3. fair_pvp::CFairMatch::GetPlayCountPerOneTeam()
//        T _ZN8fair_pvp10CFairMatch22GetPlayCountPerOneTeamEv @0x08562834
//        （ORIG 体：mov $0x1,%eax; leave; ret —— 恒返 1）
//   4. online_preliminary::COnlinePreliminary::_ReturnTeam(PvP_Room*,int,CUser*)
//        T _ZN18online_preliminary18COnlinePreliminary11_ReturnTeamEP8PvP_RoomiP5CUser
//        @0x08561e40（objdump 逐指令转写，尾部 this+0x4=0）
// 报告依据：docs/class_func_reports/{CLeagueMatchRoomList,CMultiMatchRoomList,
//   fair_pvp__CFairMatch/_ReturnTeam.md,online_preliminary__COnlinePreliminary}。
// ============================================================================
#include <utility>

#include "MatchRoomList.h"      // CLeagueMatchRoomList / CMultiMatchRoomList / IRoomList / IMatch
#include "CFairMatch.h"         // fair_pvp::CFairMatch::GetPlayCountPerOneTeam 声明
#include "OnlinePreliminary.h"  // online_preliminary::COnlinePreliminary 声明（_ReturnTeam 等）
#include "CUser.h"
#include "PvP_Room.h"           // get_user_seat/get_team/OnSetPVPSeatState

// ---------------------------------------------------------------------------
// CLeagueMatchRoomList / CMultiMatchRoomList 构造函数（ORIG 0x08568fb0 /
// 0x08566efc）连同 AddRoom/DelRoom/Match key function（ORIG vtable
// 0x8ca3ba8 / 0x8ca3bc0）由 MatchRoomList.cpp 唯一定义；此处不重复，
// 避免同符号多重定义。
// ---------------------------------------------------------------------------

namespace fair_pvp
{

// ---------------------------------------------------------------------------
// 3. CFairMatch::GetPlayCountPerOneTeam —— ORIG 0x08562834 恒返 1。
//    [b28 判词·N146 式切片伪差·Main 裁定] nm size 0x15 把尾 nop×10+dead ret 计入
//    切片（range [0x08562834,0x08562849)），函数体 5 条 push/mov/mov$1/pop/ret 与
//    本实现逐字全等；strip_nop_padding 不吞 body 内 dead ret → 判 DIFF(1)。
//    与 b25c NL21 同判例族；源码侧无可动，引 ORIG 地址为证。
// ---------------------------------------------------------------------------
int CFairMatch::GetPlayCountPerOneTeam()
{
    return 1;
}

}  // namespace fair_pvp

namespace online_preliminary
{

// ---------------------------------------------------------------------------
// 4. _ReturnTeam —— ORIG 0x08561e40 逐指令转写。
//    ORIG 帧：-0x18 = seat；-0x14 = newRoom；-0x10 = seatIdx；-0xc = team。
// ---------------------------------------------------------------------------
void COnlinePreliminary::_ReturnTeam(PvP_Room* room, int team, CUser* user)
{
    PvP_Room* newRoom = 0;   // -0x14
    for (int seatIdx = 0; seatIdx <= 7; ++seatIdx) {          // setbe 于 cmpl $0x7
        // get_user_seat(seatIdx)；0 = 空座位
        if (room->get_user_seat(seatIdx) == 0)
            continue;                                          // nop → 循环尾

        // get_team(seatIdx)
        const int curTeam = room->get_team(seatIdx);           // -0xc
        const int userSeat = room->get_user_seat(seatIdx);     // -0x18

        if ((unsigned int)team == (unsigned int)curTeam && team == 2)
            room->OnSetPVPSeatState(user, seatIdx,
                                    (ENUM_SEAT_STATE)1, (ENUM_WALKOUT_TYPE)2);
        else {
            // 排除席（ORIG 0x8561ed1..：cmpl $0,user; je 续; cmp -0x18,user;
            // je skip）。PvP_Room::get_user_seat(int) 的槽位实际存放 CUser*
            // 而 ABI 按 int 返回，故此处与 user 指针值直接比较 —— ORIG 忠实。
            if (user != 0 &&
                userSeat == static_cast<int>(reinterpret_cast<size_t>(user)))
                continue;

            _PullOutRoom(user, room, seatIdx);
            if (newRoom == 0)
                newRoom = _MakeRoom(user);                     // ORIG 返回 PvP_Room*
            else
                _PutInRoom(user, newRoom, true);               // 观战者入新房间
        }
        // 各分支落到循环尾 nop → continue
    }
    m_field04 = 0;
}

}  // namespace online_preliminary

