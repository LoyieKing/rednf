// df_game_r 还原 —— CAuctionAveragePrice（G2-4 批次）
// 参照 docs/class_func_reports/CAuctionAveragePrice.md 与 ORIG 反汇编逐函数实现。
// 原源文件 AuctionAveragePrice.cpp；数据表 auction_average_price。
#include <algorithm>
#include <map>
#include <utility>
#include <vector>

#include "CEnvironment.h"
#include "DBMgr.h"
#include "GlobalData.h"

// ---- 拍卖均价表（item_id + upgrade 唯一；mangling 20CAuctionAveragePrice） ----
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

// ---- find_if 谓词（ORIG 独立弱符号 FindPrice，取同命名空间同名） ----
class FindPrice
{
public:
    FindPrice(unsigned char upgrade)
    {
        m_upgrade = upgrade;
    }

    bool operator()(const std::pair<unsigned char, int>& item) const
    {
        if (item.first == m_upgrade)
        {
            return true;
        }
        return false;
    }

private:
    unsigned char m_upgrade;
};

CAuctionAveragePrice::CAuctionAveragePrice()
{
}

CAuctionAveragePrice::~CAuctionAveragePrice()
{
}

bool CAuctionAveragePrice::LoadAveragePrice()
{
    MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_C, E_SERVER_GROUP_NONE);
    mysql->set_query("seLect item_id,upgrade,average_price from auction_average_price");
    if (!mysql->exec(true))
    {
        return false;
    }
    if (mysql->get_n_rows() == 0)
    {
        return true;
    }

    int itemID = 0;
    int price = 0;
    unsigned char upgrade = 0;
    char local11 = 0;
    unsigned int count;
    for (count = 0; count < (unsigned int)mysql->get_n_rows(); ++count)
    {
        if (!mysql->fetch())
        {
            return false;
        }
        if (!mysql->get_int(0, itemID))
        {
            return false;
        }
        if (!mysql->get_ubyte(1, upgrade))
        {
            return false;
        }
        if (!mysql->get_int(2, price))
        {
            return false;
        }
        SetAveragePrice(itemID, upgrade, price);
    }
    return true;
}

void CAuctionAveragePrice::SetAveragePrice(int itemID, unsigned char upgrade, int price)
{
    std::map<int, std::vector<std::pair<unsigned char, int> > >::iterator it =
        m_averagePriceMap.find(itemID);
    if (it != m_averagePriceMap.end())
    {
        std::pair<unsigned char, int> p(upgrade, price);
        it->second.push_back(p);
    }
    else
    {
        std::vector<std::pair<unsigned char, int> > vec;
        std::pair<unsigned char, int> p(upgrade, price);
        vec.push_back(p);
        m_averagePriceMap.insert(std::make_pair(itemID, vec));
    }
}

int CAuctionAveragePrice::GetAveragePrice(int itemID, unsigned char upgrade) const
{
    std::map<int, std::vector<std::pair<unsigned char, int> > >::const_iterator it =
        m_averagePriceMap.find(itemID);
    if (it != m_averagePriceMap.end())
    {
        std::vector<std::pair<unsigned char, int> >::const_iterator pit =
            std::find_if(it->second.begin(), it->second.end(), FindPrice(upgrade));
        if (pit != it->second.end())
        {
            return pit->second;
        }
    }
    return 0;
}
