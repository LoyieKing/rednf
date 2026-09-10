// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Store/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
// df_game_r Game/ Store 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 对照 docs/class_func_reports/Store.md：
//   GetSellItemPrice(Inven_Item&,...) 0x08618ea0
//   GetSellItemPrice(CUser*,...)      0x08619008
//   repair_equip                      0x08619cec
//   user_buy_item                     0x08618a44
//   user_sell_item                    0x086193f8
#include "CInventory.h"
#include "CDataManager.h"
#include "CItemList.h"
#include "CGameManager.h"
#include "CEnvironment.h"   // GlobalInstance<Store>（ORIG G_Store 调 inst_ptr）
#include "CDungeon.h"
#include "GlobalData.h"
#include "CServerProxyMgr.h"
#include <string.h>
#include <vector>
#include "CHackLog_InvitationPaperSellToNPC.h"   // user_sell_item hack 检查
#include "CHackAnalyzer.h"                        // addServerHackCnt
#include "HistoryLog.h"                           // WriteSellItem

#include "CStatisticServerProxy.h"
#include "Packet_DeathTower_Statistic_Value.h"   // 类带 PacketHeader(0xc3b,0x15) ctor（ORIG C1 0x0860ffc2）

// ---------------------------------------------------------------------------
// user_buy_item(CUser*, int itemId, int count)
// ---------------------------------------------------------------------------
// [b28d-solo-Store] 真语义 bug（ORIG 0x08618b0e 实证）：可堆叠路的类型分派是
// 三次独立**虚调 GetItemType()**（vptr+0xc ⇒ _ZTV10CEquipItem word[5]=
// CEquipItem::GetItemType 0x08514d26），映射 field1=3/10/4；旧实现误用非虚
// GetAttachType()（0x080f12e2）且缓存成具名局部 + else-if 链。
// 另修：insertItemIntoInventory 末两 bool 实参极性（ORIG 0x08618c52 压 (0,1,0)
// ⇒ (item, reason0, true, false)）；getCurCharacInvenR/W 改链式（P183）；
// cMyTrace 改全表达式临时链（P227）；第三实参改直读 m_addInfo2（ORIG -0x6e 直读）。
#include "Store/user_buy_item.inc"
{
    if (user->CheckInTrade())
    {
        return 0x13;
    }

    CItem* pItem = G_CDataManager()->m_itemList->find_item(itemId);
    if (pItem == NULL)
    {
        return 0x11;
    }

    int totalPrice;
    if (pItem->is_stackable())
    {
        totalPrice = pItem->get_price() * count;
    }
    else
    {
        totalPrice = pItem->get_price();   // ORIG +0x71 分支：不可堆叠同样取价（不乘 count）
    }
    if (user->getCurCharacInvenR()->get_money() < totalPrice)
    {
        return 0xa;
    }

    Inven_Item item;
    item.m_addInfo = itemId;

    if (pItem->is_stackable())
    {
        // 可堆叠物品：ORIG 0x08618b0e 三次独立虚调 GetItemType()（P48）。
        if (pItem->GetItemType() == 1)
            item.m_field1 = 3;
        if (pItem->GetItemType() == 2)
            item.m_field1 = 10;
        if (pItem->GetItemType() == 8)
            item.m_field1 = 4;
        item.set_add_info(count);
        if (count > 0x3e7)
        {
            // ORIG 0x08618b89：ctor 前先读 item.m_addInfo 入 %ebx 跨 ctor 保活
            // ⇒ 全表达式临时链（P227）。
            // [P2S6 2026-09-11 P139] ORIG 0x8ce2180 size=0x2b 的 prett 串与手写串
            //   逐字节相同（pstr.py 实测）⇒ 换回 __PRETTY_FUNCTION__（发射
            //   _ZZN5Store13user_buy_itemEP5CUseriiE19__PRETTY_FUNCTION__，零语义差）。
            cMyTrace(__PRETTY_FUNCTION__, 0x3d, 5)
                ("Store::user_buy_item,  ITEM #%d, %d", item.m_addInfo, count);
        }
        item.m_fieldb = 0;
        item.ResetItemAttr();
    }
    else
    {
        // 装备类
        CEquipItem* pEquip = static_cast<CEquipItem*>(pItem);
        if (pEquip->m_itemType == 0xb)
        {
            return 0x17;
        }
        if (pEquip->GetAttachType() == 3)
        {
            item.m_field0 = 1;
        }
        item.m_field1 = 1;
        item.set_add_info(count);
        item.m_fieldb = (unsigned short)pEquip->get_endurance();
        item.ResetItemAttr();
    }

    // ORIG 0x08618c52 压栈 (0x44=reason 0, 0x48=1, 0x4c=0) ⇒ (true, false)。
    int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
        item, eItemAddReason_0, true, false);
    if (slot == -1)
    {
        return 4;
    }

    if (!user->getCurCharacInvenW()->use_money(totalPrice, eMoneySubReason_0, true))
    {
        return 0xa;
    }

    // 统计
    int sellPrice = 0;
    // ORIG 0x08618d38 直读 item+0x07（m_addInfo2）并 cwtl ⇒ (short)item.m_addInfo2。
    GetSellItemPrice(item, pItem, (short)item.m_addInfo2, false, sellPrice);
    GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)6, user, sellPrice);
    GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0xd, user, totalPrice);

    // 发送结果包
    PacketGuard packet;
    packet.put_header(1, 0x15);
    packet.put_byte(1);
    packet.put_int(user->getCurCharacInvenR()->get_money());
    packet.put_short(slot);
    packet.put_int(itemId);
    packet.put_int(count);
    packet.finalize(true);
    user->Send(packet);

    return 0;
}

