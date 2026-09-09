// ============================================================================
// df_game_r 还原 —— secretshop 域（G4 商城/交易批次）
// 参照 docs/class_func_reports/secretshop__*.md + ORIG 反汇编。
// 覆盖：SALE_INFO/BUY_INFO、RETAILER/SHOPPER、IBuyRule、
// CBuyItembyGold/CBuyItembyRecipe、CSecretShopStatistic、CSecretShop。
// SECRET_SHOP_DATA/SECRET_SHOP_INFO 由 SECRET_SHOP_DATA.cpp 提供。
// Dispatcher_SecretShop* / Timer_SecretShopStatistic 属分发域（G7），
// 本批不实现（GameStubs 桩保留）。
// ============================================================================

#include <map>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <ctime>

#include "Inven_Item.h"
#include "CDataManager.h"
#include "DNFLexWrapper.h"  // importSecretShopScript 的 Scan*/loadRDARScriptFile
#include "CInventory.h"
#include "CUser.h"
#include "CServerProxyMgr.h"
#include "CStatisticServerProxy.h"
#include "CCubeStatistic.h"
#include "GlobalData.h"
#include "SECRET_SHOP_DATA.h"
#include "CSecretShop.h"   // eBuyRule/CSecretShopStatistic/IBuyRule/CSecretShop 唯一声明点
#include "CParty.h"
#include "CDungeon.h"  // CheckLottery 遍历 m_dungeonMap
#include "PvPTypes.h"  // 权威全局 CMTRand（0x9c8，MT19937；实现见 CMTRand.cpp）
#include "Packet_Secret_Shop_Statistic.h"  // 权威包头（shared/packet/include）
#include "CGuildServerProxy.h"  // CEventBase/CEventManager 权威定义（LotteryItems 虚调 IsEventing）

#include <cstring>
// eBuyRule / CSecretShopStatistic / IBuyRule / CSecretShop 类声明已提取至
// CSecretShop.h（跨 TU 唯一声明点，供 CParty::dungeon_start 等调用方）。
// ---- CSecretShop::CSecretShop（GI 键名对齐：ORIG 该 ctor 所在 TU（SecretShop.cpp）
//      的首个强外部实体即本 ctor（_ZN10secretshop11CSecretShopC2Ev），故将定义
//      前置于本 TU 首位；定义体逐字未改，仅移动。）----

secretshop::CSecretShop::CSecretShop()
{
    // [b28d-solo-CSecretShop 判词·N] ORIG 124 条 vs 我们 126 条（gap -2）：
    // 残差全部为两处 `pair<eBuyRule,CBuyItembyGold*>` 等 sret 调用后的
    // `sub $0x4,%esp`（4.4 outgoing 预留，ORIG 4.1 无）+ lea 目标寄存器
    // %eax↔%edx 分配差（N7/N115 族）；调用点集合与槽位集逐项一致，语义完成。
    // ORIG：map.insert(pair)（非 operator[]）；m_pRand = new CMTRand(time(0))
    // （PvPTypes.h 权威 CMTRand 的 const unsigned long& 种子 ctor，
    //  符号 _ZN7CMTRandC1ERKm @0x080cba2a；ORIG 无 CMTRand(time_t) 重载，
    //  time(0) 经临时 unsigned long 绑定 const 引用）。
    m_rules.insert(std::make_pair((eBuyRule)0, new CBuyItembyGold(m_statistic)));
    m_rules.insert(std::make_pair((eBuyRule)1, new CBuyItembyRecipe(m_statistic)));
    m_pRand = new CMTRand(time(0));
}

// ---- importSecretShopScript（ORIG 0x8a81cf0，_Z22importSecretShopScriptP16SecretShopScriptPKc）----

