#ifndef NSL_DBTRANSACTIONDESIGN_H_
#define NSL_DBTRANSACTIONDESIGN_H_

#include <string.h>

#include "Thread.h"
#include "AuctionItem.h"

typedef unsigned long long __int64;

namespace nsl {

#pragma pack(push, 1)
struct DBTR_HEADER
{
    int dbId;
    int dbtrId;
    int userid;
    bool mbWillDelete;
    int mOwnerWorkId;
    unsigned int sLength;

    DBTR_HEADER()
    {
    }

    int getSize()
    {
        return sLength;
    }

    void setSize(int n)
    {
        sLength = n;
    }

    char* _getData()
    {
        return (char*)this + sizeof(DBTR_HEADER);
    }

    DBTR_HEADER* _getHeader()
    {
        return this;
    }
};

#pragma pack(pop)

typedef DBTR_HEADER* LPDBTR_HEADER;

} // namespace nsl

#pragma pack(push, 1)
struct tagAUCTION_DB_PING : public nsl::DBTR_HEADER
{
    tagAUCTION_DB_PING()
    {
        memset(this, 0, sizeof(tagAUCTION_DB_PING));
        dbId = 1;
        dbtrId = 9;
        mOwnerWorkId = nsl::tlsThreadId;
        mbWillDelete = false;
        setSize(sizeof(tagAUCTION_DB_PING));
    }
};

struct tagAUCTION_DB_GET_AVERAGE_PRICE : public nsl::DBTR_HEADER
{
    unsigned long item_id;
    unsigned char upgrade;
    int average_price;
    ROI_Category ROI_Key;

    tagAUCTION_DB_GET_AVERAGE_PRICE()
    {
        memset(this, 0, sizeof(tagAUCTION_DB_GET_AVERAGE_PRICE));
        dbId = 1;
        dbtrId = 4;
        mOwnerWorkId = nsl::tlsThreadId;
        mbWillDelete = false;
        setSize(sizeof(tagAUCTION_DB_GET_AVERAGE_PRICE));
    }
};

struct tagAUCTION_DB_GET_ROI_AVERAGE_PRICE_INFO : public nsl::DBTR_HEADER
{
    tagAUCTION_DB_GET_ROI_AVERAGE_PRICE_INFO()
    {
        memset(this, 0, sizeof(tagAUCTION_DB_GET_ROI_AVERAGE_PRICE_INFO));
        dbId = 1;
        dbtrId = 0x12;
        mOwnerWorkId = nsl::tlsThreadId;
        mbWillDelete = false;
        setSize(sizeof(tagAUCTION_DB_GET_ROI_AVERAGE_PRICE_INFO));
    }
};

struct tagAUCTION_DB_GET_REGISTED_ITEM : public nsl::DBTR_HEADER
{
    __int64 auction_id;
    long expire_time;
    int owner_id;
    char owner_name[13];
    int buyer_id;
    char buyer_name[13];
    int price;
    int instant_price;
    DnfItemInfo item_info;
    unsigned char owner_type;
    unsigned int black_point;
    int unit_price;
    ROI_Category _reg_roi_category_key;
    char owner_nexon_id[32];

    tagAUCTION_DB_GET_REGISTED_ITEM()
    {
        memset(this, 0, sizeof(tagAUCTION_DB_GET_REGISTED_ITEM));
        dbId = 1;
        dbtrId = 0;
        mOwnerWorkId = nsl::tlsThreadId;
        mbWillDelete = false;
        setSize(sizeof(tagAUCTION_DB_GET_REGISTED_ITEM));
    }
};

struct tagAUCTION_DB_REGIST_ITEM : public nsl::DBTR_HEADER
{
    __int64 auction_id;              // @21
    long expire_time;                // @29
    int owner_id;                    // @33
    char owner_name[13];             // @37
    int buyer_id;                    // @50
    char buyer_name[13];             // @54
    int price;                       // @67
    int instant_price;               // @71
    DnfItemInfo item_info;           // @75
    unsigned char owner_type;        // @128
    unsigned int black_point;        // @129
    int unit_price;                  // @133
    ROI_Category _reg_roi_category_key;  // @137
    char owner_nexon_id[32];         // @149

    tagAUCTION_DB_REGIST_ITEM()
    {
        memset(this, 0, sizeof(tagAUCTION_DB_REGIST_ITEM));
        dbId = 1;
        dbtrId = 1;
        mOwnerWorkId = nsl::tlsThreadId;
        mbWillDelete = false;
        setSize(sizeof(tagAUCTION_DB_REGIST_ITEM));
    }
};

// ORIG：ROI_AverageKey/ROI_Average_Constraint 位于 pack 区之外（自然对齐，
// long long 成员使 ROI_AverageKey 对齐 8，局部栈槽 8 对齐，如
// onAUCTION_DB_GET_AVERAGE_PRICE 的 _temp_roi_average@-0x48）；
// 仅 DBTR 报文结构保持 pack(1)。2026-08-11 复检修正。
#pragma pack(pop)
struct ROI_AverageKey
{
    unsigned long baseItem_index;              // @0
    ROI_Category option_category;              // @4
    union
    {
        // ORIG DWARF：option_index_key 是 signed long long（非 __int64 typedef）
        long long option_index_key;            // @16
        struct
        {
            short option_index_value[3];
        } _oiv;
    };

