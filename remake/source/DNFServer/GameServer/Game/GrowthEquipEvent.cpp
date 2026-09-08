// df_game_r Game/ GrowthEquipEvent 实现。
// 依据 docs/class_func_reports/GrowthEquipEvent/*.md + ORIG elf 反汇编：
//   ctor 0x080f6ef0：CEventBase() + 本类 vptr（无自有成员）
//   D1 0x080f6f0c / D0 0x080f6f3c：恢复 vptr 后调基类析构
//   StartEvent 0x080f6f5a / StartEvent(Word_Param) 0x080f6fac /
//   EndEvent 0x080f6ffe：SetEventFlag + cMyTrace
//   StartAction 0x080f7710：遍历世界用户表，state>2 者补发活动信息
//   IsEventing 0x080f7820（W）：转发 CEventBase::IsEventing
//   SelectGrowthWeapon 0x080f73be / SendGrowthWeaponEventInfo 0x080f7226
//   GiftGrowthWeaponBox 0x080f7050（礼包邮件 + DB_EventSaveGrowthWeapon）
// ============================================================================
// [b28d-55] 本 TU 判词表（verdict = game_func_report --no-compile，fresh .o）：
//   StartAction 0x080f7710           DIFF(51/51) 修法：非 const _Rb_tree_iterator
//     + `it++` 后置（P50/P96/P28）+ 去 world 具名局部、G_GameWorld() 内联进
//     begin/end 表达式（P0/P183）；P167 CLEAR(9/9)。
//   SendGrowthWeaponEventInfo 0x080f7226 DIFF(117/116) P167 CLEAR(22/22)；
//     残差 = P77 short→int 拓宽实参形 + 帧槽差（N82 族）。
//   GiftGrowthWeaponBox 0x080f7050   DIFF(115/116) P167 EXTRA ×1：
//     CEnvironment::get_server_group 多 1 次（ORIG 该路径不调）——待后续核对。
// ============================================================================
#include "GrowthEquipEvent.h"

#include "CUser.h"
#include "CDataManager.h"
#include "CEnvironment.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GlobalData.h"
#include "DB_EventUpdateGrowthWeaponCharac.h"
#include "DB_EventSaveGrowthWeapon.h"
#include "Inven_Item.h"
#include "RDARScriptStringManager.h"
#include "CUserCharacInfo.h"
#include "CMailBoxHelper.h"

#include <cstring>
#include "cMyTraceNoop.h"

// ============================================================================
// GrowthEquipEvent::SelectGrowthWeapon @ 0x080f73be
// 使用成长武器道具（Dispatcher_UseBoosterItem::_onBoosterItemUseResult_OnlyOne）：
// ORIG 守卫比较 CDataManager+0x4bc4 == itemId。该字段即 m_growthWeapon.m_giftBox
// （get_GiftGrowthWeaponBox 0x836561c 读同一偏移，CDataManager.h 布局 +0x4bac+0x18），
// 用具名字段替代裸偏移。
// ============================================================================
// [b23判词·N-PTR] P167 调用点差集 CLEAR（13/13）：ORIG 无 CSE，getGrowthWeaponEventdata()
// 调 3 次（已内联，不缓存 data）、serverGroup 走 [G_CEnvironment()+0x378] 直读成员（无
// get_server_group() call）。残余 gap+2 = ORIG 指针成员写用 `mov reg,DISP(base)` 直存，
// gcc 4.4 对 `*(T*)(void*+off)=v` 折成 `lea DISP(base),r; mov reg,(r)`（#2/#3 各 +1），
// 跨版本指针写形不可源码复现，语义等价。
void GrowthEquipEvent::SelectGrowthWeapon(CUser& user, int itemId, int addInfo)
{
    // ORIG +0x1b sete：仅本活动道具（== itemId）进入主体。
    if (G_CDataManager()->m_growthWeapon.m_giftBox == itemId)
    {
        // [b28c-L18] P211 注（pad 解引用⇒LHS 地址跨 call 缓存反例）：原裸偏移
        // `*(int*)(cast+0x04)=call()` 使 4.4 恒发 `lea 0x4(base),%ebx` 地址槽化；
        // 改 GrowthWeaponEvent 具名同型字段直存后复现 ORIG `mov %eax,%ebx` +
        // `mov %eax,0x4(%ebx)`（ORIG 0x080f70xx 段实证 base 指针入 ebx、disp 折叠 store）。
        // P48：ORIG 无 CSE，getGrowthWeaponEventdata() 调 3 次，每次结果寄存器直传。
        static_cast<GrowthWeaponEvent*>(user.getGrowthWeaponEventdata())->m_field4 =
            user.get_charac_no(-1);
        static_cast<GrowthWeaponEvent*>(user.getGrowthWeaponEventdata())->m_field8 = addInfo;

        // 渠道组：整合 PvP 基础渠道用用户 server group，否则直读环境
        // m_serverEnvir.m_serverGroup（ORIG [G_CEnvironment()+0x378]，无 get_server_group() call）。
        // ORIG 先置局部 0（mov [ebp-0xc],0），再 if/else 覆盖。
        int serverGroup = 0;
        if (G_GameWorld()->IsIntegratedPvPBaseChannel())
            serverGroup = (int)user.GetServerGroup();
        else
            serverGroup = G_CEnvironment()->m_serverEnvir.m_serverGroup;
        static_cast<GrowthWeaponEvent*>(user.getGrowthWeaponEventdata())->m_field12 =
            (short)serverGroup;

        DB_EventUpdateGrowthWeaponCharac::makeRequest(
            user.get_acc_id(), (unsigned int)user.get_charac_no(-1),
            addInfo, (short)serverGroup);

        SendGrowthWeaponEventInfo(user);
    }
}

