// ============================================================================
// DispLinkA2.cpp —— 链接核销批次 A2（LinkHelper.DispLinkA2 独占 TU）。
//
// 14 个 Dispatcher 类符号（调用方 Dispatchers.cpp，本 TU 不改调用方）：
//   1  Dispatcher_DisJointAvatar::_selectJewelTypeIndex(stAvatarEmblemInfo_t const*) const
//      ORIG T 0x08218598 _ZNK25Dispatcher_DisJointAvatar21_selectJewelTypeIndexEPK20stAvatarEmblemInfo_t
//   2  Dispatcher_EPLPCommand::_BroadCastPacket(CParty*, MSG_EPLP_COMMAND&)
//      ORIG T 0x081fee14 _ZN22Dispatcher_EPLPCommand16_BroadCastPacketEP6CPartyR16MSG_EPLP_COMMAND
//   3  Dispatcher_EPLPCommand::_SendEPLPError(CParty*, unsigned char, unsigned char)
//      ORIG T 0x081fed4c _ZN22Dispatcher_EPLPCommand14_SendEPLPErrorEP6CPartyhh
//   4  Dispatcher_EPLPCommand::excludeEplpState(CUser*)
//      ORIG T 0x0825f760 _ZN22Dispatcher_EPLPCommand16excludeEplpStateEP5CUser
//   5  Dispatcher_EPLPCommand::isReTurnToVillage(CUser*)
//      ORIG T 0x0825f7f4 _ZN22Dispatcher_EPLPCommand17isReTurnToVillageEP5CUser
//   6  Dispatcher_EnchantByBead::check_error(CUser*, MSG_BASE&, ParamBase&)
//      ORIG T 0x081d233a _ZN24Dispatcher_EnchantByBead11check_errorEP5CUserR8MSG_BASER9ParamBase
//   7  Dispatcher_EnterDisjointStore::check_error(...)
//      ORIG T 0x081d00d8 _ZN29Dispatcher_EnterDisjointStore11check_errorEP5CUserR8MSG_BASER9ParamBase
//   8  Dispatcher_GuildCargo::check_error(...)
//      ORIG T 0x081d0a6a _ZN21Dispatcher_GuildCargo11check_errorEP5CUserR8MSG_BASER9ParamBase
//   9  Dispatcher_GuildCargoHistory::check_error(...)
//      ORIG T 0x081d0c3c _ZN28Dispatcher_GuildCargoHistory11check_errorEP5CUserR8MSG_BASER9ParamBase
//   10 Dispatcher_ImageCommunicationEquipmentUse::read(PacketBuf&, MSG_BASE&)
//      ORIG T 0x081e21cc _ZN41Dispatcher_ImageCommunicationEquipmentUse4readER9PacketBufR8MSG_BASE
//      （ORIG 忠实转写：函数体即 ret 0，合法空实现）
//   11 Dispatcher_New_Gmdebug_Command::oldGmRoutine(CUser*, PacketBuf&, std::vector<std::string>&)
//      ORIG T 0x0820bf2c _ZN30Dispatcher_New_Gmdebug_Command12oldGmRoutineEP5CUserR9PacketBufRSt6vectorISsSaISsEE
//   12 Dispatcher_PrecheckSoloTeleport::check_error(...)
//      ORIG T 0x081e1a9e _ZN31Dispatcher_PrecheckSoloTeleport11check_errorEP5CUserR8MSG_BASER9ParamBase
//   13 Dispatcher_RequestDisjointItem::check_error(...)
//      ORIG T 0x081cfc04 _ZN30Dispatcher_RequestDisjointItem11check_errorEP5CUserR8MSG_BASER9ParamBase
//   14 Dispatcher_Request_Seria_Buff::check_error(...)
//      ORIG T 0x081dbcd8 _ZN29Dispatcher_Request_Seria_Buff11check_errorEP5CUserR8MSG_BASER9ParamBase
//
// 证据：docs/class_func_reports/Dispatcher_<类>/<方法>.md（汇编 + 反编译 C 全文）。
// ============================================================================

#include "Dispatchers.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "CItem.h"
#include "CStackableItem.h"
#include "CDataManager.h"
#include "GameWorld.h"
#include "GlobalData.h"
#include "CParty.h"
#include "PvP_Room.h"
#include "MatchRoomList.h"        // IMatch（m_matching +0x6e4 槽的虚表接口，OnAskRematch @+0x68）
#include "CBattle_Field.h"
#include "GlobalScriptTutorial.h"
#include "CExpertJob.h"
#include "Teleport.h"
#include "ServiceRestrictManager.h"
#include "CEventTable.h"          // CGuildServerProxy.h 经此可见 CEventBase/CEventManager
#include "CGuildServerProxy.h"    // CEventManager::GetRepeatEvent / CServerProxyMgr
#include "CServerProxyMgr.h"

#include <string>
#include <vector>
#include <utility>
#include <set>

// ---- 跨 TU 声明（ORIG 真实符号、尚无 owner 定义；本 TU 只调用不定义） ----

// ORIG T 0x0820ba90 _Z19DoUserDefineCommandP5CUseriPKc（GM 自定义命令解释器，
// 归 GmCmds/GameMaster 域批次）
int DoUserDefineCommand(CUser* user, int cmd, const char* arg);

// ORIG T 0x0820bb68 _Z28SendVillageAttackedGMCommandP5CUseri（村庄攻击 GM 通知，
// 归 Village/GmCmds 域批次）
void SendVillageAttackedGMCommand(CUser* user, int state);

// ORIG T 0x08545d24 _ZN16CItemMakingSkill21SetItemMakingSkillPointEiiP5CUser
// （CItemMakingSkill 类当前无权威头；引用方仅 oldGmRoutine "itemskill"。
//  GetCharacExpandData(user,10) 返回该扩展数据指针。）
class CItemMakingSkill
{
public:
    void SetItemMakingSkillPoint(int a, int b, CUser* user);
};
// ORIG T 0x08644272 _ZNK17TowerOfDespairMgr31IsTowerOfDespairDungeon_ByIndexEi：
// m_layerSet(+0x328, set<int>).find(idx) != end()。
// 已由 TowerOfDespairMgr.cpp（TOD 域 owner）按报告定义真实成员，
// 本 TU 只调用不定义。
#include "TowerOfDespairMgr.h"

// ============================================================================
// 局部辅助
// ============================================================================

// ORIG 自由符号 _Z22getAvatarEmblemTypeIdxt（W 0x0822a98c，定义在本 TU 尾部）
int getAvatarEmblemTypeIdx(unsigned short type);

// MSG_EPLP_COMMAND：ORIG 无独立导出符号，仅经 _BroadCastPacket 以引用访问。
// 报告反编译只读 +0x0d/+0x0e 两个命令字节（Dispatchers.cpp 定义同布局视图；
// 前 0xd 字节为包头区【推断】，不设 static_assert）。
struct MSG_EPLP_COMMAND_A2
{
    char m_pad00[0xd];        // 包头区 +0x00..+0x0c
    unsigned char m_cmd;      // +0x0d：命令字节
    unsigned char m_subCmd;   // +0x0e：子命令字节
};

// ParamBase+0x0e 的 ushort uid 视图（ORIG movzwl 0xe(%eax) disp 直读，
// 指针具名槽往返：EnterDisjointStore/RequestDisjointItem check_error 共用）。
struct __attribute__((packed)) ParamUidView_A2
{
    char m_pad00[0xe];
    unsigned short m_uid;
};

// ParamBase+0x0e/+0x12 两个 ushort 槽位视图（EnchantByBead::check_error 专用：
// ORIG 0x081d2396/0x081d23d2 为 movzwl 0xe/0x12(%eax),%eax + movswl %ax,%ebx 两段
// —— 字段按 unsigned short 读、以 (short) 截断后拓宽为 int 实参，P200 形）。
struct __attribute__((packed)) ParamSlotView_A2
{
    char m_pad00[0xe];
    unsigned short m_slot1;   // +0x0e
    unsigned short m_pad10;   // +0x10..+0x11
    unsigned short m_slot2;   // +0x12
};

// [b28d-Main·检查点18] 删除死助手 A2_SendGmMessage（全 TU 无调用点；ORIG 无该符号）。

// ============================================================================
// 1. Dispatcher_DisJointAvatar::_selectJewelTypeIndex
//    报告 docs/class_func_reports/Dispatcher_DisJointAvatar/_selectJewelTypeIndex.md
//    [b28d-solo-DispLinkA2 判词] verdict=IDENTICAL（compare_tu clean：ORIG 125/OURS 124；
//    唯一残差 = ORIG 尾部 1 条对齐 nop，§6.1 豁免）。修法两处：
//    ①`typeCounts[idx] = typeCounts[idx] + 1;` → `typeCounts[idx]++;`
//      （P38 栈数组变体：4.4 对 `a[i]=a[i]+1` 恒双读下标 `mov idx,%eax; mov idx,%edx; …`；
//       `a[i]++` 复现 ORIG 单读 `mov idx,%eax; mov a(,%eax,4),%edx; add $1; mov %edx,a(,%eax,4)`。
//       探针 /tmp/a2probe/p.cpp probeA/H 对照。）
//    ②`weight = typeCounts[j] * (10000/total);` → `(10000/total) * typeCounts[j]`
//      （ORIG 先 idiv 后取数组成员；交换书写序令 4.4 同序，P23/P75。探针 p.cpp probeD/E。）
// ============================================================================
int Dispatcher_DisJointAvatar::_selectJewelTypeIndex(
    stAvatarEmblemInfo_t const* jewelSocketData) const
{
    int typeCounts[5] = {0, 0, 0, 0, 0};   // -0x3c..-0x2c：各纹章类型计数
    int cumul[5]      = {0, 0, 0, 0, 0};   // -0x50..-0x40：累计概率分布
    int total = 0;                         // -0x28
    int totalWeight = 0;                   // -0x24
    int picked;                            // -0x20
    int i;                                 // -0x1c
    int idx;                               // -0x18
    int j;                                 // -0x14
    int weight;                            // -0x10
    int k;                                 // -0xc

    for (i = 0; i < 5; ++i)
    {
        // stAvatarEmblemInfo_t pack(1)：5×{ushort emblemSocketType_@+i*6, int emblemItemIdx_}
        if (jewelSocketData->stEmblemSocket[i].emblemSocketType_ == 0)
            continue;
        if (jewelSocketData->stEmblemSocket[i].emblemSocketType_ == 0xffef)
            continue;
        if (jewelSocketData->stEmblemSocket[i].emblemSocketType_ == 0xffff)
            continue;
        // ORIG：idx = _getJewelTypeIndex(type)（getAvatarEmblemTypeIdx），-1 → continue
        idx = _getJewelTypeIndex(
            jewelSocketData->stEmblemSocket[i].emblemSocketType_);
        if (idx == -1)
            continue;
        // [b28d-solo-DispLinkA2] P38 栈数组变体：`arr[i]=arr[i]+1` 在 4.4 恒双读下标
        // （mov idx,%eax; mov idx,%edx; …）；`arr[i]++` 复现 ORIG 单读
        // （mov idx,%eax; mov arr(,%eax,4),%edx; add $1; mov %edx,arr(,%eax,4)）。
        typeCounts[idx]++;
        total = total + 1;
    }

    if (total == 0)
        return -1;

    for (j = 0; j < 5; ++j)
    {
        // [b28d-solo-DispLinkA2] ORIG 先 idiv 后取数组成员（右操作数先求值）；
        // 交换乘法书写序令 4.4 同序（P23/P75：整乘交换律语义等价）。
        weight = (10000 / total) * typeCounts[j];
        cumul[j] = weight + totalWeight;
        totalWeight = totalWeight + weight;
    }

    picked = get_rand_int(totalWeight);         // ORIG 0x086b1b87 get_rand_int(int)

    for (k = 0; k < 5; ++k)
    {
        if (picked < cumul[k])
            return k;
    }
    return -1;
}

