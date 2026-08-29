// HdrCreate3: ORIG exported symbols, first .h declaration. Evidence: nm ORIG + InterDispatcher.cpp
// PCK_AUCTION_REGIST_ITEM_GA：ORIG 弱内联 ctor W @0x084e9a12。
// has_def=1：定义在 InterDispatcher.cpp:7330（struct : nsl::PACKET_HEADER，0xc5 字节 packed，
// 字段 char_idx@18/owner_id@22/owner_name@26/user_type@39/price@40/instant_price@44/
// item_info@48(RegPckDnfItemInfo 0x35)/jewel_info@101/expansion@131/black_point@135/
// unit_price@149/_reg_roi_category_key@153/owner_nexon_id@165；ctor memset(this,0,0xc5)，
// setCategory(0)/setPacketID(3)/setSize(0xc5)）。
// RegPckDnfItemInfo/RegPckRoiCategory 目前唯一声明点为该 TU 匿名 namespace（依赖
// RandomOption/UpgradeSeparateInfo/ReservedCapacity），无权威头，故本首头不复制字段，
// 完整布局以 InterDispatcher.cpp 为准。
#ifndef DNF_GAME_PCK_AUCTION_REGIST_ITEM_GA_H_
#define DNF_GAME_PCK_AUCTION_REGIST_ITEM_GA_H_

#include "PacketDesign.h"   // nsl::PACKET_HEADER 权威定义

struct PCK_AUCTION_REGIST_ITEM_GA : public nsl::PACKET_HEADER
{
    PCK_AUCTION_REGIST_ITEM_GA();   // ORIG W @0x084e9a12（体：memset 0xc5 + setCategory(0)/setPacketID(3)/setSize(0xc5)，InterDispatcher.cpp）
};

#endif  // DNF_GAME_PCK_AUCTION_REGIST_ITEM_GA_H_
