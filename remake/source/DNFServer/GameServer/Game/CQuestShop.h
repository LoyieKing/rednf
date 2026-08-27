#ifndef GAME_CQUESTSHOP_H_
#define GAME_CQUESTSHOP_H_

#include <map>
#include "CBoosterGage.h"

class CUser;
class PacketGuard;
struct CHARAC_QUEST_SHOP_DATA;

class CQuestShop : public charac_expand::CData
{
public:
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

    // ---- ORIG QuestShop 序列化块视图（ctor/getData 0x85ef868 / _saveData
    // 0x85efb66 / loadData 0x85efa2c 反汇编实证；单实例 0xaa、双实例 +0x154 flag）：
    //   +0x000 characNo / +0x004 qp / +0x008 initCount /
    //   +0x00c..+0x0a5 unsigned short[0x4d] statusCounts /
    //   +0x0a6 questPiece(unsigned int)。
    // DB_LoadEtc.h 的 CHARAC_QUEST_SHOP_DATA 对同区间另有一套具名拆分；
    // 本 TU 按 QuestShop 序列化语义以本视图访问，不改共享权威头。
    struct SigQuestShopView
    {
        unsigned int m_characNo;               // +0x00
        unsigned int m_qp;                     // +0x04
        unsigned int m_initCount;              // +0x08
        unsigned short m_statusCounts[0x4d];   // +0x0c
        unsigned int m_questPiece;             // +0xa6
    };

    virtual void _reset();                     // ORIG T 0x85efd74
    virtual bool loadData(CUser* user, char* data);          // ORIG T 0x85efa2c
    virtual bool _saveData(CUser* user);                     // ORIG T 0x85efb66
    virtual void getData(char* out) const;                   // ORIG T 0x85ef868
    void subQp(unsigned char status, unsigned char count);   // ORIG T 0x85eed2a
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

#endif
