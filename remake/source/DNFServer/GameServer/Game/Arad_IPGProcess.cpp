// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_IPGProcess/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "Arad_IPGProcess.h"
#include "Arad_MileageProcess.h"

#include "CDataManager.h"    // G_CDataManager / find_item / FindGoods / getItemLimitEditionMgr
#include "CUser.h"           // CUser / cUserHistoryLog（+0x79700）/ CeraShopPaymentType
#include "CInventory.h"      // AddAvatarItem / use_money / MakeItemPacket / Inven_Item
#include "CItem.h"           // isCerashopPackageItem / getUsablePeriod / getExpirationDate
#include "CItemList.h"       // CCeraShopGoods::GetSubGroupIndex / get_rand_int
#include <math.h>     // floor（GiveMileage）
#include <stdlib.h>   // atoi
#include "CMailBoxHelper.h"  // ReqDBSendNewAvatarMailCashShop
#include "RDARScriptStringManager.h"  // findString / g_scriptStringManager_
#include "LogManager.h"
#include "ItemVendingMachine.h"  // BillingRecord
#include "Arad_DataManager.h"    // ARAD::Singleton<Arad_DataManager> / Arad_Mileage_Data
#include "CUserCharacInfo.h"     // getCurCharacNo（经 CUser 继承可见）

#include <stdlib.h>   // atoi
#include <string.h>   // strlen

// ---------------------------------------------------------------------------
// Arad_IPGProcess
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// CashShopModeAvatar —— ORIG T 0x0819c2b4（报告 CashShopModeAvatar.md 全文转写）。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_IPGProcess] NEAR（助记符序列全等，仅 len 槽号差）：①gift 分支原已全等；
// ②删具名 inven/characNo 改 receiver 链式（P52/P0）后 slot=-0x20、msg=-0x1c 与 ORIG 一致；
// ③`int len` 头部声明得 -0x24，ORIG 为 -0x38（4.1 外层标量与形参 spill 区 -0x2c..-0x34
// 连续分配 vs 4.4 具名区 -0x1c..-0x24 分区分配）⇒ N82/N93 槽分配族封顶；
// size_t 化实测反劣（NEAR→DIFF）。
// [S3/M3FreeNext15B 2026-09-11 复扫] ①§6.5 串审计 CLEAR（1 kinds/1 refs：
//   game_server_msg_104 @我方 .rodata+0）；②P339 试验：`const char* msg;` 提函数头
//   （全头部声明、ORIG 槽深序 len>msg）⇒ 复编后四数 12/28/2/1 与槽分布**逐值不变**
//   （-0x1c×3 / -0x20×3 / -0x24×2 / -0x2c×4 / -0x30×4 / -0x34×2）⇒ 已回滚；
//   ③P331 不成立（头部标量仅 len 一项，相对槽序与 ORIG 同向，非倒置）。
//   ⇒ 维持 N82/N93：ORIG len 属形参 spill 相邻深区 -0x38（4.1 函数级标量区），
//   4.4 具名区自 -0x1c 起（msg -0x1c / slot -0x20 / len -0x24），两侧帧同为 0x5c。
#include "Arad_IPGProcess/CashShopModeAvatar.inc"
                                         int a3, int a4, char c1, char c2,
                                         char const* str, int& out,
                                         unsigned int accId, int a10, char buyFlag)
{
    (void)a3;  // ORIG p3 在两分支均未使用（ebp 0x14 无读取）
    // [P6AuditOldN 2026-09-11 复核·维持 NEAR] 本件已试新钥匙：①P339 声明序（本处 len 置于
    // 函数头 = 声明序最前，我方槽 -0x24 为三个局部中最深 ⇒ 声明序已与 ORIG 同向）；
    // ②「声明即初始化」变体（`int len = (int)strlen(msg);` 移入 if 块）⇒ 实测把 **slot**
    // 从 -0x1c 推深到 -0x24（diff 由 len 单处变为 slot 单处，仍 NEAR）⇒ 已回滚。
    // 残差定性（ORIG 0x0819c2b4 直读）：ORIG 局部区自 -0x38 起（len 最深），我方自 -0x24 起
    // ⇒ ORIG 在 [-0x38,-0x24) 另有 0x14 字节被占（20B，无对应 store 指令 ⇒ 非我方漏建模的
    // 具名局部，属 4.4 内部命名临时），无源码旋钮 ⇒ N 封顶。
    int len;   // ORIG -0x38（形参 spill 区之后）⇒ 头部声明（4.1 先声明深）

    if (buyFlag == 1)
    {
        // mangled _processAvatarGift(CUser*, i, i, c, c, PKc, j, i)
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processAvatarGift(
            user, (int)itemIdx, a4, c1, c2, str, accId, a10);
        return;
    }

    // P52：receiver 链保持单语句，ORIG 把 c2/c1/itemIdx 先物化 edi/esi/ebx 跨 getter 保活。
    int slot = user->getCurCharacInvenW()->AddAvatarItem(
        (int)itemIdx, a4, false, c1, c2, str, (eAvatarItemAddReason)0, false, 0);
    if (slot < 0)
    {
        const char* msg = g_scriptStringManager_.findString(4, "game_server_msg_104", (bool*)0);
        len = (int)strlen(msg);
        // P0：characNo 无槽往返 ⇒ 原码是实参内联（ORIG 无 -0xc 槽）。
        WongWork::CMailBoxHelper::ReqDBSendNewAvatarMailCashShop(
            user, user->getCurCharacNo(), (int)itemIdx, a4, c1, c2, str, msg, len);
    }
    else
    {
        user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)1, slot);
    }
}

