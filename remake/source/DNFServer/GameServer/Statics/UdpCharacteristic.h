#ifndef UDP_CHARACTERISTIC_H_
#define UDP_CHARACTERISTIC_H_

#include "PacketHeader.h"

class CServerHandler;
class Packet_Pvp_Ping_Statistic;
class Packet_Party_Ping_Statistic;
class Packet_Fair_Pvp_Ping_Statistic;
class Packet_Party_Result_Statistic;
class Packet_Abnormal_Exit_Statistic;

// ---- FrameLag 专用包（0xc29-0xc30 族，UdpCharacteristic 使用）----
// [2026-09-15 W10-Static] m_i 元素与 Item::m_pair 元素的**共用真类型**：
//   原先两处各写一份等价布局（FrameLagDataStruct::Pair vs Item::LagPair），
//   类型不同 ⇒ .cpp 只能 `*reinterpret_cast<const LagPair*>(&data.m_i[k][j])`
//   整体赋值（取址 ⇒ GCC 物化指针 + 间接读，多 lea/add 且少两条直接寻址）。
//   统一为同一类型后可直接整体赋值 ⇒ 复现 ORIG 的
//   `mov 0x18(%ecx,%eax,8),%edx; mov 0x14(%ecx,%eax,8),%eax`。
struct FrameLagPairUnion
{
    union
    {
        struct { int m_i; float m_f; } s;
        int m_words[2];
    };
};
#pragma pack(push, 1)
class Packet_Frame_Lag_Statistic_Reload_Spec : public PacketHeader
{
public:
    Packet_Frame_Lag_Statistic_Reload_Spec();
    unsigned char m_fieldA;  // +0xa
    int m_fieldB;            // +0xb
};

class Packet_Frame_Lag_Statistic_Write_Lag_Index : public PacketHeader
{
public:
    Packet_Frame_Lag_Statistic_Write_Lag_Index();
    struct Item
    {
        // [2026-09-15 W10-Static 机理 W-A] 原 `short m_s[4]`：数组下标形被 4.4 折进
        //   索引常量锚（`sub $0x175-2i; disp 0x7`，每字段换锚），ORIG 恒锚
        //   （`sub $0x177`）+ disp 递增（0x9/0xb/0xd/0xf）⇒ 独立标量成员
        //   （布局/sizeof item 0x38 不变）。
        short m_s0;        // +0x00（相对 item 基址）
        short m_s1;        // +0x02
        short m_s2;        // +0x04
        short m_s3;        // +0x06
        // [2026-09-15 W7] 原 `int m_pair[6][2]`：逐 int 赋值使 SaveFrameLagData
        //   每条语句重算一次 `it->second`（ORIG 内层循环每轮仅一次 operator*）。
        //   同 8 字节结构整体赋值 ⇒ 布局不变（packed，0x30 字节）。
        typedef ::FrameLagPairUnion LagPair;   // 与 FrameLagDataStruct::m_i 元素同型
        LagPair m_pair[6];  // +0x08（0x30 字节，布局同 int[6][2]）
    };
    char m_serverGroup;    // +0xa
    int m_key;             // +0xb
    int m_value;           // +0xf
    short m_part[8];       // +0x13
    short m_ratio1;        // +0x23
    short m_ratio2;        // +0x25
    Item m_items[6];       // +0x27（每项 0x38）
    // [2026-09-15 W5] 原 `char m_tail[4]`：ORIG 在 SaveFrameLagData 里对它做 4 字节
    //   标量写（`mov -0x30(%ebp),%eax; mov %eax,-0x38(%ebp)`，disp 直落）⇒ 是
    //   **标量 int 成员**；写成 char[4] 时经 `*(int*)` 走数组 decay ⇒ 生成
    //   `lea -0x1af(%ebp),%eax; lea 0x177(%eax),%edx; mov (%edx)` 的地址物化 ✗
    //   大小仍为 4 ⇒ 类大小/布局不变（全库无其它使用点）。
    int m_tail;            // +0x177
} __attribute__((packed));

class Packet_Frame_Lag_Statistic_Write_Query : public PacketHeader
{
public:
    Packet_Frame_Lag_Statistic_Write_Query();
    char m_data[0x400];  // +0xa（packetSize 0x40a）
};

class Packet_Frame_Lag_Used_Memory_Write_Query : public PacketHeader
{
public:
    Packet_Frame_Lag_Used_Memory_Write_Query();
    char m_data[0x400];  // +0xa（packetSize 0x40a）
};

class Packet_Udp_Characteristic : public PacketHeader
{
public:
    Packet_Udp_Characteristic();
    char m_serverGroup;  // +0xa（p2pnetwork_statistic.server_group）
    int m_values[10]; // +0xb
} __attribute__((packed));
#pragma pack(pop)

// ---- UdpCharacteristic：0x2C ----
class UdpCharacteristic
{
public:
    UdpCharacteristic();
    ~UdpCharacteristic();
    void PushPvpPingData(Packet_Pvp_Ping_Statistic* pkt);
    void PushPartyPingData(Packet_Party_Ping_Statistic* pkt);
    void PushFairPvpPingData(Packet_Fair_Pvp_Ping_Statistic* pkt);
    void PushPartyResultData(Packet_Party_Result_Statistic* pkt);
    void PushAbnormalExitData(Packet_Abnormal_Exit_Statistic* pkt);
    void InitUdpCharacteristicData();
    void SaveUdpCharacteristicData(CServerHandler* handler, int interval);
    int m_intervalTick;         // +0（保存间隔计数）
    int m_successPartyTry;      // +4（p2pnetwork_statistic.success_party_try）
    int m_totalPartyTry;        // +8（p2pnetwork_statistic.total_party_try）
    int m_dungeonBadPing;       // +0xc（p2pnetwork_statistic.dungeon_bad_ping）
    int m_dungeonTotal;         // +0x10（p2pnetwork_statistic.dungeon_total）
    int m_pvpBadPing;           // +0x14（p2pnetwork_statistic.pvp_bad_ping）
    int m_pvpTotal;             // +0x18（p2pnetwork_statistic.pvp_total）
    int m_fairPvpBadPing;       // +0x1c（p2pnetwork_statistic.fair_pvp_bad_ping）
    int m_fairPvpTotal;         // +0x20（p2pnetwork_statistic.fair_pvp_total）
    int m_successDungeonClear;  // +0x24（p2pnetwork_statistic.success_dungeon_clear）
    int m_totalDungeonClear;    // +0x28（p2pnetwork_statistic.total_dungeon_clear）
};

#endif // UDP_CHARACTERISTIC_H_