// ORIG 汇编（0x0821857c sz 0x1b）：形参 spill 后 movzwl 直传尾调，不读返回值。
int Dispatcher_DisJointAvatar::_getJewelTypeIndex(unsigned short type) const
{
    return getAvatarEmblemTypeIdx(type);
}

// 自由函数 getAvatarEmblemTypeIdx（ORIG W 0x0822a98c，定义见下）：
// cmpw $0x10; jbe → type>0x10 返 -1；自 4 位向 0 位 sar 扫描首个 (type>>i)==1
// 的位序号，未命中返回 -1。
int getAvatarEmblemTypeIdx(unsigned short type)
{
    if (type > 0x10)
        return -1;
    for (int i = 4; i >= 0; --i)
    {
        if ((type >> i) == 1)
            return i;
    }
    return -1;
}
// ============================================================================
// 2/3. Dispatcher_EPLPCommand::_SendEPLPError / _BroadCastPacket
//     报告 docs/class_func_reports/Dispatcher_EPLPCommand/{_SendEPLPError,_BroadCastPacket}.md
// ============================================================================
void Dispatcher_EPLPCommand::_SendEPLPError(CParty* party, unsigned char subCmd,
                                            unsigned char errCode)
{
    PacketGuard guard;                       // 0x0858dd4c
    guard.put_header(1, 0x4b);
    guard.put_byte(0);                       // 类型 0：错误应答
    guard.put_byte(static_cast<int>(errCode));   // -0x20 参数（先写）
    guard.put_byte(static_cast<int>(subCmd));    // -0x1c 参数（后写）
    guard.finalize(true);
    party->send_to_party(guard);             // 0x0859d14e
}

void Dispatcher_EPLPCommand::_BroadCastPacket(CParty* party, MSG_EPLP_COMMAND& msg)
{
    PacketGuard guard;
    guard.put_header(1, 0x4b);
    guard.put_byte(1);                       // 类型 1：广播
    guard.put_byte(static_cast<int>(         // msg+0x0d（ORIG 参数槽直读）
        reinterpret_cast<MSG_EPLP_COMMAND_A2&>(msg).m_cmd));
    guard.put_byte(static_cast<int>(         // msg+0x0e
        reinterpret_cast<MSG_EPLP_COMMAND_A2&>(msg).m_subCmd));
    guard.finalize(true);
    party->send_to_party(guard);
}

// ============================================================================
// 4. Dispatcher_EPLPCommand::excludeEplpState
//    报告 docs/class_func_reports/Dispatcher_EPLPCommand/excludeEplpState.md
// ============================================================================
bool Dispatcher_EPLPCommand::excludeEplpState(CUser* user)
{
    CParty* party = static_cast<CParty*>(user->GetParty());        // 0x0865514c
    if (party == 0)
        return false;

        // ORIG: TowerOfDespairMgr::IsTowerOfDespairDungeon_ByIndex(
    //           GlobalData::s_pTowerOfDespairMgr, bf(party).get_dungeon_index())
    // （TowerOfDespairMgr.cpp 按报告定义的 const 成员，0x08644272）
    if (static_cast<TowerOfDespairMgr*>(GlobalData::s_pTowerOfDespairMgr)
            ->IsTowerOfDespairDungeon_ByIndex(
                static_cast<int>(reinterpret_cast<CBattle_Field*>(
                    &party->m_battleField)->get_dungeon_index())))
        return false;

    if (user->getMoveSpace() == (UserSpace::T)1)   // 0x08690eca
        return false;

    // ORIG: GlobalScriptTutorial::IsGlobalTutorialDungeon(idx)（0x08ab976a，静态成员）
    if (GlobalScriptTutorial::IsGlobalTutorialDungeon(
            reinterpret_cast<CBattle_Field*>(&party->m_battleField)
                ->get_dungeon_index()))
        return false;
    return true;
}

// ============================================================================
// 5. Dispatcher_EPLPCommand::isReTurnToVillage
//    报告 docs/class_func_reports/Dispatcher_EPLPCommand/isReTurnToVillage.md
// ============================================================================
bool Dispatcher_EPLPCommand::isReTurnToVillage(CUser* user)
{
    if (user == 0)
        return false;
    CParty* party = static_cast<CParty*>(user->GetParty());
    if (party == 0)
        return false;
    if (user->getMoveSpace() == (UserSpace::T)1)
        return true;
    // [b28c-30] ORIG 0x0825f7f4 段：mov -0xc(%ebp),%eax; add $0xb24,%eax;
    // mov %eax,(%esp); call CBattle_Field::get_dungeon_index ⇒ 直成员调用形
    // （原 A2_dungeonIndexOf 局部 helper 生 call _ZL17A2_dungeonIndexOfP6CParty）。
    if (GlobalScriptTutorial::IsGlobalTutorialDungeon(
            reinterpret_cast<CBattle_Field*>(&party->m_battleField)
                ->get_dungeon_index()))
        return true;
    return false;
}

// ============================================================================
// 6. Dispatcher_EnchantByBead::check_error
//    报告 docs/class_func_reports/Dispatcher_EnchantByBead/check_error.md
//    [b28d-solo-DispLinkA2 判词] verdict=IDENTICAL_AE（compare_tu clean：107/107；唯一残差
//    `mov &s_pSecuProtectionField,%eax` vs .o 未解析 R_386_32 显示 0x0，P264 归一口径）。
//    **真语义 bug（实参取错形参）**：原码 `reinterpret_cast<ParamSlotView_A2*>(&param)`
//    读 0x14(%ebp)=param；ORIG 0x081d238d 读 **0x10(%ebp)=msg**。铁证：ORIG process
//    0x081d2534 亦从 msg(0x10(%ebp)) 读 +0xd/0xe/0x10/0x12 四字段；本 TU 其它 check_error
//    注释同判（「ORIG mov 0x10(%ebp) 即 msg」）⇒ 已改 `&msg`。
//    其余修法：②删 static inline A2_paramShort helper（P49：-O0 static inline 产真实 call
//    `_ZL13A2_paramShortR9ParamBasei`）+ 具名 short 局部 → 槽位以 `(short)` 截断作普通实参
//    直传（P200：4.4 产 ORIG 的 `movzwl 0xe/0x12(%eax),%eax; movswl %ax,%ebx` 两段）；
//    ③删缓存 inven/dm，改链式单语句（P52：m_addInfo 先物化 %ebx 跨 G_CDataManager() 保活；
//    getCurCharacInvenW 每处重调，ORIG 两次独立 call）；④set 访问改 const
//    （p167：ORIG-only `std::set<int>::find(int const&) const`，我方原为非 const 版）
//    + 具名 const_iterator 令 find 先于 end 求值（ORIG sret -0x28/-0x20 序）。
// ============================================================================
int Dispatcher_EnchantByBead::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    if (user->get_state() <= 2)
        return 0x7fffffff;

    // CSecu_ProtectionField::Check(CUser*, SECURITY_PROTCTION)（ORIG 0x08288a02）
    int err = GlobalData::s_pSecuProtectionField->Check(
        user, static_cast<SECURITY_PROTCTION>(0x2b));
    if (err != 0)
        return err;

    // [b28d-solo-DispLinkA2] 真语义 bug（实参取错形参）：ORIG 0x081d238d 读的是
    // **0x10(%ebp)=msg**（同 TU 其它 check_error 注释「ORIG mov 0x10(%ebp) 即 msg」
    // 同判；铁证 = ORIG process 0x081d2534 亦从 msg 读 +0xd/0xe/0x10/0x12）。
    // msg 指针落 -0x18 具名槽，两处槽位以 (short) 截断后作普通实参直传 ⇒ 4.4 复现
    // ORIG 的 movzwl 0xe/0x12(%eax),%eax; movswl %ax,%ebx（P200 拓宽直传 + P52 保活）。
    ParamSlotView_A2* view = reinterpret_cast<ParamSlotView_A2*>(&msg);

    Inven_Item* item1 = user->getCurCharacInvenW()->GetInvenRef(   // 0x080da28e / 0x084fc2ae
        1, static_cast<short>(view->m_slot1));
    if (item1 == 0)
        return 0x11;

    Inven_Item* item2 = user->getCurCharacInvenW()->GetInvenRef(
        1, static_cast<short>(view->m_slot2));
    if (item2 == 0)
        return 0x11;

    // ORIG 0x081d240b：m_addInfo 先物化 %ebx、跨 G_CDataManager() 保活（P52 单语句链式）。
    CItem* pItem = G_CDataManager()->find_item(item1->m_addInfo);   // Inven_Item+0x02
    if (pItem == 0)
        return 0x11;

    // CItem 派生视图 +0x3d8 的可装配珠子类型集合（CStackableItem::m_set3d8）；
    // ORIG 每处重新取址（无引用局部槽），且 find 走 **const 版**
    // （p167：ORIG-only `set::find(int const&) const`）。
    // 具名 const_iterator 令 find 先求值（ORIG 先 find 后 end，sret -0x28/-0x20）。
    if (!static_cast<const CStackableItem*>(pItem)->m_set3d8.empty())
    {
        std::set<int>::const_iterator it =
            static_cast<const CStackableItem*>(pItem)->m_set3d8.find(
                item2->m_addInfo);
        if (it == static_cast<const CStackableItem*>(pItem)->m_set3d8.end())
            return 0x13;
    }
    return 0;
}