// ORIG W 0x0817a2ba：push ebp; mov esp,ebp; mov 0xc(%ebp),%edx; mov %edx,(%eax); pop ebp; ret
#include "Arad_IPGProcess/Arad_IPGProcess.inc"

// ORIG W 0x0817a2c8：push ebp; mov esp,ebp; pop ebp; ret（空体）
#include "Arad_IPGProcess/dtor_Arad_IPGProcess.inc"

// ---------------------------------------------------------------------------
// CashShopModeCoin —— ORIG T 0x0819c434（报告 CashShopModeCoin.md 全文转写）。
// ---------------------------------------------------------------------------
#include "Arad_IPGProcess/CashShopModeCoin.inc"
                                       unsigned int param3, int param4, char buyFlag)
{
    if (buyFlag == 1)
    {
        // esp: shop / user / (uint)param2 / param3 —— mangled (CUser*, t, j, i)
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processCoinGift(user, param2, param3, param4);
    }
    else
    {
        // esp: shop / user / (uint)param2 / 1 —— mangled (CUser*, t, b)；bool 实参 = 1
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processCoin(user, param2, true);
    }
}

// ---------------------------------------------------------------------------
// CashShopModeItem —— ORIG T 0x0819c4a0（报告 CashShopModeItem.md 全文转写）。
// ---------------------------------------------------------------------------
#include "Arad_IPGProcess/CashShopModeItem.inc"
                                       unsigned short stack, int count, long seq,
                                       std::vector<Inven_Item>& out,
                                       WongWork::CCeraShop::stCeraShopItemParam_t& param,
                                       int& restrictOut, unsigned int accId,
                                       int characNo, char buyFlag)
{
    CItem* item = G_CDataManager()->find_item((int)itemIdx);
    if (item == 0)
        return;

    Inven_Item localItem;
    item->make_item(localItem);
    localItem.m_addInfo = (int)itemIdx;    // ORIG -0x47 = item +2
    localItem.m_addInfo2 = (int)stack;     // ORIG -0x42 = item +7

    if (buyFlag == 1)
    {
        // mangled _processItemGift(CUser*, Inven_Item, j, i)：esp 0x48=accId、0x4c=characNo
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processItemGift(user, localItem, accId, characNo);
    }
    else
    {
        // mangled _processItem(CUser*, Inven_Item, i count, Ri restrict, l seq,
        //   b 0, b 1, Rvector, RN..param_t&)
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processItem(
            user, localItem, count, restrictOut, seq, false, true, out, param);
    }
}

