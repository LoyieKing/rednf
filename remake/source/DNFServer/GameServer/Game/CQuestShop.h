#ifndef GAME_CQUESTSHOP_H_
#define GAME_CQUESTSHOP_H_

#include <map>
#include "CBoosterGage.h"
#include "SigTypes.h"   // SIG_QUEST_SHOP_DATA（本头 LOAD_BLOCK 需完整类型）

class CUser;
struct CHARAC_QUEST_SHOP_DATA;
class CQuestShop : public charac_expand::CData
{
public:
    CQuestShop();                    // ORIG C1/C2 0x85eebc8
    virtual ~CQuestShop();           // ORIG D1/D2 0x85eec60 / D0 0x85eecea
    int getQp() const;
    int getInitQp() const;
    int getInitCount() const;
    int getPiece() const;
    void setQp(int value);
    void setInitQp(int value);
    void setInitCount(int value);
    void setPiece(int value);
    void addQp(int value);
    void subQp(int value);
    void incInitQp(int value);
    void incInitCount();
    void addPiece(int value);
    void clearBuyingStatuslist();
    void sendCharacQp(CUser* user, char flag);
    void sendCharacQuestPiece(CUser* user);  // ORIG 0x85ef7cc
    void clearQP(CUser* user);
    void makePacket_BuyingStatusList(PacketGuard& packet, bool flag);
    // ---- ORIG loadData 双实例加载区（SIG_LOAD_ETC [0xd5a7..)：
    // 主实例 0xaa / tag 实例 0xaa / 尾随 1 字节 flag @0x154；
    // loadData 0x85efa2c 反汇编实证。pack(1) 紧排，sizeof 锁定 0x155。
#pragma pack(push, 1)
    struct SigQuestShopLoadBlock
    {
        SIG_QUEST_SHOP_DATA m_main;      // +0x000
        SIG_QUEST_SHOP_DATA m_tag;       // +0x0aa
        unsigned char m_tagFlag;         // +0x154
    };
#pragma pack(pop)

    virtual void _reset();                     // ORIG T 0x85efd74
    virtual bool loadData(CUser* user, char* data);          // ORIG T 0x85efa2c
    virtual bool _saveData(CUser* user);                     // ORIG T 0x85efb66
    virtual void getData(char* out) const;                   // ORIG T 0x85ef868
    int subQp(unsigned char status, unsigned char count);    // ORIG T 0x85eed2a
    int calUsedQp();                                         // ORIG T 0x85ef214
    bool clearUsedQp(CUser* user);                           // ORIG T 0x85ef426
    int getInitCost(int count);                              // ORIG T 0x85ef1ba
    int findStatusQp(unsigned char status);                  // ORIG T 0x85eeda8
    int findStatusType(unsigned char status);                // ORIG T 0x85eee6a
    int getStatusBuyingCount(unsigned char status);          // ORIG T 0x85ef02e
    void incStatusBuyingCount(unsigned char status, unsigned char count); // ORIG T 0x85eef28
    bool checkBuyingStatus_LimitLevel(unsigned char status, short count,
                                      int characLevel);      // ORIG T 0x85ef0ce
    bool SetTagCharacQuestShopData(CHARAC_QUEST_SHOP_DATA* data); // ORIG T 0x85efdc0
    bool buyStatus(CUser* user, unsigned char status,
                   unsigned char count, short& result);      // ORIG T 0x85ef306

private:
    std::map<unsigned char, short> m_buyingStatus; // +0x08
    std::map<unsigned char, short> m_tagBuyingStatus; // +0x20
    int m_qp; // +0x38
    int m_initQp; // +0x3c
    int m_initCount; // +0x40
    int m_piece; // +0x44
};

static_assert(sizeof(CQuestShop::SigQuestShopLoadBlock) == 0x155,
              "SigQuestShopLoadBlock ABI drift");

#endif

