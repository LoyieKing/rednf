bool MakePVPRoom(CUser* user, MSG_MAKE_PVP_ROOM& msg, ENUM_CMDPACKET cmd, PVP_BATTLE_MODE mode);
//   ^ ORIG T 0x081f793c _Z11MakePVPRoomP5CUserR17MSG_MAKE_PVP_ROOM14ENUM_CMDPACKET15PVP_BATTLE_MODE
//     （LinkE2：尾参改 ORIG 枚举 PVP_BATTLE_MODE 对齐 mangled；定义 MakePVPRoom.cpp）

//   ^ ORIG T 0x081ed09a _Z12EnterPVPRoomP5CUserP8PvP_Room14ENUM_CMDPACKETPKc（wave1：
//     DisPatcher_EnterPVP::dispatch_sig 消费；全仓无声明定义，定义由 Main 分派属主批）
//   ^ ORIG W 0x082352c5 _Z18RentAvatarErrorProcP5CUseri（同上）