// ---------------------------------------------------------------------------
// CashShopModeCreature —— ORIG T 0x0819c670（报告 CashShopModeCreature.md 转写）。
// 期限槽实证：esp0x50=08110c60 getUsablePeriod、esp0x54=080f1306 getExpirationDate。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_IPGProcess] AE：删两组具名 usablePeriod/expirationDate 局部改实参内联
// （P0，ORIG %ebx 跨 call 保活、无槽），右起求值序自然复现（expirationDate 先 call）；
// 帧 0xc4、localItem -0x49、spill -0x6c/-0x70 与 ORIG 全等。
#include "Arad_IPGProcess/CashShopModeCreature.inc"
                                           unsigned short stack, int& outCodeA,
                                           int& outCodeB, unsigned int accId,
                                           int characNo, char buyFlag)
{
    CItem* item = G_CDataManager()->find_item((int)itemIdx);
    if (item == 0)
        return;

    Inven_Item localItem;
    item->make_item(localItem);
    localItem.m_addInfo = (int)itemIdx;
    localItem.m_addInfo2 = (int)stack;

    if (buyFlag == 1)
    {
        // mangled _processCreatureGift(CUser*, Inven_Item, j, i, i, i)：
        //   esp 0x48=accId、0x4c=characNo、0x50=usablePeriod、0x54=expirationDate
        // P0：两 getter 结果无槽往返（ORIG ebx 跨 call 保活）⇒ 实参内联；
        // ORIG 求值序 = 右起（expirationDate 先 call、usablePeriod 后）。
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processCreatureGift(
            user, localItem, accId, characNo, item->getUsablePeriod(),
            item->getExpirationDate());
    }
    else
    {
        // mangled _processCreature(CUser*, Inven_Item, Ri, Ri, b, i, i)
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processCreature(
            user, localItem, outCodeA, outCodeB, true, item->getUsablePeriod(),
            item->getExpirationDate());
    }
}

// ---------------------------------------------------------------------------
// CashShopModeVendingItem —— ORIG T 0x0819c862（报告 CashShopModeVendingItem.md）。
// 非特殊商品：insertItemIntoInventory 直插背包（失败转邮件补发）；
// 特殊商品：ProcessSpecialItem 通道。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_IPGProcess] AE + 两处真语义 bug：①IsSpecialItem 第 1 实参按 ORIG
// 0x0819c8dd 从 localItem.m_addInfo(-0x58) 读（原写形参 itemIdx）；②日志条件极性反置
// （ORIG 0x0819cacc `test %al,%al; je end` ⇒ result!=0 才打日志，原写 !result）；
// ③删具名 ceraShop/result（P0）后槽位全对齐：localItem -0x5a / item -0x14 /
// outType -0x18 / outValue -0x1c / special -0x1d / msg -0xc / slot -0x10，
// strlen 结果 %ebx 跨 getCurCharacNo 保活。
#include "Arad_IPGProcess/CashShopModeVendingItem.inc"
    CUser* user, itemIndexOnlyServer_t itemIdx, unsigned short stack, int count,
    long seq, std::vector<Inven_Item>& out,
    WongWork::CCeraShop::stCeraShopItemParam_t& param, int& restrictOut,
    int characNo, char buyFlag)
{
    CItem* item = G_CDataManager()->find_item((int)itemIdx);
    if (item == 0)
        return;

    Inven_Item localItem;
    item->make_item(localItem);
    localItem.m_addInfo = (int)itemIdx;   // ORIG -0x58 = item +2
    localItem.m_addInfo2 = (int)stack;    // ORIG -0x53 = item +7

    int outType = 0;
    int outValue = 0;
    bool special = false;
    // ORIG 0x0819c8dd：IsSpecialItem 第 1 实参从 localItem.m_addInfo(-0x58) 读，非形参槽；
    // m_vm 无具名局部（每次 `mov 0x8(%ebp),%eax; mov (%eax),%eax` 重取）。
    if (!reinterpret_cast<WongWork::CCeraShop*>(m_vm)->IsSpecialItem(
            localItem.m_addInfo, outType, outValue, special))
    {
        int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            localItem, (eItemAddReason)0x1e, true, false);
        if (slot != -1)
        {
            user->SendUpdateItemList(CUser::eSendTarget_1,
                                     (ENUM_ITEMSPACE)0, slot);
            user->SaveInventory();
        }
        else
        {
            const char* msg =
                g_scriptStringManager_.findString(4, "game_server_msg_104", 0);
            WongWork::CMailBoxHelper::ReqDBSendNewMailCashShop(
                user, localItem, 0, (unsigned int)user->getCurCharacNo(), true, msg,
                (int)strlen(msg));
            slot = 0;
        }
    }
    else
    {
        // ORIG 0x0819cacc：`call; test %al,%al; je end`（result 无槽）⇒ 条件内联。
        // 极性：result!=0 才打日志（原 !result 写反）。
        if (reinterpret_cast<WongWork::CCeraShop*>(m_vm)->ProcessSpecialItem(
                user, (int)itemIdx, count, restrictOut, seq, true, out, param,
                0, 0, false, 1))
        {
            LogManager::logFormat(1, "localjapan/Arad_IPGProcess.cpp",
                                  __PRETTY_FUNCTION__, 0x1a2,
                                  "ProcessSpecialItem failed");
        }
    }
}

