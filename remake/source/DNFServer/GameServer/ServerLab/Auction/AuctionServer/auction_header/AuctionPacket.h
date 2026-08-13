#ifndef NSL_AUCTIONPACKET_H_
#define NSL_AUCTIONPACKET_H_

#include <string.h>

#include "PacketDesign.h"
#include "AuctionItem.h"

typedef unsigned int memberIdentificationNumber_t;

typedef unsigned char BYTE;

#pragma pack(push, 1)
struct TSearchByItemId_
{
    unsigned int startIndex;             // @0
    BYTE upgradeStart;                   // @4
    BYTE upgradeEnd;                     // @5
    BYTE itemIdNum;                      // @6
    unsigned short category;             // @7
    ROI_Category ROI_Search_category_key; // @9
    BYTE refineStart;                    // @21
    BYTE refineEnd;                      // @22
};
typedef TSearchByItemId_ TSearchByItemId;
typedef TSearchByItemId_* PSearchByItemId;

struct TSearchByCategory_
{
    unsigned int startIndex;             // @0
    unsigned short category;             // @4
    BYTE rarity;                         // @6
    BYTE upgradeStart;                   // @7
    BYTE upgradeEnd;                     // @8
    BYTE lvStart;                        // @9
    BYTE lvEnd;                          // @10
    ROI_Category ROI_Search_category_key; // @11
    BYTE refineStart;                    // @23
    BYTE refineEnd;                      // @24
};
typedef TSearchByCategory_ TSearchByCategory;
typedef TSearchByCategory_* PSearchByCategory;

struct AuctionItemInfo
{
    unsigned long long auction_id;       // @0
    int price;                           // @8
    int instant_price;                   // @12
    int average_price;                   // @16
    char owner_name[13];                 // @20
    double owner_reliability;            // @33
    unsigned char expire_time;           // @41
    DnfItemInfo item_info;               // @42
    stAvatarEmblemInfo_t emblem_info;    // @95
    stAvatarExpansionInfo_t expansion;   // @125
    unsigned int black_point;            // @129
    int unit_price;                      // @133
};

struct MyRegistedItemInfo
{
    unsigned long long auction_id;       // @0
    int price;                           // @8
    int instant_price;                   // @12
    char buyer_name[13];                 // @16
    unsigned char expire_time;           // @29
    DnfItemInfo item_info;               // @30
    stAvatarEmblemInfo_t emblem_info;    // @83
    stAvatarExpansionInfo_t expansion;   // @113
};

struct MyBiddingItemInfo
{
    unsigned long long auction_id;       // @0
    int price;                           // @8
    int instant_price;                   // @12
    char owner_name[13];                 // @16
    double owner_reliability;            // @29
    unsigned char expire_time;           // @37
    DnfItemInfo item_info;               // @38
    stAvatarEmblemInfo_t emblem_info;    // @91
    stAvatarExpansionInfo_t expansion;   // @121
};

#pragma pack(pop)

struct ExpireTimeDictionaryData;

struct AuctionDictionaryData
{
    int price;                           // @0
    int instant_price;                   // @4
    int owner_id;                        // @8
    int buyer_id;                        // @12
    long expire_time;                    // @16
    DnfItemInfo item_info;               // @20
    unsigned char owner_type;            // @73
    unsigned int black_point;            // @76
    int unit_price;                      // @80
    ROI_Category _reg_roi_category_key;  // @84
    char owner_nexon_id[32];             // @96
    ExpireTimeDictionaryData* expire_table_ptr;  // @128
};

// ---- GA request packets (read via GetPacket casts; ctor only where emitted) ----

#pragma pack(push, 1)
struct PCK_AUCTION_SEARCH_BY_ITEMKEY_GA : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int character_id;                    // @22
    TSearchByItemId tSearchByItemId;     // @26
    unsigned long item_id_array[20];     // @49

    PCK_AUCTION_SEARCH_BY_ITEMKEY_GA()
    {
        memset(this, 0, 0x81);
        setCategory(0);
        setPacketID(6);  // AUCTION_SEARCH_BY_ITEMKEY_GA
        setSize(0x81);
    }
};

struct PCK_AUCTION_ASK_AVERAGE_PRICE_GA : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int character_id;                    // @22
    unsigned long item_id;               // @26
    unsigned char item_upgrade_value;    // @30
    unsigned char randomOption_index[3]; // @31
    ROI_Category roi_search_key;         // @34
    unsigned char item_refine_value;     // @46
};

