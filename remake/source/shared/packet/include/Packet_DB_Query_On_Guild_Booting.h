//
// df_guild_r — 启动时向 DBMW 查询公会引导数据的包
//

#ifndef PACKET_DB_QUERY_ON_GUILD_BOOTING_H
#define PACKET_DB_QUERY_ON_GUILD_BOOTING_H

#include "PacketHeader.h"

class Packet_DB_Query_On_Guild_Booting : public PacketHeader {
public:
    Packet_DB_Query_On_Guild_Booting() : PacketHeader(0x677, 0xb){};
} __attribute__((packed));

#endif  // PACKET_DB_QUERY_ON_GUILD_BOOTING_H