// ---------------------------------------------------------------------------
// Arad_MileageProcess
// ---------------------------------------------------------------------------
// ORIG W 0x0817a2ce：同 Arad_IPGProcess ctor 形态。
#include "Arad_IPGProcess/Arad_MileageProcess.inc"

// ORIG W 0x0817a2dc：空体。
#include "Arad_IPGProcess/dtor_Arad_MileageProcess.inc"

// ---------------------------------------------------------------------------
// BuySuccess —— ORIG T 0x0819e220（报告 BuySuccess.md 全文转写）。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_IPGProcess] AE（200=200）：①kResultCode/kGiftFlag 改非 const 局部
// （ORIG 从槽 `movsbl -0x9` / `mov -0x10` 读，const 被前端折叠成立即数；同 BuyFail
// b28c-28 判据）；②迭代器 end 缓存（P13：end 临时 -0x14 + 转换构造 -0x2c）；
// ③getCurCharacInvenR() 两次独立调用（P48，原缓存 inven 少一次调用）。
#include "Arad_IPGProcess/BuySuccess.inc"
                                     int a6, int a7, std::vector<Inven_Item>& items)
{
    user->UseMileage(a5);

    // b28c-28 判据（同 BuyFail）：ORIG 在 PacketGuard ctor 前发 `movl $0xb,-0x10` /
    // `movb $0x0,-0x9` 且 put_byte 从槽 `movsbl -0x9` 读 ⇒ 非 const 局部（const 被折叠成立即数）。
    int kResultCode = 0xb;
    char kGiftFlag = 0;
    PacketGuard packet;
    packet.put_header(1, 0x43);
    packet.put_byte(1);
    packet.put_byte((int)kGiftFlag);
    packet.put_int(kResultCode);
    packet.put_int(a2);
    packet.put_int(a3);
    packet.put_int(a6);
    packet.put_int(a4);
    packet.put_short((int)items.size());
    // P13：ORIG end() 在循环初始化区求值一次（end 临时 -0x14 + 转换构造 -0x2c）。
    for (std::vector<Inven_Item>::const_iterator it = items.begin(),
             endIt = items.end(); it != endIt; ++it)
    {
        packet.put_int(it->m_addInfo);    // ORIG put_int(it+2)
        packet.put_int(it->m_addInfo2);   // ORIG put_int(it+7)
    }
    if (a6 == a7)
    {
        user->SendCashData(false);
        PacketGuard itemPacket;
        itemPacket.put_header(0, 0xe);
        itemPacket.put_byte(0);
        itemPacket.put_short(2);
        // ORIG 0x0819e421/0x0819e44b：getCurCharacInvenR() 两次独立调用（P48，无缓存）。
        user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, 0, itemPacket);
        user->getCurCharacInvenR()->MakeItemPacket((INVEN_TYPE)1, 2, itemPacket);
        itemPacket.finalize(true);
        user->Send(itemPacket);
    }
    packet.finalize(true);
    user->Send(packet);
}