// 按 docs/class_func_reports/_lt_global_gt_/importSecretShopScript.md 忠实转写：
//   - 所有取值扫描均为 int ScanInt(bool* ok) 形式；占位扫描传 (bool*)0
//     （ORIG movl $0x0 压参），成功标志仅用于条目循环终止判断。
//   - [dungeon npc]/[level npc]：每条 7 次扫描（key/npcIdx/rate 带 flag +
//     4 次 flag=0 占位，ORIG +0x11c 循环 i=0..3），循环至 flag 失败后
//     verifyNpc 对应 multimap。
//   - [npc]：npcKey 存入局部 stNpcSales::m_field0；内层 token 循环处理
//     [dungeon]/[level]（先 dungeonKey/maxCount 两次占位扫描，随后 itemIdx
//     带 flag 的商品循环）、[/npc]（整表插入 m_dungeonNpcMap）；
//     [dungeon cash item] 为无操作继续。
//   - [event item] 在外层：首个占位扫描写 m_eventDungeonSales.m_maxCount
//     （ORIG script+0x64），其后 itemIdx 带 flag 的商品循环插入 m_sales。
//   - stDungeonNpc 各字段与 key/npcIdx/rate 的对应为 [推断]（ORIG 经局部
//     辅助构造函数写入，未逐指令核对）。
int importSecretShopScript(SecretShopScript* script, const char* path)
{
    // [b28d-solo-CSecretShop 判词·N + 真 bug 修复] 超大件按 §6.2 三件套：
    // ① p167 调用点差集 CLEAR（106=106，仅 C1/C2、D1/D2 同址别名伪差，P93）；
    // ② gap = -2（ORIG 644 vs 我们 646）；③ 残差为控制流岛布局（4.1 循环头
    //    `jmp/nop/jmp/nop` 双岛 + continue 岛 vs 4.4 单 jmp）与 N82 槽号差。
    // 本轮修复（ORIG 证据）：① 分支物理序改 ORIG 序 [dungeon npc]→[level
    //    section]→[level npc]（P324，ndiff 113→45 行）；② [dungeon npc]/[level
    //    npc] 的 ScanInt 结果直接写 stDungeonNpc 字段（ORIG -0x260 对象槽、pair
    //    两实参同址），删 3 条拷贝；③ 补 pScript 具名局部（ORIG -0x18，P0）；
    //    ④ 补死局部 unusedSlot（ORIG -0x14 死初始化，P232）；⑤ **[真 bug]**
    //    [secret shop end] 置 flag=true 且函数返回该 flag（ORIG 0x08a827b8
    //    movb $1 + 尾 0x08a827c2 movzbl 返回；旧实现恒 return 1）；⑥ [add slot
    //    cash user stackable count] 补 continue（ORIG 0x08a82794 jne 岛，P223）。
    if (!loadRDARScriptFile("", path))
    {
        return 0;
    }
    // [b28d-solo-CSecretShop] ORIG 0x08a81d2b `mov 0x8(%ebp),%eax; mov
    // %eax,-0x18(%ebp)`，其后全程经 -0x18 读 script（14 处）⇒ 原码有具名
    // 指针局部（P0 槽往返律），补之；旧实现直读形参少 2 条 store/reload。
    SecretShopScript* pScript = script;
    pScript->clear();
    std::string token;
    // [b28d-solo-CSecretShop] ORIG 0x08a81d3e `movl $0x0,-0x14(%ebp)`：全函数
    // 无读的死初始化槽（P232：未使用局部照样占帧/发射初始化），补之以对齐槽图。
    int unusedSlot = 0;
    (void)unusedSlot;
    // [b28d-solo-CSecretShop] p167：ORIG 的 [dungeon npc]/[level npc] 是两段
    // 独立代码（[level npc] 为 1 带 flag + 2 无 flag + 4 占位），旧实现合并导致
    // ScanInt 调用点少 12；flag 为函数级单 bool（ORIG local_255）。
    bool flag = false;
    while (ScanType(token, true))
    {
        if (token == "[dungeon npc]")
        {
            for (;;)
            {
                // [b28d-solo-CSecretShop] ORIG 0x08a81e0x：ScanInt 结果**直接**写
                // stDungeonNpc 字段（对象槽 -0x260，pair 两实参同址 &npc）——旧
                // 实现用独立 key/npcIdx/rate 再拷贝，多 3 条 mov 且帧 0x330 vs
                // ORIG 0x320。
                stDungeonNpc npc;
                npc.m_field0 = ScanInt(&flag);   // [推断]
                if (!flag)
                    break;
                npc.m_npcIdx = ScanInt(&flag);
                if (!flag)
                    break;
                npc.m_rate = ScanInt(&flag);
                if (!flag)
                    break;
                for (int i = 0; i < 4; ++i)
                    ScanInt((bool*)0);   // 4 个占位扫描（ORIG +0x11c）
                pScript->m_npcMap.insert(
                    std::pair<int, stDungeonNpc>(npc.m_field0, npc));
            }
            pScript->verifyNpc(pScript->m_npcMap);
        }
        // [b28d-solo-CSecretShop] ORIG 分支物理序（按串地址）：[dungeon npc]
        // 0x08a81d73 → [level section] 0x08a81ea6 → [level npc] 0x08a81f5e
        // ⇒ if-else 链必须同序（P324 叶块序=书写序）。
        else if (token == "[level section]")
        {
            for (;;)
            {
                stLevelSection section;
                section.m_levelIdx = ScanInt(&flag);
                if (!flag)
                    break;
                section.m_minLev = ScanInt((bool*)0);
                section.m_maxLev = ScanInt((bool*)0);
                pScript->m_levelSections.insert(section);
            }
        }
        else if (token == "[level npc]")
        {
            for (;;)
            {
                int key = ScanInt(&flag);
                if (!flag)
                    break;
                int npcIdx = ScanInt((bool*)0);
                int rate = ScanInt((bool*)0);
                for (int i = 0; i < 4; ++i)
                    ScanInt((bool*)0);
                stDungeonNpc npc;
                npc.m_field0 = key;
                npc.m_npcIdx = npcIdx;
                npc.m_rate = rate;
                pScript->m_levelNpcMap.insert(
                    std::pair<int, stDungeonNpc>(key, npc));
            }
            pScript->verifyNpc(pScript->m_levelNpcMap);
        }
        else if (token == "[npc]")
        {
            stNpcSales sales;
            sales.m_field0 = ScanInt((bool*)0);   // npcKey
            // ORIG 内层检查顺序： [dungeon] → [level] → [dungeon cash item] → [/npc]
            // （P46 叶块序），ScanType 返回值丢弃（ORIG 直接调用）。
            for (;;)
            {
                ScanType(token, true);
                if (token == "[dungeon]")
                {
                    stDungeonSales ds;
                    ds.m_field0 = ScanInt((bool*)0);       // dungeonKey
                    ds.m_maxCount = ScanInt((bool*)0);     // +0x04
                    for (;;)
                    {
                        int itemIdx = ScanInt(&flag);
                        if (!flag)
                            break;
                        stSaleInfo sale;
                        sale.m_itemIdx = itemIdx;
                        sale.m_rule = (char)ScanInt((bool*)0);
                        sale.m_price = ScanInt((bool*)0);
                        sale.m_materialCount = ScanInt((bool*)0);
                        sale.m_limit = ScanInt((bool*)0);
                        sale.m_rate = ScanInt((bool*)0);
                        ds.m_sales.insert(std::pair<int, stSaleInfo>(itemIdx, sale));
                    }
                    sales.m_dungeonSales.insert(
                        std::pair<int, stDungeonSales>(ds.m_field0, ds));
                }
                else if (token == "[level]")
                {
                    stDungeonSales ds;
                    ds.m_field0 = ScanInt((bool*)0);
                    ds.m_maxCount = ScanInt((bool*)0);
                    for (;;)
                    {
                        int itemIdx = ScanInt(&flag);
                        if (!flag)
                            break;
                        stSaleInfo sale;
                        sale.m_itemIdx = itemIdx;
                        sale.m_rule = (char)ScanInt((bool*)0);
                        sale.m_price = ScanInt((bool*)0);
                        sale.m_materialCount = ScanInt((bool*)0);
                        sale.m_limit = ScanInt((bool*)0);
                        sale.m_rate = ScanInt((bool*)0);
                        ds.m_sales.insert(std::pair<int, stSaleInfo>(itemIdx, sale));
                    }
                    sales.m_eventSales.insert(
                        std::pair<int, stDungeonSales>(ds.m_field0, ds));
                }
                else if (token == "[dungeon cash item]")
                {
                }
                else if (token == "[/npc]")
                {
                    pScript->m_dungeonNpcMap.insert(
                        std::pair<int, stNpcSales>(sales.m_field0, sales));
                    break;
                }
            }
        }
        else if (token == "[event item]")
        {
            pScript->m_eventDungeonSales.m_maxCount = ScanInt((bool*)0);  // +0x64
            for (;;)
            {
                int itemIdx = ScanInt(&flag);
                if (!flag)
                    break;
                stSaleInfo sale;
                sale.m_itemIdx = itemIdx;
                sale.m_rule = (char)ScanInt((bool*)0);
                sale.m_price = ScanInt((bool*)0);
                sale.m_materialCount = ScanInt((bool*)0);
                sale.m_limit = ScanInt((bool*)0);
                sale.m_rate = ScanInt((bool*)0);
                pScript->m_eventDungeonSales.m_sales.insert(
                    std::pair<int, stSaleInfo>(itemIdx, sale));
            }
        }
        else if (token == "[price random]")
        {
            pScript->m_priceVarPercent = ScanInt((bool*)0);   // +0x80
        }
        else if (token == "[eplp wait time]")
        {
            pScript->m_int84 = ScanInt((bool*)0);             // +0x84
        }
        else if (token == "[add slot cash user stackable count]")
        {
            // ORIG 0x08a82794 `jne 08a81d4e`（nop 岛 → jmp cond）⇒ 原码是
            // 空处理 + continue（P223 continue 岛）。
            continue;
        }
        else if (token == "[secret shop end]")
        {
            // ORIG 0x08a827b8 `movb $0x1,-0x251(%ebp)` ⇒ 置 flag 后 break；
            // 函数返回值即该 flag（ORIG 尾 0x08a827c2 `movzbl -0x251,%ebx` +
            // `mov %ebx,%eax`），旧实现恒 return 1 属真 bug。
            flag = true;
            break;
        }
    }
    return flag;
}

