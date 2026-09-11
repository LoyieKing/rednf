// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DispLinkA2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "Dispatchers.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "CItem.h"
#include "CStackableItem.h"
#include "CDataManager.h"
#include "GameWorld.h"
#include "GlobalData.h"
#include "CParty.h"
#include "PvP_Room.h"
#include "MatchRoomList.h"        // IMatch（m_matching +0x6e4 槽的虚表接口，OnAskRematch @+0x68）
#include "CBattle_Field.h"
#include "GlobalScriptTutorial.h"
#include "CExpertJob.h"
#include "Teleport.h"
#include "ServiceRestrictManager.h"
#include "CEventTable.h"          // CGuildServerProxy.h 经此可见 CEventBase/CEventManager
#include "CGuildServerProxy.h"    // CEventManager::GetRepeatEvent / CServerProxyMgr
#include "CServerProxyMgr.h"

#include <string>
#include <vector>
#include <utility>
#include <set>

// ---- 跨 TU 声明（ORIG 真实符号、尚无 owner 定义；本 TU 只调用不定义） ----

#include "DispLinkA2/DoUserDefineCommand.inc"

#include "DispLinkA2/SendVillageAttackedGMCommand.inc"

// ORIG T 0x08545d24 _ZN16CItemMakingSkill21SetItemMakingSkillPointEiiP5CUser
// （CItemMakingSkill 类当前无权威头；引用方仅 oldGmRoutine "itemskill"。
//  GetCharacExpandData(user,10) 返回该扩展数据指针。）
class CItemMakingSkill
{
public:
    void SetItemMakingSkillPoint(int a, int b, CUser* user);
};
// ORIG T 0x08644272 _ZNK17TowerOfDespairMgr31IsTowerOfDespairDungeon_ByIndexEi：
// m_layerSet(+0x328, set<int>).find(idx) != end()。
// 已由 TowerOfDespairMgr.cpp（TOD 域 owner）按报告定义真实成员，
// 本 TU 只调用不定义。
#include "TowerOfDespairMgr.h"

// ============================================================================
// 局部辅助
// ============================================================================

#include "DispLinkA2/getAvatarEmblemTypeIdx.inc"

// MSG_EPLP_COMMAND：ORIG 无独立导出符号，仅经 _BroadCastPacket 以引用访问。
// 报告反编译只读 +0x0d/+0x0e 两个命令字节（Dispatchers.cpp 定义同布局视图；
// 前 0xd 字节为包头区【推断】，不设 static_assert）。
struct MSG_EPLP_COMMAND_A2
{
    char m_pad00[0xd];        // 包头区 +0x00..+0x0c
    unsigned char m_cmd;      // +0x0d：命令字节
    unsigned char m_subCmd;   // +0x0e：子命令字节
};

// ParamBase+0x0e 的 ushort uid 视图（ORIG movzwl 0xe(%eax) disp 直读，
// 指针具名槽往返：EnterDisjointStore/RequestDisjointItem check_error 共用）。
struct __attribute__((packed)) ParamUidView_A2
{
    char m_pad00[0xe];
    unsigned short m_uid;
};

// ParamBase+0x0e/+0x12 两个 ushort 槽位视图（EnchantByBead::check_error 专用：
// ORIG 0x081d2396/0x081d23d2 为 movzwl 0xe/0x12(%eax),%eax + movswl %ax,%ebx 两段
// —— 字段按 unsigned short 读、以 (short) 截断后拓宽为 int 实参，P200 形）。
struct __attribute__((packed)) ParamSlotView_A2
{
    char m_pad00[0xe];
    unsigned short m_slot1;   // +0x0e
    unsigned short m_pad10;   // +0x10..+0x11
    unsigned short m_slot2;   // +0x12
};

// [b28d-Main·检查点18] 删除死助手 A2_SendGmMessage（全 TU 无调用点；ORIG 无该符号）。

#include "DispLinkA2/_selectJewelTypeIndex.inc"


#include "DispLinkA2/_getJewelTypeIndex.inc"


