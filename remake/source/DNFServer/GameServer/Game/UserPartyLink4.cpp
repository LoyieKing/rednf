// ============================================================================
// UserPartyLink4.cpp —— LinkMisc4 批次 CUser/CParty/CDataManager 域 5 符号。
// 全部逐报告忠实转写（docs/class_func_reports/{CUser,CDataManager,CParty}/），
// ORIG mangled 由本 TU 的真实 C++ 定义产生：
//   _ZN5CUser14RewardTutorialEj                T 0x086475d8
//   _ZN5CUser18adjust_charac_statEv            T 0x08664766
//   _ZN5CUser18deleteGuildExpBookEv            T 0x0867e898
//   _ZN12CDataManager19getDungeonIndexListEv   T 0x08365434（非 const）
//   _ZN6CParty12drop_item_gmEP5CUserj          T 0x085a73a6
// ============================================================================
// b28d-45 判词汇总（本 TU 3 件 + 1 NEAR）：
//   deleteGuildExpBook 0x0867e898 NEAR2：N98 pair<ENUM,int>→pair<int,int> 转换
//     临时槽序（ORIG first=-0x10/second=-0xc）；右值引用具名化反劣 117→121 已回退。
//   adjust_charac_stat 0x08664766 DIFF127 N82/N89：_Additioal_info 栈对象 +
//     234:199 大 gap（std 模板/槽复用）。
//   RewardTutorial 0x086475d8 DIFF139 N82/N93：帧 0x60/0x50 + vector<int> 槽向。
//   drop_item_gm 0x085a73a6 DIFF245 N82/N89：257:255 CheckInTrade 早退岛 + 槽序。

#include <string.h>

#include <utility>
#include <vector>

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h   // CBattle_Field::drop_item(map_item) + WarField.h map_item
#include "CDataManager.h"    // G_CDataManager + m_serverParameter(+0x68) + find_item
#include "CInventory.h"      // insert_event_items / NumberToString
#include "Inven_Item.h"      // Inven_Item 布局与访问器
#include "LogManager.h"
#include "PacketGuard.h"
#include "ServerParameterScript.h"  // GetEscaladeTutorialReward

// ORIG 文件级枚举（同 CGameManager.cpp 模式；值 0 = 公会经验书 hardcode item）。
enum ENUM_HARDCODE_ITEM
{
    ENUM_HARDCODE_ITEM_0 = 0
};

// ---------------------------------------------------------------------------
// CUser::RewardTutorial(unsigned int)
// ORIG T 0x086475d8 sz 0x245，报告 docs/class_func_reports/CUser/RewardTutorial.md。
// 流程：取 ServerParameterScript::GetEscaladeTutorialReward(flag) 奖励清单；
// 非空时经 CInventory::insert_event_items 入包（moneyReason=3、itemReason=8、
// bLog=true、两个空串日志标签），随后 put_header(1,0x92) 包回执：byte(1)、
// byte(入包数)；>0 时对返回的槽位三元组逐个 put_byte/put_int/put_int，
// finalize(true) 后 Send。
// ---------------------------------------------------------------------------
void CUser::RewardTutorial(unsigned int flag)
{
    std::vector<int> outSlots;

    // P0：ORIG 0x086475eb `call G_CDataManager; lea 0x68(%eax),%edx; mov %edx,(%esp)`
    // 全程无槽往返 ⇒ 原码是内联表达式（无 serverParameter 具名局部）；
    // 同理 0x08647615 getCurCharacInvenW 结果留 %eax 直传 insert_event_items，
    // 源码须把 receiver 写成调用表达式（禁具名 inven 局部）。
    std::vector<std::pair<int, int> >* rewards =
        G_CDataManager()->m_serverParameter.GetEscaladeTutorialReward(flag);
    if (rewards == 0)
        return;

    int insertedCount = getCurCharacInvenW()->insert_event_items(
        *rewards, outSlots, (eMoneyAddReason)3, (eItemAddReason)8, true, "", "");

    PacketGuard packet;
    packet.put_header(1, 0x92);
    packet.put_byte(1);
    packet.put_byte(insertedCount);
    if (insertedCount > 0)
    {
		// ORIG 循环每次消费 3 个 int（槽位三元组）：short/int/int；
		// P28：ORIG 0x086476fb/0x08647734/0x0864776d 三处增量段恒调后置
		// `__normal_iterator::operator++(int)`（ppEi：`movl $0x0,0x8(%esp)` dummy
		// + sret 临时 -0x1c/-0x18/-0x14）⇒ 源形必须写 `it++`；`++it` 走 ppEv
		// 免临时槽（每处少 4 条，gap -8 主因）。
		std::vector<int>::iterator it = outSlots.begin();
		while (it != outSlots.end())
		{
			packet.put_short(*it);
			it++;
			packet.put_int(*it);
			it++;
			packet.put_int(*it);
			it++;
		}
	}
    packet.finalize(true);
    Send(packet);
}