// ============================================================================
// GrowthEquipEvent::SendGrowthWeaponEventInfo @ 0x080f7226
// 包头 0x1b9：byte 状态（2=礼包已领 / 1=当前角色已选且组一致 / 0=未选）、
// int 已选道具 idx、short +0x10 字段。
// ============================================================================
void GrowthEquipEvent::SendGrowthWeaponEventInfo(CUser& user)
{
    PacketGuard packet;
    packet.put_header(0, 0x1b9);

    // [b23S3C] ORIG 0x080f7226 形差（本件真语义缺口 gap 5）：
    // ①`movl $0x0,-0xc(%ebp)` 预清 ⇒ serverGroup 写 `= 0` + 分支内独立赋值（P198①两句式）；
    // ②getGrowthWeaponEventdata() ORIG 5 次重取（+0x6a/+0xab/+0xbd/+0x10e/+0x12b），
    //   无 `data` 具名指针槽 ⇒ P48 每用点重取；字段经 GrowthWeaponEvent（CUser.h:1797）
    //   具名访问出单条 disp（原 `*(int*)(data+0xc)` 下标形多 1 条 add，帧亦多 0x10）；
    // ③+0x12/+0x10 为 short 字段：`movzwl 0x12(%eax); movswl %ax,%edx` = 具名 short 成员
    //   与 int 局部比较的直传拓宽形（P200）。
    // [b23S3C] 续：①serverGroup 判定改正向 `if (IsIntegratedPvPBaseChannel())`（ORIG
    // fall-through = GetServerGroup 支，`test %al,%al; je <env>` 无 xor 归一，P191 极性）；
    // ②env 支 ORIG `call G_CEnvironment; mov 0x378(%eax),%eax` 直读字段 ⇒ 改
    //   m_serverEnvir.m_serverGroup（+0x198+0x1e0），不发 get_server_group() call
    // ③selected 去 bool 具名槽（ORIG 结果只占 %eax、
    //   `test %al,%al` 直测）⇒ 内联进 if/else 两 put_byte call。
    // [b28c-L18] 残余 3 复核：`movzwl+movswl %ax,%edx; cmp %eax,%edx` vs 4.4
    // `cwtl; cmp MEM,%eax` = N18a/N22（单段 regmove/内存直比不可翻）；操作数对调
    // （short 字段置左）已试不翻；**Main P230 推论实测负例**：本 TU 追加
    // `-march=i486 -mtune=generic` 重编，regmove/cmp 折叠形原样不动（cmov 族外，
    // 旗标对 lifetime/折叠类差异无效）。
    int serverGroup = 0;
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        serverGroup = (int)user.GetServerGroup();
    }
    else
    {
        serverGroup = G_CEnvironment()->m_serverEnvir.m_serverGroup;  // +0x378
    }

    if (static_cast<GrowthWeaponEvent*>(user.getGrowthWeaponEventdata())->m_fieldc != 0)
    {
        packet.put_byte(2);
    }
    else
    {
        if ((user.get_charac_no(-1) ==
             static_cast<GrowthWeaponEvent*>(user.getGrowthWeaponEventdata())->m_field4) &&
            // [b28c-L18] 操作数序对调：ORIG cmp 的 dst=短字段拓宽值（`movswl %ax,%edx`
            // 后 `cmp %eax,%edx`），即 short 字段为左操作数。
            (static_cast<GrowthWeaponEvent*>(user.getGrowthWeaponEventdata())->m_field12 ==
             serverGroup))
        {
            packet.put_byte(1);
        }
        else
        {
            packet.put_byte(0);
        }
    }
    packet.put_int(
        static_cast<GrowthWeaponEvent*>(user.getGrowthWeaponEventdata())->m_field8);
    packet.put_short(
        static_cast<GrowthWeaponEvent*>(user.getGrowthWeaponEventdata())->m_field10);
    packet.finalize(true);
    user.Send(packet);
}

