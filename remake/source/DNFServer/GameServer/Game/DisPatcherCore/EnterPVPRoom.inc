int EnterPVPRoom(CUser* user, PvP_Room* pvp, ENUM_CMDPACKET cmd, char const* pass);

// ============================================================================
// ORIG T _Z12EnterPVPRoomP5CUserP8PvP_Room14ENUM_CMDPACKETPKc @ 0x081ed09a
// (size 0x62e)  <global>::EnterPVPRoom(CUser*, PvP_Room*, ENUM_CMDPACKET, char const*)
// 报告：docs/class_func_reports/_lt_global_gt_/EnterPVPRoom.md（## 汇编 + ## 反编译 C 已完整阅读）
// 语义：进入 PvP 房间前置校验链 → join_room → 成功时广播席位并传送；返回 1 成功 / 0 失败。
// 依赖声明核验：
//   CUser.h:2610 int Send(PacketGuard&)；:2572 int get_state()；:2825 bool CheckInTrade()；
//            :2827 bool CheckInParty()；:2820 void* GetParty()；:2813 int get_unique_id() const；
//            :2745 const char* get_acc_name() const；:2623 SendCmdErrorPacket(cmd,err,PacketGuard&)
//   CUserCharacInfo.h:845 unsigned char IsCurCharacGhost()；:797 signed char getCurCharacVill() const；
//            :1048 bool IsBusyDisjointStore()；:673 const char* getCurCharacName() const
//   PvP_Room.h:87 int check_joinable(CUser*)；:46 int get_pvp_battle_mode()；:102 unsigned char
//            IsExistPassword()；:101 bool CheckPassword(const char*, int)；:77 int join_room(CUser*,
//            int&, bool)；:43 int get_index()；:153 int make_seat_info(char*, int)
//   CPrivateStoreMgr.h:347/361 IsBusyPrivateStore / GetInstancePrivateStoreMgr
//   CParty.h:438 void send_invite_pvp(int)    CGameManager.h:470/626 CheckOutParty / G_CGameManager
//   GameWorld.h:160/197/242/340 IsSameTeamChannelJoin / send_all / goto_pvp / G_GameWorld
//   PacketGuard.h(InterfacePacketBuf.h:22/34/36) put_header(int,int)/put_byte(int)/finalize(bool)
//   CSwitchLog.h ctor(const char*, int, int, int) / operator()(const char*, ...)
// ============================================================================