// ---------------------------------------------------------------------------
// CUser::adjust_charac_stat()
// ORIG T 0x08664766 sz 0x381，报告 docs/class_func_reports/CUser/adjust_charac_stat.md。
// 流程：_Additioal_info 局部对象 ctor+clear → calc_lev_stat(calc) → 打印 before
// 快照（当前角色 addInfo 各字段）→ memcpy(calc → getCurCharacAddInfoRefW, 0x52)
// → 打印 after 快照。__PRETTY_FUNCTION__ 处按报告字面量转写。
// ---------------------------------------------------------------------------
void CUser::adjust_charac_stat()
{
    _Additioal_info calc;
    calc.clear();
    calc_lev_stat(calc);

    const _Additioal_info* before =
        static_cast<const _Additioal_info*>(getCurCharacAddInfoRefR());
    cMyTrace(__PRETTY_FUNCTION__, 0x44d1, 0)(
        "before adjust stat(%s(%d) : %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        getCurCharacName(), getCurCharacNo(),
        before->m_field00, before->m_field04,
        (int)before->m_field08, (int)before->m_field0a,
        (int)before->m_field0c, (int)before->m_field0e,
        before->m_field42,
        (short)before->m_field10[0], (short)before->m_field10[1],
        (short)before->m_field10[2], (short)before->m_field10[3]);

    _Additioal_info* current =
        static_cast<_Additioal_info*>(getCurCharacAddInfoRefW());
    // P83/N84：ORIG 0x08664911..0x08664987 为 0x52 字节结构整体赋值的内联展开
    // （对齐 peel 1B/2B + rep movsl + 尾 2B/1B，共 40 条、无 memcpy call）⇒
    // 源码必须是结构整体赋值让 4.4 展开；写 memcpy 恒退化为 `call memcpy`（gap -35 根因）。
    *current = calc;

    cMyTrace(__PRETTY_FUNCTION__, 0x44d7, 0)(
        "after adjust stat(%s(%d) : %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
        getCurCharacName(), getCurCharacNo(),
        current->m_field00, current->m_field04,
        (int)current->m_field08, (int)current->m_field0a,
        (int)current->m_field0c, (int)current->m_field0e,
        current->m_field42,
        (short)current->m_field10[0], (short)current->m_field10[1],
        (short)current->m_field10[2], (short)current->m_field10[3]);
}