namespace secretshop
{

// ---- SALE_INFO / BUY_INFO（ORIG W 0x08a832aa / 0x08ab1606 逐字段清零；
//      operator== ORIG W 0x08a832ee / 0x08ab161e，sete 直返 bool，
//      供 std::find 实例（0x08a839dd / 0x08ab16ec）比较）----
SALE_INFO::SALE_INFO()
{
    m_itemIdx = 0;
    m_rule = 0;
    m_price = 0;
    m_limit = 0;
    m_material = 0;
    m_materialCount = 0;
    m_field18 = 0;
}

bool SALE_INFO::operator==(int itemIdx)
{
    return m_itemIdx == itemIdx;
}

BUY_INFO::BUY_INFO()
{
    m_itemIdx = 0;
    m_count = 0;
}

bool BUY_INFO::operator==(int itemIdx)
{
    return m_itemIdx == itemIdx;
}

RETAILER::RETAILER()
{
}

RETAILER::~RETAILER()
{
}

void RETAILER::clear()
{
    m_sales.clear();
}

SALE_INFO* RETAILER::GetSaleInfo(int itemIdx)
{
    // ORIG T 0x08ab12a0：std::find + ne 比较；迭代器先声明后赋值（P28/P50）。
    std::vector<SALE_INFO>::iterator it;
    it = std::find(m_sales.begin(), m_sales.end(), itemIdx);
    if (it != m_sales.end())
        return &*it;
    return 0;
}

SHOPPER::SHOPPER()
{
}

SHOPPER::~SHOPPER()
{
}

void SHOPPER::clear()
{
    m_buys.clear();
}

BUY_INFO* SHOPPER::GetBuyInfo(int itemIdx)
{
    // ORIG T 0x08ab1344：std::find + ne 比较。
    std::vector<BUY_INFO>::iterator it;
    it = std::find(m_buys.begin(), m_buys.end(), itemIdx);
    if (it != m_buys.end())
        return &*it;
    return 0;
}

void SHOPPER::BuyItem(int itemIdx, int count)
{
    // ORIG T 0x08ab13e8：内联 std::find；命中累加，未命中 ctor+push_back。
    std::vector<BUY_INFO>::iterator it;
    it = std::find(m_buys.begin(), m_buys.end(), itemIdx);
    if (it != m_buys.end()) {
        (*it).m_count += count;   // ORIG 用 operator*（de）
    } else {
        BUY_INFO b;
        b.m_itemIdx = itemIdx;
        b.m_count = count;
        m_buys.push_back(b);
    }
}


CSecretShopStatistic::CSecretShopStatistic()
{
}

CSecretShopStatistic::~CSecretShopStatistic()
{
    for (int i = 0; i < 3; ++i)
        m_pos[i].clear();
}

void CSecretShopStatistic::Clear()
{
    for (int i = 0; i < 3; ++i)
        m_pos[i].clear();
}

int CSecretShopStatistic::GetNpcPos(int npcIdx)
{
    if (npcIdx == 0x3ea)
        return 0;
    if (npcIdx == 0x3eb)
        return 1;
    if (npcIdx == 0x3ec)
        return 2;
    return 0;
}

int CSecretShopStatistic::GetNpcIndex(int pos)
{
    if (pos == 0)
        return 0x3ea;
    if (pos == 1)
        return 0x3eb;
    if (pos == 2)
        return 0x3ec;
    return 0x3ea;
}

int CSecretShopStatistic::GetDungeonData(int npcIdx, int dungeonIdx)
{
    // [b28d-solo-CSecretShop] P183/P123：ORIG 0x085fd147 每处访问都重算
    // `pos*0x18 + this`（无 `map& m` 缓存局部）；insert 返回值被丢弃、随后
    // **再次 find** 取迭代器（ORIG 0x085fd1f7 后 0x085fd230 第二次 call find，
    // 旧实现用 insert().first 少 1 次 find/1 组地址重算）；SECRET_SHOP_STATISTIC_DATA
    // ctor（0x085fd45a）已清 5 个 int ⇒ 只写 m_data[0]（旧实现多写 4 条 movl）。
    // 判词·AE（本轮修复）：`std::make_pair` 改显式 `std::pair<int,T>(dungeonIdx,
    // data)`（ORIG 0x085fd1f7 直接调 pair 模板 ctor，无 make_pair sret）⇒ 消除
    // 调用后 `sub $0x4,%esp`，101→100，gap 0。
    int pos = GetNpcPos(npcIdx);
    std::map<int, SECRET_SHOP_STATISTIC_DATA>::iterator it =
        m_pos[pos].find(dungeonIdx);
    if (it != m_pos[pos].end())
        return (int)&it->second;
    SECRET_SHOP_STATISTIC_DATA data;
    data.m_data[0] = dungeonIdx;
    m_pos[pos].insert(std::pair<int, SECRET_SHOP_STATISTIC_DATA>(dungeonIdx, data));
    it = m_pos[pos].find(dungeonIdx);
    return (int)&it->second;
}

void CSecretShopStatistic::RecordShow(int npcIdx, int dungeonIdx, int price)
{
    SECRET_SHOP_STATISTIC_DATA* data =
        (SECRET_SHOP_STATISTIC_DATA*)GetDungeonData(npcIdx, dungeonIdx);
    data->m_data[1] += 1;
    data->m_data[2] += price;
}

void CSecretShopStatistic::RecordBuy(int npcIdx, int dungeonIdx)
{
    SECRET_SHOP_STATISTIC_DATA* data =
        (SECRET_SHOP_STATISTIC_DATA*)GetDungeonData(npcIdx, dungeonIdx);
    data->m_data[3] += 1;
}

void CSecretShopStatistic::RecordPrice(int npcIdx, int dungeonIdx, int price)
{
    SECRET_SHOP_STATISTIC_DATA* data =
        (SECRET_SHOP_STATISTIC_DATA*)GetDungeonData(npcIdx, dungeonIdx);
    data->m_data[4] += price;
}


void CSecretShopStatistic::SendSecretShopStatistic()
{
    // [b28d-solo-CSecretShop] 真语义 bug 修正 + P0/P197④/P83：
    // ① ORIG 0x085fd2a6 `mov %eax,-0xfbc`（packet+0xe=m_shopIdx）与 0x085fd2c3
    //    `mov %eax,-0xfc0`（packet+0xa=m_count）证明原码逐 pos 写这两个包字段
    //    （旧实现漏写）；② `empty()` 改 `size()`（ORIG call size，P197④）；
    // ③ memcpy 改结构整体赋值（ORIG 5×mov 逐字段，P83/N84）；
    // ④ proxy 无具名局部（ORIG call 结果直作 this，P0）；count 声明在循环外
    //    （ORIG -0x10 函数头 + 循环内两次清零）。
    Packet_Secret_Shop_Statistic packet;
    int count = 0;
    for (int pos = 0; pos < 3; ++pos) {
        packet.m_shopIdx = pos;
        packet.m_count = (int)m_pos[pos].size();
        if (packet.m_count == 0)
            continue;
        count = 0;
        memset(packet.m_items, 0, sizeof(packet.m_items));
        for (std::map<int, SECRET_SHOP_STATISTIC_DATA>::iterator it =
                 m_pos[pos].begin();
             it != m_pos[pos].end(); ++it, ++count) {
            packet.m_items[count] = it->second;
        }
        GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
            ->SendPacket((char*)&packet, 0xfb2);
    }
}




// [b28d-solo-CSecretShop] 返回类型 int→bool（判词见 CSecretShop.h 声明处）。
bool IBuyRule::CheckLimit(SECRET_SHOP_INFO& info, int itemIdx, int count, int& remain)
{
    SALE_INFO* sale = info.m_retailer.GetSaleInfo(itemIdx);
    BUY_INFO* buy = info.m_shopper.GetBuyInfo(itemIdx);
    if (sale == 0)
        return 0;
    if (buy != 0) {
        if (sale->m_limit < buy->m_count + count)
            return 0;
        remain = sale->m_limit - (buy->m_count + count);
    } else {
        if (sale->m_limit < count)
            return 0;
        remain = sale->m_limit - count;
    }
    return 1;
}

int IBuyRule::InsertItemIntoInventory(CUser* user, int itemIdx, int count,
                                      Inven_Item& out)
{
    // [b28d-solo-CSecretShop] P0/P48：ORIG 0x085fb6ce 无 dm 具名局部（G_CDataManager
    // 结果直接作 find_item 的 this 压栈，无槽往返）⇒ 内联调用表达式；item 局部保留
    // （ORIG -0x10 有槽往返）。残余虚调用目标寄存器差记 N7。
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
        return -1;
    item->make_item(out);
    out.m_addInfo = itemIdx;
    if (item->is_stackable())
        out.m_addInfo2 = count;
    // [b28d-solo-CSecretShop] P0：ORIG 0x085fb73a 把 getCurCharacInvenW 结果留 %edx
    // 跨 Inven_Item 栈拷贝（无槽往返）⇒ 无具名 inven 局部，内联接收者表达式。
    int ret = ((CInventory*)user->getCurCharacInvenW())
                  ->insertItemIntoInventory(out, (eItemAddReason)0x2c, 1, 0);
    if (ret < 0)
        return -1;
    return ret;
}

void IBuyRule::SendSecretShopBuyItem(CUser* user, int slot, Inven_Item& item,
                                     int a, int b, int c)
{
    // [b28d-solo-CSecretShop] P183/P0：ORIG 0x085fb566 每处调用都重算
    // `lea -0x14(%ebp),%eax`（无 buf/inven 具名局部，无槽往返）⇒ 全表达式内联，
    // 不缓存 InterfacePacketBuf*/CInventory*。PacketGuard 槽保持 -0x14。
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(1, 0x129);
    ((InterfacePacketBuf*)&packet)->put_byte(1);
    ((InterfacePacketBuf*)&packet)->put_int(
        ((CInventory*)user->getCurCharacInvenR())->get_money());
    ((InterfacePacketBuf*)&packet)->put_short(slot);
    ((InterfacePacketBuf*)&packet)->put_int(item.m_addInfo);
    ((InterfacePacketBuf*)&packet)->put_int(item.m_addInfo2);
    ((InterfacePacketBuf*)&packet)->put_byte(item.m_amp.getAbilityType() & 0xff);
    ((InterfacePacketBuf*)&packet)->put_short(item.m_amp.getAbilityValue() & 0xffff);
    ((InterfacePacketBuf*)&packet)->put_int(a);
    ((InterfacePacketBuf*)&packet)->put_int(b);
    ((InterfacePacketBuf*)&packet)->put_int(c);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    user->Send(packet);
}

void IBuyRule::LogCubeStatistic(CUser* user, std::pair<int, int>& item)
{
    // ORIG 0x85fb8b2：
    // GetInstanceCubeStatistic()->collectCubeStatistics(
    //     item.first, item.second, user, (CUBE_STATISTIC_FIELD)0x6a)
    GetInstanceCubeStatistic()->collectCubeStatistics(
        item.first, item.second, user, (CUBE_STATISTIC_FIELD)0x6a);
}

void IBuyRule::LogValueStatistic(CUser* user, unsigned int value)
{
    // ORIG 0x85fb886：
    // GetInstanceValueStatistic()->AddValueStatistic(
    //     (VALUE_STATISTIC_FIELD)0xd, user, value)
    GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0xd,
                                                   user, value);
}

