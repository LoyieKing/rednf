// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GmCmds_M/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// GmCmds_M.cpp — game_master::CMoveCharacCmd（id=0x01 "move"，样板实现）
// + 9 个命令类的 execute/parse 补齐（vtable key function 落地 TU）。
// 对照 docs/class_func_reports/game_master__{CPvPStartCmd,CItemDropRateCmd,
// CResetTutorialFlagCmd,CRunMacroCmd,CAchievementReset,CAchievementReport,
// CAchievementClear,CAchievementAllClear,CAchievementNoClear}/ 逐函数转写。
// b28d-45 判词汇总（本 TU 2 件）：
//   CAchievementReport::execute 0x084b0830 DIFF60：patterns 末「memset 首参 edx
//     中转」待解条目实证件 —— ORIG `mov $0x17318,%edx; mov %edx,0x8(%esp)`，
//     直写立即数不可复现（65:64，语义等价）。
//   CMoveCharacCmd::execute 0x084a8d02 DIFF125 N82/N89：帧 0x50/0x60 +
//     find_user_by_charac_name 结果具名局部槽化差。
// [G5 2026-09-10 · L12 round3 复核] 2 件全核（p167 + widthscan + 判据⑪）：
//   · **真缺口修复 1**：CMoveCharacCmd::execute —— p167 报 MISSING 4×
//     `_ZN11game_master8CCommand7GetUserEv`（OUR calls 27 vs ORIG 31）；旧码
//     `CUser* self = GetUser();` 把 5 个用点合并成 1 次调用（注释「无 self 命名槽」
//     与代码不符）⇒ 按 P48/P183 删缓存、5 个用点逐点 fresh 重发 ⇒ **calls 31=31、
//     p167 CLEAN**（残留仅 PacketGuard C1/D1 ↔ C2/D2 同址别名）。指令 161→172
//     （补回 4 次调用及实参装载），残余为槽/形族（N82）。
//   · CAchievementReport::execute：p167 MISSING=EXTRA=2（别名）；残余为
//     `memset` 首参 %edx 中转（ORIG `mov $0x17318,%edx; mov %edx,0x8(%esp)`）——
//     同 CQueryCounter 族，探针 /tmp/probe_memset.cpp 四形恒直写 ⇒ N 封顶（65:64）。
#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "CEnvironment.h"          // trim(std::string&)
#include "CTitleBook.h"            // CTitleBook::_gm_clear/sendList
#include "CAchievement.h"          // CAchievement::_gm_*/setTrigger/sendList
#include "PvP_Room.h"             // PvP_Room（GetPVPRoom 返回类型）
#include "MatchRoomList.h"         // IMatch::OnRegisterMatch
#include "LogManager.h"            // LogManager::logFormat

#include <cstdlib>                 // atoi
#include <cstdio>                  // sprintf
#include <cstring>                 // memset/strcpy/strlen

namespace game_master
{

// ORIG ctor 0x082a7ffe：基类 vptr → 自身 vptr(0x8c81ca0)；string@+0x08 默认构造。

// ORIG parse 0x084a8cce：trim(parser->args)；m_str0 = parser->args（目标角色名）。
#include "GmCmds_M/parse.inc"
{
    trim(m_parser->m_args);
    m_str0 = m_parser->m_args;
}

// ORIG execute 0x084a8d02。
#include "GmCmds_M/execute.inc"
{
    // P48：ORIG 每用点 fresh G_GameWorld()（find/move_area/send_to_area 各调）
    CUser* target =
        G_GameWorld()->find_user_by_charac_name(m_str0.c_str());
    if (target == 0)
    {
        return;
    }
    // [N2-S5 负结论] ORIG 两处早退 `je 0x084a8f62` 同目标（单一落点），但合并为
    // `target == 0 || getCurCharacR() == 0` 实测 4.4 反劣 170→171（`||` 求值序
    // 使 getCurCharacR 结果多一次布尔归一）⇒ 保持两条独立 if。
    if (target->getCurCharacR() == 0)   // ORIG CUserCharacInfo::getCurCharacR 判 0
    {
        return;
    }
    int vill = static_cast<int>(target->getCurCharacVill());
    int area = target->get_area(false);
    // [N2-S5] ORIG 槽 -0x10/-0xc 由 `movzwl %ax,%eax` 提升后直存 int 槽
    // （get_posX/Y 返回 unsigned short）；试 int 具名（原 unsigned + `& 0xffff`
    // 使 move_area 实参预取到 %edi/%esi 并多 push/pop %edi）。
    int x = target->get_posX();
    int y = target->get_posY();

    // ORIG(0x084a8dd4)：同 vill && 同 area → set_position，否则 move_area。
    // [G5 2026-09-10 P167/P183 真缺口修复] 旧码 `CUser* self = GetUser();` 把 5 个用点
    //   合并成 1 次调用；p167 实证 ORIG 调 `CCommand::GetUser()` **5 次**
    //   （MISSING 4×，OUR calls 27 vs ORIG 31）⇒ 删具名缓存，逐用点 fresh 重发
    //   （P48/P183：ORIG 在实参位直读、无 self 槽）。
    // [N2-S5] ORIG 0x084a8daf `jne 0x084a8dd4` + 0x084a8dd2 `je 0x084a8ddb`
    // ⇒ 条件为 `vill != curVill || area != curArea`（不等=真），真支落
    // fall-through 0x084a8de4 move_area，假支 0x084a8e48 set_position。
    // 旧码写相等 `&&` 使两分支布局镜像（move_area 被挤到尾部，多 1 条 jmp
    // 与 %edi 保活 push/pop）。
    if (static_cast<int>(GetUser()->getCurCharacVill()) != vill ||
        GetUser()->get_area(false) != area)
    {
        G_GameWorld()->move_area(GetUser(), vill, area,
                                 static_cast<int>(x), static_cast<int>(y),
                                 0, false, 0, 0, 0);
    }
    else
    {
        GetUser()->set_position(static_cast<unsigned short>(x),
                                static_cast<unsigned short>(y), 0);
    }

    PacketGuard packet;
    packet.put_header(0, 0x8a);
    packet.put_short(static_cast<int>(GetUser()->get_unique_id() & 0xffff));
    packet.put_short(static_cast<int>(x));
    packet.put_short(static_cast<int>(y));
    packet.put_byte(0);
    packet.finalize(true);
    if (vill != 0 || area != 0)
    {
        G_GameWorld()->send_to_area(vill, area, packet);
    }
}