// ============================================================================
// 7. Dispatcher_EnterDisjointStore::check_error
//    报告 docs/class_func_reports/Dispatcher_EnterDisjointStore/check_error.md
// ============================================================================
int Dispatcher_EnterDisjointStore::check_error(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    if ((user->get_state() <= 2) ? true : (user->getCurCharacR() == 0))
        return -1;

    ParamUidView_A2* pUid = reinterpret_cast<ParamUidView_A2*>(&msg);  // ORIG mov 0x10(%ebp) 即 msg
    CUser* target = G_GameWorld()->find_from_world(pUid->m_uid);   // 0x086c4b9c
    if (target == 0)
        return 0x15;

    if (target->GetCurCharacExpertJobType() != 3)          // 0x0822f894
        return 0x13;
    expert_job::CExpertJob* job = 0;                               // ORIG movl $0,-0xc 声明预清
    job = target->GetCurCharacExpertJob();                         // 0x0822f8d4
    if (job == 0)
        return 0x13;
    if (job->GetType() != 3)                               // 0x08234796
        return 0x13;
    return 0;
}

// ============================================================================
// 8. Dispatcher_GuildCargo::check_error
//    报告 docs/class_func_reports/Dispatcher_GuildCargo/check_error.md
// ============================================================================
int Dispatcher_GuildCargo::check_error(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // P196 真语义 bug：ORIG 当 getCurCharacR()==0 才返 -1（原 !=0 方向反）。
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
        return -1;

    if (user->get_charac_guildkey() == 0)                  // 0x0822f46c
        return 100;
    if (!user->IsExistGuildAgit())                         // 0x082301d8（ORIG xor$1 形）
        return 0xb5;
    return 0;
}

// ============================================================================
// 9. Dispatcher_GuildCargoHistory::check_error
//    报告 docs/class_func_reports/Dispatcher_GuildCargoHistory/check_error.md
// ============================================================================
int Dispatcher_GuildCargoHistory::check_error(CUser* user, MSG_BASE& msg,
                                              ParamBase& param)
{
    // P196 真语义 bug：ORIG 当 getCurCharacR()==0 才返 -1（原 !=0 方向反）。
    if (user->get_state() != 3 || user->getCurCharacR() == 0)
        return -1;

    if (user->get_charac_guildkey() == 0)
        return 100;
    if (!user->IsExistGuildAgit())
        return 0xb5;
    return 0;
}

// ============================================================================
// 10. Dispatcher_ImageCommunicationEquipmentUse::read
//     报告 docs/class_func_reports/Dispatcher_ImageCommunicationEquipmentUse/read.md
//     ORIG 忠实转写：函数体为 ret 0（合法空实现，非桩）。
// ============================================================================
int Dispatcher_ImageCommunicationEquipmentUse::read(PacketBuf& packet, MSG_BASE& msg)
{
    (void)packet;
    (void)msg;
    return 0;
}

// ============================================================================
// 12. Dispatcher_PrecheckSoloTeleport::check_error
//     报告 docs/class_func_reports/Dispatcher_PrecheckSoloTeleport/check_error.md
// ============================================================================
int Dispatcher_PrecheckSoloTeleport::check_error(CUser* user, MSG_BASE& msg,
                                                 ParamBase& param)
{
    if (user == 0)
        return -1;
    int result = 0;                       // ORIG：先 0 初始化再赋 check_rule 结果（栈槽 -0xc）
    result = Teleport::check_rule(user);
    return result;
}

// ============================================================================
// 13. Dispatcher_RequestDisjointItem::check_error
//     报告 docs/class_func_reports/Dispatcher_RequestDisjointItem/check_error.md
// ============================================================================
int Dispatcher_RequestDisjointItem::check_error(CUser* user, MSG_BASE& msg,
                                                ParamBase& param)
{
    if ((user->get_state() <= 2) ? true : (user->getCurCharacR() == 0))
        return -1;

    if (ARAD::Singleton<ServiceRestrictManager>::Get()     // 0x081625e6（结果直传）
            ->isRestricted(user, RestrictCategory::Enum(1), 0x13))
        return 0xd1;

    ParamUidView_A2* pUid = reinterpret_cast<ParamUidView_A2*>(&msg);  // ORIG mov 0x10(%ebp) 即 msg
    CUser* target = G_GameWorld()->find_from_world(pUid->m_uid);
    if (target == 0)
        return 3;
    return 0;
}

// ============================================================================
// 14. Dispatcher_Request_Seria_Buff::check_error
//     报告 docs/class_func_reports/Dispatcher_Request_Seria_Buff/check_error.md
// ============================================================================
int Dispatcher_Request_Seria_Buff::check_error(CUser* user, MSG_BASE& msg,
                                               ParamBase& param)
{
    if (user->getCurCharacR() == 0)
        return -1;

    CEventBase* event = GlobalData::s_event_manager->GetRepeatEvent(0x5d);  // 0x08115998
    if (event->IsEventing(0))                        // vptr+0x34（slot 13）
    {
        if (!G_CDataManager()->m_seriaBlessing.is_possible_lv(user->get_charac_level()))
            return -1;
    }
    else
    {
        if (G_CDataManager()->m_growthPower.get_idx_break_away_section(
                user->get_charac_level()) < 0)
            return -1;
    }
    return 0;
}

// ============================================================================
// 11. Dispatcher_New_Gmdebug_Command::oldGmRoutine
//     报告 docs/class_func_reports/Dispatcher_New_Gmdebug_Command/oldGmRoutine.md
//     （ORIG 0x0820bf2c..0x082134a7，约 0x757b 字节；旧式 GM 调试命令解释器，
//      空格分隔命令字 → 分支分发。Dispatchers.cpp:11927 调用。）
//     [b28d-solo-DispLinkA2 判词] verdict=DIFF（超大函数 §6.2 三件套：**语义完成、
//     byte 层 N82+N-形态 封顶**）。
//     ① 调用点差集（p167_callsite.py，新鲜 /tmp/tu_DispLinkA2.o）：
//        ORIG calls=1374 / OUR calls=1374，**0 MISSING / 0 EXTRA = CLEAR** ⇒ 无语义缺口。
//     ② gap：OURS 6676 − ORIG 6625 = **+51**（本轮 6824→6676）。
//     ③ 帧/槽：ORIG `push edi/esi/ebx; sub $0xb7c` vs OURS 同三 push `sub $0xc3c`
//        （差 0xc0，本轮 0xd00→0xc3c）。深部对象偏移统一差 0xc0（base[10] 内部 0xb2c→0xbec、
//        char 缓冲 0x668→0x728、CSkillChanger 0x568→0x628）；浅部 dbgCmd/guard 差 0x68
//        （0x250→0x2b8）。逐块抽查（/tmp/a2_diff.py）差异主体为纯槽号平移
//        （`lea -0x32c(%ebp)` ↔ `lea -0x3b4(%ebp)`），属 N82（4.1 不按 lifetime 复用互斥块
//        局部 / 4.4 复用 ⇒ 全帧槽号错位）+ 跨版本 callee-saved 分配（P52/N7）与 bool 物化
//        形态（ORIG -0x12d 死存储 `mov %al,-0x12d`×4，返回路径恒 `mov $0x0,%ebx` = P171 死写）。
//        本轮实证修复（帧 −0xc4、insn −148）：`char b[0x100]; A2_ZERO(b);`（always_inline
//        循环，4.4 恒保留循环 + 每处 3 个辅助槽 -0x98/-0x90/-0x94 族）→
//        `char b[0x100] = {0};`（P214③/P312：复现 ORIG 内联 `mov $0x40,%edx; rep stos`），
//        并恢复 `push %edi`。残余 +51 条为上述形态差（无调用差集），按 §6.2 不再耗轮次。
// ============================================================================

#include "GameMasterCmd.h"              // DisPatcher_DebugCommand 前向域依赖
#include "DisPatcherCore.h"             // DisPatcher_DebugCommand / MSG_DEBUG_COMMAND
#include "CServerProxyMgr.h"            // CServerProxyMgr<T>::GetServerProxy
#include "CFairPvPScore.h"              // fair_pvp::CFairPvPScore
#include "CUserProc.h"                  // APSystem::CUserProc::GM_Set/GM_Reset
#include "CDailyScheduleManager.h"      // GM_Open*/GM_Close*
#include "SkillSlot.h"                  // CSkillChanger / SkillSlot 访问
#include "CRandomOptionItemHandleTest.h"
#include "CCirculationStatistic.h"
#include "RefPvpGrade.h"
#include "AvatarCoin.h"
#include "AvatarRechargeServer.h"       // GmReduceEndurance / GmChangeEnduranceReduceRate
#include "CMailBoxHelper.h"             // WongWork::CMailBoxHelper MultiMail
#include "StatisticsGmCmdPacket.h"
#include "Packet_CollectItemsGm.h"
#include "Packet_Game_Monitor_GM_Village_Attacked.h"
#include "Packet_Pcroom_Response.h"        // 全局 Packet_Pcroom_Response（ORIG pcroom 分支用无命名空间版）
#include "DB_UpdateInventoryCapacity.h"
#include "BreakAwayPreventSystem.h"

#include "Arad_DataManager.h"           // ARAD::Singleton / findGameScript
#include "Arad_EventPeriodDataManager.h"
#include "Arad_ServerStateManager.h"    // deleteState / insertState
#include "AradServerStateMessage.h"
#include "Arad_InternalDispatcher.h"    // make_internal_stream_jpn
#include "MoreTypes.h"                  // arad 域 Sig POD（SigSetCharacCreateTime）
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "TimerQueue.h"
#include "PacketDispatcher.h"
#include "CTimeGate.h"
#include "DataManagerInitDepends.h"   // TOD_UserState（wjfakd）
#include "CStatisticServerProxy.h"    // 完整类型（CServerProxyMgr<T> 实例化）
#include "CMonitorServerProxy.h"      // 完整类型 / SendTcpPacket
// ORIG _Z14StringToNumberPKc（T 0x081090fa，定义 DNFFunctionLib.cpp）
int StringToNumber(const char* str);

// ORIG _ZN4ARAD10uint32TimeEPs（T 0x081979c0，返回 long long；ARAD 域 TU 定义）
namespace ARAD
{
long long uint32Time(short* fields);
}

#include "BattleData.h"                // hellvalue（SetHellPartyValueTotal）
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

// CLocalChina_DB_LastDay_BreakAway（ORIG T 0x08128152）：CLocalChina 域类，
// 当前无权威头；最小成员声明（不产生定义，链接归 CLocalChina 域批次）。
class CLocalChina_DB_LastDay_BreakAway
{
public:
    // ORIG 0x08128152 静态（调用点只压 3 个参数，无 this；
    // 见 Dispatcher_New_Gmdebug_Command/oldGmRoutine.md +0x7414..+0x7428）。
    static void makeRequest(unsigned int accId, int day, int flag);
};