// ---- CBuyItembyGold（金币购买规则） ----
// 无 virtual 析构；ctor 委托 IBuyRule(CSecretShopStatistic&) 存 +0x04，vptr 置
// &_ZTVN10secretshop14CBuyItembyGoldE+0x8（BuyItem 于 vtable[0]，ORIG 确认）。

CBuyItembyGold::CBuyItembyGold(CSecretShopStatistic& statistic)
    : IBuyRule(statistic)
{
}

bool CBuyItembyGold::BuyItem(CUser* user, SECRET_SHOP_INFO& info, int itemIdx,
                             int count)
{
    // [b28d-solo-CSecretShop] P0/P48：ORIG 0x085fb8f4 的 inven（0x085fba8d 结果
    // 直接作 use_money 的 this）、dungeonIdx（%ebx 跨 GetSecretShopData）、
    // m_pStatistic（call 前直读 this+4）均无槽往返 ⇒ 全部内联；
    // CheckLimit 已 bool 化（见头声明）复现 `call; xor $1; test; je`。
    // 残余：money 比较 `cmp mem,reg; setl` vs 4.4 直折（setcc-fold 族）记 N。
    SALE_INFO* sale = info.m_retailer.GetSaleInfo(itemIdx);
    unsigned int err = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x42);
    if (err != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, static_cast<char>(err));
        return 0;
    }
    if (sale == 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 0x11);
        return 0;
    }
    int remain = 0;
    if (!CheckLimit(info, itemIdx, count, remain)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 0x5f);
        return 0;
    }
    // [b28d-solo-CSecretShop] total 声明 int：ORIG 0x085fb9f8 `cmp -0x10(%ebp),%eax;
    // setl` 为内存直比；unsigned 局部 + (int) cast 会多一条 `mov %edx` 中转（P41 变体）。
    int total = sale->m_price * count;
    if (user->getCurCharacMoney() < total) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 10);
        return 0;
    }
    Inven_Item item;
    int slot = InsertItemIntoInventory(user, itemIdx, count, item);
    if (slot < 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 4);
        return 0;
    }
    ((CInventory*)user->getCurCharacInvenW())
        ->use_money(total, (eMoneySubReason)0x27, 1);
    SendSecretShopBuyItem(user, slot, item, -1, 0, remain);
    LogValueStatistic(user, total);
    info.m_shopper.BuyItem(itemIdx, count);
    m_pStatistic->RecordPrice(*(int*)user->GetSecretShopData(),
                              user->getDungeonIdxAfterClear(), total);
    return 1;
}