// ---------------------------------------------------------------------------
// GiveMileage —— ORIG T 0x0819dc9c（报告 GiveMileage.md 全文转写）。
// 汇编算术：fild rate; fild amount; flds [1000.0f @0x8ba7330]; fdivrp; fmulp;
//   fstpl; call floor; fistp（fldcw 0xc 截断模式）→ (int)floor(...)。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_IPGProcess] IDENTICAL：三条钥匙——①`int rate=0` 前置清零 +
// `int found;` 两句式（-0x14/-0x10 槽序按声明序 rate→found 复现）；②Get 链全程右值
// 直传（P0，无具名 mgr 局部）；③ROUND 结果具名 `int mileage` 落 -0xc，x87 左操作数
// 写 `(float)amount * ((float)rate / 1000.0f)` 复现 fild amount→fild rate 序（P243）。
// 51=51 逐条全等（fresh .o 核验）。
#include "Arad_IPGProcess/GiveMileage.inc"
{
    if (user == 0)
        return;

    // ORIG -0x14（rate）前置清零、-0x10（found）独立槽；Get 链全程右值直传（P0）。
    int rate = 0;
    int found;
    found = ARAD::Singleton<ARAD::Arad_DataManager>::Get()
                ->getAradMileageData()->find(ipgNo);
    if (found >= 0 && found <= 1000)
        rate = found;
    else
        rate = ARAD::Singleton<ARAD::Arad_DataManager>::Get()
                   ->getAradMileageData()->default_value();

    // ORIG x87 序：fild amount → fild rate → flds 1000.0f → fdivrp → fmulp
    // ⇒ 左操作数 amount 先求值；ROUND 结果落具名 int 槽 -0xc。
    int mileage = (int)floor((float)amount * ((float)rate / 1000.0f));
    user->AddMileage(mileage, (eCoinAddReason)3);
}

// ---------------------------------------------------------------------------
// BuyFail —— ORIG T 0x0819e50e（报告 BuyFail.md 全文转写）。
// ---------------------------------------------------------------------------
#include "Arad_IPGProcess/BuyFail.inc"
{
    PacketGuard packet;
    // b28c-28：两值必须是** PacketGuard ctor 之后**初始化的非 const 局部——
    // ORIG 0x0819e50e+0x24 在 put_header 前才发 `movl $0xb,-0x10(%ebp)` /
    // `movb $0x0,-0x9(%ebp)`，且两处实参从槽装载
    // （`movsbl -0x9,%eax; mov %eax,0x4(%esp)`、`mov -0x10,%eax; mov %eax,0x4(%esp)`）；
    // const 形被前端折叠成 `movl $imm,0x4(%esp)`（少 2 条）。
    int resultCode = 0xb;        // ORIG -0x10（int 槽）
    char giftFlag = 0;           // ORIG -0x9（signed char 槽 ⇒ movsbl）
    packet.put_header(1, 0x43);
    packet.put_byte(0);
    packet.put_byte(0);
    packet.put_short((int)errCode);
    packet.put_byte((int)giftFlag);
    packet.put_int(resultCode);
    packet.put_int(a3);
    packet.put_int(a4);
    packet.put_int(a5);
    packet.put_int(-1);
    packet.finalize(true);
    user->Send(packet);
}

