#ifndef AUCTION_AUCTIONITEM_H_
#define AUCTION_AUCTIONITEM_H_

// Rebuilt from df_auction_r DWARF layouts (auction item domain data), 2026-08-08
#include <string.h>

typedef unsigned long long __int64;

enum ENUM_RANDOM_OPTION_NUMBER
{
    ENUM_RANDOM_OPTION_FIRST = 0,
    ENUM_RANDOM_OPTION_SECOND = 1,
    ENUM_RANDOM_OPTION_THIRD = 2,
    ENUM_RANDOM_OPTION_MAX = 3,
};

struct RandomOptionField
{
    unsigned char option_index_;
    unsigned char first_value_;
    unsigned char second_value_;

    void reset()
    {
        option_index_ = '\0';
        first_value_ = '\0';
        second_value_ = '\0';
    }

    bool empty() const
    {
        return option_index_ == '\0';
    }
};

struct RandomOptionSeed
{
    union
    {
        struct
        {
            unsigned char option_give_type_ : 2;
            unsigned char seed_value_ : 6;
        };
        unsigned char seed_;
    };

    void reset()
    {
        seed_ = '\0';
    }
};

struct RandomOption
{
    RandomOptionField option_[ENUM_RANDOM_OPTION_MAX];
    RandomOptionSeed seed_;
    RandomOptionField modify_option_;
    RandomOptionSeed modify_seed_;

    void reset()
    {
        option_[0].reset();
        option_[1].reset();
        option_[2].reset();
        seed_.reset();
        modify_option_.reset();
        modify_seed_.reset();
    }

    bool change_option(ENUM_RANDOM_OPTION_NUMBER optionNumber)
    {
        // ORIG：直接读 2 位位域 option_give_type_（字节加载 + and $3 形态），
        // 而非 seed_ & 3（int 提升后 and，形态不同）。
        return (modify_option_.empty() == false) &&
               (modify_seed_.option_give_type_ == optionNumber);
    }

    unsigned char get_option_index(ENUM_RANDOM_OPTION_NUMBER optionNumber)
    {
        if (change_option(optionNumber))
        {
            return modify_option_.option_index_;
        }
        else
        {
            return option_[optionNumber].option_index_;
        }
    }

    unsigned char get_first_value(ENUM_RANDOM_OPTION_NUMBER optionNumber)
    {
        if (change_option(optionNumber))
        {
            return modify_option_.first_value_;
        }
        else
        {
            return option_[optionNumber].first_value_;
        }
    }

    unsigned char get_second_value(ENUM_RANDOM_OPTION_NUMBER optionNumber)
    {
        if (change_option(optionNumber))
        {
            return modify_option_.second_value_;
        }
        else
        {
            return option_[optionNumber].second_value_;
        }
    }
};

struct UpgradeSeparateInfo
{
    union
    {
        struct
        {
            unsigned char upgrade : 5;
            unsigned char tradeRestriction : 1;
            unsigned char others : 2;
        } bits;
    };

    UpgradeSeparateInfo()
    {
        reset();
    }

    void reset()
    {
        bits.upgrade = 0;
        bits.tradeRestriction = 0;
        bits.others = 0;
    }

    unsigned char GetUpgradeSeparate() const
    {
        return bits.upgrade;
    }

    void SetUpgradeSeparate(unsigned char nValue)
    {
        bits.upgrade = nValue;
    }
};

#pragma pack(push, 1)
struct ReservedCapacity
{
    int reserved_1[2];
    char reserved_2;

    ReservedCapacity()
    {
        reset();
    }

    void reset()
    {
        reserved_1[0] = 0;
        reserved_1[1] = 0;
        reserved_2 = '\0';
    }
};

struct itemGloballyUniqueIdentifier_t
{
    char data_[10];

    void reset()
    {
        memset(data_, 0, sizeof(data_));
    }
};

struct ROI_Category
{
    union
    {
        struct
        {
            short _high_category_value[3];
            short _reserved;
        } _hcv;
        // ORIG DWARF：成员名 _high_category_key，类型 signed long long
        long long _high_category_key;
    } field_0;

    union
    {
        struct
        {
            char _low_category_value[3];
        } _lcv;
        int _low_category_key;
    } field_1;

    ROI_Category()
    {
        field_0._high_category_key = 0;
        field_1._low_category_key = 0;
    }

    bool isEmpty() const
    {
        if ((field_0._high_category_key == 0) && (field_1._low_category_key == 0))
        {
            return true;
        }
        return false;
    }

    bool isMatching(const ROI_Category& _cmp) const
    {
        if (_cmp.isEmpty())
        {
            return false;
        }
        if (isEmpty())
        {
            return false;
        }
        ROI_Category cmp = _cmp;
        ROI_Category ori = *this;
        for (int i = 0; i < 3; i = i + 1)
        {
            if (cmp.field_0._hcv._high_category_value[i] != -1)
            {
                for (int j = 0; j < 3; j = j + 1)
                {
                    if (ori.field_0._hcv._high_category_value[j] == cmp.field_0._hcv._high_category_value[i])
                    {
                        cmp.field_0._hcv._high_category_value[i] = 0;
                        ori.field_0._hcv._high_category_value[j] = 0;
                        break;
                    }
                }
            }
        }
        for (int k = 0; k < 3; k = k + 1)
        {
            if (cmp.field_0._hcv._high_category_value[k] > 0)
            {
                return false;
            }
        }
        return true;
    }