// ---- CBuyItembyRecipe（配方购买规则） ----
// 无 virtual 析构；ctor 委托 IBuyRule(CSecretShopStatistic&)，vptr 置
// &_ZTVN10secretshop16CBuyItembyRecipeE+0x8（BuyItem 于 vtable[0]，ORIG 确认）。

CBuyItembyRecipe::CBuyItembyRecipe(CSecretShopStatistic& statistic)
    : IBuyRule(statistic)
{
}

bool CBuyItembyRecipe::BuyItem(CUser* user, SECRET_SHOP_INFO& info, int itemIdx,
                               int count)
{
    // [b28d-solo-CSecretShop] P0/P183：ORIG 0x085fbc52 先构造 pair 再直写
    // cubeStat.first/second（无 materialIdx/need 独立槽）；invenR/invenW/invenR2
    // 结果均无槽往返 ⇒ 全内联；remain2 复用 cubeStat.second 的 if/else 双写
    // （ORIG -0x24 两处 store）；need 跨 getCurCharacInvenW 由 %ebx 保活。
    SALE_INFO* sale = info.m_retailer.GetSaleInfo(itemIdx);
    unsigned int err = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x43);
    if (err != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, static_cast<char>(err));
        return 0;
    }
    if (sale == 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 0x11);
        return 0;
    }
    int remain = 0;
    if (!CheckLimit(info, itemIdx, count, remain)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 0x5f);
        return 0;
    }
    std::pair<int, int> cubeStat;
    cubeStat.first = sale->m_material;
    cubeStat.second = sale->m_materialCount * count;
    Inven_Item material;
    int materialSlot = ((CInventory*)user->getCurCharacInvenR())
                           ->GetInvenData(cubeStat.first, material);
    if (materialSlot == -1 || material.m_addInfo2 < cubeStat.second) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 10);
        return 0;
    }
    Inven_Item item;
    int slot = InsertItemIntoInventory(user, itemIdx, count, item);
    if (slot < 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 4);
        return 0;
    }
    ((CInventory*)user->getCurCharacInvenW())
        ->delete_item((INVEN_TYPE)1, materialSlot, cubeStat.second,
                      (eItemDelReason)0x1a, 1);
    const Inven_Item* ref = ((const CInventory*)user->getCurCharacInvenR())
                                ->GetInvenRef(1, materialSlot);
    if (ref == 0)
        cubeStat.second = 0;
    else
        cubeStat.second = ref->m_addInfo2;
    SendSecretShopBuyItem(user, slot, item, cubeStat.first, cubeStat.second, remain);
    LogCubeStatistic(user, cubeStat);
    info.m_shopper.BuyItem(itemIdx, count);
    return 1;
}


// ---- CSecretShop（ORIG 布局：map<eBuyRule,IBuyRule*> +0x00、
//      CMTRand* +0x18、CSecretShopStatistic +0x1c） ----

// ---- SecretShopScript 跨类方法（CDataManager +0xa700，ORIG 真实符号；
//      声明见 CDataManager.h，本 TU 直接真实调用） ----

CSecretShop::~CSecretShop()
{
    // ORIG 顺序：先 ::operator delete(m_pRand)（_ZdlPv），再遍历 map 用
    // ::operator delete 释放每个无 virtual 析构的 IBuyRule 指针，
    // m_rules/m_statistic 的析构由成员声明顺序自动完成。
    if (m_pRand != 0)
        ::operator delete(m_pRand);
    for (std::map<eBuyRule, IBuyRule*>::iterator it = m_rules.begin();
         it != m_rules.end(); ++it)
        ::operator delete(it->second);
    m_rules.clear();
}

IBuyRule* CSecretShop::GetRule(eBuyRule rule)
{
    // ORIG：`if (it != end) return it->second; return 0;`（ne 比较，非 eq+取反）。
    std::map<eBuyRule, IBuyRule*>::iterator it = m_rules.find(rule);
    if (it != m_rules.end())
        return it->second;
    return 0;
}