#include "DispLinkA2/getAvatarEmblemTypeIdx_1.inc"

#include "DispLinkA2/_SendEPLPError.inc"


#include "DispLinkA2/_BroadCastPacket.inc"


#include "DispLinkA2/excludeEplpState.inc"


#include "DispLinkA2/isReTurnToVillage.inc"


#include "DispLinkA2/check_error.inc"


#include "DispLinkA2/check_error_1.inc"


#include "DispLinkA2/check_error_2.inc"


#include "DispLinkA2/check_error_3.inc"


#include "DispLinkA2/read.inc"


#include "DispLinkA2/check_error_4.inc"


#include "DispLinkA2/check_error_5.inc"


#include "DispLinkA2/check_error_6.inc"


// ============================================================================
// 11. Dispatcher_New_Gmdebug_Command::oldGmRoutine
//     报告 docs/class_func_reports/Dispatcher_New_Gmdebug_Command/oldGmRoutine.md
//     （ORIG 0x0820bf2c..0x082134a7，约 0x757b 字节；旧式 GM 调试命令解释器，
//      空格分隔命令字 → 分支分发。Dispatchers.cpp:11927 调用。）
//     [b28d-solo-DispLinkA2 判词] verdict=DIFF（超大函数 §6.2 三件套：**语义完成、
//     byte 层 N82+N-形态 封顶**）。
//     ① 调用点差集（p167_callsite.py，新鲜 /tmp/tu_DispLinkA2.o）：
//        ORIG calls=1374 / OUR calls=1374，**0 MISSING / 0 EXTRA = CLEAR** ⇒ 无语义缺口。
//     ② gap：OURS 6676 − ORIG 6625 = **+51**（本轮 6824→6676）。
//     ③ 帧/槽：ORIG `push edi/esi/ebx; sub $0xb7c` vs OURS 同三 push `sub $0xc3c`
//        （差 0xc0，本轮 0xd00→0xc3c）。深部对象偏移统一差 0xc0（base[10] 内部 0xb2c→0xbec、
//        char 缓冲 0x668→0x728、CSkillChanger 0x568→0x628）；浅部 dbgCmd/guard 差 0x68
//        （0x250→0x2b8）。逐块抽查（/tmp/a2_diff.py）差异主体为纯槽号平移
//        （`lea -0x32c(%ebp)` ↔ `lea -0x3b4(%ebp)`），属 N82（4.1 不按 lifetime 复用互斥块
//        局部 / 4.4 复用 ⇒ 全帧槽号错位）+ 跨版本 callee-saved 分配（P52/N7）与 bool 物化
//        形态（ORIG -0x12d 死存储 `mov %al,-0x12d`×4，返回路径恒 `mov $0x0,%ebx` = P171 死写）。
//        本轮实证修复（帧 −0xc4、insn −148）：`char b[0x100]; A2_ZERO(b);`（always_inline
//        循环，4.4 恒保留循环 + 每处 3 个辅助槽 -0x98/-0x90/-0x94 族）→
//        `char b[0x100] = {0};`（P214③/P312：复现 ORIG 内联 `mov $0x40,%edx; rep stos`），
//        并恢复 `push %edi`。残余 +51 条为上述形态差（无调用差集），按 §6.2 不再耗轮次。
// ============================================================================

#include "GameMasterCmd.h"              // DisPatcher_DebugCommand 前向域依赖
#include "DisPatcherCore.h"             // DisPatcher_DebugCommand / MSG_DEBUG_COMMAND
#include "CServerProxyMgr.h"            // CServerProxyMgr<T>::GetServerProxy
#include "CFairPvPScore.h"              // fair_pvp::CFairPvPScore
#include "CUserProc.h"                  // APSystem::CUserProc::GM_Set/GM_Reset
#include "CDailyScheduleManager.h"      // GM_Open*/GM_Close*
#include "SkillSlot.h"                  // CSkillChanger / SkillSlot 访问
#include "CRandomOptionItemHandleTest.h"
#include "CCirculationStatistic.h"
#include "RefPvpGrade.h"
#include "AvatarCoin.h"
#include "AvatarRechargeServer.h"       // GmReduceEndurance / GmChangeEnduranceReduceRate
#include "CMailBoxHelper.h"             // WongWork::CMailBoxHelper MultiMail
#include "StatisticsGmCmdPacket.h"
#include "Packet_CollectItemsGm.h"
#include "Packet_Game_Monitor_GM_Village_Attacked.h"
#include "Packet_Pcroom_Response.h"        // 全局 Packet_Pcroom_Response（ORIG pcroom 分支用无命名空间版）
#include "DB_UpdateInventoryCapacity.h"
#include "BreakAwayPreventSystem.h"