// ---- 前向声明 ----
enum DEATHTOWER_VALUE_STATISTIC_FIELD
{
    DEATHTOWER_VALUE_STATISTIC_FIELD_0 = 0,
    DEATHTOWER_VALUE_STATISTIC_FIELD_9 = 9
};

class CDeathTowerValueStatistic
{
public:
    bool SendDeathTowerValueStatistic(DEATHTOWER_VALUE_STATISTIC_FIELD field, char a, char b, unsigned int c);
};

extern CDeathTowerValueStatistic* GetInstanceDeathTowerValueStatistic()
{
    static CDeathTowerValueStatistic instance;
    return &instance;
}

// [b28d-solo-Store] 真语义 bug 修复（ORIG 0x0860f342 实证）：field != 10 时 ORIG 不早退，
// 而是 packet.m_counterIdx = field 后照常发送并 return true（+0xcc 块 `mov 0xc(%ebp),%eax;
// mov %eax,-0x10(%ebp)` 直落共享尾 +0xd2）。旧实现 `return false` 属漏发包。
// P0：ORIG 无独立 f 局部槽（m_counterIdx 即 -0x10 槽，3/4/5/6/field 直写成员）。
#include "Store/SendDeathTowerValueStatistic.inc"
                                                              char a, char b, unsigned int c)
{
    Packet_DeathTower_Statistic_Value packet;
    packet.m_type = a;
    packet.m_level = (short)b;

    if (field == (DEATHTOWER_VALUE_STATISTIC_FIELD)10)
    {
        // ORIG 0x0860f37a：`mov 0x18(%ebp),%eax; cmp $1; je; cmp $1; jb; cmp $2;
        // je; cmp $3; je; jmp default` 无栈槽线性决策树 + 叶块按 case 升序
        // ⇒ 源形是 switch(c)（P251/P46），不是 if-else 链。
        switch (c)
        {
        case 0:
            packet.m_counterIdx = 3;
            c = 1;
            break;
        case 1:
            packet.m_counterIdx = 4;
            c = 1;
            break;
        case 2:
            packet.m_counterIdx = 5;
            c = 1;
            break;
        case 3:
            packet.m_counterIdx = 6;
            c = 1;
            break;
        default:
            // ORIG 0x0860f3db：logFormat(1, "Statistics.cpp", __PRETTY_FUNCTION__,
            // 0x5d5, "name(%d), 존재하지 않는 아이템 등급", field)
            LogManager::logFormat(
                1, "Statistics.cpp", __PRETTY_FUNCTION__, 0x5d5,
                "name(%d), \xc1\xb8\xc0\xe7\xc7\xcf\xc1\xf6 \xbe\xca\xb4\xc2 \xbe\xc6\xc0\xcc\xc5\xdb \xb5\xee\xb1\xde", field);
            return false;
        }
    }
    else
    {
        packet.m_counterIdx = (unsigned int)field;
    }

    packet.m_value = c;

    // ORIG 0x0860f41a：`lea -0x1d(%ebp),%ebx` 先物化 &packet 并跨 GetServerProxy 调用保活
    // （P52 链式 receiver+R2L 实参 callee-saved 独占形），故必须写成单条链式表达式；
    // 具名 proxy 局部会把它降级为栈槽中转（mov %eax,-0xc 回读）。无条件发送、返回 true。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket((char*)&packet, 0x15);
    return true;
}

// 修理价格系数（由 InitEquipmentScript 从脚本加载）
static float g_repairRate = 0.0f;

#include "Store/getEquipmentRepairPrice.inc"
                            int growthGrade, bool flag, float rate)
{
    if (maxEndurance == 0 || maxEndurance == curEndurance)
        return 0;

    float price = 0;
    price = (float)(((growthGrade + 5) * baseCost) / 10)
            * g_repairRate / (float)maxEndurance * (float)(maxEndurance - curEndurance);
    price *= rate;
    return (int)price;
}

// getItemSellPrice 的唯一实现归 ItemUpgradeVtableB2.cpp（ORIG 0x0898c86e）；
// 本 TU 的 Store::GetSellItemPrice 仅调用该共享自由函数。
#include "Store/getItemSellPrice.inc"

Store* G_Store()
{
    return GlobalInstance<Store>::inst_ptr();
}

