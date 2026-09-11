// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCeraShopAddRestrict/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// b28d-solo-CCeraShopAddRestrict 判词汇总（本 TU 9 件，ORIG 地址见各函数注释）：
//   Manager::C1 0x080e09bc AE：P299/CWG434——CUser.h ScriptLoader 删用户空 ctor
//     使 m_loader 的 value-init 前置零初始化（5×`movl $0`）复现（/tmp/probe_sl.cpp）。
//   InputBuyInfoDetail 0x080e1196 AE：m_count int→unsigned（P211档③，CUser.h 代落）
//     + pair 临时直构（P278）+ `new paramDaily()` value-init（P90/P299）。
//   readTime 0x08abf164 AE：`return 1` 移入 try 块（P229/N104 正常路径尾物化）。
//   LoadProcess 0x080e142a AE：`new paramDaily()` + SIG `m_items` 具名数组视图
//     （P291，SigTypes_B.h 字段化）+ m_count unsigned。
//   FindUserInfoDetail 0x080e0a43 NEAR→N 封顶：P196 比较方向 + m_count unsigned 已修；
//     残余 N95 槽基差 8 + N86 insert sret 槽位；p167 CLEAR 129=129。
//   InputBuyInfo 0x080e129a AE：m_result 具型化 paramDaily*（P211档③ LHS-first，
//     CUser.h 代落）+ switch（P251/P261）。
//   LoadScript 0x08abf3cc AE：`while(true){if(!ScanType)break;}`（P283 头部条件）
//     + 3 个 dead bool 预清（P232）。
//   makeSyncPacket 0x080e0f3a AE：P274 累加异型 cast + 去 buf 具名指针（P183）
//     + empty 正极性 else 形（P292）+ P223 尾空标签 nop 岛。
//   LoadScriptDaily 0x08abf4de N 封顶：P292 readTime 返回 bool、P214③ t1/t2={0}、
//     P223 双岛 goto、P167 operator==（printf 岛布局）、P185 printf 原文；残余
//     N95 槽基差 8 + N93 块内槽序 + N7 寄存器中转（edi/eax）；p167 CLEAR 154=154。
//   注：__static_initialization_and_destruction_0ii 为 TU 级静态初始化（非清单件）。

#include <ctime>
#include <cstring>
#include <map>
#include <string>
#include "RDARScriptStringManager.h"
#include "CGameManager.h"
#include "CInventory.h"

#include "CUser.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "InterfacePacketBuf.h"
#include "Arad_DataManager.h"
#include "CItemLimitEdition.h"
#include "CEnvironment.h"   // G_CEnvironment()（GetCalcDay 读 env+0x37c）
#include "LogManager.h"
#include "CCeraShopBonusItemEvent.h"   // 权威声明（P104 ODR 收敛，2026-08-29 EventManagerWave）
#include "CCeraShopAddRestrict/ScanTypeOrInt.inc"

// DB 商城限购请求类（权威定义见 DB_CerashopAddRestrict.cpp，ORIG T @ 0x080e1ff6/0x080e17ba）。
class DB_LoadCerashopAddRestrictType
{
public:
    static void makeRequest(CUser* user);
};
class DB_SaveCerashopAddRestrictType
{
public:
    static void makeRequest(CUser* user, CerashopAddRestrict::DBInputData& data);
};


// ARAD::Singleton 模板定义已在 Arad_DataManager.h 提供（唯一）；此处不再定义。
// 供 ARAD::Singleton<CerashopAddRestrict::Manager>::Get() 隐式实例化，产出 ORIG 符号
// _ZN4ARAD9SingletonIN19CerashopAddRestrict7ManagerEE3GetEv（W @ 0x080e274d）。

#include "CCeraShopAddRestrict/loadRDARScriptFile.inc"
#include "CCeraShopAddRestrict/ScanType.inc"
#include "CCeraShopAddRestrict/ScanInt.inc"
#include "CCeraShopAddRestrict/ScanStr.inc"

// DB 请求类（定义于 DB_CerashopAddRestrict.cpp，ORIG 0x080e1ff6/0x080e17ba）。
// 全局作用域类；SAVE 参数类型为 CerashopAddRestrict::DBInputData。
class DB_LoadCerashopAddRestrictType;
class DB_SaveCerashopAddRestrictType;