    bool operator<(const ROI_Category& _rhp) const
    {
        // ORIG：64 位 == 合并比较（xor/or/test）+ 64 位有符号 <
        if (field_0._high_category_key == _rhp.field_0._high_category_key)
        {
            return field_1._low_category_key < _rhp.field_1._low_category_key;
        }
        else
        {
            return field_0._high_category_key < _rhp.field_0._high_category_key;
        }
    }

    void _sort()
    {
        // ORIG DWARF：_temp_high(2585)/_temp_low(2586) 函数级声明一次。
        short _temp_high;
        char _temp_low;
        if (field_0._hcv._high_category_value[0] > field_0._hcv._high_category_value[1])
        {
            _temp_high = field_0._hcv._high_category_value[0];
            field_0._hcv._high_category_value[0] = field_0._hcv._high_category_value[1];
            field_0._hcv._high_category_value[1] = _temp_high;
            _temp_low = field_1._lcv._low_category_value[0];
            field_1._lcv._low_category_value[0] = field_1._lcv._low_category_value[1];
            field_1._lcv._low_category_value[1] = _temp_low;
        }
        if (field_0._hcv._high_category_value[1] > field_0._hcv._high_category_value[2])
        {
            _temp_high = field_0._hcv._high_category_value[1];
            field_0._hcv._high_category_value[1] = field_0._hcv._high_category_value[2];
            field_0._hcv._high_category_value[2] = _temp_high;
            _temp_low = field_1._lcv._low_category_value[1];
            field_1._lcv._low_category_value[1] = field_1._lcv._low_category_value[2];
            field_1._lcv._low_category_value[2] = _temp_low;
        }
        if (field_0._hcv._high_category_value[0] > field_0._hcv._high_category_value[1])
        {
            _temp_high = field_0._hcv._high_category_value[0];
            field_0._hcv._high_category_value[0] = field_0._hcv._high_category_value[1];
            field_0._hcv._high_category_value[1] = _temp_high;
            _temp_low = field_1._lcv._low_category_value[0];
            field_1._lcv._low_category_value[0] = field_1._lcv._low_category_value[1];
            field_1._lcv._low_category_value[1] = _temp_low;
        }
    }
};
#pragma pack(pop)

#pragma pack(push, 1)
struct DnfItemInfo
{
    bool seal;
    unsigned long item_id;
    union
    {
        struct
        {
            unsigned char btUpgrade : 5;
            unsigned char btSealCount : 3;
        };
        unsigned char uniItemAttr;
    };
    int add_info;
    unsigned short endurance;
    unsigned long extendInfo;
    unsigned char abilityType_;
    unsigned short abilityValue_;
    itemGloballyUniqueIdentifier_t guid_;
    RandomOption random_option_;
    UpgradeSeparateInfo separate_info;
    ReservedCapacity reserved_capacity;

    DnfItemInfo()
    {
        reset();
    }

    void reset()
    {
        seal = false;
        item_id = 0;
        uniItemAttr = '\0';
        add_info = 0;
        endurance = 0;
        extendInfo = 0;
        abilityType_ = '\0';
        abilityValue_ = 0;
        random_option_.reset();
        separate_info.reset();
        reserved_capacity.reset();
    }

    unsigned long GetItemId() const
    {
        return item_id;
    }

    unsigned char GetUpgradeValue() const
    {
        return uniItemAttr & 0x1f;
    }

    unsigned char getAbilityType() const
    {
        // ORIG：!hasAbility/!isIdentified 各自 xor $1 物化 + 早退块（jmp EPI）。
        if (!hasAbility())
        {
            return '\0';
        }
        if (!isIdentified())
        {
            return 0x80;
        }
        return abilityType_;
    }

    unsigned short getAbilityValue() const
    {
        // ORIG：!(hasAbility() && isIdentified()) 单 bool 寄存器物化 +
        // 再次 test 后分支（与 getAbilityType 的双 if 形态不同，照抄）。
        if (!(hasAbility() && isIdentified()))
        {
            return 0;
        }
        return abilityValue_;
    }

    bool isIdentified() const
    {
        // ORIG：not %eax; shr $0x7,%al（8 位逻辑移位直返）。
        // 不加 & 1 会被提升为 int 后 sar + test/setne（多 3 条）。
        return (bool)((unsigned char)~abilityType_ >> 7 & 1);
    }

    bool hasAbility() const
    {
        return abilityType_ != '\0';
    }
};
#pragma pack(pop)

// Avatar emblem/expansion info (auction binary DWARF, decl lines 7380..7420)
typedef unsigned short avatarEmblemType_t;

#pragma pack(push, 1)
struct stAvatarEmblemInfo_t
{
    struct
    {
        unsigned short emblemSocketType_;   // @0
        int emblemItemIdx_;                 // @2
    } stEmblemSocket[5];                    // @0, total 30B

    void init()
    {
        memset(this, 0, sizeof(stAvatarEmblemInfo_t));
    }
};

#pragma pack(pop)

struct stAvatarExpansionInfo_t
{
    short color[2];                     // @0, total 4B

    void init()
    {
        memset(color, 0, sizeof(color));
    }

    bool operator==(const stAvatarExpansionInfo_t& _rhp) const
    {
        return color[0] == _rhp.color[0] && color[1] == _rhp.color[1];
    }
};

#endif // AUCTION_AUCTIONITEM_H_