    ROI_AverageKey()
    {
        baseItem_index = 0;
        option_index_key = 0;
    }

    bool operator<(const ROI_AverageKey& _rhp) const
    {
        if (baseItem_index == _rhp.baseItem_index)
        {
            // ORIG：64 位比较为有符号（高字 jl/jg + 低字 jb），不要无符号强转
            if (option_index_key == _rhp.option_index_key)
            {
                return option_category < _rhp.option_category;
            }
            return option_index_key < _rhp.option_index_key;
        }
        return baseItem_index < _rhp.baseItem_index;
    }
};

struct ROI_Average_Constraint
{
    int inf_max_price;         // @0
    int inf_min_price;         // @4
    int inf_prob;              // @8
    int inf_limit_count;       // @12
    int inf_base_mul_min_a;    // @16
    int inf_base_mul_max_b;    // @20

    ROI_Average_Constraint()
    {
        inf_max_price = 0xa;
        inf_min_price = 0x154;
        inf_prob = 0x14;
        inf_limit_count = 0x5;
        inf_base_mul_min_a = 0x96;
        inf_base_mul_max_b = 0xfa;
    }

    bool isVaildRange() const;
};

#pragma pack(push, 1)
struct tagAUCTION_DB_UPPER_BIDDING : public nsl::DBTR_HEADER
{
    __int64 auction_id;      // @21
    int buyer_id;            // @29
    char buyer_name[13];     // @33
    int price;               // @46

    tagAUCTION_DB_UPPER_BIDDING()
    {
        memset(this, 0, sizeof(tagAUCTION_DB_UPPER_BIDDING));
        dbId = 1;
        dbtrId = 2;
        mOwnerWorkId = nsl::tlsThreadId;
        mbWillDelete = false;
        setSize(sizeof(tagAUCTION_DB_UPPER_BIDDING));
    }
};

struct tagAUCTION_DB_DELETE_ITEM : public nsl::DBTR_HEADER
{
    __int64 auction_id;      // @21

    tagAUCTION_DB_DELETE_ITEM()
    {
        memset(this, 0, sizeof(tagAUCTION_DB_DELETE_ITEM));
        dbId = 1;
        dbtrId = 6;
        mOwnerWorkId = nsl::tlsThreadId;
        mbWillDelete = false;
        setSize(sizeof(tagAUCTION_DB_DELETE_ITEM));
    }
};

struct tagAUCTION_DB_UPDATE_ITEM : public nsl::DBTR_HEADER
{
    __int64 auction_id;      // @21
    int instant_price;       // @29
    int add_info;            // @33

    tagAUCTION_DB_UPDATE_ITEM()
    {
        memset(this, 0, sizeof(tagAUCTION_DB_UPDATE_ITEM));
        dbId = 1;
        dbtrId = 0x11;
        mOwnerWorkId = nsl::tlsThreadId;
        mbWillDelete = false;
        setSize(sizeof(tagAUCTION_DB_UPDATE_ITEM));
    }
};

struct tagAUCTION_DB_BUYER_HISTORY : public nsl::DBTR_HEADER
{
    __int64 auction_id;          // @21
    int pre_buyer_id;            // @29
    int buyer_id;                // @33
    int pre_price;               // @37
    int price;                   // @41
    unsigned int pre_buyer_postal_id;  // @45

    tagAUCTION_DB_BUYER_HISTORY()
    {
        memset(this, 0, sizeof(tagAUCTION_DB_BUYER_HISTORY));
        dbId = 1;
        dbtrId = 8;
        mOwnerWorkId = nsl::tlsThreadId;
        mbWillDelete = false;
        setSize(sizeof(tagAUCTION_DB_BUYER_HISTORY));
    }
};

struct tagAUCTION_DB_EXPIRE_HISTORY : public nsl::DBTR_HEADER
{
    __int64 auction_id;               // @21
    long expire_time;                 // @29
    unsigned char event_type;         // @33
    int owner_id;                     // @34
    int buyer_id;                     // @38
    int price;                        // @42
    DnfItemInfo item_info;            // @46
    unsigned int owner_postal_id;     // @99
    unsigned int buyer_postal_id;     // @103
    int unit_price;                   // @107
    ROI_Category _reg_roi_category_key;  // @111
    int commission;                   // @123
    unsigned char owner_type;         // @127

    tagAUCTION_DB_EXPIRE_HISTORY()
    {
        memset(this, 0, sizeof(tagAUCTION_DB_EXPIRE_HISTORY));
        dbId = 1;
        dbtrId = 7;
        mOwnerWorkId = nsl::tlsThreadId;
        mbWillDelete = false;
        setSize(sizeof(tagAUCTION_DB_EXPIRE_HISTORY));
    }
};