namespace CerashopAddRestrict
{

#include "CCeraShopAddRestrict/Manager.inc"


#include "CCeraShopAddRestrict/LoadScript.inc"


#include "CCeraShopAddRestrict/LoadScriptDaily.inc"


#include "CCeraShopAddRestrict/ClearScript.inc"


#include "CCeraShopAddRestrict/GetRestrictInfo.inc"


#include "CCeraShopAddRestrict/FindIpgNo.inc"


#include "CCeraShopAddRestrict/readTime.inc"


unsigned int uint32Time(short* buf)
{
    time_t t = time(0);
    struct tm* tm = localtime(&t);
    tm->tm_year = buf[0] - 1900;
    tm->tm_mon = buf[1] - 1;
    tm->tm_mday = buf[2];
    tm->tm_hour = buf[3];
    tm->tm_min = 0;
    tm->tm_sec = 0;
    return (unsigned int)mktime(tm);
}

// ---- 命名空间级辅助（ORIG 报告 CerashopAddRestrict/*.md）----

// ORIG 0x080e0a43（static，L mangled；FindUserInfoDetail.md 全文转写）：
//   idxMap.find(key)：未命中 insert(key,0) 取返回迭代器；it->second += add；
//   dailyMap.find(key)：命中 → now<nextDate 时 (count+idx)<=field8 → 1 否则 0；
//     超期 → count=0，idx<=field8 → 1 否则 0；未命中 → field8<idx → 0 否则 1。
// [L6S6 2026-09-11 P331 **命中**，破旧判 N86/N95] insert 的返回值
// `pair<iterator,bool>` 起具名局部（`ins`）⇒ sret 槽与 it 槽分离（ORIG 0x080e0ac5
// 的 sret 落 -0x34、调用后 `mov -0x34,%eax; mov %eax,-0x28(%ebp)`），全部槽位
// （it -0x28 / end -0x24 / insert sret -0x34 / it2 -0x2c / daily -0x10 / now -0xc）
// 与 ORIG 0x080e0a43 逐一对齐 ⇒ 129=129 AE。旧判词：NEAR→N 封顶（129=129、p167
// CLEAR；残余 N95 槽基差 8 + N86 insert sret 槽位；P196 比较方向与 m_count unsigned 已修）。
// [L6-S1 2026-09-11 P292 链] int→bool：本函数全部 return 均为常量 0/1（bool 化后
// 生成不变，探针 /tmp/l6p/p8.cpp bconst 实证 `mov $0x1,%eax`），而使
// `CheckBuyableProduct` 的 `return FindUserInfoDetail(...)` 成为 bool 直返
// （否则恒多 test/setne，ORIG 0x080e0cdc 直通无归一化）。
static bool FindUserInfoDetail(
    std::map<unsigned int, unsigned int>& idxMap,
    std::map<unsigned int, paramDaily*>& dailyMap,
    unsigned int key, InfoDaily* info, unsigned int add)
{
    std::map<unsigned int, unsigned int>::iterator it = idxMap.find(key);
    if (it == idxMap.end()) {
        // [L6S6 2026-09-11 P331 试形] ORIG 0x080e0ac5/0x080e0acd：insert 的
        // pair<iterator,bool> sret 槽(-0x34) 与 it 槽(-0x28) **分离**，调用后
        // `mov -0x34,%eax; mov %eax,-0x28`；我方把 sret 直接落在 it 邻槽(-0x28)
        // 再回写 it(-0x30)。显式 pair 局部试复现 ORIG 槽序。
        std::pair<std::map<unsigned int, unsigned int>::iterator, bool> ins =
            idxMap.insert(
                std::map<unsigned int, unsigned int>::value_type(key, 0));
        it = ins.first;
    }
    it->second += add;

    std::map<unsigned int, paramDaily*>::iterator it2 = dailyMap.find(key);
    if (it2 != dailyMap.end()) {
        paramDaily* daily = it2->second;
        time_t now = time(0);
        if (now < daily->m_nextDate) {
            if ((unsigned int)(daily->m_count + it->second) <=
                (unsigned int)info->m_field8)
                return 1;
        } else {
            daily->m_count = 0;
            if ((unsigned int)it->second <= (unsigned int)info->m_field8)
                return 1;
        }
        return 0;
    }
    // [b28d-solo P196] ORIG 0x080e0bcd 段：先载 it->second 到 %edx、后载
    // info->m_field8 到 %eax，`cmp %eax,%edx; seta` ⇒ 源码须写
    // `it->second > m_field8`；反向书写 `m_field8 < it->second` 使 4.4
    // 先物化左操作数到 %ebx 并产 `setb`（比较方向反演）。
    if ((unsigned int)it->second > (unsigned int)info->m_field8)
        return 0;
    return 1;
}

// ORIG 0x080e10d7（static，L mangled；GetCalcDay.md 全文转写）：
//   days=(m_end-m_start)/0x15180+1；m_limit>0 → days=m_limit；t=m_start，
//   循环 t+=days*0x15180 至 t>now；localtime_r 置 tm_hour=G_CEnvironment()+0x37c、
//   min/sec=0，mktime 返回。
static int GetCalcDay(const InfoDaily* info)
{
    int days = (info->m_end - info->m_start) / 0x15180 + 1;
    if (info->m_limit > 0)
        days = info->m_limit;
    time_t t = (time_t)info->m_start;
    time_t now = time(0);
    while (t <= now)
        t += days * 0x15180;
    struct tm local44;
    localtime_r(&t, &local44);
    local44.tm_hour = G_CEnvironment()->m_serverEnvir.m_fatigueTime;  // env+0x37c
    local44.tm_min = 0;
    local44.tm_sec = 0;
    return (int)mktime(&local44);
}

// ORIG 0x080e1196（命名空间级，T mangled；InputBuyInfoDetail.md 全文转写）：
//   m.find(key)：命中 → m_count += count；未命中 → new paramDaily 清零、
//   m_count=count、m_ipgNo=key 后 insert；尾 m_nextDate=GetCalcDay(info)，返回 pd。
// b28d-solo-CCeraShopAddRestrict: AE（77=77，仅 .o 重定位显示差；P211档③ m_count
// unsigned + P278 pair 临时 + P90/P299 value-init）。
paramDaily* InputBuyInfoDetail(
    std::map<unsigned int, paramDaily*>& m,
    const InfoDaily* info, unsigned int key, unsigned int count)
{
    paramDaily* result = 0;
    std::map<unsigned int, paramDaily*>::iterator it = m.find(key);
    if (it != m.end()) {
        result = it->second;
        // [b28c-4 P41/P211] ORIG 0x080e11fb 段 = `mov 0x4(%eax),%eax; mov %eax,%edx;
        // add 0x14(%ebp),%edx`（内存直加）；`(int)count` 反使 4.4 先载 eax。
        result->m_count += count;
    } else {
        // [b28d-solo P90/P299] ORIG 0x080e120e：`call _Znwj` 后立即内联 3 条
        // `movl $0x0,(|0x4|0x8)(%eax)` 且 pd 存槽在清零之后 ⇒ 原码是 value-init
        // `new paramDaily()`（POD 零初始化），不是「具名 pd + 三条手动清零」
        //（后者 4.4 先存槽再逐条重载，多 3 条 mov）。
        paramDaily* pd = new paramDaily();
        pd->m_count = count;
        pd->m_ipgNo = key;
        // [b28d-solo P278] ORIG 0x080e1248：pair 临时直构于 -0x14（ctor this 即
        // `lea -0x14,(%esp)`）、insert sret -0x1c；具名 p 形把 pair 落 -0x28 且
        // insert sret 落 -0x14（槽序整体反向）。类型仍用 map::value_type 直构，
        // 保持 ORIG 的 pair<unsigned&,paramDaily*&> 转换 ctor 符号。
        m.insert(std::map<unsigned int, paramDaily*>::value_type(key, pd));
        result = pd;
    }
    result->m_nextDate = GetCalcDay(info);
    return result;
}

#include "CCeraShopAddRestrict/dtor_Manager.inc"


#include "CCeraShopAddRestrict/Load.inc"


#include "CCeraShopAddRestrict/Destroy.inc"


#include "CCeraShopAddRestrict/FindIpgNo_1.inc"


#include "CCeraShopAddRestrict/CheckBuyableProduct.inc"


#include "CCeraShopAddRestrict/UpdateBuyableRestrictItem.inc"


#include "CCeraShopAddRestrict/ClearBuyRestrictItem.inc"


#include "CCeraShopAddRestrict/LoadRequestBuyRestrictItem.inc"


#include "CCeraShopAddRestrict/SendSyncPacket.inc"


#include "CCeraShopAddRestrict/GetItemListForPacket.inc"


#include "CCeraShopAddRestrict/makeSyncPacket.inc"


#include "CCeraShopAddRestrict/InputBuyInfo.inc"


#include "CCeraShopAddRestrict/IsBuyableRestrictItem.inc"


#include "CCeraShopAddRestrict/LoadProcess.inc"


}  // namespace CerashopAddRestrict
// ============================================================================
// CCeraShopBonusItemEvent / CEventCeraShopRewardPoint（商城事件）
// ============================================================================
// 类声明见 CCeraShopBonusItemEvent.h（权威，继承 CGuildServerProxy.h 的
// CEventBase；vtable 序 / +0x0c m_rate 布局依 ORIG 报告）。方法定义如下。

#include "CCeraShopAddRestrict/CCeraShopBonusItemEvent.inc"


#include "CCeraShopAddRestrict/dtor_CCeraShopBonusItemEvent.inc"


#include "CCeraShopAddRestrict/StartEvent.inc"


#include "CCeraShopAddRestrict/StartEvent_1.inc"


#include "CCeraShopAddRestrict/EndEvent.inc"

#include "CCeraShopAddRestrict/getBonusRate.inc"