// AradServerStateMessage::AradServerStateMessage（ORIG W 0x08186652）已由
// AradServerStateMessage.h 声明（定义归 ARAD 域 TU）。


// [b28d-solo-DispLinkA2] GM 消息缓冲（ORIG local_66c，栈上 0x100 字节）：
// ORIG 0x0820d0e3 段为内联 `mov $0x40,%edx; rep stos`（P214③）⇒ 源形必须是
// `char b[0x100] = {0};`。原 always_inline A2_ZERO 循环在 4.4 恒保留循环 + 每处
// 3 个辅助槽（-0x98/-0x90/-0x94 族），帧 0xd00 vs ORIG 0xb7c 的主因之一。
// GM 文本消息包（ORIG show lp / max lp / ulti max / dimen max 内联展开序列）：
// 局部 PacketGuard + put_header(0,0xc)/put_byte(0)/put_short(0)/put_byte(0) +
// put_int(str.size())/put_str(str.c_str(), str.size())/finalize(true)/Send。
// 用宏保证逐调用点内联展开（ORIG 无共享 helper 函数），len 走 std::string::size()。
#define A2_SEND_STR(puser, sobj)                                              \
    do {                                                                      \
        PacketGuard _g;                                                       \
        _g.put_header(0, 0xc);                                                \
        _g.put_byte(0);                                                       \
        _g.put_short(0);                                                      \
        _g.put_byte(0);                                                       \
        _g.put_int((sobj).size());                                           \
        _g.put_str((sobj).c_str(), (sobj).size());                           \
        _g.finalize(true);                                                    \
        (puser)->Send(_g);                                                    \
    } while (0)