    // -------------------------------------------------------------------
    // 注册链 ctor 补齐（GmRegister.cpp new 调用；ORIG W 弱符号，基类
    // CCommand ctor @GmCore.cpp）。sizeof=0x8 类无自有成员，空体即由
    // 编译器生成「调基类 ctor + 设自身 vptr」，与 ORIG 一致。
    // -------------------------------------------------------------------

    // -------------------------------------------------------------------
    // CPvPStartCmd — ORIG execute 0x084a8f6a。
    // 取 PVPRoom，空则记日志；否则对 +0x6e4 登记的 IMatch 匹配器
    // 虚调用 vtable+0x30 = OnRegisterMatch(user, pRoom)。
    // -------------------------------------------------------------------
    // PvP_Room +0x6e4 视图（PvP_Room.h 该槽现标 int；ORIG 实为 create_room
    // 时 CMatchingSystem::GetMatchingSystem 登记的匹配器指针）。
    struct PvpRoomMatcherSlot
    {
        char m_pad[0x6e4];
        IMatch* m_matcher;
    };

    void CPvPStartCmd::execute()
    {
        PvP_Room* pRoom = (PvP_Room*)GetUser()->GetPVPRoom();
        if (pRoom == 0)
        {
            LogManager::logFormat(1, "GameMaster.cpp",
                __PRETTY_FUNCTION__, 0x169,
                "PVPRoom pRoom is NULL");
            return;
        }
        ((PvpRoomMatcherSlot*)pRoom)->m_matcher
            ->OnRegisterMatch(GetUser(), pRoom);       // ORIG 虚表+0x30
    }

    // -------------------------------------------------------------------
    // CItemDropRateCmd — ORIG execute 0x084a9002。
    // 函数级静态 ItemDropRate 角色对象（ORIG guard 0x0943dc40/数据
    // 0x0943dc48），SetGameMasterCharacter 注入到当前 GM 用户。
    // -------------------------------------------------------------------
    void CItemDropRateCmd::execute()
    {
        static CGameMasterCharacterItemDropRate gameMasterCharacter;
        GetUser()->SetGameMasterCharacter(&gameMasterCharacter);
    }

    // -------------------------------------------------------------------
    // CResetTutorialFlagCmd — ORIG execute 0x084a94c6。
    // 重置教学标志后回读 0..0x7f 标志位拼接 "(n) " 列表，
    // put_header(0,0xc)/byte/short/byte/int(len)/str 后 Send 给用户。
    // -------------------------------------------------------------------
    void CResetTutorialFlagCmd::execute()
    {
        GetUser()->resetCharacTutorialFlag();

        std::string msg = "tutorial flag : ";
        char num[255] = { '\0' };          // ORIG 内联清零展开（0xff 字节）
        for (int i = 0; i <= 0x7f; ++i)    // ORIG cmpl $0x7f+setle（有符号）
        {
            // ORIG test %al：按单字节布尔读（头文件声明 int，此处收窄）。
            if ((unsigned char)GetUser()->getCurCharacEscaladeTutorialFlag(i))
            {
                sprintf(num, "(%d) ", i);
                msg += num;
            }
        }

        PacketGuard packet;
        packet.put_header(0, 0xc);
        packet.put_byte(0);
        packet.put_short(0);
        packet.put_byte(0);
        packet.put_int((int)msg.size());
        packet.put_str(msg.c_str(), (int)msg.size());
        packet.finalize(true);
        GetUser()->Send(packet);
    }