#include "Arad_DataManager.h"           // ARAD::Singleton / findGameScript
#include "Arad_EventPeriodDataManager.h"
#include "Arad_ServerStateManager.h"    // deleteState / insertState
#include "AradServerStateMessage.h"
#include "Arad_InternalDispatcher.h"    // make_internal_stream_jpn
#include "MoreTypes.h"                  // arad 域 Sig POD（SigSetCharacCreateTime）
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "TimerQueue.h"
#include "PacketDispatcher.h"
#include "CTimeGate.h"
#include "DataManagerInitDepends.h"   // TOD_UserState（wjfakd）
#include "CStatisticServerProxy.h"    // 完整类型（CServerProxyMgr<T> 实例化）
#include "CMonitorServerProxy.h"      // 完整类型 / SendTcpPacket
#include "DispLinkA2/StringToNumber.inc"

// ORIG _ZN4ARAD10uint32TimeEPs（T 0x081979c0，返回 long long；ARAD 域 TU 定义）
namespace ARAD
{
long long uint32Time(short* fields);
}

#include "BattleData.h"                // hellvalue（SetHellPartyValueTotal）
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

// CLocalChina_DB_LastDay_BreakAway（ORIG T 0x08128152）：CLocalChina 域类，
// 当前无权威头；最小成员声明（不产生定义，链接归 CLocalChina 域批次）。
class CLocalChina_DB_LastDay_BreakAway
{
public:
    // ORIG 0x08128152 静态（调用点只压 3 个参数，无 this；
    // 见 Dispatcher_New_Gmdebug_Command/oldGmRoutine.md +0x7414..+0x7428）。
    static void makeRequest(unsigned int accId, int day, int flag);
};

// AradServerStateMessage::AradServerStateMessage（ORIG W 0x08186652）已由
// AradServerStateMessage.h 声明（定义归 ARAD 域 TU）。


// [b28d-solo-DispLinkA2] GM 消息缓冲（ORIG local_66c，栈上 0x100 字节）：
// ORIG 0x0820d0e3 段为内联 `mov $0x40,%edx; rep stos`（P214③）⇒ 源形必须是
// `char b[0x100] = {0};`。原 always_inline A2_ZERO 循环在 4.4 恒保留循环 + 每处
// 3 个辅助槽（-0x98/-0x90/-0x94 族），帧 0xd00 vs ORIG 0xb7c 的主因之一。
// GM 文本消息包（ORIG show lp / max lp / ulti max / dimen max 内联展开序列）：
// 局部 PacketGuard + put_header(0,0xc)/put_byte(0)/put_short(0)/put_byte(0) +
// put_int(str.size())/put_str(str.c_str(), str.size())/finalize(true)/Send。
// 用宏保证逐调用点内联展开（ORIG 无共享 helper 函数），len 走 std::string::size()。
#define A2_SEND_STR(puser, sobj)                                              \
    do {                                                                      \
        PacketGuard _g;                                                       \
        _g.put_header(0, 0xc);                                                \
        _g.put_byte(0);                                                       \
        _g.put_short(0);                                                      \
        _g.put_byte(0);                                                       \
        _g.put_int((sobj).size());                                           \
        _g.put_str((sobj).c_str(), (sobj).size());                           \
        _g.finalize(true);                                                    \
        (puser)->Send(_g);                                                    \
    } while (0)

#include "DispLinkA2/oldGmRoutine.inc"