int Dispatcher_New_Gmdebug_Command::oldGmRoutine(CUser* user, PacketBuf& packet,
                                                 std::vector<std::string>& args)
{
    (void)packet;                        // ORIG 仅持引用，函数体不消费
    DisPatcher_DebugCommand dbgCmd;      // local_254（ORIG 栈构造）
    PacketGuard guard;                   // local_260（函数级持有，末尾析构）
    unsigned int ret = 0;


    // ---- lawless：SetCurCharacChaosPoint（0 < v < 10000）----
    if (args[0] == "lawless")
    {
        int v = std::atoi(args[1].c_str());
        if (0 < v && v < 10000)
        {
            user->SetCurCharacChaosPoint(std::atoi(args[1].c_str()));
            user->SendNotiPacket(CUser::eSendTarget_0, (ENUM_NOTIPACKET)2, 0);
            guard.put_header(1, 0xb3);
            guard.put_byte(1);
            guard.put_byte(100);
            guard.finalize(true);
            user->Send(guard);
            return 0;
        }
    }
    // ---- bal：help abuse ratio（0 < v < 10000，×100）----
    else if (args[0] == "bal")
    {
        int v = std::atoi(args[1].c_str());
        if (0 < v && v < 10000)
        {
            user->setCurCharacHelpAbuseComputedRatio(std::atoi(args[1].c_str()) * 100);
            user->SendNotiPacket(CUser::eSendTarget_0, (ENUM_NOTIPACKET)2, 0);
            guard.put_header(1, 0xb3);
            guard.put_byte(1);
            guard.put_byte(0x65);
            guard.finalize(true);
            user->Send(guard);
            return 0;
        }
    }
    // ---- questclear2 / q：GM 任务旗标开关/执行 ----
    else if (args[0] == "questclear2" || args[0] == "q")
    {
        if (!(args[1] == "on") && !(args[1] == "off"))
        {
            int v = std::atoi(args[1].c_str());
            if (0 < v && v < 100000)
            {
                user->setGmQuestFlag(true);
                user->quest_action(0x23, v, 0, 0);
                guard.put_header(1, 0xb3);
                guard.put_byte(1);
                guard.put_byte(0x66);
                guard.finalize(true);
                user->Send(guard);
                ret = 0;
                return 0;
            }
        }
    }
    // ---- guild exp ----
    else if (args[0] == "guild")
    {
        if (args[1] == "exp")
        {
            int v = std::atoi(args[2].c_str());
            if (0 < v && v < 10000)
            {
                user->set_guild_exp(std::atoi(args[2].c_str()));
                user->m_field796d9 = 1;                  // +0x796d9
                user->WorkPerFiveMin();
                guard.put_header(1, 0xb3);
                guard.put_byte(1);
                guard.put_byte(0x67);
                guard.finalize(true);
                user->Send(guard);
                ret = 0;
                return 0;
            }
        }
    }
    // ---- troublemaker on/off ----
    else if (args[0] == "troublemaker")
    {
        if (args[1] == "on")
        {
            user->setCurrCharacBlackCount(100);
            user->SendNotiPacket(CUser::eSendTarget_0, (ENUM_NOTIPACKET)2, 0);
            guard.put_header(1, 0xb3);
            guard.put_byte(1);
            guard.put_byte(0x68);
            guard.finalize(true);
            user->Send(guard);
            return 0;
        }
        if (args[1] == "off")
        {
            user->setCurrCharacBlackCount(0);
            user->SendNotiPacket(CUser::eSendTarget_0, (ENUM_NOTIPACKET)2, 0);
            guard.put_header(1, 0xb3);
            guard.put_byte(1);
            guard.put_byte(0x68);
            guard.finalize(true);
            user->Send(guard);
            return 0;
        }
    }
    // ---- fatigue set / fatigue <v> ----
    else if (args[0] == "fatigue")
    {
        if (args[1] == "set")
        {
            int v = std::atoi(args[2].c_str());
            if (-1 < v && v < 10000)
            {
                unsigned int maxFatigue = user->getCurCharacMaxFatigue();
                user->setCurCharacFatigue(static_cast<int>(maxFatigue & 0xffff) - v);
                user->SendFatigue();
                guard.put_header(1, 0xb3);
                guard.put_byte(1);
                guard.put_byte(0x69);
                guard.finalize(true);
                user->Send(guard);
                ret = 0;
                return 0;
            }
        }
        else
        {
            int v = std::atoi(args[1].c_str());
            if (-1 < v && v < 10000 && 0 < v)
            {
                user->FatigueUp(v);
                user->SendFatigue();
            }
        }
    }
    // ---- show lp：查询幸运点 ----
    else if (args[0] == "show" && args[1] == "lp")
    {
        int luck = user->GetCurCharacLuckPoint();
        std::string text("remained luck point : ");
        char b[0x100] = {0};
        std::sprintf(b, "(%d) ", luck);
        text += b;
        A2_SEND_STR(user, text);
        return 0;
    }
    // ---- max lp：+100000 幸运点 ----
    else if (args[0] == "max" && args[1] == "lp")
    {
        user->IncCurCharacLuckPoint(100000);
        user->enableSaveCharacStat();
        {
            std::string s("100000 luck point!!");
            A2_SEND_STR(user, s);
        }
        return 0;
    }
    // ---- item：GM 发物 ----
    else if (args[0] == "item")
    {
        unsigned long v = static_cast<unsigned long>(std::atoi(args[1].c_str()));
        dbgCmd._debugCommandAddItem(user, v, 100);
        return 0;
    }
    // ---- show / hide（顶级可见性）----
    else if (args[0] == "show")
    {
        MSG_DEBUG_COMMAND dbg;
        dbgCmd._debugCommandVisible(user, dbg);
        return 0;
    }
    else if (args[0] == "hide")
    {
        MSG_DEBUG_COMMAND dbg;
        dbgCmd._debugCommandInvisible(user, dbg);
        return 0;
    }
    // ---- dirty ----
    else if (args[0] == "dirty")
    {
        DoUserDefineCommand(user, 0x1f, args[1].c_str());
    }
    // ---- avatar：GM 发头像 ----
    else if (args[0] == "avatar")
    {
        MSG_DEBUG_COMMAND dbg;
        dbg.m_arg = std::atoi(args[1].c_str());
        dbg.payload.avatar.periodKind = static_cast<unsigned char>(std::atoi(args[2].c_str()));
        dbg.payload.avatar.slotByte = static_cast<signed char>(std::atoi(args[3].c_str()));
        dbgCmd._debugCommandAddAvatar(user, dbg);
        return 0;
    }
    // ---- combo：无操作（ORIG 匹配后无动作）----
    else if (args[0] == "combo")
    {
        // ORIG 空分支
    }
    // ---- coin：扣/清金币 ----
    else if (args[0] == "coin")
    {
        int v = std::atoi(args[1].c_str());
        unsigned int coin = user->getCurCharacInvenR()->GetCoin();
        unsigned int left = coin - v;
        if (static_cast<int>(left) < 0)
            left = 0;
        user->getCurCharacInvenW()->SetCoin(left);
        if (v == 0)
        {
            user->getCurCharacInvenW()->SetCoin(0);
            user->getCurCharacInvenW()->SetPayCoin(0);
            user->getCurCharacInvenW()->SetEventCoin(0);
        }
        user->SendCoin();
    }
    // ---- use item：重复 increase_status ----
    else if (args[0] == "use")
    {
        if (args[1] == "item")
        {
            int kind = std::atoi(args[2].c_str());
            int count = std::atoi(args[3].c_str());
            if (kind < 1 || 6 < kind)
            {
                ret = 0;
                return 0;
            }
            kind = kind + 2;
            for (int i = 0; i < count; ++i)
                user->increase_status(static_cast<short>(kind));
            user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, kind);
        }
    }
    // ---- npcfav / favterm / pickup / refund / durable / bluemarble：双参透传 ----
    else if (args[0] == "npcfav")
    {
        char b[0x100] = {0};
        std::sprintf(b, "%s %s", args[1].c_str(), args[2].c_str());
        DoUserDefineCommand(user, 0x24, b);
    }
    else if (args[0] == "favterm")
    {
        char b[0x100] = {0};
        std::sprintf(b, "%s %s", args[1].c_str(), args[2].c_str());
        DoUserDefineCommand(user, 0x23, b);
    }
    // ---- sumhell / sdale ----
    else if (args[0] == "sumhell")
    {
        DoUserDefineCommand(user, 0x15, args[1].c_str());
    }
    else if (args[0] == "sdale")
    {
        DoUserDefineCommand(user, 0x1c, args[1].c_str());
    }
    // ---- mdt / monsterdroptest / startmdt / endmdt / goldmdt ----
    else if (args[0] == "mdt" || args[0] == "monsterdroptest" || args[0] == "startmdt" ||
             args[0] == "endmdt" || args[0] == "goldmdt")
    {
        char b[0x100] = {0};
        if (args[0] == "startmdt")
            std::sprintf(b, "%d %s", 1, args[1].c_str());
        else if (args[0] == "endmdt")
            std::sprintf(b, "%d %s", 2, args[1].c_str());
        else if (args[0] == "goldmdt")
            std::sprintf(b, "%d %s", 3, args[1].c_str());
        else
            std::sprintf(b, "%d %s", 0, args[1].c_str());
        DoUserDefineCommand(user, 0x4b, b);
    }
    // ---- cli / checklottoitem ----
    else if (args[0] == "cli" || args[0] == "checklottoitem")
    {
        char b[0x100] = {0};
        if (args[1] == "ven")
        {
            std::sprintf(b, "%d %s", 1, args[2].c_str());
            DoUserDefineCommand(user, 0x4d, b);
        }
        else
        {
            std::sprintf(b, "%s %s", args[1].c_str(), args[2].c_str());
            DoUserDefineCommand(user, 0x4c, b);
        }
    }
    // ---- cbind / checkbind ----
    else if (args[0] == "cbind" || args[0] == "checkbind")
    {
        DoUserDefineCommand(user, 0x4e, args[1].c_str());
    }
    // ---- oneadayitem / craneDay / cinven ----
    else if ("oneadayitem" == args[0])
    {
        DoUserDefineCommand(user, 0x52, args[1].c_str());
    }
    else if ("craneDay" == args[0])
    {
        DoUserDefineCommand(user, 0x79, args[1].c_str());
    }
    else if (args[0] == "cinven")
    {
        DoUserDefineCommand(user, 0x46, args[1].c_str());
    }
    // ---- pickup / refund ----
    else if (args[0] == "pickup")
    {
        char b[0x100] = {0};
        std::sprintf(b, "%s %s", args[1].c_str(), args[2].c_str());
        DoUserDefineCommand(user, 0x47, b);
    }
    else if (args[0] == "refund")
    {
        char b[0x100] = {0};
        std::sprintf(b, "%s %s", args[1].c_str(), args[2].c_str());
        DoUserDefineCommand(user, 0x48, b);
    }
    // ---- tb reset/adjust/no ----
    else if (args[0] == "tb")
    {
        if (args[1] == "reset")
            DoUserDefineCommand(user, 0x5d, args[1].c_str());
        else if (args[1] == "adjust")
            DoUserDefineCommand(user, 0x5e, args[1].c_str());
        else if (args[1] == "no")
            DoUserDefineCommand(user, 0x5f, args[1].c_str());
    }
    // ---- achieve/ac reset/report/clear/all/no ----
    else if (args[0] == "achieve" || args[0] == "ac")
    {
        if (args[1] == "reset")
            DoUserDefineCommand(user, 0x60, args[1].c_str());
        else if (args[1] == "report")
            DoUserDefineCommand(user, 0x61, args[1].c_str());
        else if (args[1] == "clear")
            DoUserDefineCommand(user, 0x62, args[2].c_str());
        else if (args[1] == "all")
            DoUserDefineCommand(user, 99, args[1].c_str());
        else if (args[1] == "no")
            DoUserDefineCommand(user, 100, args[1].c_str());
    }
    // ---- power userpoint/guildpoint/win ----
    else if (args[0] == "power")
    {
        if (args[1] == "userpoint")
        {
            MSG_DEBUG_COMMAND dbg;
            dbg.m_arg = std::atoi(args[2].c_str());
            dbgCmd._debugCommandUserPowerwarPointUp(user, dbg);
            return 0;
        }
        else if (args[1] == "guildpoint")
        {
            int v = std::atoi(args[2].c_str());
            unsigned int characNo = user->getCurCharacNo();
            unsigned int guildKey = user->get_charac_guildkey();
            CGuildServerProxy* proxy =
                GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup());
            proxy->SendIncreaseDecreasePowerWarPoint(guildKey, characNo, v);
        }
        else if (args[1] == "win")
        {
            DoUserDefineCommand(user, 0x2b, args[2].c_str());
        }
    }
    // ---- townatk success/fail/end/其它 ----
    else if (args[0] == "townatk")
    {
        if (args[1] == "success")
            SendVillageAttackedGMCommand(user, 1);
        else if (args[1] == "fail")
            SendVillageAttackedGMCommand(user, 2);
        else if (args[1] == "end")
            SendVillageAttackedGMCommand(user, 3);
        else
            DoUserDefineCommand(user, 0x1e, args[1].c_str());
    }
    // ---- rec start/stop/run ----
    else if (args[0] == "rec")
    {
        if (args[1] == "start")
            DoUserDefineCommand(user, 0x12, args[1].c_str());
        else if (args[1] == "stop")
            DoUserDefineCommand(user, 0x13, args[1].c_str());
        else if (args[1] == "run")
            DoUserDefineCommand(user, 0x14, args[1].c_str());
    }
    // ---- spd / durable ----
    else if (args[0] == "spd")
    {
        DoUserDefineCommand(user, 9, args[1].c_str());
    }
    else if (args[0] == "durable")
    {
        char b[0x100] = {0};
        std::sprintf(b, "%s %s", args[1].c_str(), args[2].c_str());
        DoUserDefineCommand(user, 0x29, b);
    }
    // ---- sep upgrade/up success|s|fail|f ----
    else if (args[0] == "sep" && (args[1] == "upgrade" || args[1] == "up"))
    {
        char b[0x100] = {0};
        if (args[2] == "success" || args[2] == "s")
        {
            if (user->getUpgradeSeparateTrueFlag() == 0)
            {
                char ok[4];
                ok[0] = 'o'; ok[1] = 'k'; ok[2] = '\0'; ok[3] = '\0';
                user->setUpgradeSeparateTrueFlag(1);
                user->setUpgradeSeparateFalseFlag(0);
                std::sprintf(b, "sep upgrade success - %s", ok);
            }
            else
            {
                char end[5];
                end[0] = 'e'; end[1] = 'n'; end[2] = 'd'; end[3] = '\0'; end[4] = '\0';
                user->setUpgradeSeparateTrueFlag(0);
                std::sprintf(b, "sep upgrade success - %s", end);
            }
        }
        else if (args[2] == "fail" || args[2] == "f")
        {
            if (user->getUpgradeSeparateFlaseFlag() == 0)
            {
                char ok[4];
                ok[0] = 'o'; ok[1] = 'k'; ok[2] = '\0'; ok[3] = '\0';
                user->setUpgradeSeparateTrueFlag(0);
                user->setUpgradeSeparateFalseFlag(1);
                std::sprintf(b, "sep upgrade fail - %s", ok);
            }
            else
            {
                char end[5];
                end[0] = 'e'; end[1] = 'n'; end[2] = 'd'; end[3] = '\0'; end[4] = '\0';
                user->setUpgradeSeparateFalseFlag(0);
                std::sprintf(b, "sep upgrade fail - %s", end);
            }
        }
        {
            PacketGuard g;
            g.put_header(0, 0xc);
            g.put_byte(0);
            g.put_short(0);
            g.put_byte(0);
            g.put_int(static_cast<int>(std::strlen(b)));
            g.put_str(b, static_cast<int>(std::strlen(b)));
            g.finalize(true);
            user->Send(g);
        }
        return 0;
    }
    // ---- initstat / guilddebug / delguild / changename ----
    else if (args[0] == "initstat")
    {
        DoUserDefineCommand(user, 0x3f, args[1].c_str());
    }
    else if (args[0] == "guilddebug")
    {
        DoUserDefineCommand(user, 0x42, args[1].c_str());
    }
    else if (args[0] == "delguild")
    {
        DoUserDefineCommand(user, 0x2c, args[1].c_str());
    }
    else if (args[0] == "changename")
    {
        DoUserDefineCommand(user, 0x2d, args[1].c_str());
    }
    // ---- startevent（三参）/ endevent ----
    else if (args[0] == "startevent")
    {
        char b[0x100] = {0};
        std::sprintf(b, "%s %s %s", args[1].c_str(), args[2].c_str(), args[3].c_str());
        DoUserDefineCommand(user, 0x2e, b);
    }
    else if (args[0] == "endevent")
    {
        DoUserDefineCommand(user, 0x2f, args[1].c_str());
    }
    // ---- shopcheck / shopnpc ----
    else if (args[0] == "shopcheck")
    {
        DoUserDefineCommand(user, 0x30, args[1].c_str());
    }
    else if (args[0] == "shopnpc")
    {
        DoUserDefineCommand(user, 0x31, args[1].c_str());
    }
    // ---- cargo delete ----
    else if (args[0] == "cargo")
    {
        if (args[1] == "delete")
            DoUserDefineCommand(user, 0x36, args[1].c_str());
    }
    // ---- guildfund / dropitem ----
    else if (args[0] == "guildfund")
    {
        DoUserDefineCommand(user, 0x41, args[1].c_str());
    }
    else if (args[0] == "dropitem")
    {
        DoUserDefineCommand(user, 0x34, args[1].c_str());
    }
    // ---- premiumdelete/pd / mailitem ----
    else if (args[0] == "premiumdelete" || args[0] == "pd")
    {
        DoUserDefineCommand(user, 0x3c, args[1].c_str());
    }
    else if (args[0] == "mailitem")
    {
        DoUserDefineCommand(user, 0x3d, args[1].c_str());
    }
    // ---- ci true/false ----
    else if (args[0] == "ci")
    {
        if (args[1] == "true")
            DoUserDefineCommand(user, 0x6d, args[2].c_str());
        else if (args[1] == "false")
            DoUserDefineCommand(user, 0x6d, args[1].c_str());
    }
    // ---- ritemclear / difficulty ----
    else if (args[0] == "ritemclear")
    {
        DoUserDefineCommand(user, 0x76, args[1].c_str());
    }
    else if (args[0] == "difficulty")
    {
        DoUserDefineCommand(user, 0x78, args[1].c_str());
    }
    // ---- bluemarble ----
    else if (args[0] == "bluemarble")
    {
        char b[0x100] = {0};
        std::sprintf(b, "%s %s", args[1].c_str(), args[2].c_str());
        DoUserDefineCommand(user, 0x7d, b);
    }
    // ---- ulti max ----
    else if (args[0] == "ulti" && args[1] == "max")
    {
        int v = G_CDataManager()->get_limit_inout_count((ENUM_BLOOD_TYPE)2);
        user->setUltimateInoutValue(2, v);
        {
            std::string s("Add ultimate inout max count!!");
            A2_SEND_STR(user, s);
        }
    }
    // ---- dimen max ----
    else if (args[0] == "dimen" && args[1] == "max")
    {
        for (int i = 0; i < 6; ++i)
        {
            int v = G_CDataManager()->get_dimensionInout(i);
            user->setDemensionInoutValue(static_cast<char>(i), v);
        }
        {
            std::string s("Add dimension inout max count!!");
            A2_SEND_STR(user, s);
        }
    }
    // ---- dontsend on/off ----
    else if (args[0] == "dontsend")
    {
        if (args[1] == "on")
            user->SetDebugCommand(1);
        else if (args[1] == "off")
            user->SetDebugCommand(0);
    }
    // ---- pvpskilllevel ----
    else if (args[0] == "pvpskilllevel")
    {
        int v = std::atoi(args[1].c_str());
        if (0x25 < v)
        {
            return 0;
        }
        int nextPoint = GlobalData::s_ref_pvp_grade->GetPvpNextRankPoint(v);
        user->getPVPResultW()->m_expPoint = nextPoint;   // +0x08
        user->getPVPResultW()->m_pvpGrade = v;           // +0x14
        G_GameWorld()->send_AllBasicInfo(user);
        user->givePvPSkillTree(0, false, 3);
        user->send_skill_info();
    }
    // ---- lethe：清技能 ----
    else if (args[0] == "lethe")
    {
        reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())->clear_all_skills_both();
        reinterpret_cast<SkillSlot*>(user->getCurCharacSkillW())->set_parent(user);
        {
            WongWork::CSkillChanger changer;
            changer.SkillInitialize(user, 0, false);
            user->givePvPSkillTree(0, false, 3);
            user->send_skill_info();
        }
    }
    // ---- rematch ----
    else if (args[0] == "rematch")
    {
        PvP_Room* pvpRoom = static_cast<PvP_Room*>(user->GetPVPRoom());
        if (pvpRoom != 0)
        {
            // IMatch 接口已权威声明（MatchRoomList.h）：OnAskRematch(CUser*,int)
            // 位于虚表 +0x68。具名虚调用与原裸槽直调生成同一 vtable+0x68 派发，
            // 语义/链接不变，仅消除 hardcode 偏移（§4.3；同 PvP_Room.cpp 的
            // ((IMatch*)m_matching)->... 口径）。m_matching 以 int 存储。
            IMatch* match = reinterpret_cast<IMatch*>(pvpRoom->m_matching);
            if (match != 0)
                match->OnAskRematch(user, std::atoi(args[1].c_str()));
        }
    }
    // ---- wjfakd / wjfakd2 ----
    else if ("wjfakd" == args[0])
    {
        TOD_Layer layer(static_cast<unsigned short>(std::atoi(args[1].c_str())));
        TOD_UserState* state =
            reinterpret_cast<TOD_UserState*>(user->GetCharacExpandData(
                static_cast<ENUM_CHARAC_EXPAND_TYPE>(0xd)));
        state->setEnterLayer(layer);
        TOD_Layer today = state->getTodayEnterLayer();
        char b[0x100] = {0};
        std::sprintf(b, "set Enter Layer : %d",
                     static_cast<int>(today.GetLayer()) & 0xffff);
        TowerOfDespairMgr::SendMessageToCUser(user, b);
    }
    else if ("wjfakd2" == args[0])
    {
        if ("reload" == args[1])
        {
            Stream* s = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0xb719);
            CStreamGuard g(s, true);
            (*g)->operator<<(0x277);
            (*g)->operator<<(-1);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), g);
        }
        else if ("randomselect" == args[1])
        {
            Stream* s = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0xb720);
            CStreamGuard g(s, true);
            (*g)->operator<<(0x275);
            (*g)->operator<<(-1);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), g);
        }
        else
        {
            // ORIG：反序比较 "layerinfo" == args[1]，结果未使用（空分支）。
            (void)("layerinfo" == args[1]);
        }
    }
    // ---- mission 系列 ----
    else if ("mission" == args[0])
    {
        int v2 = std::atoi(args[2].c_str());
        int v1 = std::atoi(args[1].c_str());
        user->SetMissionParameter(v1, v2);
        user->send_MissionList();
    }
    else if ("newdailymission" == args[0])
    {
        user->ResetDailyMission();
        user->send_MissionList();
    }
    else if ("clearmission" == args[0])
    {
        user->ClearMission(static_cast<short>(std::atoi(args[1].c_str())));
        user->send_MissionList();
        user->send_pvp_record();
    }
    else if ("clearmissionkind" == args[0])
    {
        user->ClearMissionKind(static_cast<short>(std::atoi(args[1].c_str())));
        user->send_MissionList();
        user->send_pvp_record();
    }
    else if ("resetmissionlist" == args[0])
    {
        user->ResetMissionList();
        user->send_MissionList();
    }
    // ---- resetdaily ----
    else if ("resetdaily" == args[0])
    {
        G_GameWorld()->ResetAllFatigue();
        G_GameWorld()->ReselectDailyQuest();
        G_GameWorld()->AddDailyItem();
    }
    // ---- combomission ----
    else if ("combomission" == args[0])
    {
        short v = static_cast<short>(std::atoi(args[1].c_str()));
        short kind = user->getCurCharacGrowType() == 0 ? 0x1f : 0x20;
        user->Update_ComboMission(kind, v);
    }
    // ---- roiregen：ORIG 空分支（args[0] 匹配后直接 return 0）----
    else if ("roiregen" == args[0])
    {
        // ORIG 空分支
    }
    // ---- qp ----
    else if (args[0] == "qp")
    {
        user->setQuestPoint(std::atoi(args[1].c_str()));
    }
    // ---- registauction / burningstage ----
    else if (args[0] == "registauction")
    {
        random_option::CRandomOptionItemHandleTest test;
        test.test_regist_auction(user);
    }
    else if (args[0] == "burningstage")
    {
        int v = std::atoi(args[1].c_str());
        reinterpret_cast<fair_pvp::CFairPvPScore*>(user->GetCharacExpandData(
            static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))->SetRemainBonusCount(v);
        int daily = static_cast<int>(
            G_CDataManager()->m_serverParameter.m_schoolMatchDailyPlayLimit) - v;
        reinterpret_cast<fair_pvp::CFairPvPScore*>(user->GetCharacExpandData(
            static_cast<ENUM_CHARAC_EXPAND_TYPE>(7)))->SetDailyPlayCount(daily);
    }
    // ---- randitem ----
    else if (args[0] == "randitem")
    {
        if (args[1] == "")
        {
            return 0;
        }
        int itemIdx = std::atoi(args[1].c_str());
        Inven_Item item;                       // local_4d5（0x3d 字节）
        item.m_addInfo = itemIdx;              // +0x02
        CItem* pItem = G_CDataManager()->find_item(itemIdx);
        if (pItem == 0)
        {
            return 0;
        }
        pItem->make_item(item);                // vtbl+0x08 槽 make_item
        // ORIG +0x25 起连续写入 RandomOption 的三个 3-byte 字段
        // （m_field0/m_field3/m_field6）；不得从 m_field3.m_field1（+0x29）
        // 再叠加 +0x29，否则会越过 Inven_Item 的随机选项区。
        unsigned char* raw = &item.m_random.m_random.m_field0.m_field0;
        int idx = 2;
        for (int i = 0; i < 3; ++i)
        {
            if (args[idx] == "")
                break;
            if (args[idx + 1] == "")
                break;
            // ORIG 0x82100d0/0x821011d/0x8210166：相对 local_4b1 的
            // +1/+2/+3 即 RandomOption 三字节字段的 +0/+1/+2；第三个
            // 值复用第二个参数，保持 ORIG 行为。
            raw[i * 3 + 0] = static_cast<unsigned char>(std::atoi(args[idx].c_str()));
            raw[i * 3 + 1] = static_cast<unsigned char>(std::atoi(args[idx + 1].c_str()));
            raw[i * 3 + 2] = static_cast<unsigned char>(std::atoi(args[idx + 1].c_str()));
            idx += 2;
        }
        int slot = user->getCurCharacInvenW()->insertItemIntoInventory(
            item, (eItemAddReason)6, true, true);
        if (slot >= 0)
            user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
    }
    // ---- insertitem（拼串 find_item → AddItem）----
    else if (args[0] == "insertitem")
    {
        std::string name;
        for (int i = 1; i < 10; ++i)
        {
            if (args[i].length() == 0)
                break;
            if (name.length() != 0)
                name += " ";
            name += args[i];
        }
        CItem* pItem = G_CDataManager()->find_item(name.c_str());
        if (pItem == 0)
        {
            char b[0x100] = {0};
            std::sprintf(b, "cannot find item (%s) ", name.c_str());
            PacketGuard g;
            g.put_header(0, 0xc);
            g.put_byte(0);
            g.put_short(0);
            g.put_byte(0);
            g.put_int(static_cast<int>(std::strlen(b)));
            g.put_str(b, static_cast<int>(std::strlen(b)));
            g.finalize(true);
            user->Send(g);
            return 0;
        }
        {
            char b[0x100] = {0};
            std::sprintf(b, "item_index(%d) ", pItem->get_index());
            PacketGuard g;
            g.put_header(0, 0xc);
            g.put_byte(0);
            g.put_short(0);
            g.put_byte(0);
            g.put_int(static_cast<int>(std::strlen(b)));
            g.put_str(b, static_cast<int>(std::strlen(b)));
            g.finalize(true);
            user->Send(g);
        }
        int itemIdx = pItem->get_index();
        int count = 100;
        if (pItem->is_stackable() &&
            static_cast<CStackableItem*>(pItem)->getStackableLimit() < count)
        {
            count = static_cast<CStackableItem*>(pItem)->getStackableLimit();
        }
        ENUM_ITEMSPACE space = (ENUM_ITEMSPACE)0;
        int slot = user->AddItem(itemIdx, count, (eItemAddReason)6, space, 0);
        if (slot < 0)
        {
            return 0;
        }
        {
            char* accIdStr = NumberToString(user->get_acc_id(), 0);
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xb916, 0);
            trace("%s:\'%s\' GM Mode Add Item(%d).", accIdStr,
                  user->getCurCharacName(), slot);
        }
        user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
    }
    // ---- itemskill ----
    else if (args[0] == "itemskill")
    {
        int a = std::atoi(args[1].c_str());
        int b = std::atoi(args[2].c_str());
        CItemMakingSkill* skill = reinterpret_cast<CItemMakingSkill*>(
            user->GetCharacExpandData(static_cast<ENUM_CHARAC_EXPAND_TYPE>(10)));
        skill->SetItemMakingSkillPoint(a, b, user);
    }
    // ---- multimail stack/equip ----
    else if (args[0] == "multimail")
    {
        if (args[1] == "stack")
        {
            std::vector<std::pair<int, int> > items;
            items.clear();
            items.push_back(std::make_pair(5, 0xbdd));
            items.push_back(std::make_pair(4, 0xbda));
            items.push_back(std::make_pair(3, 0xbd9));
            items.push_back(std::make_pair(2, 0xbdb));
            items.push_back(std::make_pair(1, 0xbdc));
            Inven_Item base[10];   // ORIG 原地默认构造 10 元素（无临时/赋值）
            WongWork::CMailBoxHelper::MakeSystemMultiMailPostal(items, base, 10);
            // ORIG 把消息串存入栈局部再 strlen(local_9c)，非折叠；用指针变量复现。
            const char* pMsg = "stackable item test";
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
                "multimail test", base, 5, 1000, user->getCurCharacNo(),
                pMsg,
                static_cast<int>(std::strlen(pMsg)),
                0, (ENUM_SERVER_GROUP)99, false);
        }
        else if (args[1] == "equip")
        {
            std::vector<std::pair<int, int> > items;
            items.clear();
            items.push_back(std::make_pair(5, 10000));
            items.push_back(std::make_pair(5, 0x2711));
            items.push_back(std::make_pair(5, 0x2712));
            items.push_back(std::make_pair(5, 0x2713));
            items.push_back(std::make_pair(5, 0x2714));
            items.push_back(std::make_pair(5, 0x2715));
            items.push_back(std::make_pair(5, 0x2716));
            items.push_back(std::make_pair(5, 0x2717));
            items.push_back(std::make_pair(5, 0x2718));
            items.push_back(std::make_pair(5, 0x2719));
            Inven_Item base[10];   // ORIG 原地默认构造 10 元素（无临时/赋值）
            WongWork::CMailBoxHelper::MakeSystemMultiMailPostal(items, base, 10);
            // ORIG 把消息串存入栈局部再 strlen(local_9c)，非折叠；用指针变量复现。
            const char* pMsg = "equip item test";
            WongWork::CMailBoxHelper::ReqDBSendNewSystemMultiMail(
                "multimail test", base, 10, 9999, user->getCurCharacNo(),
                pMsg,
                static_cast<int>(std::strlen(pMsg)),
                0, (ENUM_SERVER_GROUP)99, false);
        }
    }
    // ---- statistic gold ----
    else if (args[0] == "statistic")
    {
        if (args[1] == "gold")
        {
            GetInstanceCirculationStatistic()->InsertCirculationStatistic();
        }
    }
    // ---- ingameday/day ----
    else if ("ingameday" == args[0] || "day" == args[0])
    {
        DoUserDefineCommand(user, 0x66, args[1].c_str());
    }
    // ---- setinven ----
    else if (args[0] == "setinven")
    {
        int cap = std::atoi(args[1].c_str());
        int oldCap = user->getCurCharacInvenR()->get_inventory_capacity();
        user->m_historyLog.ExpandInventory(oldCap, cap, 2);
        const_cast<CInventory*>(user->getCurCharacInvenR())->set_inventory_capacity(cap);
        int newCap = user->getCurCharacInvenR()->get_inventory_capacity();
        DB_UpdateInventoryCapacity::makeRequest(user->GetServerGroup(),
                                                static_cast<int>(user->GetUID()),
                                                user->getCurCharacNo(), newCap);
        guard.clear();
        guard.put_header(0, 0x42);
        guard.put_short(0xb);
        guard.put_short(user->getCurCharacInvenR()->get_inventory_capacity());
        guard.finalize(true);
        user->Send(guard);
    }
    // ---- itemseed ----
    else if (args[0] == "itemseed")
    {
        int itemIdx = std::atoi(args[1].c_str());
        CItem* pItem = G_CDataManager()->find_item(itemIdx);
        if (pItem == 0)
        {
            char b[0x100] = {0};
            std::sprintf(b, "cannot find item (%s) ", args[1].c_str());
            PacketGuard g;
            g.put_header(0, 0xc);
            g.put_byte(0);
            g.put_short(0);
            g.put_byte(0);
            g.put_int(static_cast<int>(std::strlen(b)));
            g.put_str(b, static_cast<int>(std::strlen(b)));
            g.finalize(true);
            user->Send(g);
            return 0;
        }
        int idx = pItem->get_index();
        int count = 100;
        if (pItem->is_stackable() &&
            static_cast<CStackableItem*>(pItem)->getStackableLimit() < count)
        {
            count = static_cast<CStackableItem*>(pItem)->getStackableLimit();
        }
        ENUM_ITEMSPACE space = (ENUM_ITEMSPACE)0;
        int slot = user->AddItem(idx, count, (eItemAddReason)6, space, 0);
        if (slot < 0)
        {
            return 0;
        }
        Inven_Item* ref = user->getCurCharacInvenRefW()->GetInvenRef(1, slot);
        if (ref != 0)
            ref->m_addInfo2 = std::atoi(args[2].c_str());   // +0x07 add_info
        {
            char* accIdStr = NumberToString(user->get_acc_id(), 0);
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0xb9fb, 0);
            trace("%s:\'%s\' GM Mode Add Item(%d).", accIdStr,
                  user->getCurCharacName(), slot);
        }
        user->SendUpdateItemList(CUser::eSendTarget_1, (ENUM_ITEMSPACE)0, slot);
    }
    // ---- moveavatar/mva ----
    else if (args[0] == "moveavatar" || args[0] == "mva")
    {
        PacketDispatcher* disp = G_PacketDispatcher();
        IDispatcher* d13 = disp->getDispatcher(0x13);
        if (d13 == 0)
        {
            return 0;
        }
        {
            PacketGuard g;
            g.put_header(1, 0x13);
            g.put_byte(1);
            g.put_short(0);
            g.put_int(0);
            g.put_int(1);
            g.put_byte(3);
            g.put_short(1);
            g.put_int(0x9b07);
            g.put_int(0);
            g.put_byte(0);
            g.put_byte(0);
            g.put_byte(0);
            g.finalize(false);
            g.set_index(0xf);
            // ORIG：vtbl[0]（dispatch_template）以 (obj, user, PacketBuf*) 调用。
            d13->dispatch_template(user, *g.get());
        }
    }
    // ---- quickon/qon ----
    else if (args[0] == "quickon" || args[0] == "qon")
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0 && party->get_member_count() >= 2)
        {
            party->send_quick_party_matching_result(true, 0);
            party->set_quick_party(true);
        }
        return 0;
    }
    // ---- quickoff/qoff ----
    else if (args[0] == "quickoff" || args[0] == "qoff")
    {
        CParty* party = static_cast<CParty*>(user->GetParty());
        if (party != 0 && party->get_member_count() >= 2)
        {
            party->send_quick_party_matching_result(false, 0);
            party->set_quick_party(false);
        }
        return 0;
    }
    // ---- quickreward/qr ----
    else if (args[0] == "quickreward" || args[0] == "qr")
    {
        int v = std::atoi(args[1].c_str());
        if (v < 0 || 10 < v)
        {
            return 0;
        }
        {
            CParty* party = static_cast<CParty*>(user->GetParty());
            if (party != 0 && party->get_member_count() >= 2)
            {
                party->set_gm_random_buff_type(static_cast<QuickParty::RandomBuffType>(v));
                char b[0x100] = {0};
                std::sprintf(b, "GM QUICK PARTY RANDOM BUFF (%d) TYPE!!", v);
                PacketGuard g;
                g.put_header(0, 0xc);
                g.put_byte(0);
                g.put_short(0);
                g.put_byte(0);
                g.put_int(static_cast<int>(std::strlen(b)));
                g.put_str(b, static_cast<int>(std::strlen(b)));
                g.finalize(true);
                party->send_to_party(g);
            }
        }
        return 0;
    }
    // ---- hellvalue/hv ----
    else if (args[0] == "hellvalue" || args[0] == "hv")
    {
        int v = std::atoi(args[1].c_str());
        if (v < 0 || 100 < v)
        {
            return 0;
        }
        {
            CParty* party = static_cast<CParty*>(user->GetParty());
            if (party != 0 && party->get_member_count() >= 2)
            {
                BattleData* bd = &party->m_battleData;   // +0x32c（MonC9 具名化：m_padBattleData→BattleData m_battleData）
                bd->SetHellPartyValueTotal(v);
                party->send_hellParty_value();
                char b[0x100] = {0};
                std::sprintf(b, "HELL PARTY VALUE (%d)",
                             bd->GetHellPartyValueTotal());
                PacketGuard g;
                g.put_header(0, 0xc);
                g.put_byte(0);
                g.put_short(0);
                g.put_byte(0);
                g.put_int(static_cast<int>(std::strlen(b)));
                g.put_str(b, static_cast<int>(std::strlen(b)));
                g.finalize(true);
                party->send_to_party(g);
            }
        }
        return 0;
    }
    // ---- statisticlog/sl ----
    else if (args[0] == "statisticlog" || args[0] == "sl")
    {
        StatisticsGmCmdPacket pkt;
        pkt.m_characNo = user->getCurCharacNo();      // +0x0a（ORIG local_34c）
        CStatisticServerProxy* proxy =
            GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendPacket(reinterpret_cast<char*>(&pkt), 0xe);
    }
    // ---- itemdrop / upcard ----
    else if (args[0] == "itemdrop")
    {
        DoUserDefineCommand(user, 0x6f, args[1].c_str());
    }
    else if (args[0] == "upcard")
    {
        DoUserDefineCommand(user, 0x70, args[1].c_str());
    }
    // ---- inde item drop ----
    else if (args[0] == "inde" && args[1] == "item" && args[2] == "drop")
    {
        std::string st;
        if (user->getForceDropFlag() == 1)
            st = "off";
        else
            st = "on";
        DoUserDefineCommand(user, 0x77, st.c_str());
    }
    // ---- timegate/tg ----
    else if (args[0] == "timegate" || args[0] == "tg")
    {
        Packet_CollectItemsGm pkt;
        pkt.m_a = std::atoi(args[1].c_str());            // +0x0b（ORIG local_368）
        pkt.m_b = std::atoi(args[2].c_str());            // +0x0f（ORIG local_364）
        CMonitorServerProxy* proxy =
            GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
        proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt), 0x17);
    }
    // ---- bossdifficult / bossmap ----
    else if (args[0] == "bossdifficult")
    {
        char v = static_cast<char>(StringToNumber(args[1].c_str()));
        if (v > '\0' && v < '\x05')
            user->m_gmBossDifficulty = v;              // +0x796cc
    }
    else if (args[0] == "bossmap")
    {
        unsigned int v = static_cast<unsigned int>(StringToNumber(args[1].c_str()));
        user->m_gmBossMapIndex = v;                    // +0x796d0
    }
    // ---- reset fatigue ----
    else if (args[0] == "reset" && args[1] == "fatigue")
    {
        G_TimerQueue()->InsertTimer(
            static_cast<TimerEntry::OBJ_TYPE>(2), 0,
            static_cast<TIMER_MESSAGE>(0x50), 3, 0, 0);
        return 0;
    }
    // ---- apply_effect ----
    else if (args[0] == "apply_effect")
    {
        int code = StringToNumber(args[1].c_str());
        int value = StringToNumber(args[2].c_str());
        if (code < 0)
        {
            ret = 0;
        }
        else
        {
            AradServerStateMessage msg;
            msg.m_active = static_cast<short>(value != 0);
            msg.m_endSec = static_cast<unsigned int>(value);
            GlobalData::s_serverStateManager_->deleteState(
                static_cast<ENUM_GLOBAL_EFFECT_ARAD>(code));
            GlobalData::s_serverStateManager_->insertState(
                static_cast<ENUM_GLOBAL_EFFECT_ARAD>(code), msg);
            ret = 0;
        }
        return 0;
    }
    // ---- open sd/boss/aa ----
    else if (args[0] == "open")
    {
        if (args[1] == "sd")
            GlobalData::s_DailyScheduleManager->GM_OpenSouthernDale();
        else if (args[1] == "boss")
            GlobalData::s_DailyScheduleManager->GM_OpenBossTower();
        else if (args[1] == "aa")
            GlobalData::s_DailyScheduleManager->GM_OpenAdvanceAltar();
    }
    // ---- close sd/boss/aa ----
    else if (args[0] == "close")
    {
        if (args[1] == "sd")
            GlobalData::s_DailyScheduleManager->GM_CloseSouthernDale();
        else if (args[1] == "boss")
            GlobalData::s_DailyScheduleManager->GM_CloseBossTower();
        else if (args[1] == "aa")
            GlobalData::s_DailyScheduleManager->GM_CloseAdvanceAltar();
    }
    // ---- ap set/reset/gmmode/realmode/week ----
    else if (args[0] == "ap")
    {
        if (args[1] == "set")
        {
            APSystem::CUserProc::GM_Set(user, static_cast<unsigned int>(std::atoi(args[2].c_str())));
        }
        else if (args[1] == "reset")
        {
            APSystem::CUserProc::GM_Reset(user);
        }
        else if (args[1] == "gmmode")
        {
            APSystem::CActionPointManager::_IsGMOn = 1;
        }
        else if (args[1] == "realmode")
        {
            APSystem::CActionPointManager::_IsGMOn = 0;
            APSystem::CActionPointManager::_gmWeekday.m_weekday = 0;
        }
        else if (args[1] == "week")
        {
            int v = std::atoi(args[2].c_str());
            int weekday = 7;
            switch (v)
            {
                case 1: weekday = 1; break;
                case 2: weekday = 2; break;
                case 3: weekday = 3; break;
                case 4: weekday = 4; break;
                case 5: weekday = 5; break;
                case 6: weekday = 6; break;
                case 7: weekday = 0; break;
                default:
                    ret = 0;
                    return 0;
            }
            APSystem::CActionPointManager::_gmWeekday.m_gmOn = 1;
            APSystem::CActionPointManager::_gmWeekday.m_weekday = weekday;
        }
    }
    // ---- pcroom ----
    else if (args[0] == "pcroom")
    {
        // ORIG pcroom：全局（非 Taiwan 命名空间）Packet_Pcroom_Response，
        // setter 均内联写成员，无独立 call；长度走结构 +0x02 的 len 字段（=0x1f）。
        Packet_Pcroom_Response pkt;
        pkt.m_field12 = 1;                                        // +0x12 local_38c
        pkt.m_field1a = 1;                                        // +0x1a local_384
        pkt.m_field1b = 0xb;                                      // +0x1b local_383
        std::time_t now = std::time(0);
        pkt.m_expireTime = static_cast<int>(now) + 0x93a80;      // +0x16 local_388
        pkt.m_uid = static_cast<int>(user->GetUID());            // +0x0a local_394
        pkt.m_accId = user->get_acc_id();                        // +0x0e local_390
        {
            Stream* s = GlobalData::s_stream_pool->Acquire(
                "PacketDispatcher_Impl_1.cpp", 0xbc0a);
            CStreamGuard g(s, true);
            (*g)->operator<<(0x17e);
            (*g)->operator<<(pkt.m_uid);                                       // ORIG: local_394 (复用已存 GetUID)
            g->put_binary(reinterpret_cast<char*>(&pkt),
                          static_cast<int>(sizeof(pkt)));         // ORIG len=local_39c=0x1f
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(1), g);
        }
    }
    // ---- avatarcoin ----
    else if (args[0] == "avatarcoin")
    {
        unsigned int v = static_cast<unsigned int>(std::atoi(args[1].c_str()));
        AvatarCoin* coinR = const_cast<AvatarCoin*>(
            &user->getCurCharacInvenR()->m_avatarCoin);  // +0x658（ORIG 用 R 读）
        AvatarCoin* coinW = &user->getCurCharacInvenW()->m_avatarCoin;  // +0x658
        unsigned int total = v + coinR->Get();
        coinW->Set(total);
        AvatarCoin::SaveToDB(user);
        AvatarCoin::SendSyncPacket(user);
    }
    // ---- avr sorneh/ghkrfbf ----
    else if (args[0] == "avr")
    {
        if (args[1] == "sorneh")
        {
            short a = static_cast<short>(std::atoi(args[2].c_str()));
            short b = static_cast<short>(std::atoi(args[3].c_str()));
            // ORIG 0x08212b42：GmReduceEndurance 为 static，直呼三参无 this
            // （不引入 Singleton<>::Get() call，cmp_calls 实证 ORIG 侧无该 call）。
            AvatarRechargeServer::GmReduceEndurance(user, a, b);
        }
        else if (args[1] == "ghkrfbf")
        {
            if (args[2] == "on")
                AvatarRechargeServer::GmChangeEnduranceReduceRate(
                    user, true, std::atoi(args[3].c_str()));
            else if (args[2] == "off")
                AvatarRechargeServer::GmChangeEnduranceReduceRate(user, false, 0);
        }
    }
    // ---- startva ----
    else if (args[0] == "startva")
    {
        if (user != 0)
        {
            Packet_Game_Monitor_GM_Village_Attacked pkt;
            pkt.m_flag = 1;                           // +0x0a（ORIG local_3b5）
            pkt.m_a = std::atoi(args[1].c_str());        // +0x0c（ORIG local_3b4）
            pkt.m_b = std::atoi(args[2].c_str());        // +0x10（ORIG local_3b0）
            pkt.m_c = std::atoi(args[3].c_str());        // +0x14（ORIG local_3ac）
            CMonitorServerProxy* proxy =
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup());
            proxy->SendTcpPacket(reinterpret_cast<char*>(&pkt), 0x17);
        }
    }
    // ---- SetAvengerReady ----
    else if (args[0] == "SetAvengerReady")
    {
        int v = std::atoi(args[1].c_str());
        ARAD::Arad_DataManager* dm = ARAD::Singleton<ARAD::Arad_DataManager>::Get();
        ARAD::Arad_EventPeriodDataManager* period =
            static_cast<ARAD::Arad_EventPeriodDataManager*>(
                // ORIG：std::string 直接就地构造进 by-value 形参槽（无 copy ctor），
                // 故内联临时，避免额外 _ZNSsC1ERKSs 与其析构。
                dm->findGameScript(std::string("Arad_EventPeriodDataManager")));
        period->setCreatePeriod_GM(v);
    }
    // ---- SetCreateTime ----
    else if (args[0] == "SetCreateTime")
    {
        if (args.size() < 5)
        {
            return 0;
        }
        {
            short fields[4] = {0, 0, 0, 0};
            fields[0] = static_cast<short>(std::atoi(args[1].c_str()));
            fields[1] = static_cast<short>(std::atoi(args[2].c_str()));
            fields[2] = static_cast<short>(std::atoi(args[3].c_str()));
            fields[3] = static_cast<short>(std::atoi(args[4].c_str()));
            long long t = ARAD::uint32Time(fields);
            user->set_charac_createtime(t);
            arad::SigSetCharacCreateTime sig;
            unsigned int createTime = user->getCurCharacCreateTime();
            unsigned int characNo = user->get_charac_no(-1);
            sig.set(characNo, createTime);
            Stream* s = GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0xbc83);
            CStreamGuard g(s, true);
            unsigned int uid = user->GetUID();
            ARAD::DISPATCHER::make_internal_stream_jpn(
                g, ARAD::ENUM_INTERNALPACKET_JPN(0x24), static_cast<int>(uid));
            g->put_binary(reinterpret_cast<char*>(&sig), 8);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::QUEUE_IDX(2), g);
        }
    }
    // ---- aa time/star/ticket/reset ----
    else if (args[0] == "aa")
    {
        advancealtar::CharacAdvanceAltarManager* mgr =
            &user->m_advanceAltar;                     // +0x8df60
        if (args[1] == "time")
        {
            int v = std::atoi(args[2].c_str());
            if (0 < v && v < 0x33)
                advancealtar::Manager::gmTimeSpeed_ = v;
        }
        else if (args[1] == "star")
        {
            int v = std::atoi(args[2].c_str());
            if (-1 < v && v < 1000)
                mgr->gmSetStar(v);
        }
        else if (args[1] == "ticket")
        {
            int v = std::atoi(args[2].c_str());
            if (-1 < v && v < 100)
                mgr->gmSetTicket(v);
        }
        else if (args[1] == "reset" && args[2] == "star")
        {
            int v = std::atoi(args[3].c_str());
            if (-1 < v && v < 2)
                mgr->resetStar(user, static_cast<CMDPacketStruct::ResetStarType::T>(v));
        }
    }
    // ---- SetRewardLast / SetQuestLast / SetBreak ----
    else if (args[0] == "SetRewardLast")
    {
        int v = StringToNumber(args[1].c_str());
        CLocalChina_DB_LastDay_BreakAway::makeRequest(user->get_acc_id(), v, 1);
    }
    else if (args[0] == "SetQuestLast")
    {
        int v = StringToNumber(args[1].c_str());
        CLocalChina_DB_LastDay_BreakAway::makeRequest(user->get_acc_id(), v, 0);
    }
    else if (args[0] == "SetBreak")
    {
        int v = StringToNumber(args[1].c_str());
        GlobalData::s_BreakAwaySys->GM_UpdateUvList(
            user->get_acc_id(), static_cast<unsigned int>(v));
    }

    return 0;
}