// ============================================================================
// GrowthEquipEvent::GiftGrowthWeaponBox @ 0x080f7050（const）
// 活动礼包箱领取（每角色一次，data+0x00 状态字节守卫）：按脚本配置的礼包箱
// idx（m_growthWeapon.m_giftBox，ORIG CDataManager+0x4bc4）构造 Inven_Item，
// 经 RDAR 字符串 game_server_msg_277/278 走系统邮件，再发 DB 存档请求并刷新
// 邮件提醒 tooltip。
// ============================================================================
void GrowthEquipEvent::GiftGrowthWeaponBox(CUser& user) const
{
    char* data = static_cast<char*>(user.getGrowthWeaponEventdata());
    if (*data == '\x01')            // 已领取过：直接返回
        return;
    *data = '\x01';

    const int giftBoxIdx = G_CDataManager()->m_growthWeapon.m_giftBox;

    Inven_Item item;                // ORIG Inven_Item::Inven_Item() 0x080cb854
    CItem* itemBase = G_CDataManager()->find_item(giftBoxIdx);

    int serverGroup;
    if (!G_GameWorld()->IsIntegratedPvPBaseChannel())
        serverGroup = G_CEnvironment()->get_server_group();
    else
        serverGroup = (int)user.GetServerGroup();

    if (itemBase == 0)
        return;

    // vtable+0x08 = CItem::make_item(Inven_Item&) const
    itemBase->make_item(item);
    item.set_add_info(1);           // ORIG Inven_Item::set_add_info(item,1)

    // 邮件正文：RDAR 字符串 game_server_msg_278（strncpy 0xff 忠实保留）。
    char title[256];
    strncpy(title,
            g_scriptStringManager_.findString(
                4, "game_server_msg_278", static_cast<bool*>(0)),
            0xff);

    if (giftBoxIdx != 0)
    {
        const int titleLen = static_cast<int>(strlen(title));
        const unsigned int characNo =
            static_cast<unsigned int>(user.getCurCharacNo());
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            g_scriptStringManager_.findString(
                4, "game_server_msg_277", static_cast<bool*>(0)),
            item, 0u, characNo, title, titleLen, 0u,
            static_cast<ENUM_SERVER_GROUP>(serverGroup), false, false);
    }

    DB_EventSaveGrowthWeapon::makeRequest(user.get_acc_id(),
                                          static_cast<short>(serverGroup));
    user.SendEventMailAlaramTooltip();
}

// ============================================================================
// 生命周期（vtable 全覆盖，保证本 TU 发出的 _ZTV16GrowthEquipEvent 可解析）
// ============================================================================
GrowthEquipEvent::GrowthEquipEvent()
{
    // ORIG：CEventBase() 置基类 vptr 与 stADDINFO/+0x08 flag 后写本类 vptr；
    // 无自有成员，编译器生成的基类调用 + vptr 赋值即为忠实还原。
}

GrowthEquipEvent::~GrowthEquipEvent()
{
    // ORIG D1：恢复本类 vptr 后调 CEventBase::~CEventBase()。
}

void GrowthEquipEvent::StartEvent()
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x18, 0);
    tr("GrowthEquipEvent Event start!");
}

void GrowthEquipEvent::StartEvent(Word_Param /*param*/)
{
    SetEventFlag(true);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x1e, 0);
    tr("GrowthEquipEvent Event start! [monitor request]");
}

void GrowthEquipEvent::EndEvent()
{
    SetEventFlag(false);
    cMyTrace tr(__PRETTY_FUNCTION__, 0x24, 0);
    tr("GrowthEquipEvent Event end!");
}

void GrowthEquipEvent::StartAction()
{
    // ORIG：遍历 GameWorld 用户表（map<ushort,CUser*>），对 get_state()>2 的
    // 在线用户逐个补发 SendGrowthWeaponEventInfo。
    // b28d-55（P50/P96/P28）：ORIG 用**非 const** _Rb_tree_iterator（ne/ptEv/ppEi
    // 三个非 const 实例，P167 差集实证）⇒ const_iterator 会额外生成
    // _Rb_tree_const_iterator 转换构造与 const 版成员；增量段 ORIG 为 ppEi（后置）
    // ⇒ 源写 `it++`。
    // P0/P183：ORIG 无 world 槽——begin/end 两处各自 `call G_GameWorld;
    // lea 0x134(%eax),%edx` 就地折叠（ORIG 0x080f7716 / 0x080f778a 两处独立调用），
    // 故 G_GameWorld() 内联进循环头/条件表达式。
    for (std::map<unsigned short, CUser*>::iterator it =
             G_GameWorld()->m_UserInWorld.begin();
         it != G_GameWorld()->m_UserInWorld.end(); it++)
    {
        CUser* user = it->second;
        if (user->get_state() > 2)
            SendGrowthWeaponEventInfo(*user);
    }
}

bool GrowthEquipEvent::IsEventing(CUser* user) const
{
    // ORIG W 0x080f7820：尾调 CEventBase::IsEventing（编译器弱拷贝）。
    return CEventBase::IsEventing(user);
}
