bool EnterPVPRoom(CUser* user, PvP_Room* room, ENUM_CMDPACKET cmd, char const* password);

// ORIG R 0x08af45e4 BLEEDING_DAMAGE_IN_ACTION_RATE（.rodata 4 字节 float，与
// BLEEDING_DAMAGE_TERM 0x08af45e0 相邻）——SDC_ActiveStatus_Damage_Check 阈值。