struct tagAUCTION_DB_INSERT_AVERAGE_PRICE : public nsl::DBTR_HEADER
{
    unsigned long item_id;               // @21
    unsigned char upgrade;               // @25
    int average_price;                   // @26
    ROI_AverageKey _roi_average_key;     // @30
    unsigned char seperate_upgrade;      // @54

    tagAUCTION_DB_INSERT_AVERAGE_PRICE()
    {
        memset(this, 0, sizeof(tagAUCTION_DB_INSERT_AVERAGE_PRICE));
        dbId = 1;
        dbtrId = 3;
        mOwnerWorkId = nsl::tlsThreadId;
        mbWillDelete = false;
        setSize(sizeof(tagAUCTION_DB_INSERT_AVERAGE_PRICE));
    }
};

struct tagAUCTION_DB_UPDATE_AVERAGE_PRICE : public nsl::DBTR_HEADER
{
    unsigned long item_id;               // @21
    unsigned char upgrade;               // @25
    int average_price;                   // @26
    ROI_AverageKey _roi_average_key;     // @30
    unsigned int purchase_cnt;           // @54
    unsigned char seperate_upgrade;      // @58

    tagAUCTION_DB_UPDATE_AVERAGE_PRICE()
    {
        memset(this, 0, sizeof(tagAUCTION_DB_UPDATE_AVERAGE_PRICE));
        dbId = 1;
        dbtrId = 5;
        mOwnerWorkId = nsl::tlsThreadId;
        mbWillDelete = false;
        setSize(sizeof(tagAUCTION_DB_UPDATE_AVERAGE_PRICE));
    }
};

struct tagGAME_DB_SEND_PACKAGE : public nsl::DBTR_HEADER
{
    __int64 auction_id;                  // @21
    int owner_id;                        // @29
    unsigned int receiver;               // @33
    unsigned int money;                  // @37
    DnfItemInfo item_info;               // @41
    unsigned short letter_text_length;   // @94
    char letter_text[256];               // @96
    unsigned long temp_item_id;          // @352

    tagGAME_DB_SEND_PACKAGE()
    {
        memset(this, 0, sizeof(tagGAME_DB_SEND_PACKAGE));
        dbId = 0;
        dbtrId = 0x15;
        mOwnerWorkId = nsl::tlsThreadId;
        mbWillDelete = false;
        setSize(sizeof(tagGAME_DB_SEND_PACKAGE));
    }
};

struct tagGAME_DB_SEND_PACKAGE_BY_EXPIRE : public nsl::DBTR_HEADER
{
    unsigned char package_type;                  // @21
    bool b_exist_buyer;                          // @22
    tagGAME_DB_SEND_PACKAGE send_to_owner;       // @23
    tagGAME_DB_SEND_PACKAGE send_to_buyer;       // @379

    tagGAME_DB_SEND_PACKAGE_BY_EXPIRE()
    {
        memset(this, 0, sizeof(tagGAME_DB_SEND_PACKAGE_BY_EXPIRE));
        dbId = 0;
        dbtrId = 0x16;
        mOwnerWorkId = nsl::tlsThreadId;
        mbWillDelete = false;
        setSize(sizeof(tagGAME_DB_SEND_PACKAGE_BY_EXPIRE));
    }
};

#pragma pack(pop)

typedef tagAUCTION_DB_GET_REGISTED_ITEM* LPDBTR_AUCTION_DB_GET_REGISTED_ITEM;
typedef tagAUCTION_DB_REGIST_ITEM* LPDBTR_AUCTION_DB_REGIST_ITEM;
typedef tagAUCTION_DB_UPPER_BIDDING* LPDBTR_AUCTION_DB_UPPER_BIDDING;
typedef tagAUCTION_DB_DELETE_ITEM* LPDBTR_AUCTION_DB_DELETE_ITEM;
typedef tagAUCTION_DB_UPDATE_ITEM* LPDBTR_AUCTION_DB_UPDATE_ITEM;
typedef tagAUCTION_DB_BUYER_HISTORY* LPDBTR_AUCTION_DB_BUYER_HISTORY;
typedef tagAUCTION_DB_EXPIRE_HISTORY* LPDBTR_AUCTION_DB_EXPIRE_HISTORY;
typedef tagAUCTION_DB_INSERT_AVERAGE_PRICE* LPDBTR_AUCTION_DB_INSERT_AVERAGE_PRICE;
typedef tagAUCTION_DB_UPDATE_AVERAGE_PRICE* LPDBTR_AUCTION_DB_UPDATE_AVERAGE_PRICE;
typedef tagAUCTION_DB_GET_AVERAGE_PRICE* LPDBTR_AUCTION_DB_GET_AVERAGE_PRICE;
typedef tagGAME_DB_SEND_PACKAGE* LPDBTR_GAME_DB_SEND_PACKAGE;
typedef tagGAME_DB_SEND_PACKAGE_BY_EXPIRE* LPDBTR_GAME_DB_SEND_PACKAGE_BY_EXPIRE;

#endif // NSL_DBTRANSACTIONDESIGN_H_
