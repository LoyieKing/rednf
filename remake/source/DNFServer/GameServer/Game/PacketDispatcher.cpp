// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 PacketDispatcher/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "PacketDispatcher.h"
#include "Dispatchers.h"           // 具体业务 Dispatcher_* 完整类型
#include "GlobalData.h"            // s_msgq_mgr / s_antibotChecker_ / s_systemTime_
#include "DisPatcherCore.h"         // 本批补齐的 79 个 DisPatcher_*/ARAD 等声明
#include "MsgQueueMgr.h"
#include "CAppLoadChecker.h"       // 叶子批次（ORIG CAppLoadChecker 簇）
#include "PacketTracer.h"          // 叶子批次（CPacketTracerInstance/AddTcpLog）
#include "PacketSecurity.h"        // 叶子批次（DenyPacket/_NS_PI_Check_CMD/
                                   //  _NS_PI_2ND_MakeChecksumTo1Byte2）
#include "UserPacketCounter.h"     // message_counter::CUserPacketCounter
#include "Encryption.h"            // Encryption::crc32N
#include "CSystemTime.h"           // getCurTickCount/update
#include "GameWorld.h"             // G_GameWorld()->GetChannelType 等
#include "CGameManager.h"          // G_CGameManager()->getUser
#include "LogManager.h"
#include "cHistoryTrace.h"         // cMyTrace
#include <string.h>
#include "CEnvironment.h"    // GlobalInstance<T>

char* NumberToString(unsigned int value, int radix);  // ORIG _Z14NumberToStringji

// ======== CLocalChinaErrorDispatcher ========