int CSecretShop::LotteryNpc(int dungeonIdx, int level, int price)
{
    unsigned long randomSeed = 10000;
    int randVal = m_pRand->randInt(randomSeed);
    int npcIdx = 1000;
    // [b28-L14] P0 清绑：原 `register unsigned char fv asm("eax") … fv ^= 1`
    // 属禁用形，改内联否定直测（P168a① 裸形；char callee 的 sete 物化差记 N）。
    if (!G_CDataManager()->m_secretShop.GetNpcByDungeonIdx(npcIdx, randVal,
                                                           dungeonIdx))
        G_CDataManager()->m_secretShop.GetNpcByDungeonLev(npcIdx, randVal,
                                                          level);
    if (npcIdx != 0 && npcIdx != 1000)
        m_statistic.RecordShow(npcIdx, dungeonIdx, price);
    return npcIdx;
}

void CSecretShop::LotteryItems(std::vector<SALE_INFO>& out, int dungeonIdx,
                               int level, int price)
{
    // 判词·AE（本轮修复）：ORIG 56 = 我们 56，gap 0。
    // P48 no-CSE：ORIG 每处 use 各自 fresh G_CDataManager()（GetItemByDungeonIdx /
    // GetItemByDungeonLev 各一处，共 2 次）。
    // m_pRand 即 PvPTypes.h 权威全局 CMTRand；ORIG 0x085fac85 event =
    // s_event_manager->GetRepeatEvent(0x3a)->IsEventing(0)（vtable slot 13，
    // CEventBase 权威定义经 CGuildServerProxy.h；ev 无槽往返 ⇒ P0 无具名局部，
    // 匿名链式虚调用复现 ORIG `mov (%eax),%edx; add $0x34,%edx; mov (%edx),%edx;
    // mov %eax,(%esp); call *%edx`）。
    bool event = GlobalData::s_event_manager->GetRepeatEvent(0x3a)->IsEventing(0);
    // [b28d-solo-CSecretShop] 尾块：ORIG 0x085facd6 `test %al,%al; jne <nop 岛>` +
    // 0x085fad14 `test %al,%al; jmp <epilogue>; nop` ⇒ 平铺双早返
    // `if (f() != 0) return;`（P63：散布 return 产 jmp/nop 中继岛；4.1 把第二处
    // 等价 jne 折成 jmp）。
    if (G_CDataManager()->m_secretShop.GetItemByDungeonIdx(m_pRand, out, dungeonIdx, level, event) != 0)
        return;
    if (G_CDataManager()->m_secretShop.GetItemByDungeonLev(m_pRand, out, dungeonIdx, price, event) != 0)
        return;
}

void CSecretShop::CheckLottery()
{
    // [b28d-solo-CSecretShop] p167 定位（ORIG 56 calls / 旧实现 48）：
    // ① MISSING `_Hashtable_iterator::ptEv`×4 ⇒ ORIG 每处 `it->second` 重读（无
    //    `CDungeon* dungeon` 缓存，P183）；② MISSING `__normal_iterator::deEv`×2/
    //    `ppEv`/`ne`/`begin`/`end` + EXTRA `vector::ixEj`×2 ⇒ 内层遍历是**迭代器**
    //    形（`(*lt).m_itemIdx`，P167 星点判据）；③ EXTRA `_Rb_tree_iterator::eq` ⇒
    //    正极性 `c != counts.end()`（P220）；④ ORIG make_pair 实例 `_ZSt9make_pairIiiE`
    //    （右值版）⇒ 实参右值化。
    // 判词·N（本轮复核）：p167 CLEAR（56=56，仅 C1/C2、D1/D2 别名伪差）；
    // gap -2（304 vs 306）= 4.4 对 make_pair<int,int>(int&&,int&&) 的 xvalue
    // 实参 materialize 读值临时（ORIG 0x08a81e8b 直传 &(*lt).m_itemIdx）；已把
    // `(int)` cast 改 P234 形 `static_cast<int&&>`（符号与 ORIG 一致，仍 +2 条
    // ⇒ 跨版本 codegen 封顶）。尾部 setcc-fold（ORIG `setb;test;jne` vs 4.4
    // 直跳+常量岛，P197④ 镜像负例）与 N82/N89 失真并存；语义完成。
    // ORIG 0x85fad20：统计工具 —— 对 CDataManager 地下城表中每个地下城，
    // 以 dungeonIdx=0x3ea(1002)、level=get_index()、price=get_standard_level()
    // 抽取 10000 次 LotteryItems，统计各商品出现次数，并把百分比写入
    // secretshop_lottery.txt。
    std::vector<SALE_INFO> list;
    std::map<int, int> counts;
    FILE* fp = fopen("secretshop_lottery.txt", "wt");
    if (fp == 0)
    {
        return;
    }
    // [b28d-solo-CSecretShop] P0/P183：ORIG 0x085fad68（begin 前）与 0x085fb0d5
    // （end 前）各 fresh 调一次 G_CDataManager ⇒ 无 `hash_map& dungeons` 缓存局部。
    for (__gnu_cxx::hash_map<int, CDungeon*>::iterator it =
             G_CDataManager()->m_dungeonList->m_dungeonMap.begin();
         it != G_CDataManager()->m_dungeonList->m_dungeonMap.end(); ++it)
    {
        counts.clear();
        for (int i = 0; i < 10000; ++i)
        {
            LotteryItems(list, 0x3ea, it->second->get_index(),
                         it->second->get_standard_level());
            for (std::vector<SALE_INFO>::iterator lt = list.begin();
                 lt != list.end(); ++lt)
            {
                std::map<int, int>::iterator c =
                    counts.find((*lt).m_itemIdx);
                if (c != counts.end())
                {
                    ++c->second;
                }
                else
                {
                    // [b28d-solo-CSecretShop] ORIG 0x085fae8b `call deEv; mov
                    // %eax,%edx; …; mov %edx,0x4(%esp); call make_pair<int,int>
                    // (int&&,int&&)`——实参即 `&(*lt).m_itemIdx`（+0 字段，无读值
                    // 临时）。`(int)` cast 会先读值落临时（+2 条）；P234 形
                    // `static_cast<int&&>` 直接绑定右值引用，符号与 ORIG 一致。
                    counts.insert(std::make_pair(
                        static_cast<int&&>((*lt).m_itemIdx), 1));
                }
            }
            list.clear();
        }
        fprintf(fp, "Dungeon : %s(idx:%d, lev:%d)\n",
                it->second->GetDungeonName(), it->second->get_index(),
                it->second->get_standard_level());
        int total = 0;
        for (std::map<int, int>::iterator c = counts.begin();
             c != counts.end(); ++c)
        {
            total += c->second;
        }
        for (std::map<int, int>::iterator c = counts.begin();
             c != counts.end(); ++c)
        {
            fprintf(fp, "- %d(%d, %.2f%%)\n", c->first, c->second,
                    100.0 * ((double)c->second / (double)total));
        }
        fputc('\n', fp);
    }
    fclose(fp);
}