// ---------------------------------------------------------------------------
// GetSellItemPrice(Inven_Item&, const CItem*, short, bool, int&)
// ---------------------------------------------------------------------------
// [b28d-solo-Store] 真语义 bug：ORIG 0x08618f79 比较对象是 item.m_fieldb(+0xb)，
// 旧实现误用 count（`if (get_endurance() >= count) item.m_fieldb = ...`）——修满耐久
// 判定完全错位。另修正：`if (!isEquipableItemType())`（bool callee + P292 xor 形）、
// 正极性短路包裹（P214①）、m_itemType 判序 ==0xb 先于 <10、删 cnt 具名局部（P0）。
#include "Store/GetSellItemPrice.inc"
                            bool flag, int& outPrice)
{
    // ORIG 0x08618ebc：`call isEquipableItemType; xor $1; test %al,%al; je <装备路>`
    // ⇒ 源码 `if (!item.isEquipableItemType())`（callee 返回 bool，P292/P168a①）。
    if (!item.isEquipableItemType())
    {
        // ORIG 0x08618ed7：`cmp %edx,%eax; jl <true>; cmpw $0,-0x2c; jg <false>;
        // <true> mov $1; jmp; <false> mov $0; test; je <body>` ⇒ 负极性早返
        // （P59 guard 形：fail 块直落在前，body 在 je 目标处）。
        if ((int)item.get_add_info() < (int)count || count <= 0)
            return 0x11;
        // ORIG 0x08618f37 后 `movswl -0x2c(%ebp),%edx; imul %eax,%edx`
        // ⇒ 源码把 getItemSellPrice(...) 写在乘法左侧（call 后读 count 槽）。
        outPrice = getItemSellPrice(pItem->GetSellPrice(), 0, 0,
                                    pItem->IsRandomOption()) * count;
    }
    else
    {
        const CEquipItem* pEquip = static_cast<const CEquipItem*>(pItem);
        // ORIG 0x08618f5c：先 `cmp $0xb,%eax; je <0x17>` 再 `cmp $9; jg <继续>`
        // ⇒ `m_itemType == 0xb || m_itemType < 10`。
        if (pEquip->m_itemType == 0xb || pEquip->m_itemType < 10)
            return 0x17;
        // ORIG 0x08618f79：比较对象是 item.m_fieldb（+0xb，ushort），非 count。
        if ((int)item.m_fieldb > (int)pEquip->get_endurance())
            item.m_fieldb = (unsigned short)pEquip->get_endurance();
        outPrice = getItemSellPrice(pItem->GetSellPrice(), item.m_fieldb,
                                    pEquip->get_endurance(),
                                    pItem->IsRandomOption());
    }
    return 0;
}

// ---------------------------------------------------------------------------
// GetSellItemPrice(CUser*, char invenType, short slot, short count, bool flag,
//                  int& outPrice, int& outCount) —— ORIG 0x08619008
// [b28d-solo-Store] 真语义 bug：ORIG 0x086193b5 写第 6 参 = pItem->get_index()，
// 并把第 7 参直接作 5 参重载的价格出参（0x24(%ebp)→0x14(%esp)）；旧实现写成
// outCount = get_index() 且经局部 price 中转 → 两个引用出参交叉错位。
// 另修：invenType 分派是 switch（ORIG 0x08619037 线性决策树，P251/P46）；
// default 返 0x11（旧写 0x13）；find_item 走 CDataManager::find_item（非 m_itemList）。
// ---------------------------------------------------------------------------
#include "Store/GetSellItemPrice_1.inc"
                            bool flag, int& outPrice, int& outCount)
{
    Inven_Item item;

    switch (invenType)
    {
    case 0:
        item = user->getCurCharacInvenR()->GetInvenSlot(1, slot);   // 消息 0 → type 1
        break;
    case 2:
        item = user->getCurCharacCargoR()->get_cargo_slot(slot);
        break;
    case 3:
        // creature slot
        item = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)0, slot);   // 消息 3 → type 0
        break;
    case 7:
        // avatar slot
        item = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)3, slot);   // 消息 7 → type 3
        if (item.m_field1 == 5)   // ORIG 分支独占检查：ITEM_TYPE==5 → 0x11（objdump 0x861932c）
            return 0x11;
        break;
    default:
        return 0x11;   // ORIG 0x0861933e：default 返 0x11
    }

    if (item.m_addInfo == 0)   // ORIG 共享段检查（find_item 前，m_addInfo 直读）
    {
        return 0x11;
    }
    // ORIG 0x0861936b：call CDataManager::find_item(int) const（非 m_itemList）。
    const CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);

    if (pItem == NULL)
    {
        return 0x11;
    }

    if (pItem->GetFinishPointPrice() != 0)
        return 0x17;
    // ORIG 0x086193a6：`call GetAttachType; cmp $0x2,%eax; jne <body>` 直跳
    // （4.4 对 if 比较恒物化 setne/test/je，P197④ 镜像族）⇒ 单 case switch（P286）。
    switch (pItem->GetAttachType())
    {
    case 2:
        return 0x17;
    default:
        break;
    }
    // ORIG 0x086193b5：第 6 参（0x20(%ebp)）= get_index()。
    outPrice = pItem->get_index();
    // ORIG 0x086193c2：第 7 参（0x24(%ebp)）直传下层价格出参。
    return GetSellItemPrice(item, pItem, count, flag, outCount);
}