struct PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int character_id;                    // @22
    unsigned char inven_type;            // @26
    unsigned short slot_no;              // @27
    unsigned long item_id;               // @29
    int add_info;                        // @33
    int price;                           // @37
    int instant_price;                   // @41
    int unit_price;                      // @45
    ROI_Category ROI_Category_Key;       // @49
};

struct PCK_AUCTION_REGIST_ITEM_GA : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int owner_id;                        // @22
    char owner_name[13];                 // @26
    unsigned char user_type;             // @39
    int price;                           // @40
    int instant_price;                   // @44
    DnfItemInfo item_info;               // @48
    stAvatarEmblemInfo_t jewel_info;     // @101
    stAvatarExpansionInfo_t expansion;   // @131
    unsigned int black_point;            // @135
    itemGloballyUniqueIdentifier_t guid; // @139
    int unit_price;                      // @149
    ROI_Category _reg_roi_category_key;  // @153
    char owner_nexon_id[32];             // @165
};

struct PCK_AUCTION_REGIST_CANCEL_GA : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int owner_id;                        // @22
    __int64 auction_id;                  // @26
};

struct PCK_AUCTION_BIDDING_GA : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int buyer_id;                        // @22
    char buyer_name[13];                 // @26
    int price;                           // @39
    __int64 auction_id;                  // @43
};

struct PCK_AUCTION_SEARCH_BY_NOITEMKEY_GA : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int character_id;                    // @22
    TSearchByCategory tSearchByCategory; // @26
};

struct PCK_AUCTION_MY_REGISTED_ITEM_INFO_GA : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int owner_id;                        // @22
};

struct PCK_AUCTION_MY_BIDDING_INFO_GA : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int buyer_id;                        // @22
};

struct PCK_AUCTION_OPEN_PRIVATE_STORE_GA : public nsl::PACKET_HEADER
{
    memberIdentificationNumber_t m_id;   // @18
    int character_num;                   // @22
    int character_ids[36];               // @26
};

struct PCK_AUCTION_CLOSE_PRIVATE_STORE_GA : public nsl::PACKET_HEADER
{
    memberIdentificationNumber_t m_id;   // @18
};

struct PCK_AUCTION_BUY_ITEM_APIECE_GA : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int buyer_id;                        // @22
    int price;                           // @26
    __int64 auction_id;                  // @30
    int count;                           // @38
};

// ---- AG response packets (category 1) ----

struct PCK_AUCTION_REGIST_ACK_AG : public nsl::PACKET_HEADER
{
    PCK_AUCTION_REGIST_ACK_AG()
    {
        PACKET_CTOR_BODY(1, 0x0, 0x12);
    }
};

struct PCK_AUCTION_CHECK_AUCTION_READY_AG : public nsl::PACKET_HEADER
{
    bool db_work_done;                   // @18

    PCK_AUCTION_CHECK_AUCTION_READY_AG()
    {
        PACKET_CTOR_BODY(1, 0xd, 0x13);
    }
};

struct PCK_AUCTION_ASK_AVERAGE_PRICE_AG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int character_id;                    // @22
    int average_price;                   // @26
    int min_pirce[3];                    // @30
    int min_count[3];                    // @42

    PCK_AUCTION_ASK_AVERAGE_PRICE_AG()
    {
        PACKET_CTOR_BODY(1, 0x2, 0x36);
    }
};

struct PCK_AUCTION_ASK_REGISTED_ITEM_NUM_AG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int character_id;                    // @22
    short now_registed_num;              // @26
    unsigned char inven_type;            // @28
    unsigned short slot_no;              // @29
    unsigned long item_id;               // @31
    int add_info;                        // @35
    int price;                           // @39
    int instant_price;                   // @43
    int unit_price;                      // @47
    ROI_Category ROI_Category_Key;       // @51

    PCK_AUCTION_ASK_REGISTED_ITEM_NUM_AG()
    {
        PACKET_CTOR_BODY(1, 0x3, 0x3f);
    }
};

struct PCK_AUCTION_REGIST_RESULT_AG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int owner_id;                        // @22
    unsigned char regist_result;         // @26
    unsigned char result_because;        // @27

    PCK_AUCTION_REGIST_RESULT_AG()
    {
        PACKET_CTOR_BODY(1, 0x4, 0x1c);
    }
};

