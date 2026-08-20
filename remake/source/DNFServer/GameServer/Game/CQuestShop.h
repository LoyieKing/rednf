#ifndef GAME_CQUESTSHOP_H_
#define GAME_CQUESTSHOP_H_

#include <map>
#include "CBoosterGage.h"

class CUser;
class PacketGuard;

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
    void clearQP(CUser* user);
    void makePacket_BuyingStatusList(PacketGuard& packet, bool flag);

private:
    std::map<unsigned char, short> m_buyingStatus; // +0x08
    std::map<unsigned char, short> m_tagBuyingStatus; // +0x20
    int m_qp; // +0x38
    int m_initQp; // +0x3c
    int m_initCount; // +0x40
    int m_piece; // +0x44
};

#endif