// ---------------------------------------------------------------------------
// user_sell_item(CUser*, char invenType, short slot, short count) —— ORIG 0x86193f8
// ---------------------------------------------------------------------------
// [b28d-solo-Store] 真语义 bug：hack 检查的两个函数是**静态**调用（ORIG 0x08619c43
// `call IsInvitationPaper(int)` 前无 this 压栈、0x08619c4c `call GetHackType()` 亦无
// this）——旧实现 `CHackLog_InvitationPaperSellToNPC hackLog; hackLog.X()` 会多压 this
// 且引入对象槽。配套 .h：GetHackType 加 static（mangled 不变）。
// 另修：两处分派改 switch（ORIG 0x086194ea/0x08619708 线性决策树，P251/P46）；
// 删 isState5/int t 具名局部（P0 内联）；getCurCharacInvenW/R 改链式（P183）；
// cMyTrace 改全表达式临时链（P227，ORIG 实参在 ctor 前求值入 edi/esi/ebx）。
#include "Store/user_sell_item.inc"
{
    if (user->CheckInTrade())
    {
        return 0x13;
    }

    int price = 0;
    int ret = 0;
    int outCount = 0;
    ret = GetSellItemPrice(user, invenType, slot, count, user->get_state() == 5, price, outCount);
    if (ret != 0)
    {
        return ret;
    }

    if (!user->CheckMoney(outCount))
    {
        return 0x16;
    }

    Inven_Item item;
    // ORIG 0x086194ea：case 0/3/7 共用 GetInvenTypeFromItemSpace 路，case 2 走 cargo。
    switch (invenType)
    {
    case 0:
    case 3:
    case 7:
        item = user->getCurCharacInvenR()->GetInvenSlot(
            GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)invenType), slot);
        break;
    case 2:
        item = user->getCurCharacCargoR()->get_cargo_slot(slot);
        break;
    default:
        break;
    }

    // ORIG 0x08619708：delete_item 分派同 switch（case 升序 0/2/3/7）。
    switch (invenType)
    {
    case 0:
        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)1, slot, count, eItemDelReason_0, true))   // 消息 0 → type 1
        {
            // [P2S6 2026-09-11 P139] ORIG 0x8ce2140 size=0x3e（串=手写串逐字节同）
            //   ⇒ 五处 cMyTrace 全部换回 __PRETTY_FUNCTION__。
            cMyTrace(__PRETTY_FUNCTION__, 0x136, 5)
                ("Store::user_sell_item, delete_item failed , User ch=%d , %d %d",
                 user->getCurCharacNo(), slot, count);
            return 0x11;
        }
        break;
    case 2:
        if (!user->getCurCharacCargoW()->delete_item(slot, count, eItemDelReason_0))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x141, 5)
                ("Store::user_sell_item, delete_item failed 2, User ch=%d , %d %d",
                 user->getCurCharacNo(), slot, count);
            return 0x11;
        }
        break;
    case 3:
        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)0, slot, 1, eItemDelReason_0, true))   // 消息 3 → type 0
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x14d, 5)
                ("Store::user_sell_item, delete_item failed 3, User ch=%d , %d",
                 user->getCurCharacNo(), slot);
            return 0x11;
        }
        user->send_equip(slot);
        break;
    case 7:
        if (!user->getCurCharacInvenW()->delete_item(
                (INVEN_TYPE)3, slot, count, eItemDelReason_0, true))   // 消息 7 → type 3
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x167, 5)
                ("Store::user_sell_item, delete_item failed 4, User ch=%d , %d %d",
                 user->getCurCharacNo(), slot, count);
            return 0x11;
        }
        break;
    default:
        break;
    }

    if (outCount > 0x7a11f)
    {
        // ORIG 0x08619a23：get_money() 结果直接留 %edi（无槽往返）⇒ 无具名局部（P0），
        // 内联为 trace 最右实参（右→左求值序）。
        cMyTrace(__PRETTY_FUNCTION__, 0x172, 4)
            ("User(%s) Charac(%s) gain money(%d) by selling item(%d), old money is %d.",
             NumberToString(user->get_acc_id(), 0),
             user->getCurCharacName(), outCount, price,
             user->getCurCharacInvenR()->get_money());
    }

    user->getCurCharacInvenW()->gain_money(outCount, eMoneyAddReason_0, true, 0);

    if (!item.isEmpty())
    {
        CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
        // ORIG 0x08619b1a：两条独立守卫各自 `je <Add_RedeemInfo>` 直跳（P133），
        // 合并 && 会被 4.4 物化成 setne/sete 序列。
        if (pItem != 0)
        {
            if (pItem->is_stackable())
                item.m_addInfo2 = count;   // ORIG movswl count → item+0x7（stackable 覆写数量）
        }
        user->Add_RedeemInfo(item, outCount, true);
    }

    PacketGuard packet;
    packet.put_header(1, 0x18);
    packet.put_byte(1);
    packet.put_int(user->getCurCharacInvenR()->get_money());
    packet.put_byte(invenType);
    packet.put_short(slot);
    packet.put_short(count);
    packet.finalize(true);
    user->Send(packet);

    // ORIG 0x08619c43/0x08619c4c：两个函数均静态调用（无 this）；IsInvitationPaper
    // 返回类型 bool（ORIG `call; test %al,%al; je` 无 setne，P292/P315）。
    if (CHackLog_InvitationPaperSellToNPC::IsInvitationPaper(price))
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)CHackLog_InvitationPaperSellToNPC::GetHackType(),
            1, 0, 0);
    }
    HistoryLog::WriteSellItem((FILE*)user->m_field796f8, price, count);

    return 0;
}
// ---- repair_equip 辅助类型 ----
struct tagRepairItemInfo
{
    INVEN_TYPE m_type;       // 0=背包, 1=装备栏
    int m_slot;              // 槽位索引
    CEquipItem const* m_item;

