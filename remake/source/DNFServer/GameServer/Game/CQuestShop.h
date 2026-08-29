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
    // 主实例 0xaa / tag 实例 0xaa / 尾随 1 字节 flag @0x154；sizeof 0x155。
    // SigQuestShopLoadBlock 已并入 SIG_QUEST_SHOP_DATA（SigTypes_D.h 0x155 双实例
    // 权威定义），此处保留 typedef 兼容既有使用点。
    typedef SIG_QUEST_SHOP_DATA SigQuestShopLoadBlock;

    virtual void _reset();                     // ORIG T 0x85efd74
    virtual bool loadData(CUser* user, char* data);          // ORIG T 0x85efa2c
    virtual bool _saveData(CUser* user);                     // ORIG T 0x85efb66
    virtual void getData(char* out) const;                   // ORIG T 0x85ef868
    int subQp(unsigned char status, unsigned char count);    // ORIG T 0x85eed2a
    int calUsedQp();                                         // ORIG T 0x85ef214
    int clearUsedQp(CUser* user);                           // ORIG T 0x85ef426（ORIG int 返回，0xa 完整错误码）
    int getInitCost(int count);                              // ORIG T 0x85ef1ba
    int findStatusQp(unsigned char status);                  // ORIG T 0x85eeda8
    int findStatusType(unsigned char status);                // ORIG T 0x85eee6a
    short getStatusBuyingCount(unsigned char status);        // ORIG T 0x85ef02e（ORIG 16 位返回，调用处 cwtl）
    void incStatusBuyingCount(unsigned char status, unsigned char count); // ORIG T 0x85eef28
    bool checkBuyingStatus_LimitLevel(unsigned char status, short count,
                                      int characLevel);      // ORIG T 0x85ef0ce
    bool SetTagCharacQuestShopData(CHARAC_QUEST_SHOP_DATA* data); // ORIG T 0x85efdc0
    int buyStatus(CUser* user, unsigned char status,
                  unsigned char count, short& result);      // ORIG T 0x85ef306（ORIG int 返回，0xe 完整错误码）
private:
    std::map<unsigned char, short> m_buyingStatus; // +0x08
    std::map<unsigned char, short> m_tagBuyingStatus; // +0x20
    int m_qp; // +0x38
    int m_initCount; // +0x3c（ORIG getInitCount/setInitCount/incInitCount 访问）
    int m_initQp; // +0x40（ORIG getInitQp/setInitQp/incInitQp 访问）
    int m_piece; // +0x44
};

static_assert(sizeof(CQuestShop::SigQuestShopLoadBlock) == 0x155,
              "SigQuestShopLoadBlock ABI drift");

#endif

