#ifndef DNF_GAME_CAUCTION_AVERAGE_PRICE_H_
#define DNF_GAME_CAUCTION_AVERAGE_PRICE_H_

#include <map>
#include <utility>
#include <vector>

class CAuctionAveragePrice
{
public:
    CAuctionAveragePrice();
    ~CAuctionAveragePrice();

    bool LoadAveragePrice();
    void SetAveragePrice(int itemID, unsigned char upgrade, int price);
    int GetAveragePrice(int itemID, unsigned char upgrade) const;

private:
    std::map<int, std::vector<std::pair<unsigned char, int> > > m_averagePriceMap;
};

#endif