    // ORIG execute 0x084a9c06：纯 ret（宏回放入口由 CMacro::RudMacro 承担）。
    void CRunMacroCmd::execute()
    {
    }

    // -------------------------------------------------------------------
    // CAchievementReset — ORIG execute 0x084b076a。
    // 称号簿(扩展数据 0xe)_gm_clear + 成就(0xf)_gm_init，再双列表重发。
    // -------------------------------------------------------------------
    void CAchievementReset::execute()
    {
        ((CTitleBook*)GetUser()->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xe))->_gm_clear();
        ((CAchievement*)GetUser()->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xf))->_gm_init();
        ((CTitleBook*)GetUser()->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xe))->sendList();
        ((CAchievement*)GetUser()->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xf))->sendList();
    }

    // -------------------------------------------------------------------
    // CAchievementReport — ORIG execute 0x084b07fe。
    // 成就(0xf)_gm_report 到栈缓冲（95000 字节，先清零、限长 strcpy），
    // 经 CBoosterTest::printClient 以 achievement_report.txt 回显给 GM。
    // 【b28 判词 · 待解条目复验】唯一实差 = memset 首参物化形：ORIG 4.1
    // `lea buf,%eax` 前置 + size 经 `mov $N,%edx; mov %edx,8(%esp)` 中转；
    // 4.4 恒 `movl $N,8(%esp)` 直写后补 lea（探针 /tmp/bm3/p21 g1-g6：字面量/
    // (unsigned)/&arr[0]/移位/变量 val/sizeof 六形全直写——与范式库「待解」
    // 条目一致），语义等价，记跨版本 memset 参数物化差。
    // -------------------------------------------------------------------
    void CAchievementReport::execute()
    {
        char fileBuf[95000];
        memset(&fileBuf[0], 0, 95000);

        std::string out;
        ((CAchievement*)GetUser()->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xf))->_gm_report(out);
        if (out.length() < sizeof(fileBuf))
            strcpy(fileBuf, out.c_str());

        CBoosterTest::printClient(GetUser(), false,
                                  "achievement_report.txt", fileBuf);
    }

    // -------------------------------------------------------------------
    // CAchievementClear — ORIG parse 0x084b08f0 / execute 0x084b092a。
    // parse：trim(args)+atoi 存 m_achievementId(+0x08)；
    // execute：成就 setTrigger(id, 0xffff, 0xffff, 0xffff) 全条件达成。
    // -------------------------------------------------------------------
    void CAchievementClear::parse()
    {
        trim(m_parser->m_args);
        m_achievementId =
            (unsigned int)atoi(m_parser->m_args.c_str());
    }

    void CAchievementClear::execute()
    {
        ((CAchievement*)GetUser()->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xf))
            ->setTrigger(m_achievementId, 0xffff, 0xffff, 0xffff);
    }

    // CAchievement 扩展数据 +0x24 标志字节视图（未具名字段，参照
    // GmCmds_G AchFlagView 同类处理）。
    struct AchNoClearFlagView
    {
        char m_pad[0x24];
        unsigned char m_flag;
    };

    // ORIG execute 0x084b097c：成就(0xf) _gm_clearAll。
    void CAchievementAllClear::execute()
    {
        ((CAchievement*)GetUser()->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xf))->_gm_clearAll();
    }

    // ORIG execute 0x084b09a8：标志字节置 1（ORIG movb $1,0x24(%eax) 单指令）。
    void CAchievementNoClear::execute()
    {
        ((AchNoClearFlagView*)GetUser()->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0xf))->m_flag = 1;
    }

} // namespace game_master


// =======================================================================
// CGameMasterCharacter 族（全局命名空间，ORIG W 弱符号族）。
// 基类仅设 vptr；sendServerState 基版为纯 ret。派生版把文本以
// put_header(0,0xc)/byte/short/byte/int(strlen)/str/finalize(true)
// 打包后 user->Send（ORIG 0x084b4330，0xf6 字节逐指令转写）。
// =======================================================================
// P210 实证（b28）：本 ctor 定义前置到基类 CGameMasterCharacter() 之前，令
// 4.4 编译本函数体时基类 ctor 定义尚不可见 → 保守 outgoing 帧 sub $0x18，
// 与 ORIG 逐条全等（推翻 b23「6 形探针穷举不可达」判词——缺的正是定义顺序维度）。
#include "GmCmds_M/CGameMasterCharacterItemDropRate.inc"
{
}

#include "GmCmds_M/CGameMasterCharacter.inc"
{
}

#include "GmCmds_M/sendServerState.inc"
{
}

#include "GmCmds_M/sendServerState_1.inc"
    CUser* user, char const* msg)                            // 0x084b4330
{
    PacketGuard packet;
    packet.put_header(0, 0xc);
    packet.put_byte(0);
    packet.put_short(0);
    packet.put_byte(0);
    packet.put_int((int)strlen(msg));
    packet.put_str(msg, (int)strlen(msg));
    packet.finalize(true);
    user->Send(packet);
}