void CSecretShop::BuyItem(CUser* user, SECRET_SHOP_INFO& info, int itemIdx, int count)
{
    // [b28d-solo-CSecretShop] P0：ORIG 0x085fb241/0x085fb24e 中 dungeonIdx 留 %ebx
    // 跨 GetSecretShopData 调用、npcIdx 直读解引用，均无槽往返 ⇒ 内联实参（右→左
    // 求值序与 ORIG 一致）；局部 sale/rule/item 有槽（-0x18/-0x14/-0x10）保留。
    SALE_INFO* sale = info.GetSaleInfo(itemIdx);
    if (sale == 0)
        return;
    IBuyRule* rule = GetRule((eBuyRule)sale->m_rule);
    if (rule == 0)
        return;
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
        return;
    if (!item->is_stackable())
        count = 1;
    // [b28d-solo-CSecretShop] ORIG 0x085fb21f `mov %al,-0x9; cmpb $0x0,-0x9; je`
    // ⇒ BuyItem 结果有具名 bool 局部；0x085fb22b `movzbl m_bClear; xor $1; test; je`
    // ⇒ 字段 bool 裸否定（P292 档②，勿写 `!= 1`）。
    bool bBuy = rule->BuyItem(user, info, itemIdx, count);
    if (bBuy) {
        if (!info.m_bClear) {
            m_statistic.RecordBuy(*(int*)user->GetSecretShopData(),
                                  user->getDungeonIdxAfterClear());
            info.m_bClear = 1;
        }
    }
}

void CSecretShop::SendSecretShopNpc(CUser* user, int npcIdx)
{
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x114);
    ((InterfacePacketBuf*)&packet)->put_int(npcIdx);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    user->Send(packet);
}

void CSecretShop::SendSecretShopItemList(CUser* user, std::vector<SALE_INFO>& list)
{
    int count = (int)list.size();
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0x115);
    ((InterfacePacketBuf*)&packet)->put_int(count);
    for (int i = 0; i < count; ++i) {
        ((InterfacePacketBuf*)&packet)->put_int(list[i].m_itemIdx);
        ((InterfacePacketBuf*)&packet)->put_byte(list[i].m_rule);
        ((InterfacePacketBuf*)&packet)->put_int(list[i].m_price);
        ((InterfacePacketBuf*)&packet)->put_int(list[i].m_limit);
        ((InterfacePacketBuf*)&packet)->put_int(list[i].m_material);
        ((InterfacePacketBuf*)&packet)->put_int(list[i].m_materialCount);
        ((InterfacePacketBuf*)&packet)->put_byte(list[i].m_field18);
    }
    ((InterfacePacketBuf*)&packet)->finalize(true);
    user->Send(packet);
}

void CSecretShop::SendSecretShopStatistic()
{
    m_statistic.SendSecretShopStatistic();
    m_statistic.Clear();
}

}  // namespace secretshop
// ============================================================================
// SecretShopScript::GetItemByDungeonIdx / GetItemByDungeonLev（ORIG 0x08a82a0e /
// 0x08a82b34）。依赖链：GetItems -> CopyItem/IsExistItem/GetDomainRate，GetLevelIdx，
// GetItemByEvent -> GetItems。全部按 class_func_reports/SecretShopScript 忠实转写。
// ============================================================================
bool SecretShopScript::GetItemByDungeonIdx(CMTRand* rand,
                                           std::vector<secretshop::SALE_INFO>& out,
                                           int dungeonIdx, int level, bool event)
{
    std::map<int, stNpcSales>::iterator npcIt = m_dungeonNpcMap.find(dungeonIdx);
    if (npcIt == m_dungeonNpcMap.end())
        return 0;
    std::map<int, stDungeonSales>::iterator dunIt =
        npcIt->second.m_dungeonSales.find(level);
    if (dunIt == npcIt->second.m_dungeonSales.end())
        return 0;
    // [b28-L14] P0 清绑：原 `register unsigned char fv asm("eax")` 禁用。
    // ORIG `call GetItems; xor $0x1,%eax; test %al,%al; je <主>` ⇒ bool callee
    // 裸否定早返（P168a①）。
    if (!GetItems(rand, out, dunIt->second))
        return false;
    if (event)
        GetItemByEvent(rand, out);
    return true;
}

bool SecretShopScript::GetItemByDungeonLev(CMTRand* rand,
                                           std::vector<secretshop::SALE_INFO>& out,
                                           int dungeonIdx, int level, bool event)
{
    std::map<int, stNpcSales>::iterator npcIt = m_dungeonNpcMap.find(dungeonIdx);
    if (npcIt == m_dungeonNpcMap.end())
        return 0;
    int levelIdx = GetLevelIdx(level);
    std::map<int, stDungeonSales>::iterator dunIt =
        npcIt->second.m_eventSales.find(levelIdx);
    if (dunIt == npcIt->second.m_eventSales.end())
        return 0;
    // [b28-L14] P0 清绑（同上）。
    if (!GetItems(rand, out, dunIt->second))
        return false;
    if (event)
        GetItemByEvent(rand, out);
    return true;
}

bool SecretShopScript::GetItemByEvent(CMTRand* rand,
                                      std::vector<secretshop::SALE_INFO>& out)
{
    std::vector<secretshop::SALE_INFO> list;
    // [b28-L14] P0 清绑（原 `register unsigned char fv asm("eax")` 全域禁用）。
    // ORIG 0x8a82c70：`call GetItems; xor $0x1,%eax; test %al,%al;
    // je <主路径>` + fall-through `mov $0,%ebx` ⇒ 裸否定早返形（P168a①）。
    if (!GetItems(rand, list, m_eventDungeonSales))
        return false;
    for (int i = 0; i < static_cast<int>(list.size()); ++i) {
        list[i].m_field18 = 1;
        out.push_back(list[i]);
    }
    return true;
}

