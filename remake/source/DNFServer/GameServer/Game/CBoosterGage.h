#ifndef GAME_CBOOSTERGAGE_H_
#define GAME_CBOOSTERGAGE_H_

// ============================================================================
// df_game_r 还原 —— CBoosterGage（G1 批次）
// 布局依据 CBoosterGage.md / CDataMgr.md / ORIG 反汇编推导：
//   charac_expand::CData（pack(1)，vptr +0x0 / char +0x4，sizeof 5）
//   CBoosterGage（非 packed）：+0x8 int m_dwTotalGage（sizeof 0xc）
// BoosterGageData：vector<BoosterPoint> +0x0 / char +0xc（最大值）/
//                  char +0xd / char +0xe（多开数量），sizeof 0x10
// 虚表槽位（CData）：ResetDailyMidnight @0 / loadData @1 / getData @2 /
//   ResetDaily @3 / ~CData @4,5 / _saveData @6 / _reset @7
// ============================================================================

#include <utility>
#include <vector>

class Inven_Item;
class CUser;
class CUserCharacInfo;
class CDataManager;
class CItem;

#ifndef DNF_ENUM_EITEMADDREASON_DEFINED
#define DNF_ENUM_EITEMADDREASON_DEFINED
// ---- 枚举（值以 ORIG 立即数为准；与 CUser.h 同名枚举守卫合并） ----
enum eItemAddReason
{
    eItemAddReason_0 = 0,
    eItemAddReason_1 = 1,
    eItemAddReason_2 = 2,
    eItemAddReason_7 = 7,
    eItemAddReason_8 = 8,
    eItemAddReason_0x3a = 0x3a
};
#endif

#ifndef DNF_ENUM_EAVATARITEMADDREASON_DEFINED
#define DNF_ENUM_EAVATARITEMADDREASON_DEFINED
enum eAvatarItemAddReason
{
    eAvatarItemAddReason_0 = 0,
    eAvatarItemAddReason_0x14 = 0x14,
    eAvatarItemAddReason_0x16 = 0x16
};
#endif

#ifndef DNF_ENUM_CHARAC_EXPAND_TYPE_DEFINED
#define DNF_ENUM_CHARAC_EXPAND_TYPE_DEFINED
enum ENUM_CHARAC_EXPAND_TYPE
{
    ENUM_CHARAC_EXPAND_TYPE_0 = 0,
    ENUM_CHARAC_EXPAND_TYPE_1 = 1,
    ENUM_CHARAC_EXPAND_TYPE_2 = 2,
    ENUM_CHARAC_EXPAND_TYPE_3 = 3,
    ENUM_CHARAC_EXPAND_TYPE_4 = 4,
    ENUM_CHARAC_EXPAND_TYPE_5 = 5,
    ENUM_CHARAC_EXPAND_TYPE_6 = 6,
    ENUM_CHARAC_EXPAND_TYPE_7 = 7,
    ENUM_CHARAC_EXPAND_TYPE_8 = 8,
    ENUM_CHARAC_EXPAND_TYPE_9 = 9,
    ENUM_CHARAC_EXPAND_TYPE_10 = 10,
    ENUM_CHARAC_EXPAND_TYPE_11 = 11,
    ENUM_CHARAC_EXPAND_TYPE_12 = 12,
    ENUM_CHARAC_EXPAND_TYPE_13 = 13,
    ENUM_CHARAC_EXPAND_TYPE_14 = 14,
    ENUM_CHARAC_EXPAND_TYPE_15 = 15,
    ENUM_CHARAC_EXPAND_TYPE_16 = 16,
    ENUM_CHARAC_EXPAND_TYPE_17 = 17,
    ENUM_CHARAC_EXPAND_TYPE_18 = 18,
    ENUM_CHARAC_EXPAND_TYPE_19 = 19
};
#endif

#pragma pack(push, 1)

namespace charac_expand
{
class CData
{
public:
    virtual void ResetDailyMidnight() {}
    virtual bool loadData(CUser* pUser, char* pData) = 0;
    virtual void getData(char* pData) const {}
    virtual void ResetDaily() {}
    virtual ~CData();
    virtual bool _saveData(CUser* pUser) = 0;
    virtual void _reset() = 0;

    void alter();
    bool isSave() const;
    bool saveData(CUser* pUser);
    void reset();

protected:
    char m_bIsSave;  // +0x04
};
}

#pragma pack(pop)

struct BoosterPoint
{
    unsigned long m_dwItemIdx;  // +0x00
    unsigned char m_cPoint;     // +0x04
};

#ifndef DNF_BOOSTERGAGEDATA_DEFINED
#define DNF_BOOSTERGAGEDATA_DEFINED
class BoosterGageData
{
public:
    BoosterGageData();
    ~BoosterGageData();

    char getBoosterPoint(unsigned long dwItemIdx)
    {
        for (std::vector<BoosterPoint>::const_iterator it = m_BoosterPoint.begin();
             it != m_BoosterPoint.end(); ++it)
        {
            if (it->m_dwItemIdx == dwItemIdx)
                return it->m_cPoint;
        }
        return 0;
    }

    std::vector<BoosterPoint> m_BoosterPoint;  // +0x00（0x0c）
    unsigned char m_cMaxBoosterGage;           // +0x0c
    unsigned char m_cFieldD;                   // +0x0d
    unsigned char m_cMultiboxCount;            // +0x0e
};
#endif

class CBoosterGage : public charac_expand::CData
{
public:
    CBoosterGage();
    virtual ~CBoosterGage();

    virtual bool loadData(CUser* pUser, char* pData);
    virtual void getData(char* pData) const;
    virtual void ResetDaily();
    virtual bool _saveData(CUser* pUser);
    virtual void _reset();

    void inc_total_gage(int nGage);
    void sub_total_gage(int nGage);
    int get_booster_point(unsigned long dwItemIdx);
    static int cal_booster_gage(unsigned long dwItemIdx, CUser* pUser);
    unsigned char check_max_booster_gage(unsigned long dwItemIdx);
    void send_error_state_packet(CUser* pUser, unsigned long dwItemIdx);
    void send_data(CUser* pUser, int nParam);
    void make_postar(CUser* pUser, const Inven_Item& item, const char* pAgency, int nPeriod);
    void log_booster_gage_bonus_reward(
        CUser* pUser,
        const std::vector<std::pair<Inven_Item, bool> >& rewardList);
    int AddAvatarItem(std::vector<std::pair<int, int> >& itemList, bool bIsFirst,
                      CUser* pUser, const Inven_Item& item, int nCount, bool bFlag,
                      char cType, char cParam, const char* pAgency,
                      eAvatarItemAddReason eReason, int& nRemainDate);
    int InsertCreatureItem(std::vector<std::pair<int, int> >& itemList, bool bIsFirst,
                           CUser* pUser, const Inven_Item& item,
                           eItemAddReason eReason, bool bLog, bool bParam,
                           int nA, int nB, const char* pMailTitle);
    int insertItemIntoInventory(std::vector<std::pair<int, int> >& itemList,
                                bool bIsFirst, CUser* pUser, const Inven_Item& item,
                                eItemAddReason eReason, bool bLog);

private:
    int m_dwTotalGage;  // +0x08
};

#endif  // GAME_CBOOSTERGAGE_H_