    // ORIG C1/C2 0x0861b08e（W）：三形参顺序赋入（INVEN_TYPE, int, CEquipItem const*）。
    tagRepairItemInfo(INVEN_TYPE type, int slot, CEquipItem const* item)
        : m_type(type), m_slot(slot), m_item(item) {}
};

extern int getEquipmentRepairPrice(int baseCost, int curEndurance, int maxEndurance,
                                   int growthGrade, bool flag, float rate);

// ---------------------------------------------------------------------------
// repair_equip(CUser*, char invenType, short slot, ushort count, ushort& out) ——
// ORIG 0x08619cec（docs/class_func_reports/Store/repair_equip.md 汇编+反编译 C）。
// slot==-1：全修 —— 装备栏 3..8（invenType 1）+ 背包 10..0x15（invenType 0）扫描，
//   收集需修装备（maxEnd!=0 且 curEnd!=maxEnd，objdump +0xc00/+0xe90），累计价格后
//   use_money(totalCost, reason 1, log 1)；成功先统计(0x14)+死亡之塔(仅 state==10、
//   dungeon idx!=11000) 上报，再逐件重取槽写满耐久 update_item，最后对背包
//   10..0x15 发 SendUpdateItem(1, space 3)；失败返 10。返回 0x13=交易中。
// 单件：msg invenType 0→装备栏(type 1)/2→cargo/3→背包(type 0)，其余返 0x11；
//   空槽或非 ITEM_TYPE_EQUIP 返 0x11；*out=当前耐久；查表空/满耐久返 0x11；
//   count!=0xffff 时从 GetInvenSlot(1,count) 取工具（须 ITEM_TYPE_2 且
//   m_addInfo==0x1f77/0x201f，否则 0x15）→ use_item(count,1,0) 成功扣 1 并把
//   修理费置 0（失败 0x16）；use_money 失败返 10；耐久写满后按 msg 空间
//   update_item + SendUpdateItem(1, space 0/2/3)。
// 字段口径：判空/查表/工具 id 全程直读 +0x02 m_addInfo（item idx）；耐久为
//   +0x0b m_fieldb。m_addInfo2(+0x07, get_add_info) 是 stackable 数量域，勿混。
// ---------------------------------------------------------------------------
// [b28d-solo-Store] 判词：DIFF·N82/N85 封顶（语义完成）。
// 本轮修正（全部有 ORIG 实证）：① find_item 目标互换——第一循环（装备栏 3..8）是
// CItemList::find_item、第二循环（背包 10..0x15）是 CDataManager::find_item，旧实现写反；
// ② get_charac_level/get_endurance 逐次重读（各 +3/+4 调用点）；③ 循环内结果复用函数头
// item（4.4 对 `Inven_Item x=f()` 直用 sret 临时，N86；改赋值后逐字段拷贝形与 ORIG 一致，
// sret 临时槽 7 调用点复用同址）；④ 两处分派与 update_item 分派改 switch（ORIG 无槽
// movsbl+cmp 决策树，P251/P286）；⑤ if/else 块序交换（ORIG je 落 slot==-1 块，P59/P222）；
// ⑥ get_state/use_money/is_stackable 表达式直用（P0 无槽）；⑦ notTower 改 P158 形。
// 残余（p167 CLEAR 114=114 硬证据下）：帧 0x22c vs 0x1bc + 全帧槽号平移（N82/N95），
// rate 实参 4.1 flds/fstps 栈往返 vs 4.4 mov（N85），迭代器 operator* 返回值两段拷贝
// 与寄存器角色互换（N7/N18a）。gap = -24/1067（2.2%）。
// [P2S6 2026-09-11 本波] 逐件实测（改前 n1_v 1075/1091 gap=-16、fdiff blocks=204/
//   不等长 44/net O−N=−12）→ 改后 fdiff **blocks=175、ours=1067 = orig=1067（净 0）**、
//   n1_v 1075/1067 gap=+8（差值为 ORIG 尾部 nop/对齐）。四数 25/50/0/2 全程不变。
//   落地旋钮（全部「ORIG 有没有那个槽」判据）：
//   ① tower 链去具名（两处）：ORIG 0x0861a3f8+0x0711 / 0x0861ac7f+0x0f93 只 tower 落槽
//      （-0x40 / -0x20），getCDungeonMGr(0x08234612)/getDungeon(0x082345f0)/
//      get_index(0x080fdcf0) 三次调用全内联；notTower 形 = `cmp $0x2af8,%eax; sete %al;
//      test %al,%al; je L; movb $0x0,-0x41(%ebp); jmp; L: movb $0x1,-0x41(%ebp)`
//      ⇒ 源码写三元 `(idx==11000) ? false : true`（我方 4.4.7 生成 setne+直存，属版本差）。
//   ② hasAmp/upgrade 去具名（三处）：ORIG 0x0861a282+0x0596 `call Inven_Item::GetUpgrade;
//      movzbl %al,%ebx`（直留 %ebx）+ hasAmp 留 %esi ⇒ 两实参内联进
//      getUpgradeItemRepairCostRate(item.GetUpgrade(), item.m_amp.hasAbility())。
//   ③ maxEnd 去具名（三处）：ORIG 0x0861a2ba 无 maxEnd 槽，耐久在实参处现取。
//   负结论：把 `int cost` 内联成 `totalCost += getEquipmentRepairPrice(...)`（ORIG
//   0x0861a8xx 形为 `call; add %eax,-0x58(%ebp)`）实测**劣化**：不等长块 31→57、
//   net O−N 0→+4 ⇒ 已回滚（保留具名 cost）。
//   剩余差异性质：全帧槽号平移（我方 Inven_Item 基址 -0x1a8 vs ORIG -0x168，帧 0x20c
//   vs 0x1bc）；rate 的 4.1 `flds/fstps` vs 4.4 `mov`（N85）；bool 物化 `sete+分支`
//   vs 4.4 `setne`（notTower/needRepair）；`ok` 的 %esi 保活 vs 我方槽 —— 皆 4.1/4.4
//   前/后端行为差，源码层不可造。
#include "Store/repair_equip.inc"
                        unsigned short& outEndurance)
{
    if (user->CheckInTrade())
        return 0x13;

    Inven_Item item;
    int totalCost = 0;

    // ORIG 0x08619d0x：`cmpw $0xffff; je <slot==-1 块>` ⇒ 单件路 fall-through 在前、
    // 全修路在 je 目标（后）⇒ 源码条件取反 + 块序交换（P59/P222 布局判据）。
    if (slot != -1)
    {
        // ORIG 0x08619d25：`movsbl invenType,%eax; cmp $2; je; cmp $3; je;
        // test %eax; jne <0x11>` 无槽线性决策树 ⇒ switch(invenType)（P251/P286）。
        switch (invenType)
        {
        case 0:
            item = user->getCurCharacInvenR()->GetInvenSlot(1, slot);
            break;
        case 2:
            item = user->getCurCharacCargoR()->get_cargo_slot(slot);
            break;
        case 3:
            item = user->getCurCharacInvenR()->GetInvenSlot(0, slot);
            break;
        default:
            return 0x11;
        }

        // ORIG 全程直读 item+0x02 的 m_addInfo（item idx）；get_add_info() 对应
        // +0x07 m_addInfo2（stackable 数量域），普通装备为 0，不得用于判空/查表。
        if (item.m_addInfo == 0 || item.m_field1 != Inven_Item::ITEM_TYPE_EQUIP)
            return 0x11;

        outEndurance = item.m_fieldb;

        // ORIG 0x08619xxx：两个具名指针局部（local_58 = find_item 结果，
        // local_54 = local_58 的 CEquipItem 视图）——P0 槽往返实证。
        CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);
        if (pItem == 0)
            return 0x11;
        CEquipItem* pEquip = (CEquipItem*)pItem;

        if (pEquip->get_endurance() == 0)
            return 0x11;

        Inven_Item item2;
        if (count != 0xffff)
        {
            item2 = user->getCurCharacInvenR()->GetInvenSlot(1, count);   // ORIG GetInvenSlot(1, count)
            if (item2.m_addInfo == 0 || item2.m_field1 != Inven_Item::ITEM_TYPE_2)
                return 0x15;
            // ORIG 0x0861bxxx：直接读 item2.m_addInfo 两次（P48，无具名 toolIdx）。
            if (item2.m_addInfo != 0x1f77 && item2.m_addInfo != 0x201f)
                return 0x15;
        }

        // ORIG 0x0861bxxx：两次独立 get_charac_level()；价格出参复用 totalCost
        // 同一栈槽（ORIG local_5c，非新局部）。
        int growthGrade = pEquip->getGrowthGrade(user->get_charac_level());
        int growthCost = pEquip->getGrowthRepairCost(user->get_charac_level());
        // [P2S6 2026-09-11 N82 去槽] ORIG 0x0861a282 +0x0596..+0x05a7：
        //   `call Inven_Item::GetUpgrade; movzbl %al,%ebx`（结果直留 %ebx，**无 upgrade
        //   槽**）；hasAmp 更早求值留 %esi（**无 hasAmp 槽**）；再 `call G_CDataManager`
        //   ⇒ 右→左求值序 (arg3 hasAmp, arg2 upgrade, arg1 manager) ⇒ 两实参均内联。
        float rate = G_CDataManager()->getUpgradeItemRepairCostRate(
            item.GetUpgrade(), item.m_amp.hasAbility());
        // [P2S6 2026-09-11 N82 去槽] ORIG 0x0861a2ba 附近无 maxEnd 槽：耐久在实参处
        //   现取（`movzwl` 序列），源码为内联调用。
        totalCost = getEquipmentRepairPrice(growthCost, item.m_fieldb, pEquip->get_endurance(),
                                             growthGrade, false, rate);

        if (count != 0xffff && (item2.m_addInfo == 0x1f77 || item2.m_addInfo == 0x201f))
        {
            CInventory* invenW = user->getCurCharacInvenW();
            int ret = invenW->use_item(count, 1, 0);
            if (ret != 0)
                return 0x16;
            user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, count);
            totalCost = 0;
        }

        // ORIG 0x08619xxx：`call use_money; xor $1; test %al` 无槽 ⇒ 表达式直用（P0）。
        if (!user->getCurCharacInvenW()->use_money(totalCost, (eMoneySubReason)1, true))
            return 10;

        GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x14, user, totalCost);

        // ORIG 0x08619xxx：`call get_state; cmp $0xa,%eax; sete` 无槽往返 ⇒ 表达式直用（P0）。
        if (user->get_state() == 10)
        {
            WongWork::CDeathTower* tower = user->getDeathTower();
            if (tower)
            {
                // [P2S6 2026-09-11 N82 去多余具名槽] ORIG 0x0861a3f8 +0x0711..+0x0747：
                //   仅 tower 落槽 -0x40；getCDungeonMGr(0x08234612)/getDungeon(0x082345f0)/
                //   get_index(0x080fdcf0) 三次调用**全内联**（无 mgr/dun/idx 槽，我方原
                //   各留 4 字节槽 ⇒ 帧 +0x60、每处多 mov 往返）。
                //   notTower(-0x41) 形 = `cmp $0x2af8,%eax; sete %al; test %al,%al; je L;
                //   movb $0x0,-0x41(%ebp); jmp; L: movb $0x1,-0x41(%ebp)` ⇒ 源码是三元
                //   `(idx==11000) ? false : true`（**非** if(缺 else) 形）。
                bool notTower =
                    tower->getCDungeonMGr()->getDungeon()->get_index() == 11000 ? false : true;
                int lvl = user->get_charac_level();
                GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                    (DEATHTOWER_VALUE_STATISTIC_FIELD)9, (char)notTower, (char)lvl, totalCost);
            }
        }

        item.m_fieldb = (unsigned short)pEquip->get_endurance();

        // ORIG 0x08619xxx：`movsbl invenType; cmp $2; je; cmp $3; je` 决策链 ⇒ switch。
        switch (invenType)
        {
        case 0:
            user->getCurCharacInvenW()->update_item((INVEN_TYPE)1, slot, item);
            user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
            break;
        case 2:
            user->getCurCharacCargoW()->update_item(slot, item);
            user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)2, slot);
            break;
        case 3:
            user->getCurCharacInvenW()->update_item((INVEN_TYPE)0, slot, item);
            user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)3, slot);
            break;
        default:
            break;
        }
    }
    else
    {
        std::vector<tagRepairItemInfo> repairList;

        for (int i = 3; i < 9; i++)
        {
            // ORIG 0x0861a719：sret 临时 → 拷贝赋值给**函数头部的 item**（local_d9），
            // 非新声明局部（4.4 对 `Inven_Item x = f()` 会直用临时，N86）。
            item = user->getCurCharacInvenR()->GetInvenSlot(1, i);
            if (item.m_addInfo != 0 && item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
            {
                // ORIG 0x0861a73x：第一循环（装备栏 3..8）走 CItemList::find_item
                // （`G_CDataManager()+0xc` 的 m_itemList 成员）。
                CEquipItem* pEquip =
                    (CEquipItem*)G_CDataManager()->m_itemList->find_item(item.m_addInfo);
                if (pEquip)
                {
                    // ORIG 每处 get_charac_level() 独立重调（P157 逐次重读）。
                    int growthGrade = pEquip->getGrowthGrade(user->get_charac_level());
                    int growthCost = pEquip->getGrowthRepairCost(user->get_charac_level());
                    // [P2S6 2026-09-11 N82 去槽] 同单件分支：ORIG +0x0bb6 前
                    //   `movzbl %al,%ebx`（GetUpgrade 直留寄存器）+ hasAmp 留 %esi。
                    float rate = G_CDataManager()->getUpgradeItemRepairCostRate(
                        item.GetUpgrade(), item.m_amp.hasAbility());
                    // [P2S6 2026-09-11 负结论] 试过把 `int cost` 内联成
                    //   `totalCost += getEquipmentRepairPrice(...)`（ORIG 0x0861a8xx 是
                    //   `call; add %eax,-0x58(%ebp)`）⇒ 实测**劣化**：blocks 175→174 但
                    //   不等长块 31→57、net O−N 0→+4（我方少 4 条）；4.4 对 `+=` 形会
                    //   额外折叠。保留具名 cost（与 fdiff net=0 的最优态一致）。
                    int cost = getEquipmentRepairPrice(growthCost, item.m_fieldb, pEquip->get_endurance(),
                                                        growthGrade, false, rate);
                    totalCost += cost;

                    // ORIG 0x0861a8xx：needRepair 两次独立 get_endurance() 重调
                    // （maxEnd==0 → false；否则 m_fieldb==get_endurance() → false）。
                    bool needRepair = true;
                    if (pEquip->get_endurance() == 0)
                        needRepair = false;
                    else if (item.m_fieldb == pEquip->get_endurance())
                        needRepair = false;
                    if (needRepair)
                        repairList.push_back(tagRepairItemInfo(INVEN_TYPE_EQUIP, i, pEquip));
                }
            }
        }

        for (int i = 10; i < 0x16; i++)
        {
            item = user->getCurCharacInvenR()->GetInvenSlot(0, i);
            if (item.m_addInfo != 0 && item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
            {
                // ORIG 0x0861abxx：第二循环（背包 10..0x15）走 CDataManager::find_item
                // （0x0835fa32，非 m_itemList）——与第一循环相反。
                CEquipItem* pEquip = (CEquipItem*)G_CDataManager()->find_item(item.m_addInfo);
                if (pEquip)
                {
                    int growthGrade = pEquip->getGrowthGrade(user->get_charac_level());
                    int growthCost = pEquip->getGrowthRepairCost(user->get_charac_level());
                    // [P2S6 2026-09-11 N82 去槽] 同单件分支（ORIG +0x0e40 前同形）。
                    float rate = G_CDataManager()->getUpgradeItemRepairCostRate(
                        item.GetUpgrade(), item.m_amp.hasAbility());
                    int cost = getEquipmentRepairPrice(growthCost, item.m_fieldb, pEquip->get_endurance(),
                                                        growthGrade, false, rate);
                    totalCost += cost;   // 见第一循环注：`+=` 内联形被实测否证（net +4）。

                    bool needRepair = true;
                    if (pEquip->get_endurance() == 0)
                        needRepair = false;
                    else if (item.m_fieldb == pEquip->get_endurance())
                        needRepair = false;
                    if (needRepair)
                        repairList.push_back(tagRepairItemInfo(INVEN_TYPE_INVEN, i, pEquip));
                }
            }
        }

        CInventory* invenW = user->getCurCharacInvenW();
        bool ok = invenW->use_money(totalCost, (eMoneySubReason)1, true);
        if (ok)
        {
            GetInstanceValueStatistic()->AddValueStatistic((VALUE_STATISTIC_FIELD)0x14, user, totalCost);

            if (user->get_state() == 10)
            {
                WongWork::CDeathTower* tower = user->getDeathTower();
                if (tower)
                {
                    // [P2S6 2026-09-11 N82] 同单件分支：ORIG 0x0861ac7f +0x0f93..+0x0fd2
                    //   仅 tower 落槽 -0x20，三次调用全内联 + 三元 notTower。
                    bool notTower =
                        tower->getCDungeonMGr()->getDungeon()->get_index() == 11000 ? false : true;
                    int lvl = user->get_charac_level();
                    GetInstanceDeathTowerValueStatistic()->SendDeathTowerValueStatistic(
                        (DEATHTOWER_VALUE_STATISTIC_FIELD)9, (char)notTower, (char)lvl, totalCost);
                }
            }


            // ORIG 0x0861afxx：begin()/end() 各自经 const_iterator 转换 ctor 落具名槽
            // （P28/P270；直接写 repairList.end() 会选 (const,non-const) 混合 operator!=）。
            std::vector<tagRepairItemInfo>::const_iterator it = repairList.begin();
            std::vector<tagRepairItemInfo>::const_iterator itEnd = repairList.end();
            for (; it != itEnd; ++it)
            {
                const tagRepairItemInfo& info = *it;
                int invenType2 = info.m_type;
                int slotIdx = info.m_slot;
                CEquipItem* pEquip = const_cast<CEquipItem*>(info.m_item);

                // ORIG 迭代内重取：GetInvenSlot(type, slot) 值版；update 走 W。
                item = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)invenType2, slotIdx);
                if (item.m_addInfo != 0 && item.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
                {
                    // ORIG 0x0861b0xx：直接 store get_endurance()（无具名 maxEnd 槽，P0）。
                    item.m_fieldb = (unsigned short)pEquip->get_endurance();
                    user->getCurCharacInvenW()->update_item((INVEN_TYPE)invenType2, slotIdx, item);
                }
            }

            for (int i = 10; i < 0x16; i++)
                user->SendUpdateItem(CUser::eSendTarget_1, (ENUM_ITEMSPACE)3, i);

            return 0;
        }
        else
        {
            return 10;
        }
    }

    return 0;
}

// ---- force 发射 tagRepairItemInfo C1/C2（ORIG W 0x0861b08e）----
namespace {
#include "Store/forceTagRepairItemInfoEmission.inc"
{
    tagRepairItemInfo tri(INVEN_TYPE_EQUIP, 0, 0);
    (void)tri;
}
}  // namespace