bool SecretShopScript::GetItems(CMTRand* rand,
                                std::vector<secretshop::SALE_INFO>& out,
                                stDungeonSales& sales)
{
    // [b28d-solo-CSecretShop] P196/P0：ORIG 0x08a82d5e `cmp %edx,%eax; setb` 无符号序；
    // 0x08a82d58 的 SALE_INFO ctor 在**循环外**（sale 复用，旧实现每轮构造一次）；
    // 第二分支为 `for(; guard<=0x2710 && size<max; ++guard)` 尾条件形（ORIG
    // 0x08a82f35 `addl $1,-0xc` → 0x08a82f39 `cmpl $0x2710; jg` → 0x08a82f42 size
    // 检查），变量槽 seed(-0x1c ulong)/randVal(-0x14 uint)/acc(-0x10 uint)/guard(-0xc)。
    // 判词·N（本轮复核）：IsExistItem 判定改正向 goto 形消 4.4 的 `xor $1`
    // （ORIG 0x08a82eb4 `test %al,%al; jne <break 岛>`）⇒ 159→158，gap 0。
    // 残余 = N82（两分支 it/end 独立槽，全帧槽号浅移 0x20）+ setcc-fold
    // （ORIG 尾 `setb;test;jne` vs 4.4 `jae`+常量岛，P197④ 镜像负例）。
    if (sales.m_sales.size() < (unsigned int)sales.m_maxCount) {
        // 商品数少于上限：直接全部列出
        secretshop::SALE_INFO sale;
        for (std::map<int, stSaleInfo>::iterator it = sales.m_sales.begin();
             it != sales.m_sales.end(); ++it) {
            CopyItem(sale, it->second);
            out.push_back(sale);
        }
    }
    else {
        // 按权重随机抽取，直到填满 m_maxCount 或商品重复
        unsigned long seed = 0;      // -0x1c
        unsigned int randVal = 0;    // -0x14
        int guard = 0;
        secretshop::SALE_INFO sale;
        for (; guard <= 0x2710 &&
               out.size() < (unsigned int)sales.m_maxCount; ++guard) {
            seed = (unsigned long)GetDomainRate(sales);
            randVal = (unsigned int)rand->randInt(seed);
            unsigned int acc = 0;
            for (std::map<int, stSaleInfo>::iterator it = sales.m_sales.begin();
                 it != sales.m_sales.end(); ++it) {
                acc += (unsigned int)it->second.m_rate;
                if (randVal < acc) {
                    // [b28d-solo-CSecretShop] ORIG 0x08a82eb4 `call IsExistItem;
                    // test %al,%al; jne <break 落点>`（bool callee 直判、无 xor）。
                    // 4.4 对 `!f()`/`f()==0` 恒插 `xor $1`（P168a②/N122 实测）⇒
                    // 用正向条件 goto 复现 ORIG 的 `test;jne` 直分支形。
                    if (IsExistItem(out, it->second.m_itemIdx) != 0)
                        goto skip_copy;
                    CopyItem(sale, it->second);
                    out.push_back(sale);
skip_copy:
                    break;
                }
            }
        }
    }
    return 1;
}

bool SecretShopScript::IsExistItem(std::vector<secretshop::SALE_INFO>& out,
                                   int itemIdx)
{
    // [b28d-56 判词·AE] ORIG 0x08a82f6a 为 std::find：begin→-0x14、end→-0x10、
    // find sret→-0x18、end→-0xc 四槽（P96 非 const iterator，P293 find 先/end 后），
    // 尾 `test %al; je; mov $1; jmp; mov $0`。旧索引 for 循环仅 28 条、缺全部
    // 迭代器调用（43 vs 28）。
    std::vector<secretshop::SALE_INFO>::iterator it =
        std::find(out.begin(), out.end(), itemIdx);
    if (it != out.end())
        return true;
    return false;
}

int SecretShopScript::GetDomainRate(stDungeonSales& sales)
{
    int total = 0;
    for (std::map<int, stSaleInfo>::iterator it = sales.m_sales.begin();
         it != sales.m_sales.end(); ++it)
        total += it->second.m_rate;
    return total;
}

int SecretShopScript::GetLevelIdx(int level)
{
    for (std::set<stLevelSection>::iterator it = m_levelSections.begin();
         it != m_levelSections.end(); ++it) {
        if ((*it).m_minLev <= level && level <= (*it).m_maxLev)
            return (*it).m_levelIdx;
    }
    return 0;
}

int SecretShopScript::GetRandItemPrice(int price)
{
    int m = rand() % 3;
    if (m == 0)
        return price + (int)((double)price * ((double)m_priceVarPercent / 100.0));
    if (m == 1)
        return price - (int)((double)price * ((double)m_priceVarPercent / 100.0));
    return price;
}

void SecretShopScript::CopyItem(secretshop::SALE_INFO& sale, stSaleInfo& info)
{
    sale.m_itemIdx = info.m_itemIdx;
    sale.m_rule = info.m_rule;
    sale.m_limit = info.m_limit;
    if (sale.m_rule == (char)0) {
        sale.m_price = GetRandItemPrice(info.m_price);
    } else if (sale.m_rule == (char)1) {
        sale.m_material = info.m_price;
        sale.m_materialCount = info.m_materialCount;
    }
}

// ---- SecretShopScript::clear（ORIG 0x8372020 W）----
void SecretShopScript::clear()
{
    m_priceVarPercent = 0;   // +0x80
    m_int84 = 0;             // +0x84
    m_npcMap.clear();        // +0x00
    m_levelSections.clear(); // +0x30
    m_levelNpcMap.clear();   // +0x18
    m_dungeonNpcMap.clear(); // +0x48
}

// ---- SecretShopScript::verifyNpc（ORIG 0x8a82804）----
void SecretShopScript::verifyNpc(std::multimap<int, stDungeonNpc>& npcMap)
{
    int total = 0;
    for (std::multimap<int, stDungeonNpc>::iterator it = npcMap.begin();
         it != npcMap.end(); ++it)
    {
        std::pair<std::multimap<int, stDungeonNpc>::iterator,
                  std::multimap<int, stDungeonNpc>::iterator> range =
            npcMap.equal_range(it->first);
        total = 0;
        for (std::multimap<int, stDungeonNpc>::iterator r = range.first;
             r != range.second; ++r)
        {
            total += r->second.m_rate;
        }
        if (total != 10000)
        {
            printf("[SecretShop Error] NPC ratio is wrong!(index:%d)\n",
                   range.first->second.m_field0);
        }
    }
}

void* CUser::GetSecretShopData()
{
    CParty* party = static_cast<CParty*>(GetParty());
    if (party != 0)
        return party->GetSecretShopData();
    return 0;
}