struct PCK_AUCTION_BIDDING_RESULT_AG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int buyer_id;                        // @22
    int price;                           // @26
    unsigned char bidding_result;        // @30
    unsigned char result_because;        // @31
    __int64 auction_id;                  // @32

    PCK_AUCTION_BIDDING_RESULT_AG()
    {
        PACKET_CTOR_BODY(1, 0x5, 0x28);
    }
};

struct PCK_AUCTION_REGIST_CANCEL_RESULT_AG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int owner_id;                        // @22
    unsigned char cancel_result;         // @26
    unsigned char result_because;        // @27

    PCK_AUCTION_REGIST_CANCEL_RESULT_AG()
    {
        PACKET_CTOR_BODY(1, 0x6, 0x1c);
    }
};

struct PCK_AUCTION_ITEM_LIST_AG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int character_id;                    // @22
    unsigned char pay_type;              // @26
    unsigned int total_item_num;         // @27
    unsigned short item_num;             // @31
    AuctionItemInfo item_info[60];       // @33

    PCK_AUCTION_ITEM_LIST_AG()
    {
        PACKET_CTOR_BODY(1, 0x7, 0x203d);
    }
};

struct PCK_AUCTION_MY_REGISTED_ITEM_INFO_AG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int owner_id;                        // @22
    unsigned char pay_type;              // @26
    unsigned char item_num;              // @27
    MyRegistedItemInfo my_registed_item_info[30];  // @28

    PCK_AUCTION_MY_REGISTED_ITEM_INFO_AG()
    {
        PACKET_CTOR_BODY(1, 0x8, 0xdd2);
    }
};

struct PCK_AUCTION_MY_BIDDING_INFO_AG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int buyer_id;                        // @22
    unsigned char pay_type;              // @26
    unsigned short item_num;             // @27
    MyBiddingItemInfo my_bidding_item_info[60];  // @29

    PCK_AUCTION_MY_BIDDING_INFO_AG()
    {
        PACKET_CTOR_BODY(1, 0x9, 0x1d69);
    }
};

struct PCK_AUCTION_LOG_MESSAGE_AG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    unsigned char auction_log_type;      // @22
    __int64 auction_id;                  // @23
    int character_id;                    // @31
    int price;                           // @35
    unsigned long item_id;               // @39
    unsigned char uniItemAttr;           // @43
    int add_info;                        // @44
    unsigned short endurance;            // @48
    unsigned long extendInfo;            // @50
    unsigned char result_because;        // @54

    PCK_AUCTION_LOG_MESSAGE_AG()
    {
        PACKET_CTOR_BODY(1, 0xb, 0x37);
    }
};

struct PCK_AUCTION_BUY_ITEM_APIECE_AG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int buyer_id;                        // @22
    int price;                           // @26
    unsigned char bidding_result;        // @30
    unsigned char result_because;        // @31
    __int64 auction_id;                  // @32

    PCK_AUCTION_BUY_ITEM_APIECE_AG()
    {
        PACKET_CTOR_BODY(1, 0xe, 0x28);
    }
};

// ---- PG response packets (category 0x13) ----

#define PACKET_CTOR_BODY(cat, pid, sz) \
    memset(this, 0, sz); \
    setCategory(cat); \
    setPacketID(pid); \
    setSize(sz)

struct PCK_AUCTION_REGIST_ACK_PG : public nsl::PACKET_HEADER
{
    PCK_AUCTION_REGIST_ACK_PG()
    {
        PACKET_CTOR_BODY(0x13, 0x0, 0x12);
    }
};

struct PCK_AUCTION_CHECK_AUCTION_READY_PG : public nsl::PACKET_HEADER
{
    bool db_work_done;                   // @18

    PCK_AUCTION_CHECK_AUCTION_READY_PG()
    {
        PACKET_CTOR_BODY(0x13, 0xd, 0x13);
    }
};

struct PCK_AUCTION_ASK_AVERAGE_PRICE_PG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int character_id;                    // @22
    int average_price;                   // @26

    PCK_AUCTION_ASK_AVERAGE_PRICE_PG()
    {
        PACKET_CTOR_BODY(0x13, 0x2, 0x1e);
    }
};

struct PCK_AUCTION_ASK_REGISTED_ITEM_NUM_PG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int character_id;                    // @22
    short now_registed_num;              // @26
    unsigned char inven_type;            // @28
    unsigned short slot_no;              // @29
    unsigned long item_id;               // @31
    int add_info;                        // @35
    int price;                           // @39
    int instant_price;                   // @43

    PCK_AUCTION_ASK_REGISTED_ITEM_NUM_PG()
    {
        PACKET_CTOR_BODY(0x13, 0x3, 0x2f);
    }
};

