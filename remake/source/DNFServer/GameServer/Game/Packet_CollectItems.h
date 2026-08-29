// HdrCreate3: ORIG exported symbols, first .h declaration.
// Evidence: nm ORIG + 定义提取自 Game/Dispatchers.cpp:2844-2869（TU 内完整类型，
// 无基类；ctor 外联定义在 cpp，接入时由宿主迁除）。wire 视图：
// PacketHeader(type 0x27e6 / size 0x39) + accId/itemIdx/characNo/nameLen/name，
// 总长 0x39；ctor 清 itemIdx(+0x12)/characNo(+0x16) 两 int。
#ifndef DNF_GAME_PACKET_COLLECTITEMS_H_
#define DNF_GAME_PACKET_COLLECTITEMS_H_

class Packet_CollectItems
{
public:
    Packet_CollectItems();  // ORIG W 0x0822c75a

    unsigned short m_type;    // +0x00（header type 0x27e6）
    unsigned short m_size;    // +0x02（header size 0x39）
    unsigned short m_field4;  // +0x04
    int m_field6;             // +0x06
    char m_padA[4];           // +0x0a..+0x0d
    unsigned int m_accId;     // +0x0e
    unsigned int m_itemIdx;   // +0x12
    int m_characNo;           // +0x16
    char m_nameLen;           // +0x1a
    char m_name[0x1d];        // +0x1b..+0x37
};                            // 总长 0x39

#endif  // DNF_GAME_PACKET_COLLECTITEMS_H_