// ---------------------------------------------------------------------------
// BuyMileageItem —— ORIG T 0x0819dd4e（报告 BuyMileageItem.md 全文转写）。
// ---------------------------------------------------------------------------
// [b28d-solo-Arad_IPGProcess] NEAR（318=318，助记符序列全等）+ 三处真语义 bug：
// ①BillingRecord 第 7 实参按 ORIG 0x0819e18d 是 `(int)stack`（原写 0）；
// ②`CItem* item = 0;` 先于 localItem ctor（ORIG 0x0819dd9d 先于 0x0819ddaa）；
// ③第一次 GetSubGroupIndex 判定内联（ORIG `cmp $1; setne; test; je`）、case 0 receiver
// 链式（P52）、characNo 实参内联（P0）、case 3 两 getter 内联（%ebx 跨 call 保活）。
// 残差：len 槽 ORIG -0x98（4.1 与形参 spill 区 -0x8c..-0x94 连续分配）vs 4.4 具名区
// -0x30，连带 localItem(-0x7d→-0x81)/items/outCodeA/outCodeB 四处平移 4 ⇒ N82/N93 封顶。
// [S3/M3FreeNext15B 2026-09-11 复扫] ①§6.5 串审计 CLEAR（2 kinds/2 refs：
//   game_server_msg_104 @我方 .rodata+0、mileage item @+0x4d，与 ORIG 0x08ce.. 逐字节
//   相等）；②P331 试验：`int len;` 由函数头后移至 `int subGroup` 之后 ⇒ 指令数
//   318 不变（四数不变）但槽引用计数由 -0x24:4/-0x28:7/-0x2c:4 变为 2/4/7
//   （ORIG 基准为 4/7/4）⇒ 与 ORIG 更远，已回滚；③P339 方向不成立（ORIG 槽深序
//   len>localItem>items>outCodeB>outCodeA>goods>item>subGroup 与源码执行序冲突，
//   重排会改变 G_CDataManager 查询/构造序）。⇒ 维持 N82/N93。
#include "Arad_IPGProcess/BuyMileageItem.inc"
                                        unsigned short stack, int count, long seq,
                                        int param7, char c1, char c2, int price,
                                        int a11, int a12, char const* str,
                                        WongWork::CCeraShop::stCeraShopItemParam_t& param)
{
    int len;   // ORIG -0x98（形参 spill 区 -0x8c..-0x94 之后）
    CCeraShopGoods* goods = reinterpret_cast<CCeraShopGoods*>(G_CDataManager()->FindGoods(param2));
    if (goods == 0)
        return 0x15;

    // ORIG 0x0819dd9d：`movl $0x0,-0x28`（item）先于 localItem ctor（0x0819ddaa）。
    CItem* item = 0;
    Inven_Item localItem;

    // ORIG 0x0819ddba：`cmp $0x1,%eax; setne %al; test; je` ⇒ 判定内联（无 subGroup 槽）。
    if (goods->GetSubGroupIndex() != 1)
    {
        item = G_CDataManager()->find_item((int)itemIdx);
        if (item == 0)
            return 0x15;
        item->make_item(localItem);
        localItem.m_addInfo = (int)itemIdx;
        localItem.m_addInfo2 = (int)stack;
    }

    if (user->GetMileage() < price)
        return 0x3ed;

    int outCodeA = 0;  // local_30（creature 结果）
    int outCodeB = 0;  // local_34（错误码）
    std::vector<Inven_Item> items;

    // ORIG 0x0819de57：第二次调用结果落 -0x24 供 switch 复用。
    int subGroup = goods->GetSubGroupIndex();
    switch (subGroup)
    {
    case 0:
    {
        // P52：receiver 链式（ORIG c1/c2/itemIdx 先物化 edi/esi/ebx 跨 getter 保活）。
        int slot = user->getCurCharacInvenW()->AddAvatarItem(
            (int)itemIdx, param7, false, c1, c2, str, (eAvatarItemAddReason)0, false, 0);
        if (slot < 0)
        {
            const char* msg = g_scriptStringManager_.findString(4, "game_server_msg_104", (bool*)0);
            len = (int)strlen(msg);
            // P0：ORIG 无 characNo 槽 ⇒ 实参内联。
            WongWork::CMailBoxHelper::ReqDBSendNewAvatarMailCashShop(
                user, user->getCurCharacNo(), (int)itemIdx, param7, c1, c2, str, msg, len);
        }
        else
        {
            user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)1, slot);
        }
        break;
    }
    case 1:
        // esp: shop / user / (uint)stack / 1（同 CashShopModeCoin 非 gift 分支）
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processCoin(user, stack, true);
        break;
    case 2:
    case 7:
    case 9:
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processItem(
            user, localItem, count, outCodeB, seq, false, true, items, param);
        break;
    case 3:
    {
        // ORIG 0x0819e080：getExpirationDate 先 call（%ebx 保活）→ getUsablePeriod（P0 内联）。
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processCreature(
            user, localItem, outCodeA, outCodeB, true, item->getUsablePeriod(),
            item->getExpirationDate());
        break;
    }
    default:
        break;
    }

    if (outCodeB == 0)
    {
        BuySuccess(user, param2, (int)stack, outCodeA, price, a11, a12, items);
        // ORIG 0x0819e18d：`movzwl -0x8c(%ebp),%edx; mov %edx,0x1c(%esp)` ⇒ 第 7 实参
        // 是 (int)stack（原写 0 为真语义 bug）；0x20..0x28 = 0ULL 的低/高 + 0。
        m_vm->BillingRecord(user, param2, subGroup, price, str, "mileage item",
                            (int)stack, 0ULL, 0);
    }

    return outCodeB;
}
