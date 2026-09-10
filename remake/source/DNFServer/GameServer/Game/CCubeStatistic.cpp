// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCubeStatistic/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "CCubeStatistic/SECRET_SHOP_STATISTIC_DATA.inc"
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
#include "CCubeStatistic/CCubeStatistic.inc"
{
}

// ---- findCubeStuff（ORIG 0x860de96）：查表 "cube stuff" 值是否 == 入参 ----
#include "CCubeStatistic/findCubeStuff.inc"
{
    // ORIG：拷贝 getItemGroupNameTable() 得本地 map，find("cube stuff")；
    // 命中且 it->second == 入参 → true，否则 false。
    std::map<std::string, int> table(getItemGroupNameTable());
    std::map<std::string, int>::iterator it = table.find("cube stuff");
    if (it != table.end())
    {
        // P225：ORIG 0x0860df6e 内层条件为 `cmp; sete %al; test %al,%al; je`
        // bool 四段物化——while+return 形复现（if 短路链恒折 jne 直跳）。
        while (it->second == groupValue)
            return true;
    }
    return false;
}

// ---- sendCubeStatistic（ORIG 0x860e0d8）：组装 Packet_Cube_Statistic 发出 ----
#include "CCubeStatistic/sendCubeStatistic.inc"
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

    // ORIG 0x861002e 区：GetServerProxy 返回直传 0x4(%esp) 无判空无槽
    // （P140/P52 链式单语句；&pkt 跨 GetServerProxy 调用缓存 %ebx）。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendPacket((char*)&pkt, 0x17);
    return 1;
}

// ---- collectCubeStatistics(int,int,CUser*,CUBE_STATISTIC_FIELD)（ORIG 0x860dfb8） ----
#include "CCubeStatistic/collectCubeStatistics.inc"
                                           CUBE_STATISTIC_FIELD field)
{
    // ORIG：item=G_CDataManager()->find_item(itemIndex) 直调（无 manager NULL 防御）。
    // 两处 findCubeStuff 各自内联 getItemGroupName()（ORIG 重算两次，无 group 局部；
    // P0 槽-局部对应律禁缓存局部）。GetInstanceCubeStatistic() 也用两次。
    CItem* item = G_CDataManager()->find_item(itemIndex);
    if (item == 0)
        return;
    if (findCubeStuff(item->getItemGroupName()))
        sendCubeStatistic(itemIndex, count, user, field);
    if (GetInstanceCubeStatistic()->findCubeStuff(item->getItemGroupName()))
        GetInstanceCubeStatistic()->sendCubeStatistic(itemIndex, count, user, field);
}

// ---- collectCubeStatistics(CItem*,int,CUser*,CUBE_STATISTIC_FIELD)（ORIG 0x860e07a） ----
#include "CCubeStatistic/collectCubeStatistics_1.inc"
                                           CUBE_STATISTIC_FIELD field)
{
    // ORIG：item 非 NULL 且 findCubeStuff(item->getItemGroupName()) 命中 →
    // sendCubeStatistic(item->get_index(), count, user, field)。
    if (item == 0)
        return;
    if (findCubeStuff(item->getItemGroupName()))
        sendCubeStatistic(item->get_index(), count, user, field);
}