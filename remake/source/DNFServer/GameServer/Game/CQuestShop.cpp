// df_game_r CQuestShop::clearQP 恢复。
// 依据 docs/class_func_reports/CQuestShop.md 的反汇编与反编译报告。
#include "CUser.h"
#include "CQuestShop.h"
#include "PacketGuard.h"
#include "CDataManager.h"

int CQuestShop::getQp() const { return m_qp; }
int CQuestShop::getInitQp() const { return m_initQp; }
int CQuestShop::getInitCount() const { return m_initCount; }
int CQuestShop::getPiece() const { return m_piece; }

void CQuestShop::setQp(int value) { alter(); m_qp = value; }
void CQuestShop::setInitQp(int value) { m_initQp = value; }
void CQuestShop::setInitCount(int value) { m_initCount = value; }
void CQuestShop::setPiece(int value) { m_piece = value; }
void CQuestShop::addQp(int value) { m_qp += value; }
void CQuestShop::subQp(int value) { m_qp -= value; }
void CQuestShop::incInitQp(int value) { m_initQp += value; }
void CQuestShop::incInitCount() { ++m_initCount; }
void CQuestShop::addPiece(int value) { m_piece += value; }

void CQuestShop::clearBuyingStatuslist()
{
    alter();
    m_buyingStatus.clear();
}

void CQuestShop::sendCharacQp(CUser* user, char flag)
{
    PacketGuard packet;
    packet.put_header(0, 0x15c);
    packet.put_int(getQp());
    packet.put_short(getInitCount());
    packet.put_byte(static_cast<int>(flag));
    packet.finalize(true);
    user->Send(packet);
}

void CQuestShop::clearQP(CUser* user)
{
    const int oldQp = getQp();
    user->setInitQuestPoint();
    setQp(getInitQp());
    clearBuyingStatuslist();
    cUserHistoryLog* history = reinterpret_cast<cUserHistoryLog*>(
        reinterpret_cast<char*>(user) + 0x79700);
    history->ClearUsedQP(oldQp, getQp(), getInitCount(), 0);
}

void CQuestShop::makePacket_BuyingStatusList(PacketGuard& packet, bool flag)
{
    const std::map<unsigned char, short>& status =
        flag ? m_tagBuyingStatus : m_buyingStatus;
    packet.put_byte(static_cast<int>(status.size()));

    for (std::map<unsigned char, short>::const_iterator it = status.begin();
         it != status.end(); ++it)
    {
        int itemStatus = -1;
        const unsigned int itemIndex = static_cast<unsigned char>(it->first);
        int count = static_cast<int>(it->second);
        if (itemIndex < 0x4d)
        {
            const unsigned char* table = reinterpret_cast<const unsigned char*>(
                reinterpret_cast<const char*>(G_CDataManager()) + 0x63c0);
            itemStatus = static_cast<int>(static_cast<signed char>(table[itemIndex]));
        }
        else
            count = 0;

        packet.put_byte(itemStatus);
        packet.put_short(count);
    }
}
