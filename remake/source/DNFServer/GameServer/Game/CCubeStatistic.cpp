// ============================================================================
// df_game_r 还原 —— CCubeStatistic 缺失符号批次
// 参照 docs/class_func_reports/CCubeStatistic/*.md（sendCubeStatistic.md、
// findCubeStuff.md、collectCubeStatistics.md、collectCubeStatistics_0860e07a.md）、
// _lt_global_gt_/GetInstanceCubeStatistic.md 与 ORIG objdump 逐函数核对。
// 同时补齐 SECRET_SHOP_STATISTIC_DATA ctor（该 struct 在 CSecretShop.cpp 声明，
// ORIG ctor 清零 m_data[5]，原注释称定义于 Statics/GMAccounts.cpp 有误——实际
// 该 mangled 符号属 game 构建，须在本域定义）。
// ============================================================================
#include <map>
#include <string>

#include "CCubeStatistic.h"
#include "CDataManager.h"
#include "CItem.h"
#include "CUser.h"
#include "CEnvironment.h"
#include "Packet_Cube_Statistic.h"
#include "GlobalData.h"
#include "CServerProxyMgr.h"
#include "CStatisticServerProxy.h"

// ORIG _ZN26SECRET_SHOP_STATISTIC_DATAC1Ev @ 0x85fd45a：清零 m_data[0..4]（5 int）。
struct SECRET_SHOP_STATISTIC_DATA
{
    SECRET_SHOP_STATISTIC_DATA();
    int m_data[5];   // [0]=dungeonIdx [1]=showCount [2]=showPrice
                     // [3]=buyCount [4]=buyPrice
} __attribute__((packed));

SECRET_SHOP_STATISTIC_DATA::SECRET_SHOP_STATISTIC_DATA()
{
    m_data[0] = 0;
    m_data[1] = 0;
    m_data[2] = 0;
    m_data[3] = 0;
    m_data[4] = 0;
}

// ---- getItemGroupNameTable（ORIG 0x88c0409）：__cxa_guard 单例 map ----
// findCubeStuff 拷贝本表后查 key "cube stuff"。ORIG 单例体仅默认构造空 map
//（条目由其它加载逻辑填充），此处保持一致。
std::map<std::string, int>& getItemGroupNameTable()
{
    static std::map<std::string, int> mapItemGroupTable;
    return mapItemGroupTable;
}

// ---- GetInstanceCubeStatistic（ORIG 0x860de50）：单例 ----
CCubeStatistic* GetInstanceCubeStatistic()
{
    static CCubeStatistic instance;
    return &instance;
}

// ---- CCubeStatistic 空构造（ORIG 0x86103b8，函数体仅有 ret） ----
CCubeStatistic::CCubeStatistic()
{
}

// ---- findCubeStuff（ORIG 0x860de96）：查表 "cube stuff" 值是否 == 入参 ----
bool CCubeStatistic::findCubeStuff(int groupValue)
{
    // ORIG：拷贝 getItemGroupNameTable() 得本地 map，find("cube stuff")；
    // 命中且 it->second == 入参 → true，否则 false。
    std::map<std::string, int> table(getItemGroupNameTable());
    std::map<std::string, int>::iterator it = table.find("cube stuff");
    if (it != table.end() && it->second == groupValue)
        return true;
    return false;
}

// ---- sendCubeStatistic（ORIG 0x860e0d8）：组装 Packet_Cube_Statistic 发出 ----
int CCubeStatistic::sendCubeStatistic(int itemIndex, int count, CUser* user,
                                      CUBE_STATISTIC_FIELD field)
{
    // ORIG：Packet_Cube_Statistic(channel_no, get_charac_level, itemIndex,
    //   count, field) 通过 CStatisticServerProxy::SendPacket(pkt, 0x17=23) 上报。
    // 与 CValueStatistic::InsertValueStatistic 同型（GetServerProxy→SendPacket）。
    Packet_Cube_Statistic pkt;
    pkt.m_channel = (unsigned short)G_CEnvironment()->get_channel_no();
    pkt.m_level = (unsigned short)user->get_charac_level();
    pkt.m_itemIndex = (unsigned int)itemIndex;
    pkt.m_itemCount = count;
    pkt.m_type = (unsigned char)field;

    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(
            user->GetServerGroup());
    if (proxy)
        proxy->SendPacket((char*)&pkt, 0x17);
    return 1;
}

// ---- collectCubeStatistics(int,int,CUser*,CUBE_STATISTIC_FIELD)（ORIG 0x860dfb8） ----
void CCubeStatistic::collectCubeStatistics(int itemIndex, int count, CUser* user,
                                           CUBE_STATISTIC_FIELD field)
{
    // ORIG：item=G_CDataManager().find_item(itemIndex)；非 NULL 时
    // 1) 本对象 findCubeStuff(item->getItemGroupName()) 命中 → 本对象 send；
    // 2) 单例 GetInstanceCubeStatistic()->findCubeStuff(...) 命中 → 单例 send。
    CDataManager* dm = G_CDataManager();
    CItem* item = dm ? dm->find_item(itemIndex) : 0;
    if (item == 0)
        return;
    int group = item->getItemGroupName();
    if (findCubeStuff(group))
        sendCubeStatistic(itemIndex, count, user, field);
    if (GetInstanceCubeStatistic()->findCubeStuff(group))
        GetInstanceCubeStatistic()->sendCubeStatistic(itemIndex, count, user, field);
}

// ---- collectCubeStatistics(CItem*,int,CUser*,CUBE_STATISTIC_FIELD)（ORIG 0x860e07a） ----
void CCubeStatistic::collectCubeStatistics(CItem* item, int count, CUser* user,
                                           CUBE_STATISTIC_FIELD field)
{
    // ORIG：item 非 NULL 且 findCubeStuff(item->getItemGroupName()) 命中 →
    // sendCubeStatistic(item->get_index(), count, user, field)。
    if (item == 0)
        return;
    if (findCubeStuff(item->getItemGroupName()))
        sendCubeStatistic(item->get_index(), count, user, field);
}