// ORIG 0x8128c34：map 构造后按 ORIG 顺序插入 60 组命令检查开关。
#include "PacketDispatcher/CLocalChinaErrorDispatcher.inc"
{
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x32,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x1b,
                    _PacketErrorCheck(false, false, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x66,
                    _PacketErrorCheck(false, false, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x6,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x9,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x20,
                    _PacketErrorCheck(true, false, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x18,
                    _PacketErrorCheck(true, true, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x1a,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x1c,
                    _PacketErrorCheck(true, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x47,
                    _PacketErrorCheck(false, false, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x4c,
                    _PacketErrorCheck(false, false, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x53,
                    _PacketErrorCheck(true, true, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x54,
                    _PacketErrorCheck(false, false, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x55,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x58,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x61,
                    _PacketErrorCheck(false, false, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x69,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x89,
                    _PacketErrorCheck(false, false, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xac,
                    _PacketErrorCheck(true, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x9c,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xd3,
                    _PacketErrorCheck(true, false, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xba,
                    _PacketErrorCheck(false, true, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x14f,
                    _PacketErrorCheck(false, true, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xbb,
                    _PacketErrorCheck(false, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xbc,
                    _PacketErrorCheck(false, true, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xe1,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xef,
                    _PacketErrorCheck(true, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xf2,
                    _PacketErrorCheck(true, false, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x100,
                    _PacketErrorCheck(true, false, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x129,
                    _PacketErrorCheck(true, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x13c,
                    _PacketErrorCheck(false, true, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x15,
                    _PacketErrorCheck(true, false, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x1f,
                    _PacketErrorCheck(false, false, true, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x36,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x30,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x10,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x26,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x5d,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x65,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x61,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x9f,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xa0,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x18,
                    _PacketErrorCheck(false, false, false, true)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x2e,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x21,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x22,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x23,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x24,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x19,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x1d,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xad,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xee,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xf5,
                    _PacketErrorCheck(true, false, false, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x130,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x146,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x223,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x224,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x225,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0xcd,
                    _PacketErrorCheck(false, true, true, false)));
    m_checks.insert(std::make_pair((ENUM_CMDPACKET)0x104,
                    _PacketErrorCheck(false, true, true, false)));
}

#include "PacketDispatcher/_PacketErrorCheck.inc"
                                                                 bool safeModeAfterLogin, bool lockSeriaRoom)
{
    m_antiAddiction = antiAddiction;
    m_safeMode = safeMode;
    m_safeModeAfterLogin = safeModeAfterLogin;
    m_lockSeriaRoom = lockSeriaRoom;
}

// ORIG 0x8595094 W：仅析构 map。
#include "PacketDispatcher/dtor_CLocalChinaErrorDispatcher.inc"

// ORIG 四个 Proc* 为纯 ret（0x812b0c8/0d2/0dc/0e6），忠实实现恒返 0。
#include "PacketDispatcher/ProcAntiAddiction.inc"
#include "PacketDispatcher/ProcSafeModeUser.inc"
#include "PacketDispatcher/ProcSaefModeAfterLogin.inc"
#include "PacketDispatcher/ProcLockInSeriaRoom.inc"

// ORIG 0x812af6a 逐指令转写：find 命中后按 b0..b3 执行，非 0 结果短路返回。
#include "PacketDispatcher/doErrorDispatcher.inc"
{
    if (user == 0) return 0;
    // ORIG 0x0812af87：null 岛后 ushort→int 物化具名槽（-0x18），find key 与四次
    // Proc 实参均自该槽 lea/reload（枚举 lvalue 直传免临时槽，ORIG `lea -0x18` 实证）。
    const ENUM_CMDPACKET cmdVal = (ENUM_CMDPACKET)cmd;
    std::map<ENUM_CMDPACKET, _PacketErrorCheck>::const_iterator it =
        m_checks.find(cmdVal);
    if (it == m_checks.end()) return 0;
    int result = 0;
    const _PacketErrorCheck& chk = it->second;
    if (chk.m_antiAddiction)
        { result = ProcAntiAddiction(*user, cmdVal); if (result != 0) return result; }
    if (chk.m_safeMode)
        { result = ProcSafeModeUser(*user, cmdVal); if (result != 0) return result; }
    if (chk.m_safeModeAfterLogin)
        { result = ProcSaefModeAfterLogin(*user, cmdVal); if (result != 0) return result; }
    if (chk.m_lockSeriaRoom)
        { result = ProcLockInSeriaRoom(*user, cmdVal); if (result != 0) return result; }
    return 0;
}

// ======== PacketDispatcher ========


message_counter::CUserPacketCounter* PacketDispatcher::pUserPacketCounter_ = 0;
    // ORIG B 0x9472d18：静态指针；ORIG 由 DispatchThread::dispatch(0x847e2d2) 每轮
    // 以栈上局部 CUserPacketCounter(get_file_name(), "UserPacketCounter") 地址赋值（0x847e34f 实证）。

#include "PacketDispatcher/PacketDispatcher.inc"
{
    for (int i = 0; i <= 0x25d; ++i)
        m_dispatchers[i] = 0;
    m_dispatchers[507] = new Dispatcher_SaveQuestNotify();  // slot +0x7ec
    m_dispatchers[503] = new Dispatcher_UseRightOfChangeGrowType();  // slot +0x7dc
    m_dispatchers[516] = new Dispatcher_StackableActionUse();  // slot +0x810
    m_dispatchers[514] = new Dispatcher_BingoReward();  // slot +0x808
    m_dispatchers[515] = new Dispatcher_BingoQuiz();  // slot +0x80c
    m_dispatchers[492] = new Dispatcher_SkillInit();  // slot +0x7b0
    m_dispatchers[490] = new Dispatcher_LogIngameAdvertisement();  // slot +0x7a8
    m_dispatchers[489] = new Dispatcher_RequestIngameAdvertisement();  // slot +0x7a4
    m_dispatchers[485] = new Dispatcher_RequestCharacSkillInfo();  // slot +0x794
    m_dispatchers[488] = new Dispatcher_SelectStriker();  // slot +0x7a0
    m_dispatchers[460] = new Dispatcher_AvatarOptionChange();  // slot +0x730
    m_dispatchers[436] = new Dispatcher_PartyTeleportConfirm();  // slot +0x6d0
    m_dispatchers[435] = new Dispatcher_PartyTeleport();  // slot +0x6cc
    m_dispatchers[429] = new Dispatcher_FatigueAttendance();  // slot +0x6b4
    m_dispatchers[427] = new Dispatcher_RefundSkill();  // slot +0x6ac
    m_dispatchers[416] = new Dispatcher_ExpertExtraction();  // slot +0x680
    // ORIG 0x8590a2e ctor 第 17 个注册：MonstercardBind(new(8), C1 0x8595348 可抛带 cleanup)
    m_dispatchers[414] = new Dispatcher_MonstercardBind();  // slot +0x678
    m_dispatchers[0] = new DisPatcher_CheckConn();  // slot +0x000
    m_dispatchers[1] = new DisPatcher_Login();  // slot +0x004
    m_dispatchers[2] = new DisPatcher_SetIP();  // slot +0x008
    m_dispatchers[3] = new DisPatcher_Exit();  // slot +0x00c
    m_dispatchers[4] = new DisPatcher_SelectCharac();  // slot +0x010
    m_dispatchers[5] = new DisPatcher_CreateCharac();  // slot +0x014
    m_dispatchers[6] = new DisPatcher_DeleteCharac();  // slot +0x018
    m_dispatchers[7] = new DisPatcher_ReturnToSelectCharacter();  // slot +0x01c
    m_dispatchers[8] = new DisPatcher_GetUserInfo();  // slot +0x020
    m_dispatchers[9] = new DisPatcher_RecoverStamina();  // slot +0x024
    m_dispatchers[10] = new DisPatcher_ReqPeer();  // slot +0x028
    m_dispatchers[11] = new DisPatcher_ResPeer();  // slot +0x02c
    m_dispatchers[12] = new DisPatcher_SetPartyInfo();  // slot +0x030
    m_dispatchers[13] = new DisPatcher_LeaveParty();  // slot +0x034
    m_dispatchers[14] = new DisPatcher_WalkoutParty();  // slot +0x038
    m_dispatchers[15] = new DisPatcher_StartGame();  // slot +0x03c
    m_dispatchers[16] = new DisPatcher_SelectDungeon();  // slot +0x040
    m_dispatchers[17] = new DisPatcher_SendMess();  // slot +0x044
    m_dispatchers[18] = new DisPatcher_DeleteItem();  // slot +0x048
    m_dispatchers[19] = new DisPatcher_MoveItem();  // slot +0x04c
    m_dispatchers[20] = new DisPatcher_SortItem();  // slot +0x050
    m_dispatchers[21] = new DisPatcher_BuyItem();  // slot +0x054
    m_dispatchers[22] = new DisPatcher_RentAvatar();  // slot +0x058
    m_dispatchers[23] = new DisPatcher_ExtendAvatar();  // slot +0x05c
    m_dispatchers[24] = new DisPatcher_SellItem();  // slot +0x060
    m_dispatchers[25] = new DisPatcher_RepairEquip();  // slot +0x064
    m_dispatchers[26] = new DisPatcher_SetTradeState();  // slot +0x068
    m_dispatchers[27] = new DisPatcher_Compound();  // slot +0x06c
    m_dispatchers[28] = new DisPatcher_DisJointItem();  // slot +0x070
    m_dispatchers[29] = new DisPatcher_Lottery();  // slot +0x074
    m_dispatchers[30] = new DisPatcher_ChangeSkill();  // slot +0x078
    m_dispatchers[31] = new DisPatcher_BuySkill();  // slot +0x07c
    m_dispatchers[32] = new DisPatcher_IncreStatus();  // slot +0x080
    m_dispatchers[33] = new DisPatcher_QuestAction();  // slot +0x084
    m_dispatchers[34] = new DisPatcher_QuestAction();  // slot +0x088
    m_dispatchers[35] = new DisPatcher_QuestAction();  // slot +0x08c
    m_dispatchers[36] = new DisPatcher_QuestAction();  // slot +0x090
    m_dispatchers[37] = new DisPatcher_SetPos();  // slot +0x094
    m_dispatchers[38] = new DisPatcher_SetArea();  // slot +0x098
    m_dispatchers[39] = new DisPatcher_Fish();  // slot +0x09c
    m_dispatchers[40] = new DisPatcher_FinishLoad();  // slot +0x0a0
    m_dispatchers[41] = new DisPatcher_UseSkill();  // slot +0x0a4
    m_dispatchers[42] = new DisPatcher_DieMob();  // slot +0x0a8
    m_dispatchers[43] = new DisPatcher_DieCharacInDungeon();  // slot +0x0ac
    m_dispatchers[44] = new DisPatcher_UseCoin();  // slot +0x0b0
    m_dispatchers[45] = new DisPatcher_GiveUpGame();  // slot +0x0b4
    m_dispatchers[46] = new DisPatcher_GetItem();  // slot +0x0b8
    m_dispatchers[47] = new DisPatcher_UseStack();  // slot +0x0bc
    m_dispatchers[48] = new DisPatcher_MoveMap();  // slot +0x0c0
    m_dispatchers[49] = new DisPatcher_SetPlayResult();  // slot +0x0c4
    m_dispatchers[50] = new DisPatcher_DropItem();  // slot +0x0c8
    m_dispatchers[51] = new DisPatcher_UseEquip();  // slot +0x0cc
    m_dispatchers[52] = new Dispatcher_ReportBadP2PUser();  // slot +0x0d0
    m_dispatchers[53] = new DisPatcher_MakePVP();  // slot +0x0d4
    m_dispatchers[54] = new DisPatcher_EnterPVP();  // slot +0x0d8
    m_dispatchers[55] = new DisPatcher_SetPVPSeat();  // slot +0x0dc
    m_dispatchers[56] = new DisPatcher_SetPVPReady();  // slot +0x0e0
    m_dispatchers[57] = new DisPatcher_SetPVPTeam();  // slot +0x0e4
    m_dispatchers[58] = new DisPatcher_DiePvP();  // slot +0x0e8
    m_dispatchers[59] = new DisPatcher_PvPTimeout();  // slot +0x0ec
    m_dispatchers[60] = new DisPatcher_EndPvPResult();  // slot +0x0f0
    m_dispatchers[61] = new DisPatcher_ResPvpRank();  // slot +0x0f4
    m_dispatchers[62] = new DisPatcher_SetPVPMapIndex();  // slot +0x0f8
    m_dispatchers[65] = new DisPatcher_DebugCommand();  // slot +0x104
    m_dispatchers[63] = new DisPatcher_AddFriend();  // slot +0x0fc
    m_dispatchers[64] = new DisPatcher_RemoveFriend();  // slot +0x100
    m_dispatchers[66] = new DisPatcher_Cera();  // slot +0x108
    m_dispatchers[67] = new Dispatcher_BuyCeraShopItem();  // slot +0x10c
    m_dispatchers[68] = new Dispatcher_GenCeraTicket();  // slot +0x110
    m_dispatchers[69] = new Dispatcher_Request_PvpExpOfWeek();  // slot +0x114
    m_dispatchers[75] = new Dispatcher_EPLPCommand();  // slot +0x12c
    m_dispatchers[72] = new Dispatcher_Score_Scroll_State();  // slot +0x120
    m_dispatchers[73] = new Dispatcher_Card_Select_Right_State();  // slot +0x124
    m_dispatchers[74] = new Dispatcher_Select_Card();  // slot +0x128
    m_dispatchers[70] = new Dispatcher_CallGuildMembers();  // slot +0x118
    m_dispatchers[71] = new Dispatcher_CallGuildCreateRight();  // slot +0x11c
    m_dispatchers[76] = new Dispatcher_CallGuildLevelUp();  // slot +0x130
    m_dispatchers[77] = new Dispatcher_CallGuildInfo();  // slot +0x134
    m_dispatchers[79] = new Dispatcher_RequestMemberEnter();  // slot +0x13c
    m_dispatchers[80] = new Dispatcher_MemberEnterReply();  // slot +0x140
    m_dispatchers[81] = new Dispatcher_MemberSecede();  // slot +0x144
    m_dispatchers[82] = new Dispatcher_CallMemberList();  // slot +0x148
    m_dispatchers[83] = new Dispatcher_UpgradeItem();  // slot +0x14c
    m_dispatchers[439] = new Dispatcher_UpgradeItemSeparate();  // slot +0x6dc
    m_dispatchers[84] = new Dispatcher_ModItemAttr();  // slot +0x150
    m_dispatchers[93] = new DisPatcher_EnterWarRoom();  // slot +0x174
    m_dispatchers[94] = new DisPatcher_SetWarRoomSeatState();  // slot +0x178
    m_dispatchers[95] = new DisPatcher_DieWarRoomCharacter();  // slot +0x17c
    m_dispatchers[96] = new DisPatcher_StartWarRoomTemp();  // slot +0x180
    m_dispatchers[85] = new Dispatcher_BuyPrivateStoreItem();  // slot +0x154
    m_dispatchers[86] = new Dispatcher_EnterPrivateStore();  // slot +0x158
    m_dispatchers[87] = new Dispatcher_ExitPrivateStore();  // slot +0x15c
    m_dispatchers[88] = new Dispatcher_CreatePrivateStore();  // slot +0x160
    m_dispatchers[89] = new Dispatcher_RemovePrivateStore();  // slot +0x164
    m_dispatchers[90] = new Dispatcher_CompleteDisplay();  // slot +0x168
    m_dispatchers[91] = new Dispatcher_MoveToGate();  // slot +0x16c
    m_dispatchers[100] = new Dispatcher_PeerConnectResult();  // slot +0x190
    m_dispatchers[101] = new Dispatcher_QuickJoinRoom();  // slot +0x194
    m_dispatchers[99] = new Dispatcher_MailBox_Open();  // slot +0x18c
    m_dispatchers[97] = new Dispatcher_MailBox_Send();  // slot +0x184
    m_dispatchers[98] = new Dispatcher_MailBox_Extract();  // slot +0x188
    m_dispatchers[102] = new Dispatcher_Compound_Avatar();  // slot +0x198
    m_dispatchers[103] = new Dispatcher_RenameCreature();  // slot +0x19c
    m_dispatchers[104] = new Dispatcher_ResponseCreature();  // slot +0x1a0
    m_dispatchers[105] = new Dispatcher_HatchCreature();  // slot +0x1a4
    m_dispatchers[108] = new Dispatcher_GatheringPartyStatus();  // slot +0x1b0
    m_dispatchers[110] = new Dispatcher_GM_Command();  // slot +0x1b8
    m_dispatchers[111] = new DisPatcher_ReportClient4Hack();  // slot +0x1bc
    m_dispatchers[112] = new DisPatcher_RequestGuildWarInfo();  // slot +0x1c0
    m_dispatchers[113] = new DisPatcher_PvpHeartBeat();  // slot +0x1c4
    m_dispatchers[114] = new Dispatcher_CodeCheckSum();  // slot +0x1c8
    m_dispatchers[115] = new Dispatcher_PVPRequestFight();  // slot +0x1cc
    m_dispatchers[117] = new Dispatcher_CreatureSendMessage();  // slot +0x1d4
    m_dispatchers[118] = new Dispatcher_TraceError();  // slot +0x1d8
    m_dispatchers[116] = new Dispatcher_MouseRegister();  // slot +0x1d0
    m_dispatchers[120] = new Dispatcher_BossDieCheck();  // slot +0x1e0
    m_dispatchers[121] = new Dispatcher_RegisterToBlackList();  // slot +0x1e4
    m_dispatchers[122] = new Dispatcher_DeleteToBlackList();  // slot +0x1e8
    m_dispatchers[123] = new Dispatcher_RequestBlackList();  // slot +0x1ec
    m_dispatchers[124] = new Dispatcher_ChangeHost();  // slot +0x1f0
    m_dispatchers[125] = new Dispatcher_CreatureScriptMessage();  // slot +0x1f4
    m_dispatchers[126] = new Dispatcher_CharacterStatistic();  // slot +0x1f8
    m_dispatchers[129] = new Dispatcher_SetSubGuildMaster();  // slot +0x204
    m_dispatchers[135] = new Dispatcher_Back2Village();  // slot +0x21c
    m_dispatchers[106] = new Dispatcher_BuyAutomatItem();  // slot +0x1a8
    m_dispatchers[137] = new Dispatcher_ChangeLetterStat();  // slot +0x224
    m_dispatchers[138] = new Dispatcher_ChangeCharacName();  // slot +0x228
    m_dispatchers[139] = new Dispatcher_QueryCharacInfo();  // slot +0x22c
    m_dispatchers[140] = new Dispatcher_ReportMannerlessUser();  // slot +0x230
    m_dispatchers[141] = new Dispatcher_DieMobAll();  // slot +0x234
    m_dispatchers[143] = new Dispatcher_CallGuildAllMembers();  // slot +0x23c
    m_dispatchers[145] = new Dispatcher_HumanCertify();  // slot +0x244
    m_dispatchers[146] = new Dispatcher_ChangeTutorialFlag();  // slot +0x248
    m_dispatchers[148] = new Dispatcher_CompleteLoadAssault();  // slot +0x250
    m_dispatchers[149] = new Dispatcher_ConnectP2PAssault();  // slot +0x254
    m_dispatchers[150] = new Dispatcher_DieAssaultPlayer();  // slot +0x258
    m_dispatchers[152] = new Dispatcher_ChangeHp();  // slot +0x260
    m_dispatchers[248] = new Dispatcher_CompleteLoadAfterAssault();  // slot +0x3e0
    m_dispatchers[249] = new Dispatcher_ConnectP2PAfterAssault();  // slot +0x3e4
    m_dispatchers[153] = new Dispatcher_BVHackInfo();  // slot +0x264
    m_dispatchers[154] = new Dispatcher_CallGuildInvite();  // slot +0x268
    m_dispatchers[155] = new Dispatcher_ReplyGuildInvite();  // slot +0x26c
    m_dispatchers[156] = new Dispatcher_RequestGuildSecede();  // slot +0x270
    m_dispatchers[157] = new Dispatcher_NotifyMessageToGuild();  // slot +0x274
    m_dispatchers[158] = new Dispatcher_GuildMasterDelegate();  // slot +0x278
    m_dispatchers[159] = new Dispatcher_CheckGuildNameDouble();  // slot +0x27c
    m_dispatchers[160] = new Dispatcher_CheckGuildAddressDouble();  // slot +0x280
    m_dispatchers[161] = new Dispatcher_OpenGuildCreateWindow();  // slot +0x284
    m_dispatchers[162] = new Dispatcher_DeathTowerStageCommand();  // slot +0x288
    m_dispatchers[163] = new Dispatcher_UseBoosterItem();  // slot +0x28c
    m_dispatchers[164] = new Dispatcher_SecurityCardIssue();  // slot +0x290
    m_dispatchers[165] = new Dispatcher_SecurityCardDisuse();  // slot +0x294
    m_dispatchers[166] = new Dispatcher_SecurityCardAuthReq();  // slot +0x298
    m_dispatchers[167] = new Dispatcher_SecurityCardAuthRpy();  // slot +0x29c
    m_dispatchers[168] = new Dispatcher_SecurityCardCertKey();  // slot +0x2a0
    m_dispatchers[175] = new Dispatcher_SecurityCardAuthCancel();  // slot +0x2bc
    m_dispatchers[202] = new Dispatcher_SecurityCardRetransfer();  // slot +0x328
    m_dispatchers[169] = new Dispatcher_CallPartyMemberRealtimeInfo();  // slot +0x2a4
    m_dispatchers[170] = new Dispatcher_EvadeAssault();  // slot +0x2a8
    m_dispatchers[171] = new Dispatcher_AgreeEnchant();  // slot +0x2ac
    m_dispatchers[172] = new Dispatcher_TryEnchant();  // slot +0x2b0
    m_dispatchers[173] = new Dispatcher_PutItemForEnchant();  // slot +0x2b4
    m_dispatchers[174] = new Dispatcher_Client_Spec_Statistic();  // slot +0x2b8
    m_dispatchers[605] = new Dispatcher_Antibot_DPCallBack();  // slot +0x974
    m_dispatchers[604] = new Dispatcher_Antibot_DP();  // slot +0x970
    m_dispatchers[603] = new Dispatcher_Antibot();  // slot +0x96c
    m_dispatchers[180] = new Dispatcher_Join_Power();  // slot +0x2d0
    m_dispatchers[181] = new Dispatcher_Secede_Power();  // slot +0x2d4
    m_dispatchers[179] = new Dispatcher_New_Gmdebug_Command();  // slot +0x2cc
    m_dispatchers[183] = new Dispatcher_SDC_Damage_Check();  // slot +0x2dc
    m_dispatchers[184] = new Dispatcher_SDC_ActiveStatus_Damage_Check();  // slot +0x2e0
    m_dispatchers[182] = new Dispatcher_ChangeGuildName();  // slot +0x2d8
    m_dispatchers[185] = new Dispatcher_AuctionAskAveragePrice();  // slot +0x2e4
    m_dispatchers[186] = new Dispatcher_AuctionRegistItem();  // slot +0x2e8
    m_dispatchers[187] = new Dispatcher_AuctionRegistCancel();  // slot +0x2ec
    m_dispatchers[188] = new Dispatcher_AuctionBidding();  // slot +0x2f0
    m_dispatchers[189] = new Dispatcher_AuctionSearchByItemKey();  // slot +0x2f4
    m_dispatchers[190] = new Dispatcher_AuctionSearchByNoItmeKey();  // slot +0x2f8
    m_dispatchers[191] = new Dispatcher_AuctionMyRegistedItemInfo();  // slot +0x2fc
    m_dispatchers[192] = new Dispatcher_AuctionMyBiddingInfo();  // slot +0x300
    m_dispatchers[193] = new Dispatcher_AuctionMyAuctionHistory();  // slot +0x304
    m_dispatchers[194] = new Dispatcher_DungeonEventStoryPause();  // slot +0x308
    m_dispatchers[195] = new Dispatcher_JoinPowerWar();  // slot +0x30c
    m_dispatchers[200] = new Dispatcher_SaveGameOption_1();  // slot +0x320
    m_dispatchers[201] = new Dispatcher_SaveGameOption_2();  // slot +0x324
    m_dispatchers[448] = new Dispatcher_SaveCharacterOption();  // slot +0x700
    m_dispatchers[197] = new Dispatcher_FrameLagStatistics();  // slot +0x314
    m_dispatchers[196] = new Dispatcher_GoblinPadStatus();  // slot +0x310
    m_dispatchers[198] = new Dispatcher_PvPChannelInfo();  // slot +0x318
    m_dispatchers[199] = new Dispatcher_RequestMatch();  // slot +0x31c
    m_dispatchers[204] = new Dispatcher_UseJewel();  // slot +0x330
    m_dispatchers[205] = new Dispatcher_DisJointAvatar();  // slot +0x334
    m_dispatchers[207] = new Dispatcher_PurifyItem();  // slot +0x33c
    m_dispatchers[208] = new Dispatcher_InvestItemAmplifyOption();  // slot +0x340
    m_dispatchers[209] = new Dispatcher_AddSocketToAvatar();  // slot +0x344
    m_dispatchers[210] = new Dispatcher_Shop_Coin_Event();  // slot +0x348
    m_dispatchers[211] = new DisPatcher_MultiBoxLottery();  // slot +0x34c
    m_dispatchers[212] = new DisPatcher_UdpCharacteristic();  // slot +0x350
    m_dispatchers[213] = new DIspatcher_OneDayLetheTicket();  // slot +0x354
    m_dispatchers[214] = new Dispatcher_DisguiseRequest();  // slot +0x358
    m_dispatchers[215] = new Dispatcher_DisguiseCancel();  // slot +0x35c
    // ORIG 0x8590a2e：UseVendingMachine(new(8), C1 0x821c25c 可抛带 cleanup)紧随 215
    m_dispatchers[218] = new Dispatcher_UseVendingMachine();  // slot +0x368
    m_dispatchers[486] = new Dispatcher_UseCraneStart();  // slot +0x798
    m_dispatchers[487] = new Dispatcher_CranePickUp();  // slot +0x79c
    m_dispatchers[216] = new Dispatcher_RequestPCRoomPlayerList();  // slot +0x360
    m_dispatchers[217] = new Dispatcher_RequestPCRoomPlayerCount();  // slot +0x364
    m_dispatchers[221] = new Dispatcher_UpdateServerMessage();  // slot +0x374
    m_dispatchers[222] = new Dispatcher_CheckEnableServerMessage();  // slot +0x378
    m_dispatchers[219] = new Dispatcher_AssertManager();  // slot +0x36c
    m_dispatchers[225] = new Dispatcher_GiveGiftToNPC();  // slot +0x384
    m_dispatchers[220] = new Dispatcher_Overflow_Info();  // slot +0x370
    m_dispatchers[226] = new Dispatcher_GoblinPadRequestCryptKey();  // slot +0x388
    m_dispatchers[227] = new Dispatcher_WriteGuildMemberMemo();  // slot +0x38c
    m_dispatchers[235] = new Dispatcher_HellPartyStart();  // slot +0x3ac
    m_dispatchers[236] = new Dispatcher_PowerWarProcessInfo();  // slot +0x3b0
    m_dispatchers[238] = new Dispatcher_CreateDisjointStore();  // slot +0x3b8
    m_dispatchers[239] = new Dispatcher_RequestDisjointItem();  // slot +0x3bc
    m_dispatchers[240] = new Dispatcher_RepairDisjointMachine();  // slot +0x3c0
    m_dispatchers[241] = new Dispatcher_Teleport();  // slot +0x3c4
    m_dispatchers[242] = new Dispatcher_CompoundItemByExpertJob();  // slot +0x3c8
    m_dispatchers[243] = new Dispatcher_GiveupExpertJob();  // slot +0x3cc
    m_dispatchers[244] = new Dispatcher_UpgradeDisjointMachine();  // slot +0x3d0
    m_dispatchers[245] = new Dispatcher_EnterDisjointStore();  // slot +0x3d4
    m_dispatchers[246] = new Dispatcher_CloseDisjointStore();  // slot +0x3d8
    m_dispatchers[247] = new Dispatcher_ReportAbuseUser();  // slot +0x3dc
    m_dispatchers[265] = new Dispatcher_GuildCargo();  // slot +0x424
    m_dispatchers[266] = new Dispatcher_GuildCargoHistory();  // slot +0x428
    m_dispatchers[251] = new Dispatcher_GuildCargoPushItem();  // slot +0x3ec
    m_dispatchers[252] = new Dispatcher_GuildCargoPopItem();  // slot +0x3f0
    m_dispatchers[253] = new Dispatcher_GuildCargoMoveItem();  // slot +0x3f4
    m_dispatchers[254] = new Dispatcher_LoadingTimeReport();  // slot +0x3f8
    m_dispatchers[255] = new Dispatcher_UseSharedEffectItem();  // slot +0x3fc
    m_dispatchers[256] = new Dispatcher_BuyCeraShopLimitItem();  // slot +0x400
    m_dispatchers[312] = new Dispatcher_SecuDataControl();  // slot +0x4e0
    m_dispatchers[258] = new Dispatcher_ChangeEmotion();  // slot +0x408
    m_dispatchers[259] = new Dispatcher_DieBloodMonster();  // slot +0x40c
    m_dispatchers[260] = new Dispatcher_CompoundEmblem();  // slot +0x410
    m_dispatchers[261] = new Dispatcher_CheckAssaultMotionHack();  // slot +0x414
    m_dispatchers[262] = new Dispatcher_BloodRoundUiPrepareFinish();  // slot +0x418
    m_dispatchers[263] = new Dispatcher_RequestConditionEventReward();  // slot +0x41c
    m_dispatchers[264] = new Dispatcher_ChangeAnotherSkillTree();  // slot +0x420
    m_dispatchers[267] = new Dispatcher_FightVillageMonster();  // slot +0x42c
    m_dispatchers[268] = new Dispatcher_FinishVillageMonsterFighting();  // slot +0x430
    m_dispatchers[269] = new Dispatcher_UpgradeGuildCargo();  // slot +0x434
    m_dispatchers[271] = new Dispatcher_RequestItemLock();  // slot +0x43c
    m_dispatchers[272] = new Dispatcher_RequestItemUnlock();  // slot +0x440
    m_dispatchers[273] = new Dispatcher_RequestItemUnlockCancel();  // slot +0x444
    m_dispatchers[274] = new Dispatcher_UpgradeChronicle();  // slot +0x448
    m_dispatchers[275] = new Dispatcher_EnchantByBead();  // slot +0x44c
    m_dispatchers[276] = new Dispatcher_DungeonNPCBuffInfo();  // slot +0x450
    m_dispatchers[287] = new Dispatcher_VerifyGold();  // slot +0x47c
    m_dispatchers[283] = new Dispatcher_Security_ProcessScan();  // slot +0x46c
    m_dispatchers[284] = new Dispatcher_Security_IPScan();  // slot +0x470
    m_dispatchers[282] = new Dispatcher_Lag_Statistics();  // slot +0x468
    m_dispatchers[286] = new Dispatcher_Security_PacketIntegrity();  // slot +0x478
    m_dispatchers[288] = new Dispatcher_RequestOnTimeEventReward();  // slot +0x480
    m_dispatchers[289] = new Dispatcher_RequestAddPvPBuddy();  // slot +0x484
    m_dispatchers[290] = new Dispatcher_ResponseAddPvPBuddy();  // slot +0x488
    m_dispatchers[291] = new Dispatcher_RemovePvPBuddy();  // slot +0x48c
    m_dispatchers[292] = new Dispatcher_PvPBuddyConnList();  // slot +0x490
    m_dispatchers[294] = new Dispatcher_Security_NonClient_GetResponse();  // slot +0x498
    m_dispatchers[295] = new Dispatcher_Security_MemoryIntegrity();  // slot +0x49c
    m_dispatchers[361] = new Dispatcher_Compound_Equipment_Upgrade_Card();  // slot +0x5a4
    m_dispatchers[296] = new Dispatcher_Change_Charac_Slot();  // slot +0x4a0
    m_dispatchers[538] = new Dispatcher_SecurityLog();  // slot +0x868
    m_dispatchers[297] = new Dispatcher_SecretShopBuyItem();  // slot +0x4a4
    m_dispatchers[298] = new Dispatcher_SecretShopOpenClose();  // slot +0x4a8
    m_dispatchers[299] = new Dispatcher_CompleteLoadPvP();  // slot +0x4ac
    m_dispatchers[300] = new Dispatcher_ConnectP2PPvP();  // slot +0x4b0
    m_dispatchers[301] = new Dispatcher_BiddingRoutingItem();  // slot +0x4b4
    m_dispatchers[306] = new Dispatcher_CreateAccountCargo();  // slot +0x4c8
    m_dispatchers[307] = new Dispatcher_UpgradeAccountCargo();  // slot +0x4cc
    m_dispatchers[308] = new Dispatcher_DepositMoney();  // slot +0x4d0
    m_dispatchers[309] = new Dispatcher_WithdrawMoney();  // slot +0x4d4
    m_dispatchers[332] = new Dispatcher_Skill_Command_Customizing();  // slot +0x530
    m_dispatchers[333] = new Dispatcher_Skill_Command_All_Default();  // slot +0x534
    m_dispatchers[341] = new Dispatcher_Open_Guild_Board();  // slot +0x554
    m_dispatchers[342] = new Dispatcher_Write_On_The_GuildBoard();  // slot +0x558
    m_dispatchers[343] = new Dispatcher_Delete_GuildBoard_Text();  // slot +0x55c
    m_dispatchers[365] = new Dispatcher_Break_Away_Quest_Check();  // slot +0x5b4
    m_dispatchers[326] = new Dispatcher_Compound_ExtreamItem();  // slot +0x518
    m_dispatchers[310] = new Dispatcher_RedeemList();  // slot +0x4d8
    m_dispatchers[311] = new Dispatcher_Redeem();  // slot +0x4dc
    m_dispatchers[316] = new Dispatcher_MultiMailBoxSend();  // slot +0x4f0
    m_dispatchers[325] = new Dispatcher_QueryCharacInfoMailbox();  // slot +0x514
    m_dispatchers[317] = new Dispatcher_OperateRidableObject();  // slot +0x4f4
    m_dispatchers[318] = new Dispatcher_SelectUltimateDifficulty();  // slot +0x4f8
    m_dispatchers[335] = new Dispatcher_AuctionBuyItemApiece();  // slot +0x53c
    m_dispatchers[319] = new Dispatcher_Security_PI_HashModulation_S2C();  // slot +0x4fc
    m_dispatchers[320] = new Dispatcher_Security_PI_HashModulation_C2C();  // slot +0x500
    m_dispatchers[330] = new Dispatcer_polic_one_to_one_chat_disable();  // slot +0x528
    m_dispatchers[331] = new Dispatcher_police_queryCharacName_useMid();  // slot +0x52c
    m_dispatchers[334] = new Dispatcher_ValidateScriptHash();  // slot +0x538
    m_dispatchers[336] = new Dispatcher_Change_PartyMemberPosition();  // slot +0x540
    m_dispatchers[338] = new Dispatcher_ScanBotByDll();  // slot +0x548
    m_dispatchers[339] = new Dispatcher_UseLimitCube();  // slot +0x54c
    m_dispatchers[340] = new Dispatcher_RefreshGuildInfo();  // slot +0x550
    m_dispatchers[347] = new Dispatcher_DonateGuildFund();  // slot +0x56c
    m_dispatchers[348] = new Dispatcher_CheckJoinGuild();  // slot +0x570
    m_dispatchers[349] = new Dispatcher_RequestJoinGuild();  // slot +0x574
    m_dispatchers[350] = new Dispatcher_CancelJoinGuild();  // slot +0x578
    m_dispatchers[351] = new Dispatcher_ApproveJoinGuild();  // slot +0x57c
    m_dispatchers[352] = new Dispatcher_DenyJoinGuild();  // slot +0x580
    m_dispatchers[353] = new Dispatcher_GuildJoinList();  // slot +0x584
    m_dispatchers[356] = new Dispatcher_GuildAttendanceInfo();  // slot +0x590
    m_dispatchers[358] = new Dispatcher_MotionHack();  // slot +0x598
    m_dispatchers[552] = new APSystem::Dispatcher_ClearAction();  // slot +0x8a0
    m_dispatchers[553] = new APSystem::Dispatcher_GetTodayReward();  // slot +0x8a4
    m_dispatchers[366] = new Dispatcher_JoinGuildInfo();  // slot +0x5b8
    m_dispatchers[367] = new Dispatcher_ScanBotByDrv();  // slot +0x5bc
    m_dispatchers[368] = new Dispatcher_AskRematch();  // slot +0x5c0
    m_dispatchers[369] = new Dispatcher_SaveGameOption_QuickChatting();  // slot +0x5c4
    m_dispatchers[370] = new Dispatcher_Select_3rdChronicleItem_ForEnchant();  // slot +0x5c8
    m_dispatchers[371] = new Dispatcher_Enchant_3rdChronicleItem();  // slot +0x5cc
    m_dispatchers[372] = new Dispatcher_GoldTakeIncreasingAmount();  // slot +0x5d0
    m_dispatchers[374] = new DisPatcher_CheckSecurityProtection();  // slot +0x5d8
    m_dispatchers[375] = new Dispatcher_FairPvPScore();  // slot +0x5dc
    m_dispatchers[376] = new Dispatcher_PvpMissionHPPercent();  // slot +0x5e0
    m_dispatchers[411] = new Dispatcher_PvpMissionComboClear();  // slot +0x66c
    m_dispatchers[377] = new Dispatcher_WarRoomWpPerMonster();  // slot +0x5e4
    m_dispatchers[397] = new Dispatcher_Equipslot_Switch();  // slot +0x634
    m_dispatchers[398] = new Dispatcher_Expand_Equipslot_Flag_Update();  // slot +0x638
    m_dispatchers[401] = new Dispatcher_UnsealRandomOption();  // slot +0x644
    m_dispatchers[324] = new Dispatcher_UseCreatureEvolutionItem();  // slot +0x510
    m_dispatchers[539] = new DisPatcher_EventDungeon_DestoryObject();  // slot +0x86c
    m_dispatchers[540] = new DisPatcher_EventDungeon_ClearRoom();  // slot +0x870
    m_dispatchers[399] = new Dispatcher_Buy_Charac_Status_Using_QP();  // slot +0x63c
    m_dispatchers[400] = new Dispatcher_Clear_Used_QP();  // slot +0x640
    m_dispatchers[404] = new Dispatcher_ChangeHostWarRoom();  // slot +0x650
    m_dispatchers[405] = new Dispatcher_VerifyPrivateStoreItem();  // slot +0x654
    m_dispatchers[407] = new Dispather_RegenerationRandomOption();  // slot +0x65c
    m_dispatchers[406] = new Dispatcher_SelectItem();  // slot +0x658
    m_dispatchers[408] = new Dispatcher_UpgradeCargo();  // slot +0x660
    m_dispatchers[412] = new Dispatcher_TitleBookPut();  // slot +0x670
    m_dispatchers[413] = new Dispatcher_TitleBookGet();  // slot +0x674
    m_dispatchers[424] = new Dispatcher_TitleBookOtherUser();  // slot +0x6a0
    m_dispatchers[417] = new Dispatcher_AchievementTrigger();  // slot +0x684
    m_dispatchers[415] = new Dispatcher_CharacSlotExtendEffect();  // slot +0x67c
    m_dispatchers[425] = new Dispatcher_ItemHyperLinkMessage();  // slot +0x6a4
    m_dispatchers[426] = new Dispatcher_UserHistoryLog();  // slot +0x6a8
    m_dispatchers[431] = new Dispatcher_UpgradeInventory();  // slot +0x6bc
    m_dispatchers[432] = new Dispatcher_Select_Item_Grwoth_Power();  // slot +0x6c0
    m_dispatchers[433] = new Dispatcher_Request_Seria_Buff();  // slot +0x6c4
    m_dispatchers[437] = new Dispatcher_AbnormalUseStackable();  // slot +0x6d4
    m_dispatchers[438] = new Dispatcher_ChangeRandomOption();  // slot +0x6d8
    m_dispatchers[456] = new DispatcherResetRandomOption();  // slot +0x720
    m_dispatchers[440] = new Dispatcher_ItemDictionary();  // slot +0x6e0
    m_dispatchers[441] = new Dispatcher_Mercenary_Return();  // slot +0x6e4
    m_dispatchers[442] = new Dispatcher_Mercenary_Info();  // slot +0x6e8
    m_dispatchers[443] = new Dispatcher_Mercenary_Competition();  // slot +0x6ec
    m_dispatchers[444] = new Dispatcher_Register_Quick_Party();  // slot +0x6f0
    m_dispatchers[445] = new Dispatcher_Cancel_Quick_Party();  // slot +0x6f4
    m_dispatchers[446] = new Dispatcher_Direct_Entrance_Quick_Party();  // slot +0x6f8
    m_dispatchers[449] = new Dispatcher_Exchange_Random_Item_Reward();  // slot +0x704
    m_dispatchers[450] = new Dispatcher_Avatar_Disjoint_Random_Reward();  // slot +0x708
    m_dispatchers[447] = new Dispatcher_RequestAssaultPrice();  // slot +0x6fc
    m_dispatchers[451] = new Dispatcher_Check3rdPartyConcent();  // slot +0x70c
    m_dispatchers[453] = new Dispatcher_Float_RDATA_Modulation();  // slot +0x714
    m_dispatchers[454] = new Dispatcher_Req_Urgent_Quest();  // slot +0x718
    m_dispatchers[457] = new Dispatcher_Debug_Clear_Quest();  // slot +0x724
    m_dispatchers[458] = new Dispatcher_TournamentRewardSelectState();  // slot +0x728
    m_dispatchers[459] = new Dispatcher_TournamentRewardSelect();  // slot +0x72c
    m_dispatchers[465] = new Dispatcher_LimitNpcBuyItem();  // slot +0x744
    m_dispatchers[469] = new Dispatcher_ObjectBringUp();  // slot +0x754
    m_dispatchers[470] = new Dispatcher_PrecheckSoloTeleport();  // slot +0x758
    m_dispatchers[471] = new Dispatcher_SoloTeleport();  // slot +0x75c
    m_dispatchers[473] = new Dispatcher_SaveGameOption_ChattingEmoticon();  // slot +0x764
    m_dispatchers[572] = new advancealtar::Dispatcher_StartGame();  // slot +0x8f0
    m_dispatchers[573] = new advancealtar::Dispatcher_BuyItem();  // slot +0x8f4
    m_dispatchers[574] = new advancealtar::Dispatcher_SetSlot();  // slot +0x8f8
    m_dispatchers[575] = new advancealtar::Dispatcher_UpgradeGage();  // slot +0x8fc
    m_dispatchers[576] = new advancealtar::Dispatcher_SummonUnit();  // slot +0x900
    m_dispatchers[577] = new advancealtar::Dispatcher_ExchangeSlot();  // slot +0x904
    m_dispatchers[578] = new advancealtar::Dispatcher_Pause();  // slot +0x908
    m_dispatchers[579] = new advancealtar::Dispatcher_GetAchievementReward();  // slot +0x90c
    m_dispatchers[580] = new advancealtar::Dispatcher_ResetStar();  // slot +0x910
    m_dispatchers[585] = new ARAD::DISPATCHER::Dispatcher_AradJumpingCharacter();  // slot +0x924
    m_dispatchers[586] = new ARAD::DISPATCHER::Dispatcher_AvartarRoulette();  // slot +0x928
    m_dispatchers[587] = new ARAD::DISPATCHER::Dispatcher_AvartarCoinCount();  // slot +0x92c
    m_dispatchers[588] = new ARAD::DISPATCHER::Dispatcher_AvartarHiddenOptionChange();  // slot +0x930
    m_dispatchers[589] = new ARAD::DISPATCHER::Dispatcher_AradUseAvatarRecharge();  // slot +0x934
    m_dispatchers[591] = new ARAD::DISPATCHER::Dispatcher_AradAvatarConvert();  // slot +0x93c
    m_dispatchers[590] = new ARAD::DISPATCHER::Dispatcher_AradEmblemCompound();  // slot +0x938
    m_dispatchers[476] = new Dispatcher_ImageCommunicationEquipmentUse();  // slot +0x770
    m_dispatchers[477] = new Dispatcher_CompatibilityIndex();  // slot +0x774
    m_dispatchers[479] = new Dispatcher_P2P_Statistics();  // slot +0x77c
    m_dispatchers[478] = new Dispatcher_InformNotice();  // slot +0x778
    m_dispatchers[504] = new Dispatcher_InformNotice_2nd();  // slot +0x7e0
    m_dispatchers[480] = new Dispatcher_VerifyCreatureQuest();  // slot +0x780
    m_dispatchers[483] = new Dispatcher_CollectItems();  // slot +0x78c
    m_dispatchers[484] = new Dispatcher_TutorialLevelUp();  // slot +0x790
    m_dispatchers[493] = new Dispatcher_PcRoomPlayTimeReward();  // slot +0x7b4
    m_dispatchers[494] = new Dispatcher_PcRoomRentItem();  // slot +0x7b8
    m_dispatchers[495] = new Dispatcher_SeriaRoom_Deco_Event();  // slot +0x7bc
    m_dispatchers[496] = new Dispatcher_BlueMarble();  // slot +0x7c0
    m_dispatchers[508] = new Dispatcher_BlueMarbleConfirmInfo();  // slot +0x7f0
    m_dispatchers[499] = new Dispatcher_UseDye();  // slot +0x7cc
    m_dispatchers[505] = new Dispatcher_GrowthWeaponEventChangeInfinity();  // slot +0x7e4
    m_dispatchers[506] = new Dispatcher_GrowthWeaponEventUseMaterial();  // slot +0x7e8
    m_dispatchers[509] = new Dispatcher_ComboSkill();  // slot +0x7f4
    m_dispatchers[511] = new Dispatcher_ComboSkillExtensionQuickSlotReset();  // slot +0x7fc
    m_dispatchers[510] = new Dispatcher_UseRenameCard();  // slot +0x7f8
    m_dispatchers[519] = new Dispatcher_CeraPackageOpen();  // slot +0x81c
    m_dispatchers[512] = new Dispatcher_GrowthCreatureChangeInfinityCreature();  // slot +0x800
    m_dispatchers[513] = new Dispatcher_SeriaRoom_Ani_Deco_Event();  // slot +0x804
    m_dispatchers[526] = new Dispatcher_UpdateContractOfCubeInfo();  // slot +0x838
    m_dispatchers[597] = new Dispatcher_LevelupSupportReqestItem();  // slot +0x954
    m_dispatchers[599] = new Dispatcher_P2PHolePunchingSuccessRateStatistics();  // slot +0x95c
    m_dispatchers[600] = new ARAD::DISPATCHER::Dispatcher_EventCreateDnfRequest();  // slot +0x960
    m_dispatchers[601] = new ARAD::DISPATCHER::Dispatcher_RequestPrcoomDayilyReward();  // slot +0x964
    m_dispatchers[602] = new Dispatcher_HeroMissionEventState_Reward();  // slot +0x968
}

#include "PacketDispatcher/dtor_PacketDispatcher.inc"
{
    // [b28c-L18] N7 判词（同 TimerDispatcher::~TimerDispatcher）：虚删除
    // `call *%ecx`（ORIG vptr→%ecx/槽→%edx/基址→%eax）4.4 恒间接 call 目标=%eax，
    // 56=56 助记符全等仅寄存器轮换差，封顶。
    for (int i = 0; i <= 0x25d; ++i)
    {
        if (m_dispatchers[i] != 0) delete m_dispatchers[i];
        m_dispatchers[i] = 0;
    }
}

// ORIG 0x85948e2 逐指令转写。
#include "PacketDispatcher/get_dispatcher.inc"
{
    if (code > 0x25d) return 0;
    if (code < 0) return 0;
    // ORIG 0x0859490b `test; je +0x38(null 尾岛)`：非空体作 fall-through（技巧17
    // 块序=ORIG fall 在前），null 返 0 置尾岛。
    if (m_dispatchers[code] != 0) return m_dispatchers[code];
    return 0;
}

// ORIG 0x8231f2e W：尾调 get_dispatcher。
#include "PacketDispatcher/getDispatcher.inc"
{
    // ORIG 0x08231f31 `sub $0x18`（4.1 保守帧）：dead pad 撑帧对齐（技巧31）。
    char framePad[0x10];
    (void)framePad;
    return get_dispatcher(code);
}

// ORIG 0x825f72e：CPacketTracer 单例记录 (cls, cmd) TCP 日志。
#include "PacketDispatcher/log_recv_packet.inc"
                                       unsigned short cmd, char* data, int len)
{
    (void)user; (void)data; (void)len;
    // [b28c-L18] P52 链式 receiver+实参先物化：ORIG 0x0825f72e 先 `(ushort)cmd→%esi`、
    // `cls→%ebx`（callee-saved 跨 call 存活，push esi/ebx）再 `call
    // CPacketTracerInstance`、结果 %eax 免槽直落 (%esp)。拆「命名 tracer 局部 +
    // 成员调用」两语句使 4.4 把结果槽化（-0xc round-trip）；合并为单条链式表达式
    // 即触发 callee-saved 独占形。
    CPacketTracerInstance()->AddTcpLog((int)cls, (int)(unsigned short)cmd);
}

// ORIG 0x825f658：五队列余量 → CAppLoadChecker 检查/请求 DB。
#include "PacketDispatcher/server_load_monitoring.inc"
{
    int remainQ3 = GlobalData::s_msgq_mgr->remain((MsgQueueMgr::QUEUE_IDX)3);
    int remainQ1 = GlobalData::s_msgq_mgr->remain((MsgQueueMgr::QUEUE_IDX)1);
    int remainQ0 = GlobalData::s_msgq_mgr->remain((MsgQueueMgr::QUEUE_IDX)0);
    int remainQ2 = GlobalData::s_msgq_mgr->remain((MsgQueueMgr::QUEUE_IDX)2);
    int remainQ4 = GlobalData::s_msgq_mgr->remain((MsgQueueMgr::QUEUE_IDX)4);
    // ORIG 0x0825f6d6/0x0825f703：两次独立 CAppLoadCheckerInstance 结果免槽直落
    // (%esp)（具名 checker 槽占 -0x24 使 5 个 remain 槽整体顶深 4B，技巧36/37）。
    if (CAppLoadCheckerInstance()->CheckLoad(remainQ2, remainQ4, remainQ0, remainQ1))
        CAppLoadCheckerInstance()->RequestDB(remainQ2, remainQ4, remainQ0, remainQ1);
}

// ORIG 0x8594e52 逐指令转写。
#include "PacketDispatcher/dispatch.inc"
{
    // b28d-solo-S161 判词 [ORIG 0x8594e52]：**AE（DIFF→AE，本泳道收口）**，orig 114 /
    //   our 114 insn，p167 callsite 13/13 CLEAR（0 missing / 0 extra）。前泳道 S153 残差
    //   三项已全部消除（各附 4.4.7/4.4.6 双后端 + 探针 /tmp/pd_probe6.cpp、probe7.cpp 实证）：
    //   ①N18a「取函数指针 4.4 恒两段」：`void***` 下标形（`(*reinterpret_cast<void***>(p))[0x3c/4]`）
    //     恒产 `mov (%eax),%eax; mov %eax,%esi`；改 **单层解引用 + char* 偏移 + 纯 `register`
    //     存储类提示** `register CheckFn fn = *reinterpret_cast<CheckFn*>((char*)*(void**)p + 0x3c);`
    //     即逐条复现 ORIG 0x8594ec9 `mov (%eax),%esi` 单段直落（探针 t3/t6 对照 t8）。纯 register
    //     非 asm 绑定，P0/P231 合规。
    //   ②N7「key/checker 寄存器角色互换」：ORIG 0x8594eda `mov %eax,%edx` 中转 ⇒ CheckFn 第 2
    //     形参须 `unsigned int`（`(unsigned int)user->getAntibotKey()`），P211 档① 全链同型化。
    //   ③doDispatch 实参装载序（N7/P75）：ORIG 0x8594f65 `mov -0x14(%ebp),%eax` 把 packetClass
    //     载入排在 cmd 压栈**之后**；`int packetClass` 形恒提前载入并挤掉 cmd 的 %eax。把局部声明
    //     改 `ENUM_PACKETCLASS packetClass`（实参无转换）即复现 ORIG 序。
    //   本轮按 ORIG 逐条重排源形：①packetClass 由 `(signed char)data[0]` 初始化
    //   （复现 0x8594e5d movzbl+movsbl 两段 + 存 -0x14 槽）；②cmd 声明 short
    //   （复现 0x8594ecb movswl -0xe 单段与 0x8594f02 movzwl+movzwl 双段传参）；
    //   ③checker/fn 去命名局部，函数指针表达式内联（0x8594ec9 mov (%eax),%esi 直取）；
    //   ④history 局部删除，`user->m_historyLog.AbnormalUser` 直算 add $0x79700
    //   （0x8594f09）；⑤`if(!doDispatch(...))`（0x8594f7e xor $1 形 ⇒ doDispatch 返回 bool）；
    //   ⑥末段 && 链改嵌套 if（0x8594f9e/fad/fbc 三 je 共享出口 0x8594fd4）；
    //   ⑦S11 将 IsCurCharacLeaveTimeout 改 bool 后末段物化归正（+2 行收）。
    //   （①~⑦ 为 S153 已落源形，全部保留。）
    ENUM_PACKETCLASS packetClass = (ENUM_PACKETCLASS)(signed char)data[0];
    short cmd = *(unsigned short*)(data + 1);
    CUser* user = G_CGameManager()->getUser(userId, 5);
    if (user == 0)
        return 0;

    pUserPacketCounter_->IncreasePacket(user->get_acc_id());

    typedef int (*CheckFn)(void*, unsigned int, int);   // s_antibotChecker_ vtbl+0x3c
    // b28d-solo-S161：P231/P307 正例——纯 `register` 提示（非 asm 绑定，P0 合规）
    // + 单层解引用 `*reinterpret_cast<CheckFn*>((char*)*(void**)p + 0x3c)` 形，使 4.4
    // 把函数指针直落 callee-saved（探针 /tmp/pd_probe6.cpp t6 `mov (%eax),%ebx` 单段 +
    // 免槽；t8 的 `void***` 下标形恒两段），复现 ORIG 0x8594ec9 `mov (%eax),%esi` 直落。
    register CheckFn fn = *reinterpret_cast<CheckFn*>(
        reinterpret_cast<char*>(
            *reinterpret_cast<void**>(GlobalData::s_antibotChecker_)) + 0x3c);
    // ORIG 0x8594eda `mov %eax,%edx` 中转 ⇒ key 实参按 unsigned int 形参传递（P211 档①）。
    if (fn(GlobalData::s_antibotChecker_, (unsigned int)user->getAntibotKey(), cmd) != 0)
    {
        user->m_historyLog.AbnormalUser((ENUM_ABNORMAL_USER_LOG_TYPE)0, cmd,
                                        user->getAntibotKey(), 0);
        return 0;
    }

    if (!doDispatch(user, packetClass, cmd, data, len,
                    (ENUM_PACKETCLASS)0, 0))
        return 0;

    if (((CUserCharacInfo*)user)->getCurCharacR() != 0)
    {
        if (((CUserCharacInfo*)user)->IsCurCharacGhost())
        {
            if (((CUserCharacInfo*)user)->IsCurCharacLeaveTimeout())
            {
                ((CUserCharacInfo*)user)->LiveCurCharac();
                pvp_assault::CAssaultPlace::SendUserState(user);
            }
        }
    }
    return 1;
}

// b28d-solo-S161 判词 [ORIG 0x8594922]：DIFF，orig 300 / our 298 insn（gap=2），
//   p167 callsite 40/40 CLEAR（0 missing / 0 extra）⇒ 语义完整（§6.2 三件套口径）。
//   本波已收：①clientCrc 局部删除，`*(unsigned int*)(head+7)` 两处内联（P183 去缓存，
//   消 -0x34 槽）；②antibot `fn(...)!=0 && flag!=0x25c` 拆嵌套 if（P225 推广，消 jmp 岛）。
//   ③S161 新收（同 dispatch）：vtbl+0x30 取函数指针改「单层解引用 + 纯 register」形 ⇒
//   `mov (%eax),%esi` 单段直落（N18a 消除，0x859497d 复现），并连带恢复 cmd→%ebx /
//   fn→%esi 的 callee-saved 角色（N109 消除）；④CheckFn 第 2 形参 unsigned int 保
//   `mov %eax,%edx` 中转（P211 档①）。
//   残余 N 族（本波穷举实测）：①短路物化岛缺失——ORIG 0x08594ba8 段
//   `mov $1,%eax; jmp; mov $0,%eax; test %al,%al; je`（a/b 双落点）；P226 语句表达式
//   （our 303）与 P231 register bool（our 304）虽可造该岛，但 4.4 恒多 `mov %eax,%ebx`
//   中转且 a/b 共落点，并连锁 gap 2→4/5 ⇒ 回退直短路形（gap 最小），并入 N7/P197④ 族；
//   ②局部槽整体平移 4（ORIG -0x20/-0x24/-0x31 vs 我方 -0x1c/-0x20/-0x2d）：S161 曾用
//   P232 前置 `char framePad[4]` 撑槽（行级 diff 342→308；`[8]`/`double` 反劣 380）；
//   **S177 已改为末尾 `const int limit = 0x7d0;`（一槽两用：复现 ORIG -0xc 死 store +
//   浅槽整体深 4），行级 diff 再降至 253**；残余仅 head+7 的 disp-vs-lea/add 折叠
//   （N102/N4）与 errResult 已修（switch 复现寄存器决策链）。返回类型 int→bool 见 PacketDispatcher.h
//   （P292，dispatch 调用点 0x8594f7e xor $1 实证）。
//   [b28d-solo-S316 复核终态] head+7 的 disp-vs-lea/add 残余**已收**：旧形
//   `*(unsigned int*)(head + 7)` 三处恒 lea+add+mov（9 条），ORIG 首次 lea+add 后
//   第二/三处为 disp 直读（0x08594ab1 `mov -0x2a(%ebp),%edx`、0x08594ada
//   `mov -0x2a(%ebp),%ecx`）⇒ 改具名 pack(1) 视图 `DispatchPacketHead`（7B 前缀 +
//   dword 校验和 + 2B 尾，sizeof 0xd / align 1 ⇒ 槽基 -0x31 与帧 0x17390 零变化），
//   首次仍用指针算术 `*(unsigned int*)((char*)&head + 7)` 保 lea+add、后两处改
//   `head.m_checksum` 折叠 disp ⇒ 该段逐条同形，our 301→297（orig 300）。
//   残余（封顶，不因本波引入）：①`cmpl $0,-0x18/-0x1c(%ebp); setne %al; test %al,%al;
//   je` 两处 bool 物化（ORIG 4.1 槽比零物化；4.4 恒直跳，P197④/N122 族）；
//   ②DenyPacket 短路岛块序（P226/P197④）；③switch 无 default 时 4.4 恒 `je`+`jmp`
//   而 ORIG 用 `jne` fall-through（1 条）。verdict 仍 DIFF（gap -3 全部来自 ① 的
//   4 条 setne/test 减 ③ 的 1 条 jmp；§6.2 三件套：p167 40/40 CLEAR）。
//   [b28d-solo-PacketDispatcher S339 终态] **gap 归零：orig 300 / our 300**（S316 的 our 297 + 3）。
//     三项新钥匙（均本波实测，fresh /tmp/tu_PacketDispatcher.o）：
//     ① P225 槽比零物化：`if (v != 0)` 4.4 恒直跳 `je`，改 `while (v != 0) { …; return 0; }`
//        （循环体全路径 return ⇒ 语义等价）即精确复现 ORIG 0x08594c8d/0x08594ce8 两处
//        `cmpl $0,槽; setne %al; test %al,%al; je`（+4 条；探针 /tmp/pd_probe_while.cpp
//        f_if vs f_while 对照）。
//     ② P171 照抄：ORIG 0x08594bd5 `cmpw $0x25b` 前**无** `movl $0,-0x1c(%ebp)`
//        ⇒ `unsigned int tick0;` 去初始化（非 0x25b 路径不读该槽，语义等价；−1 条）。
//     ③ P198① 预清必写 + P46 叶块序：ORIG 0x08594bef 有 `movl $0,-0x18(%ebp)`
//        （errResult 声明预清，位于 tick0 分支之后）⇒ `int errResult = 0; errResult = …;`
//        两句式（+1 条）；且 ORIG 物理叶块 case2(0x8594c40) 在 case1(0x8594cbb) 之前、
//        case2 走 `jne` fall-through（0x8594c3a）⇒ switch 改**降序书写**（case 2 先），
//        4.4 即从 `je case2; jmp 出口`（多 1 条）翻为 ORIG 的 jne fall-through（−1 条）。
//     残余 6 行（封顶）：DenyPacket 段 ORIG 先物化 `(A||B)` 再 `call DenyPacket`，
//        4.4 恒把 DenyPacket 内联进 `&&` 短路链（块内 6 行互换，指令数 300:300 全等）。
//        五形穷举（P226 只包左项 305 / 整条件语句表达式 300 同形 / 三目 297 /
//        `!!` 300 同形 / while 条件 303）均不能移动物化点 ⇒ P197④ 镜像族封顶。
//     三件套：p167 callsite 40/40 CLEAR；clean 逐行比对 6/300 行差（全为该块内互换）；
//        verdict 仍 DIFF（compare_tu 逐行口径），TU 计数零回归（identical 394 / ae 1537）。
#include "PacketDispatcher/doDispatch.inc"
                                  unsigned short cmd, char* data, int len,
                                  ENUM_PACKETCLASS packetClass2, unsigned short flag)
{
    // b28d-solo-PacketDispatcher 判词同上（N18a / P197④ / N82，callsite CLEAR）。
    (void)packetClass2;
    // b28d-solo-S161 原判词（已被 S177 形替代，保留证据）：P232 前置 `char framePad[4]`
    // 撑槽使 head 等槽对齐（ndiff 342→308；[8]/double 反劣 380）。**S177 复核后删除
    // framePad，改由末尾 `const int limit = 0x7d0;` 一槽两用**（复现 ORIG 0x08594dd0 的
    // `movl $0x7d0,-0xc(%ebp)` 死写入 + 浅槽整体深 4 对齐 ORIG dispatcher@-0x20）；帧仍
    // 0x17390（framePad 与 limit 并存会使帧 16 对齐进位到 0x173a0，已实测回退）。
    log_recv_packet(user, packetClass, cmd, data, len);

    typedef int (*CheckFn)(void*, unsigned int, unsigned short);  // vtbl+0x30
    // b28d-solo-S161：与 dispatch 同法（register + 单层解引用）复现 ORIG 0x0859497d
    // `mov (%eax),%esi` 单段直落 + cmd 落 %ebx 的角色分配。
    register CheckFn fn = *reinterpret_cast<CheckFn*>(
        reinterpret_cast<char*>(
            *reinterpret_cast<void**>(GlobalData::s_antibotChecker_)) + 0x30);
    if (fn(GlobalData::s_antibotChecker_, (unsigned int)user->getAntibotKey(), cmd) != 0)
    {
        if (flag != 0x25c)
        {
            LogManager::logFormat(
                1, "PacketDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0xa06, "m_id:%s, cmd:%d", NumberToString(user->get_acc_id(), 0),
                (int)cmd);
        }
    }

    IDispatcher* dispatcher = get_dispatcher((int)cmd);
    if (dispatcher == 0)
        return 0;

    server_load_monitoring();
    user->backupSeed();

    if (0xd < len && flag == 0x25c)
    {
        unsigned int serverCrc = Encryption::crc32N(data + 0xb, len - 0xb);
        // [b28d-solo-S316] ORIG 0x08594a9c 首次取址 `lea -0x31;add $7`（数组形），
        // 0x08594ab1/0x08594ada 后两次读值 `mov -0x2a(%ebp),%edx/%ecx`（disp 直读）——
        // 4.4 对 `head+7` 指针形恒重发 lea+add ⇒ 用具名 pack(1) 视图（13B/align1，
        // 槽位 -0x31 与帧零变化）成员访问折叠 disp；首次仍用指针算术保 lea+add。
        struct __attribute__((packed)) DispatchPacketHead
        {
            char m_prefix[7];
            unsigned int m_checksum;
            char m_tail[2];
        };
        DispatchPacketHead head;
        memcpy(&head, data, 0xd);
        // 线格式视图：协议头 +7 处为校验和 dword（非对齐）。ORIG 0x8594a?? 实证
        // 两次 `lea head; add $7` 直接内联（P183 去缓存）：无 clientCrc 局部槽。
        _NS_PI_2ND_MakeChecksumTo1Byte2(serverCrc,
                                        *(unsigned int*)((char*)&head + 7));
        if (head.m_checksum != serverCrc)
        {
            LogManager::logFormat(
                1, "PacketDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0xa37, "Check Sum Error: server(%d), client(%d), mid(%s)",
                serverCrc, head.m_checksum,
                NumberToString(user->get_acc_id(), 0));
            user->DisConnSig((DISCONN_SIG)0xd, true, 0);
            return 0;
        }
    }

    PacketBuf packet;
    packet.bind_packet(data, len);
    user->setLastPacketID((int)cmd);

    // [b28d-solo-S400] DenyPacket 段（S339 判词推翻）：ORIG 0x08594b92..0x08594bb6 先把
    //   `(A||B)` 物化为寄存器 bool（`mov $1,%eax; jmp; mov $0,%eax; test %al,%al; je`，
    //   无槽往返）再 `call DenyPacket`。S339 穷举五形（P226 只包左项 / 整条件语句表达式 /
    //   三目 / `!!` / while）均不可；**P225 推广的「拆嵌套 if」形是钥匙**：把 `(A||B) && X`
    //   改写成 `if (A||B) { if (X) return 1; }` ⇒ 4.4 复现 ORIG 的物化岛
    //   （探针 /tmp/pd_p4.cpp n1 与 ORIG 0x8594b9a..0x8594bd5 逐条同构；n2..n9 仍直落）。
    if (G_GameWorld()->GetChannelType() == 7 ||
        G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        if (DenyPacket((int)cmd))
            return 1;
    }

    // [b28d-solo-PacketDispatcher S339] P171 照抄：ORIG 0x08594bd5 段在 `cmpw $0x25b` 前**无**
    //   `movl $0x0,-0x1c(%ebp)`（tick0 只在 cmd==0x25b 分支内赋值，非 0x25b 路径不读）
    //   ⇒ 原码是未初始化声明；我方 `= 0` 多一条死 store。
    unsigned int tick0;
    if (cmd == 0x25b)
        tick0 = GlobalData::s_systemTime_.getCurTickCount();

    // [b28d-solo-PacketDispatcher S339] P198①预清必写：ORIG 0x08594bef `movl $0x0,-0x18(%ebp)`
    //   （tick0 分支之后、调用之前）⇒ 原码是「声明预清 + 独立赋值」两句式。
    int errResult = 0;
    errResult = m_chinaError.doErrorDispatcher(user, cmd);
    // [b28d-solo-PacketDispatcher S339] P225 钥匙：ORIG 0x08594c8d 段为
    //   `cmpl $0x0,-0x18(%ebp); setne %al; test %al,%al; je`（4.1 槽比零 bool 物化），
    //   4.4 对 `if (v != 0)` 恒直跳 `je`（探针 /tmp/pd_probe_while.cpp f_if）。
    //   `while (v != 0) { …; return 0; }` 形精确复现 setne/test 两段（同探针 f_while），
    //   循环体全路径 return ⇒ 语义与 if 完全等价。
    while (errResult != 0)
    {
        // ORIG 0x08594c9a 段：`mov -0x18(%ebp),%eax; cmp $0x1,%eax; je; cmp $0x2,%eax;
        // jne` 寄存器决策链（P251/P286：无内存直比）⇒ 原码是 switch（值在寄存器）。
        // [b28d-solo-PacketDispatcher S339] P46 叶块序：ORIG 物理叶块 case2(ERROR_TING, 0x8594c40) 在
        //   case1(ERROR_NOT_TING, 0x8594cbb) 之前，且 case2 为 `jne` fall-through
        //   （0x8594c3a jne 出口）⇒ 原码 case 书写序为降序（case 2 先）；升序书写时
        //   4.4 恒产 `je case2; jmp 出口`（多 1 条 jmp）。
        switch (errResult)
        {
        case 2:
            LogManager::logFormat(
                1, "PacketDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0xa5b, "[doErrorDispatcher] ERROR_TING - code : %d, m_id :%s",
                (int)cmd, NumberToString(user->get_acc_id(), 0));
            user->DisConnSig((DISCONN_SIG)0xa, true, 0);
            break;
        case 1:
            LogManager::logFormat(
                1, "PacketDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0xa60, "[doErrorDispatcher] ERROR_NOT_TING - code : %d, m_id :%s",
                (int)cmd, NumberToString(user->get_acc_id(), 0));
            break;
        }
        return 0;
    }

    int handlerResult = dispatcher->dispatch_template(user, packet);
    // [b28d-solo-PacketDispatcher S339] 同上：ORIG 0x08594ce8 段 `cmpl $0x0,-0x1c(%ebp); setne %al;
    //   test %al,%al; je` 槽比零物化 ⇒ 用 while 形复现（P225）。
    while (handlerResult != 0)
    {
        user->DisConnSig((DISCONN_SIG)0xa, true, handlerResult);
        return 0;
    }

    if (cmd == 0x25b)
    {
        GlobalData::s_systemTime_.update();
        unsigned int elapsed = GlobalData::s_systemTime_.getCurTickCount() - tick0;
        // ORIG 0x08594dd0 `movl $0x7d0,-0xc(%ebp)` + `cmpl $0x7d0,-0x10(%ebp)`：
        // 原码是具名 const 局部（4.1 发射初始化 store、读取处折叠为立即数比较，P171 死写入）。
        // 该局部同时补齐我方浅槽整体浅 4（ORIG dispatcher@-0x20 vs 我方 -0x1c）的帧差。
        const int limit = 0x7d0;
        if (limit < elapsed)
        {
            LogManager::logFormat(
                1, "PacketDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0xa7a, "Delay notice, OnRecvBufferFromClient dispatch time : %u",
                elapsed);
        }
    }
    if (cmd != 0x25b)
    {
        _NS_PI_Check_CMD(packetClass, cmd, packet, user);
    }
    return 1;
}

// ---- 全局单例访问器（ORIG W 0x0823453e）----
PacketDispatcher* G_PacketDispatcher()
{
    return GlobalInstance<PacketDispatcher>::inst_ptr();
}
