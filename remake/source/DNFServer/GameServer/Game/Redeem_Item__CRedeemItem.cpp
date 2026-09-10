// df_game_r Redeem_Item::CRedeemItem —— 恢复实现（独立 TU）。
// 语义依据 docs/class_func_reports/Redeem_Item__CRedeemItem/（ORIG 反汇编 + Ghidra C）。
// 仅实现 CUser::Add_RedeemInfo 调用的两个方法 + 私有辅助 _isFPItem；
// 其余 ORIG 方法（ctor/dtor/loadData/_saveData/GetItemPrice/RedeemItem 等）未建模。

#include "Redeem_Item__CRedeemItem.h"

#include "CDataManager.h"
#include "CItem.h"
#include "GameWorld.h"        // G_GameWorld / IsEnchantRevisionChannel
#include "PacketGuard.h"

namespace Redeem_Item
{

// ORIG 命名空间内 static 常量（_ZN11Redeem_ItemL...E，r 段；值实测
// 0x08cd21c8..）：MAX_COUNT=21 / THROW_FEE=1000 / BUY_FEE=1000。
static const int MAX_COUNT = 21;
static const int THROW_FEE = 1000;
static const int BUY_FEE = 1000;

// ORIG 0x085f7c04：itemId 在 (0x1963, 0x1b57] 区间 → true（"FP 物品"判定）。

bool CRedeemItem::_isFPItem(const Inven_Item& item) const
{
    // ORIG：无局部槽，两次完整读 0x2(%eax)，无符号 jbe/ja（P48 双读）
    return item.m_addInfo > 0x1963 && item.m_addInfo <= 0x1b57;
}

// ORIG 0x085f76f0：空物品/FP 物品/固定 ID 表/物品不存在/低稀有度可升级
// 装备/禁兑换物品 → 0；否则 1。
// [b28d-46 判词·N/P168a③b] 99/101；残余唯一处：ORIG 0x085f7803
// `call IsBanRedeemItem; test %al,%al; je`（uchar 返回直判）；4.4-O0 对
// uchar callee 的条件恒插 `setne %al; test %al,%al` 归一。正解需 CItem::
// IsBanRedeemItem 返回类型 uchar→bool（mangled 不变），但定义在清单外
// CItem.cpp（R1 禁编辑）⇒ 跨版本 ABI 差封顶，语义等价。
bool CRedeemItem::isAddableFilter(const Inven_Item& item)
{
    if (item.isEmpty())
        return 0;
    if (_isFPItem(item))
        return 0;
    switch (item.m_addInfo)
    {
    case 0x114c:
    case 0xcf1:
    case 0xbdd:
    case 0x1057:
    case 0x10fe:
    case 0x1157:
    case 0x1159:
    case 0x1973:
    case 0x1ee1:
    case 0x28e770:
        return 0;
    default:
        break;
    }
    CItem* ci = G_CDataManager()->find_item(item.m_addInfo);
    if (ci == 0)
        return 0;
    // ORIG 0x85f77e4：三条件常量岛汇聚（mov$1/jmp/mov$0）后 test 直判
    if (item.isEquipableItemType() && ci->get_rarity() == 0 &&
        item.GetUpgrade() == 0)
        return 0;
    // ORIG 0x085f77f4 调用 CItem::IsBanRedeemItem()（非内联，生成 call）；
    // 后随 `test %al,%al; je` 直判（无 setne 归一）。
    // [b28d-L28 round2 实测] 调用点由 `!= 0` 改隐式转 bool **无效**（4.4 对
    // 非 bool 返回的条件恒插 `test; setne %al; test; je`，99/101 不变）⇒ 正解在
    // **被调方返回类型**（CItem::IsBanRedeemItem 应 bool，属 CItem 头，需 spec 上报）。
    if (ci->IsBanRedeemItem() != 0)
        return 0;
    return 1;
}

// ORIG 0x085f71a8：构造 Inven_Item_Expand_RedeemInfo（复制物品前 0x3c 字节 +
// 末字节 + count=param2+1000/1000），alter()，list（this+8）超 0x14 项时
// pop_front，push_back，返回 1。
int CRedeemItem::AddRedeemList(const Inven_Item& item, int param2, bool param3)
{
    // [b28d-46 判词·N/N84 翻正] ORIG 0x085f71a8 对载荷为**整体结构赋值**形：
    // 0x085f7202..0x085f7261 逐 dword（15 条 `mov off(%eax),%edx; mov %edx,-N(%ebp)`）
    // + 尾 `movzbl 0x3c(%eax); mov %al,-0x10(%ebp)` ⇒ 源写 `info.m_item = item;`
    // （N84：结构体直接赋值，禁 memcpy ⇒ 4.4 亦按块搬移发射，不再 `call memcpy`）。
    Inven_Item_Expand_RedeemInfo info;
    info.m_item = item;
    // ORIG 0x085f7266..0x085f727f：then 臂 `add $0x3e8,%eax; mov %eax,-0xc(%ebp);
    // jmp`、else 臂 `movl $0x3e8,-0xc(%ebp)` 直落 —— 两臂各自 store ⇒ if/else 形
    //（P47：三目会生单 store + 常量入 eax 的异形）。
    if (param3)
        info.m_count40 = param2 + THROW_FEE;
    else
        info.m_count40 = BUY_FEE;
    alter();
    if (m_redeemList.size() > 0x14)
        m_redeemList.pop_front();
    m_redeemList.push_back(info);
    return 1;
}

// 未达 end()），逐项整体赋值 0x44 字节到 pData + i*0x44 + 8（ORIG 为
// rep movsl 0x11，结构整体赋值即内联该复制）。
void CRedeemItem::getData(char* pData) const
{
    // [b28d-L28 round2 + 2026-09-10 复测] ORIG 0x085f78bc 入口即在 begin() **之前**
    // `mov 0xc(%ebp),%eax; mov %eax,-0x20(%ebp)`（形参→具名局部 pData 副本），
    // 之后每处写点都 `mov -0x20(%ebp),%edx` 重载 ⇒ 原码**有**具名 `char* dst`
    // 局部（P0 槽往返律；旧「删缓存」判词作废）。
    // 循环计数：ORIG 尾测 `mov -0x1c(%ebp),%eax; cmp $0x14,%eax; ja`（两段 +
    // 无符号）⇒ 局部声明 int + 比较处显式 `(unsigned int)` 转换（P310 翻案形），
    // 实测复现两段寄存器形（`unsigned int i` 直写会被 4.4 折成 mem 直比）。
    // [复测读数] 本函数改后 **75=75 gap=0**；残余 = ①`dst+4` 写点 4.4 提前
    // `lea 0x4(%eax),%ebx` 缓存（ORIG `mov -0x20(%ebp),%edx; mov %eax,0x4(%edx)`）
    // ②循环体 RHS(*it) vs LHS 地址求值序（N76 族）③end() 临时槽位互换。
    // `char* volatile dst` 形实测 76 条（+1，volatile 中转）⇒ 已回退。
    char* dst = pData;
    std::list<Inven_Item_Expand_RedeemInfo>::const_iterator it =
        m_redeemList.begin();
    *(int*)(dst + 4) = (int)m_redeemList.size();
    int i = 0;
    for (; (unsigned int)i <= 0x14 && it != m_redeemList.end(); ++it, ++i)
        *(Inven_Item_Expand_RedeemInfo*)(dst + i * 0x44 + 8) = *it;
}


// ORIG T 0x085f73ee _ZN11Redeem_Item11CRedeemItem15Send_RedeemListEP5CUser：
// header(0,0x121)/byte(list size)/逐项（≤0x15 项）：+0x40 计数、Inven_Item
// 字段展开、附魔通道分支、增幅属性、3rd Chronicle 子包与 put_packet，最后
// finalize + Send 并返回 1。
int CRedeemItem::Send_RedeemList(CUser* user)
{
    // [b28d-46 判词·N/N82 翻正] ORIG 0x085f73ee 全程**无 Inven_Item 局部副本**：
    // 循环首 `call _ZNKSt14_List_iterator...ptEv; mov %eax,-0xc(%ebp)` 后逐处
    // `mov -0xc(%ebp),%eax; mov 0x2(%eax),%eax`（具名 entry 指针 + 每处重载，
    // P0/P167 `it->` 形）⇒ 删 memcpy/局部对象即消 `call memcpy` 与 0x3d 字节
    // 栈副本（帧 0x80→ORIG 0x30）。
    PacketGuard packet;
    packet.put_header(0, 0x121);
    packet.put_byte((int)m_redeemList.size());

    int count = 0;
    for (std::list<Inven_Item_Expand_RedeemInfo>::iterator it =
             m_redeemList.begin();
         // ORIG 0x085f75c1/0x085f75c4/0x085f75c7：`mov -0x10(%ebp),%eax;
         // cmp $0x14,%eax; ja` = **无符号序 + 两段寄存器形**（count > 20 退出）。
         // P196 前置：ORIG 无符号 ⇒ 禁改 int；按 P310 翻案形保留 int 局部 +
         // 比较处显式 (unsigned int) 转换即复现两段 + ja（直写 unsigned 局部
         // 被 4.4 折成单段 mem 直比 + jg，即改前态）。
         (unsigned int)count < (unsigned int)MAX_COUNT &&
             it != m_redeemList.end();
         ++it, ++count)
    {
        // ORIG 0x085f7452 单次 ptEv 后存具名指针 -0xc；其后每处字段访问
        // 都从该槽重载（禁缓存到 callee-saved）。
        Inven_Item_Expand_RedeemInfo* entry = it.operator->();

        packet.put_int(entry->m_count40);                    // +0x40 兑换计数
        packet.put_int(entry->m_item.m_addInfo);             // +0x02 物品 idx
        packet.put_int(entry->m_item.m_addInfo2);            // +0x07 数量/强化值
        packet.put_byte((int)entry->m_item.GetItemAttr());   // 属性字节
        packet.put_short((int)entry->m_item.m_fieldb);       // +0x0b 耐久
        packet.put_byte((int)(unsigned char)entry->m_item.m_field0); // +0x00 绑定标记

        if (!G_GameWorld()->IsEnchantRevisionChannel())
            packet.put_int(entry->m_item.m_fieldd);          // +0x0d 强化信息
        else
            packet.put_int(0);

        packet.put_byte((int)entry->m_item.m_amp.getAbilityType() & 0xff);
        packet.put_short((int)entry->m_item.m_amp.getAbilityValue() & 0xffff);

        WongWork::CItemUpgrade* upgrade = G_CDataManager()->m_itemUpgrade;
        upgrade->make3rdChroniclePacket(user, entry->m_item, &packet);
        packet.put_packet(entry->m_item);
    }

    packet.finalize(true);
    user->Send(packet);
    return 1;
}

}  // namespace Redeem_Item