// ---------------------------------------------------------------------------
// CUser::deleteGuildExpBook()
// ORIG T 0x0867e898 sz 0x1c4，报告 docs/class_func_reports/CUser/deleteGuildExpBook.md。
// 流程：deleteSpecificItem({make_pair((ENUM_HARDCODE_ITEM)0, 0x4b0)}, deleted)
// → send_itemspace(0) 失败则 logFormat(0x7c57) 返回 → send_itemspace(2) 失败则
// logFormat(0x7c5c) 返回 → send_RedeemInfo()。ORIG 尾部不设置返回寄存器（void）。
// ---------------------------------------------------------------------------
void CUser::deleteGuildExpBook()
{
    std::vector<std::pair<int, int> > requestItems;
    std::vector<std::pair<int, int> > deletedItems;

    // 保持 enum 实参：实测改成 `(int)` 会让 4.4 走另一条内联展开路径，
    // ndiff 4→27（gap +5，多出 mov/lea/call），TU 合计 90→87 连带回退三分，
    // 已回退。pair 的 first/second 写入序差（ORIG 先 first 后 second）
    // 属 4.4 对 `pair<ENUM,int>` → `pair<int,int>` 转换的固定形态，标 N。
    // b28d-45 复测：右值引用具名化（static_cast<T&&> 控声明序）反劣 117→121、
    // NEAR→DIFF，确认 pair 临时槽序为 4.4 固定形态，维持 N 封顶（ORIG
    // 0x0867e8b6 first 临时=-0x10 / second=-0xc）。
    requestItems.push_back(
        std::make_pair((ENUM_HARDCODE_ITEM)ENUM_HARDCODE_ITEM_0, 0x4b0));

    deleteSpecificItem(requestItems, deletedItems);

    // P248 档① 定论（P168a X1 不成立）：ORIG 0x0867e930 `call send_itemspace;
    // xor $0x1,%eax; test %al,%al; je` 的唯一复现形 = **callee 返回 bool +
    // 调用点裸 `if (!f())`**（P292 表：bool 返回的 `!f()` 直产 xor/test/je）。
    // 本仓 send_itemspace 早已 bool 化（CUser.h:3007 / CUser.cpp:8255），旧写
    // `^ 1` 恒被 4.4 折成 `cmp $1; setne; test` 三段 ⇒ 是配套漏改，非 N 封顶。
    if (!send_itemspace((int)ENUM_ITEMSPACE_0))
    {
        LogManager::logFormat(
            1, "user.cpp", __PRETTY_FUNCTION__, 0x7c57,
            "deleteGuildExpBook, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_INVENTORY ) return FAILED",
            NumberToString(get_acc_id(), 0));
        return;
    }
    if (!send_itemspace(2))  // ENUM_ITEMSPACE_CARGO（P248 档① 同形）
    {
        LogManager::logFormat(
            1, "user.cpp", __PRETTY_FUNCTION__, 0x7c5c,
            "deleteGuildExpBook, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_CARGO ) return FAILED",
            NumberToString(get_acc_id(), 0));
        return;
    }
    send_RedeemInfo();
}