struct PCK_AUCTION_REGIST_RESULT_PG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int owner_id;                        // @22
    unsigned char regist_result;         // @26
    unsigned char result_because;        // @27

    PCK_AUCTION_REGIST_RESULT_PG()
    {
        PACKET_CTOR_BODY(0x13, 0x4, 0x1c);
    }
};

struct PCK_AUCTION_BIDDING_RESULT_PG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int buyer_id;                        // @22
    int price;                           // @26
    unsigned char bidding_result;        // @30
    unsigned char result_because;        // @31
    __int64 auction_id;                  // @32
    char owner_nexon_id[32];             // @40
    int charge_point;                    // @72

    PCK_AUCTION_BIDDING_RESULT_PG()
    {
        PACKET_CTOR_BODY(0x13, 0x5, 0x4c);
    }
};

struct PCK_AUCTION_REGIST_CANCEL_RESULT_PG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int owner_id;                        // @22
    unsigned char cancel_result;         // @26
    unsigned char result_because;        // @27

    PCK_AUCTION_REGIST_CANCEL_RESULT_PG()
    {
        PACKET_CTOR_BODY(0x13, 0x6, 0x1c);
    }
};

struct PCK_AUCTION_ITEM_LIST_PG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int character_id;                    // @22
    unsigned char pay_type;              // @26
    unsigned int total_item_num;         // @27
    unsigned short item_num;             // @31
    AuctionItemInfo item_info[60];       // @33

    PCK_AUCTION_ITEM_LIST_PG()
    {
        PACKET_CTOR_BODY(0x13, 0x7, 0x203d);
    }
};

struct PCK_AUCTION_MY_REGISTED_ITEM_INFO_PG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int owner_id;                        // @22
    unsigned char pay_type;              // @26
    unsigned char item_num;              // @27
    MyRegistedItemInfo my_registed_item_info[30];  // @28

    PCK_AUCTION_MY_REGISTED_ITEM_INFO_PG()
    {
        PACKET_CTOR_BODY(0x13, 0x8, 0xdd2);
    }
};

struct PCK_AUCTION_MY_BIDDING_INFO_PG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    int buyer_id;                        // @22
    unsigned char pay_type;              // @26
    unsigned short item_num;             // @27
    MyBiddingItemInfo my_bidding_item_info[60];  // @29

    PCK_AUCTION_MY_BIDDING_INFO_PG()
    {
        PACKET_CTOR_BODY(0x13, 0x9, 0x1d69);
    }
};

struct PCK_AUCTION_LOG_MESSAGE_PG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    unsigned char auction_log_type;      // @22
    __int64 auction_id;                  // @23
    int character_id;                    // @31
    int price;                           // @35
    unsigned long item_id;               // @39
    unsigned char uniItemAttr;           // @43
    int add_info;                        // @44
    unsigned short endurance;            // @48
    unsigned long extendInfo;            // @50

    PCK_AUCTION_LOG_MESSAGE_PG()
    {
        PACKET_CTOR_BODY(0x13, 0xb, 0x36);
    }
};

struct PCK_AUCTION_ASK_OWNER_IS_VIP_PG : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    __int64 auction_id;                  // @22
    bool is_vip;                         // @30
    int average_price;                   // @31
    int buyer_id;                        // @35
    char buyer_name[13];                 // @39
    int price;                           // @52
    int item_id;                         // @56

    PCK_AUCTION_ASK_OWNER_IS_VIP_PG()
    {
        PACKET_CTOR_BODY(0x13, 0xe, 0x3c);
    }
};

struct PCK_AUCTION_ASK_OWNER_IS_VIP_GP : public nsl::PACKET_HEADER
{
    int char_idx;                        // @18
    __int64 auction_id;                  // @22
    int buyer_id;                        // @30
    char buyer_name[13];                 // @34
    int price;                           // @47
    int item_id;                         // @51
};

#pragma pack(pop)

struct OwnerInfo
{
    bool is_vip;                         // @0
    int average_price;                   // @4

    OwnerInfo()
    {
        is_vip = false;
        average_price = 0xffffffff;
    }
};


#endif // NSL_AUCTIONPACKET_H_