// ---------------------------------------------------------------------------
// CParty::drop_item_gm(CUser*, unsigned int)
// ORIG T 0x085a73a6 sz 0x3df，报告 docs/class_func_reports/CParty/drop_item_gm.md。
// GM 掉落指定物品到队伍战场。ORIG eax 返回错误码：0 成功 / -1 物品不存在 /
// 0x13 目标交易中 / 0x16 战场掉落失败 / 0x17 绑定装备禁止 GM 丢弃；
// 本 TU 按 CParty.h:630 既有 void 声明定义（调用方 GmCmds_D 忽略返回值）。
// ---------------------------------------------------------------------------
int CParty::drop_item_gm(CUser* user, unsigned int itemIdx)
{
    if (user->CheckInTrade())
        return 0x13;                                   // ORIG 0x085a73c1 `mov $0x13,%ebx`

    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
        return -1;                                     // ORIG 0x085a73e8 `mov $0xffffffff,%ebx`

    // ORIG 0x085a73f2/0x085a7400：map_item ctor（-0xc8）**先于** Inven_Item ctor
    // （-0x71）发射 ⇒ 原码 `map_item drop;` 声明在 `Inven_Item invenItem;` 之前
    // （4.1 ctor 序 = 声明序；我方原把 drop 声明放在调用点处 ⇒ 多出 ctor 重排）。
    map_item drop;
    Inven_Item invenItem;
    invenItem.m_addInfo = (int)itemIdx;
    item->make_item(invenItem);                        // vtable slot make_item
    // ORIG 0x085a742a `mov -0x6f(%ebp),%eax; test %eax,%eax; je 085a754f`：
    // m_addInfo==0 时**跳过** equip 检查（非 early return）。
    // [本波·P225 推广] `&&` 两条件 4.4 恒折直跳（多一条 xor 归一）；拆嵌套 if
    // 复现 ORIG 0x085a742a/0x085a7439 各自独立的 `test/cmp + je`。
    if (invenItem.m_addInfo != 0)
    {
        if (invenItem.m_field1 == Inven_Item::ITEM_TYPE_EQUIP)
        {
            // 装备类物品实为 CEquipItem；+0x234 = CEquipItem::m_itemType。
            const CEquipItem* equip = static_cast<const CEquipItem*>(item);
            if (equip->m_itemType == Inven_Item::ITEM_TYPE_INVALID ||
                equip->m_itemType <= Inven_Item::ITEM_TYPE_9)
                return 0x17;                           // ORIG 0x085a745f `mov $0x17,%ebx`
        }
    }

    drop.m_count = 0;                                  // byte +0x00 = 0
    // ORIG 0x085a7470/0x085a747a：m_field1(+0x01)/m_addInfo(+0x02) 的拷贝发射在
    // set_add_info 之前（`movzbl -0x70; mov %al,-0xb7` / `mov -0x6f; mov %eax,-0xb6`）
    // ⇒ 原码这两条赋值写在 set_add_info(1) 之前；后移会整段错位。
    drop.m_item.m_field1 = invenItem.m_field1;         // byte +0x11（ITEM_TYPE）
    drop.m_item.m_addInfo = invenItem.m_addInfo;       // dword +0x12（item idx）
    drop.m_item.set_add_info(1);                       // 内嵌 item.add_info = 1
    drop.m_item.SetItemAttr(invenItem.GetItemAttr());  // +0x16 ← src GetItemAttr
    drop.m_item.m_fieldb = invenItem.m_fieldb;         // word +0x1b ← src endurance
    // ORIG 0x085a74ca：m_field0(+0x00) 的拷贝夹在 m_fieldb 与 m_fieldd 之间
    // （`movzbl -0x71; mov %al,-0xb8` 位于 0x0b/0x0d 两条之间）⇒ 语句序须如此。
    drop.m_item.m_field0 = invenItem.m_field0;         // byte +0x10
    drop.m_item.m_fieldd = invenItem.m_fieldd;         // dword +0x1d ← src union
    // ORIG 0x085a74dd..0x085a7503：m_amp（stAmplifyOption_t，14B）拆为
    // guid 视图三条（+0x15/+0x19/+0x1d）在前、head 视图一条（+0x11）在后
    // ⇒ 原码是两次视图赋值（`m_guid` 先、`m_amp0Head` 后），整体赋值 4.4 恒按
    // 偏移升序（0x11 提前）错位。
    drop.m_item.m_amp.m_guid = invenItem.m_amp.m_guid;         // +0x15/+0x19/+0x1d
    drop.m_item.m_amp.m_amp0Head = invenItem.m_amp.m_amp0Head; // +0x11
    drop.m_item.m_random.m_random =                    // RandomOption 子块整体（14B）
        invenItem.m_random.m_random;
    drop.m_item.m_upgradeSep.SetUpgradeSeparate(
        invenItem.m_upgradeSep.GetUpgradeSeparate());  // +0x43

    int slotIdx = reinterpret_cast<CBattle_Field*>(&m_battleField)
                      ->drop_item(drop);               // 按值传 0x54 字节
    if (slotIdx == -1)
        return 0x16;                                   // ORIG 0x085a758b `mov $0x16,%ebx`

    // [本波·宽度旋钮] ORIG 0x085a7595/0x085a759b 发 `movw $0x12c,-0x20(%ebp)` /
    // `movw $0xf0,-0x1e(%ebp)`（两个 **short 具名局部**），传参时经 short→int
    // 提升 `movswl`（0x085a75e4/0x085a75f7）⇒ 原码是 short 局部而非字面量直传
    // （我方原写 `put_short(300)` 产 `movl $0x12c` 直压）。
    short field12c = 300;
    short fieldf0 = 240;
    PacketGuard packet;
    packet.put_header(0, 0x28);
    packet.put_short((unsigned short)user->get_unique_id());
    packet.put_short(field12c);                        // 0x12c
    packet.put_short(fieldf0);                         // 0xf0
    packet.put_short(slotIdx);
    packet.put_int(drop.m_item.m_addInfo);
    packet.put_byte(drop.m_item.GetItemAttr());
    packet.put_int(drop.m_item.get_add_info());
    packet.put_short(drop.m_item.m_fieldb);
    packet.put_int(drop.m_item.m_fieldd);
    packet.put_byte(drop.m_item.m_amp.getAbilityType());
    packet.put_short(drop.m_item.m_amp.getAbilityValue());
    packet.put_packet(drop.m_item);
    // ORIG 0x085a7703..0x085a7717：`movzbl; test %al,%al; je L; mov $0x0,%eax;
    // jmp E; L: mov $0x1,%eax; E:` —— 4.1 直测无 `xor $1` 归一。写 `x==0 ? 1 : 0`
    // 时 4.4 具 XOR 归一（等价变换为 `x!=0 ? 0 : 1`）⇒ 改按**反极性**直接写
    // `x ? 0 : 1` 复现直测形（语义等值：m_count!=0→0、==0→1）。
    packet.put_byte(drop.m_count ? 0 : 1);
    packet.finalize(true);
    send_to_party(packet);
    return 0;                                          // ORIG 0x085a774b `mov $0x0,%ebx`
}
