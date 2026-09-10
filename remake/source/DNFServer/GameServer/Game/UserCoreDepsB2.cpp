// ============================================================================
// UserCoreDepsB2.cpp —— 用户/安全核心叶子恢复（独占 TU；不触碰 CUser.cpp）。
//
// 主函数均逐指令对照 docs/class_func_reports/<类>/<函数>.md 的
// ## 汇编 与 ## 反编译 C 转写（报告地址见各函数头注释）：
//   1. CUser::MakeTagCharacInfo            ORIG T 0x08690776（CUser/MakeTagCharacInfo.md）
//   2. AvatarRechargeServer::CalcDurabilityChangeEquip
//                                          ORIG T 0x081905b4（AvatarRechargeServer/CalcDurabilityChangeEquip.md）
//   3. online_preliminary::COnlinePreliminary::CheckUnusableEquipment
//                                          ORIG T 0x08560920（online_preliminary__COnlinePreliminary/CheckUnusableEquipment.md）
//   4. _checkItemMovable                   ORIG T 0x0865f1e1（_lt_global_gt_/_checkItemMovable.md）
//   5. Secu_HackLogCheck::MoveMap          ORIG T 0x0827a7f4（Secu_HackLogCheck/MoveMap.md）
//   6. CSpecialItemRoutingManager::ProcessFailEtc
//                                          ORIG T 0x0860b332（CSpecialItemRoutingManager/ProcessFailEtc.md）
//   7. CSpecialItemRoutingManager::processRouting
//                                          ORIG T 0x0860ac6a（CSpecialItemRoutingManager/processRouting.md）
//
// 直接依赖且当前无任何定义的 ORIG 叶子（有报告者读报告；无报告者以 objdump
// 反汇编逐指令转写并在注释标注地址；全部符号经 ORIG nm 核实存在）：
//   - Secu_MonsterCheck::Reset                    T 0x0827ac7a（Secu_MonsterCheck/Reset.md）
//   - WongWork::CHackAnalyzer::getServerHackAccumulatedCnt
//                                                 W 0x0822eef6（无报告，objdump）
//   - CParty::IsUseFatigueDungeon                 T 0x085be9c8（无报告，objdump）
//   - CParty::CheckMapClear                       T 0x085bea4a（无报告，objdump；转发 check_grid_clear）
//   - CBattle_Field::check_grid_clear             T 0x0830a0e8（无报告，objdump）
//   - CEquipItem::GetUsableEquipmentType          W 0x0832e036（无报告，objdump；返回 +0x1bc）
//   - _Additioal_info::operator+=                 W 0x08693cea——已收敛至唯一定义
//     CUserPremium.cpp（add/subtract 同处），本 TU 不再重复定义
//
// 注：CUser::Send(PacketGuard&) 已由 CUser.cpp:2361 按报告实现；
//     CUser::getCurCharacInvenCheckSum/getCurCharacSkillCheckSum 已由
//     CUserDepsB.cpp 实现——本 TU 不重复定义。
// ============================================================================

#include <string.h>
#include <stdlib.h>

#include "CUser.h"                    // CUser/_Charac_info/Secu_HackLogCheck/
                                      // Secu_MonsterCheck/WongWork::CHackAnalyzer/
                                      // ENUM_HACKTYPE/WongWork::CUserPremium(m_premium)
#include "CUserCharacInfo.h"          // getCurCharacR/getTagCharacR/getTagCharacSkillW/
                                      // getTagCharacInvenR/getCurCharacInvenW/
                                      // getCurCharacNo/get_charac_10_level_section/
                                      // getCurCharacStrikerSkillIndex/
                                      // online_preliminary::COnlinePreliminary
#include "PacketGuard.h"              // PacketGuard（InterfacePacketBuf put_*/finalize）
#include "CQuestShop.h"               // makePacket_BuyingStatusList（GetCharacExpandData(0xb)）
#include "AvatarRechargeServer.h"     // CalcDurabilityChangeEquip/CalcDurabilityDetail 声明
#include "CInventory.h"               // MakeEquipList/GetInvenSlot/insertItemIntoInventory/
                                      // GetInvenTypeFromItemSpace/_checkItemMovable 声明
#include "CExpandEquipslot.h"         // GetCharacExpandDataR(9) 返回类型与 GetInvenSlot
#include "CEquipItem.h"               // CEquipItem 视图（GetUsableEquipmentType/m_field1bc）
#include "CItem.h"                    // is_stackable/get_index/getUsableLevel/GetItemType
#include "CDataManager.h"             // G_CDataManager/find_item/m_serverParameter(+0x68)
#include "CQuestShop.h"               // makePacket_BuyingStatusList（GetCharacExpandData(0xb)）
#include "CCharacter.h"               // _Additioal_info（GetAdditionalInfo 返回值）
#include "CBattle_Field.h"            // check_grid_clear（CheckMapClear 转发目标）
#include "CDungeon.h"                 // isNoFatigueDungeon/m_villageAttackEventFlag/m_bloodDungeonType
#include "CParty.h"                   // 路由 API/IsUseFatigueDungeon/CheckMapClear/
                                      // DeleteDungeonDropItem/send_to_party
#include "CSpecialItemRoutingManager.h"
#include "CBossTower.h"               // WongWork::CBossTower（getBossTower 返回值）
#include "WarField.h"                 // MapInfo::m_itemMap（+0xa18 = CBossStage+0x9f4+0x24）
#include "CGenUniqueNo.h"             // WongWork::CGenUniqueNo::genIPGNo
#include "GameRand.h"                 // get_rand_int
#include "CStatisticServerProxy.h"    // statistc_proxy::add
#include "CSystemTime.h"              // getCurDate
#include "GlobalData.h"               // s_systemTime_
#include "LogManager.h"               // logFormat

// ---------------------------------------------------------------------------
// Secu_HackLogCheck::Init —— ORIG T 0x0827a7d0：写基类 +0x34 = user，
// 再调基类 SetUserPtr(user)。Secu_MonsterCheck::SetUserPtr —— ORIG T
// 0x0827abec：m_user = user。
// ---------------------------------------------------------------------------
void Secu_HackLogCheck::Init(CUser* user)
{
    m_user34 = user;
    SetUserPtr(user);
}

// ---------------------------------------------------------------------------
// 1. CUser::MakeTagCharacInfo —— ORIG T 0x08690776（报告 CUser/MakeTagCharacInfo.md）
// tag 角色不存在 → 0；否则写入：uid 低 16 位、名字、等级/job/+0x29、striker 技能
// 序号、附加信息 0x52 字节（premium 时叠加 CUserPremium::GetAdditionalInfo）、
// 装备列表、技能强化循环、求购单包。成功返回 1。
// ---------------------------------------------------------------------------
// [b28d-solo-UserCoreDepsB2] 236→289（ORIG 276），P167 40=40 CLEAR。本轮修入：
// ①去 tag 具名局部（ORIG 每字段独立 `call getTagCharacR`，无槽；P0）；
// ②nameLen 改 unsigned char（ORIG `mov %al,-0x29` + movzbl 回读，int 槽不符）；
// ③去 inven 具名局部改内联 receiver（ORIG 无槽）；
// ④**P167 重载修复**：`put_binary(infoBuf,…)` 原选 `put_binary(char*,int)`（ORIG 为
//   `put_binary(char const*,int)`）⇒ 加 `(const char*)` cast 后 MISSING/EXTRA 清零；
// ⑤**消 memcpy EXTRA**：ORIG 无 memcpy call（0x52 块搬移内联 rep movsl）⇒ 改
//   `*(_Additioal_info*)infoBuf = *(const _Additioal_info*)src;`（P83 结构整体赋值），
//   隐式 operator= 被 4.4 内联展开、零新增符号（nm 仅 U operator+=）；
// ⑥premium 分支改正极性（ORIG `call; test; je` 直判 ⇒ `if (IsHavePremiumAdvantage())`
//   then=premium）；⑦循环体 `base[i*2]` 下标形。
// 残差（byte 层封顶）：N78（ORIG memcpy 内联对齐+rep movsl 40 条 vs 4.4 结构赋值逐字段
// 展开）、N82/N89（帧 0xec/0xec±、槽号与 STL/日志内联布局）、N4/P30（`i*2+base` add 链
// vs lea 折叠）。ORIG 0x08690776，语义等价。
// [b28d-PoolGlobalData 二压 P329] baseSmall 具名 bool 内联回 if 条件（消 -0x1d
// 槽与其 `mov %al,-slot; cmpb $0,-slot` 往返）⇒ ndiff 91→46，-0x29/-0x20/-0x24
// 标量槽与 ORIG 逐位复齐；余 46=N78 结构展开 10 行 + 双 0x52 缓冲槽向（见函数
// 体内实测回退记录）+ idxPos -0x80/-0x30 随动，封顶。
// ---------------------------------------------------------------------------
// [D01 · P142 派生判据 · Main 批准闸3 A/B] `_Additioal_info` 为 pack(1)
// （alignof=1）⇒ 其 0x52 块整体赋值在 4.4 下只能发 1 字节 peel
// （`and $0x1,%ecx`）；ORIG 为 **2 字节 peel**（`mov $0x52,%ebx; and $0x2,%ecx;
// … rep movsl`，ORIG 0x086908ce 段直证）⇒ 原类型 alignof ≥ 2。
// 「改头 alignof 1→2」已判**结构性不可行**（Main 登记）：CUserPremium 无 pack
// 外层于 +0x8bd/+0x90f（奇数偏移）内嵌该类型，插 1B padding 会连锁改
// m_pad961/m_returnItemMap/m_mailBox 偏移（闸1 嵌入点清单 7 点）。
// 本视图只用于**块拷贝点**：同字段序/同宽 POD + pack(2)（alignof=2、布局与
// _Additioal_info 逐字段等价、sizeof 0x52），使 4.4 复现 2 字节 peel；
// 纯 0x52 块搬移、无指针/引用成员 ⇒ 无别名/生命周期风险。
#pragma pack(push, 2)
struct AddInfoMove2
{
    int            m_field00;      // +0x00
    int            m_field04;      // +0x04
    unsigned short m_field08;      // +0x08
    unsigned short m_field0a;      // +0x0a
    unsigned short m_field0c;      // +0x0c
    unsigned short m_field0e;      // +0x0e
    unsigned short m_field10[4];   // +0x10..+0x17（union 8B 视图同宽）
    unsigned short m_field18[17];  // +0x18..+0x39（union 0x22B 视图同宽）
    int            m_field3a;      // +0x3a（2 对齐、非 4 对齐）
    unsigned short m_field3e;      // +0x3e
    unsigned short m_field40;      // +0x40
    int            m_field42;      // +0x42（2 对齐）
    unsigned short m_field46;      // +0x46
    unsigned short m_field48;      // +0x48
    unsigned short m_field4a;      // +0x4a
    unsigned short m_field4c;      // +0x4c
    int            m_field4e;      // +0x4e（2 对齐）→ 共 0x52
};
#pragma pack(pop)
typedef char AddInfoMove2_size_0x52[(sizeof(AddInfoMove2) == 0x52) ? 1 : -1];
typedef char AddInfoMove2_align_2[(__alignof__(AddInfoMove2) == 2) ? 1 : -1];
typedef char AddInfoMove2_off_10[(__builtin_offsetof(AddInfoMove2, m_field10) == 0x10) ? 1 : -1];
typedef char AddInfoMove2_off_18[(__builtin_offsetof(AddInfoMove2, m_field18) == 0x18) ? 1 : -1];
typedef char AddInfoMove2_off_3a[(__builtin_offsetof(AddInfoMove2, m_field3a) == 0x3a) ? 1 : -1];
typedef char AddInfoMove2_off_42[(__builtin_offsetof(AddInfoMove2, m_field42) == 0x42) ? 1 : -1];
typedef char AddInfoMove2_off_4e[(__builtin_offsetof(AddInfoMove2, m_field4e) == 0x4e) ? 1 : -1];

bool CUser::MakeTagCharacInfo(PacketGuard& packet)
{
    if (getCurCharacR() == 0)
        return false;
    if (getTagCharacR() == 0)
        return false;

    packet.put_short(get_unique_id() & 0xffff);

    unsigned char nameLen = (unsigned char)strlen(getTagCharacR()->m_name);
    packet.put_int(nameLen & 0xff);
    packet.put_str(getTagCharacR()->m_name, nameLen & 0xff);

    packet.put_byte((int)(short)getTagCharacR()->m_level);   // movswl +0x27 → byte
    packet.put_byte((int)(char)getTagCharacR()->m_field26);  // movsbl +0x26
    // [Main 2026-09-10] m_field29 已由 L31CharacPad 类型化为 STGrowthType（sizeof 1，
    // 断言锁定）；此处 ORIG 为 `movsbl +0x29`（有符号字节读）⇒ 取首字节视角并显式
    // 有符号转换（避免 4.4 折恒等，见判据⑤）。
    packet.put_byte((int)(signed char)getTagCharacR()->m_field29.m_growthType);  // movsbl +0x29

    packet.put_byte(getCurCharacStrikerSkillIndex() & 0xff);

    // [b28d-PoolGlobalData] 0x52 双缓冲槽向：ORIG 4.1 infoBuf@-0xd2（深）/
    // GetAdditionalInfo sret 临时@-0x7b（浅）；4.4 首用分配恒反向。
    // 实测「additional 提升为函数域先声明」形反劣（46→93：_Additioal_info
    // 非 POD，先声明触发 EXTRA `call C1Ev` 位域全写 + 帧 -0x138；TU missing
    // 26→27）⇒ 回退块内声明形，槽向记 N82 封顶。
    char infoBuf[0x52];
    if (IsHavePremiumAdvantage())
    {
        // [D01 闸3 A/B] 块搬移改用 pack(2) POD 镜像（布局逐字段等价、sizeof 0x52）
        // ⇒ 复现 ORIG 2 字节 peel（原 _Additioal_info 为 pack(1) ⇒ 1 字节 peel）。
        *(AddInfoMove2*)infoBuf =
            *(const AddInfoMove2*)getTagCharacAddInfoR();
        _Additioal_info additional = m_premium.GetAdditionalInfo();  // sret @-0x7b
        *(_Additioal_info*)infoBuf += additional;                // ORIG 0x08693cea
        packet.put_int(0x52);
        packet.put_binary((const char*)infoBuf, 0x52);
    }
    else
    {
        packet.put_int(0x52);
        packet.put_binary((const char*)getTagCharacAddInfoR(), 0x52);
    }

    // ORIG：getTagCharacInvenR() 返回 CInventory*（_Charac_info.m_inven 同址；
    // 本源码声明返回 Inven_Item*，沿用 CUser.cpp:7541 的同址转换惯例）。
    reinterpret_cast<const CInventory*>(getTagCharacInvenR())
        ->MakeEquipList(&packet, true, (ENUM_USERINFO)1, (ENUM_EQUIPSLOT)0);

    int idxPos = packet.get_index();   // 占位字节位置（回填强化条目数）
    int count = 0;
    packet.put_byte(0);
    for (int i = 0; i <= 0xcb; ++i)    // cmp $0xcb; setle
    {
        // 每轮重取 tagR（ORIG 循环体内重复调用 getTagCharacR）。
        // [b28d-PoolGlobalData P329] 去 baseSmall 具名 bool（其 -0x1d 槽把全帧
        // 标量顶深 4 + 产 `mov %al,-slot; cmpb $0,-slot` 往返；ORIG 0x086907xx
        // 短路直落 `test %al,%al; je`）⇒ 条件内联回 if。
        char* base;
        if ((char)getTagCharacR()->m_field1239 == (char)-1 ||
            (char)getTagCharacR()->m_field1239 == (char)0)
            base = getTagCharacSkillW() + 0x46;
        else
            base = getTagCharacSkillW() + 0x1de;
        if (base[i * 2] != 0)
        {
            packet.put_byte(i);
            packet.put_byte(*(unsigned char*)(base + i * 2));
            packet.put_byte((int)*(char*)(base + i * 2 + 1));
            ++count;
        }
    }
    packet.put_byte(idxPos, count);    // put_byte(int&, int) 回填占位

    CQuestShop* questShop =
        (CQuestShop*)GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xb);
    questShop->makePacket_BuyingStatusList(packet, true);
    return true;
}

// ---------------------------------------------------------------------------
// 2. AvatarRechargeServer::CalcDurabilityChangeEquip —— ORIG T 0x081905b4
// （报告 AvatarRechargeServer/CalcDurabilityChangeEquip.md）
// 状态 5 且组队中、疲劳副本、未清图时，槽位 0..9 执行 CalcDurabilityDetail。
// ---------------------------------------------------------------------------
// [b28d-solo-UserCoreDepsB2] AE-：P0 去 party 具名局部（ORIG 三次独立 call GetParty）
// + P225 dead 变体 `while (!call()) break;` 复现 `xor $1; test %al,%al`，71→77
// （ORIG 78）。残 1 条 = N104 EH 尾块相对序：ORIG 把共享 dtor 段置于 EH 岛**之后**
// （正常路径 `jmp +0xc7`），4.4 置于 EH 岛**之前**（正常路径 fallthrough，少 1 jmp）。
// 守卫 nop 岛 4 组、setne/sete 物化、寻址/槽位逐条同 ORIG（ORIG 0x081905b4）。语义等价。
void AvatarRechargeServer::CalcDurabilityChangeEquip(CUser* user, int equipSlot)
{
    if (user->get_state() != 5)
        return;
    if (user->GetParty() == 0)
        return;
    if (!user->GetParty()->IsUseFatigueDungeon())
        return;
    if (user->GetParty()->CheckMapClear())
        return;

    std::vector<int> rates;            // ORIG 栈 vector（空构造即传入）
    if (-1 < equipSlot && equipSlot < 10)
    {
        while (!CalcDurabilityDetail(user, equipSlot, rates))
            break;
    }
}

// ---------------------------------------------------------------------------
// 4. _checkItemMovable —— ORIG T 0x0865f1e1（报告 _lt_global_gt_/_checkItemMovable.md）
// exchange_interspace(_expand) 前置校验：源/目标空间为 2 或 0xc 直接放行；
// 否则按 INVEN_TYPE 取槽内 item idx（5=扩充栏 9 号页 / 6=金库期望清零 /
// 其余走背包），两侧 idx 均与期望一致才允许移动。
// ---------------------------------------------------------------------------
// [b28d-solo-UserCoreDepsB2] P0 去 inven 具名局部（ORIG else 分支把
// getCurCharacInvenR() 内联进实参，无该槽）+ foundFrom/foundTo 零初始化合并到
// 分支前（ORIG `movl $0,-0x18; movl $0,-0x14` 连续）：116→112（ORIG 114），帧
// $0x128=ORIG $0x124+push ebx、标量槽 -0x20/-0x1c/-0x18/-0x14 与 ORIG 逐位一致。
// 残 2 条 = ①N93 四个块级 sret 临时槽分配方向相反（ORIG 首分支 -0x114 最深→
// -0x5d 最浅，4.4 反向）；②N7 else 分支 fromType 4.1 升 %ebx 跨 receiver call
// 保活（push %ebx/pop），4.4 恒 mem→%ecx（P226 语句表达式、裸 register 双旋钮实测：
// 前者无效、后者过度升格为 %esi+%ebx 且帧 0x120/异常 epilogue 反劣，已回退）。
// 语义闭合（ORIG 0x0865f1e1）。
bool _checkItemMovable(const CUser* user, ENUM_ITEMSPACE fromSpace, int fromSlot,
                       unsigned long fromItemIdx, ENUM_ITEMSPACE toSpace, int toSlot,
                       unsigned long toItemIdx)
{
    if (fromSpace == (ENUM_ITEMSPACE)2 || toSpace == (ENUM_ITEMSPACE)2)
        return true;
    if (fromSpace == (ENUM_ITEMSPACE)0xc || toSpace == (ENUM_ITEMSPACE)0xc)
        return true;

    int fromType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)fromSpace);
    int toType = GetInvenTypeFromItemSpace((ENUM_ITEMSPACE)toSpace);

    int foundFrom = 0;
    int foundTo = 0;
    if (fromType == 5)
    {
        const CExpandEquipslot* expand =
            user->GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)9);
        Inven_Item slot = expand->GetInvenSlot((INVEN_TYPE)fromType, fromSlot);
        foundFrom = slot.m_addInfo;                     // 读 sret 结果 +2
    }
    else if (fromType == 6)
    {
        fromItemIdx = 0;                                // ORIG 清参数槽 0x14(%ebp)
    }
    else
    {
        Inven_Item slot =
            user->getCurCharacInvenR()->GetInvenSlot(fromType, fromSlot);
        foundFrom = slot.m_addInfo;
    }

    if (toType == 5)
    {
        const CExpandEquipslot* expand =
            user->GetCharacExpandDataR((ENUM_CHARAC_EXPAND_TYPE)9);
        Inven_Item slot = expand->GetInvenSlot((INVEN_TYPE)toType, toSlot);
        foundTo = slot.m_addInfo;
    }
    else if (toType == 6)
    {
        toItemIdx = 0;                                  // ORIG 清参数槽 0x20(%ebp)
    }
    else
    {
        Inven_Item slot =
            user->getCurCharacInvenR()->GetInvenSlot(toType, toSlot);
        foundTo = slot.m_addInfo;
    }

    return foundFrom == (int)fromItemIdx && foundTo == (int)toItemIdx;
}

// ---------------------------------------------------------------------------
// 5a. Secu_HackLogCheck::MoveMap —— ORIG T 0x0827a7f4
// （报告 Secu_HackLogCheck/MoveMap.md）：三检查使能位置 1 后调基类 Reset。
// ---------------------------------------------------------------------------
void Secu_HackLogCheck::MoveMap()
{
    m_flag223 = 1;
    m_flag224 = 1;
    m_flag225 = 1;
    Reset();
}

// ---------------------------------------------------------------------------
// 5b. Secu_MonsterCheck::Reset —— ORIG T 0x0827ac7a
// （报告 Secu_MonsterCheck/Reset.md）：各检查 word==1 且用户在册时上报
// hacktype 0xdf/0xe0/0xe1 各 1 次，随后清三个 active 字节。
// ---------------------------------------------------------------------------
void Secu_MonsterCheck::Reset()
{
    if (m_field10 == 1 && m_user != 0)
        m_user->getHackAnalyzer()->addServerHackCnt(
            m_user, (WongWork::ENUM_HACKTYPE)0xdf, 1, 0, 0);
    if (m_field1a == 1 && m_user != 0)
        m_user->getHackAnalyzer()->addServerHackCnt(
            m_user, (WongWork::ENUM_HACKTYPE)0xe0, 1, 0, 0);
    if (m_field26 == 1 && m_user != 0)
        m_user->getHackAnalyzer()->addServerHackCnt(
            m_user, (WongWork::ENUM_HACKTYPE)0xe1, 1, 0, 0);
    m_active223 = 0;
    m_active224 = 0;
    m_active225 = 0;
}

// ---------------------------------------------------------------------------
// 6. CSpecialItemRoutingManager::ProcessFailEtc —— ORIG T 0x0860b332
// （报告 CSpecialItemRoutingManager/ProcessFailEtc.md）：路由开启且首个有效
// 成员状态 ∈ {5, 0xc} 时广播 0x11a 失败包（byte 0/失败码/路由 uniqueId）、
// 取消计时器并结束路由。
// [b28c-L20·N-EH布局] fresh orig=86 our=86 gap=0 ndiff=6：唯一真差为成功路径析构落点——
// ORIG 4.1 在 SetEndRouting 后内联「lea;mov;call ~PacketGuard」再 jmp epilogue；4.4.7 让
// 成功路径 jmp 到 EH 岛之后的共享析构段（~PacketGuard 只发射一次于 epilogue 前）。早返形
// （if(party==0)return;）实测劣化 our=87（多一条 jmp）已回退；包体/谓词形不变。函数体主
// 调用序列逐条同 ORIG：IsRoutingState → GetFirstValidMemberState×2（5/0xc 短路，布尔留
// %al 无槽）→ PacketGuard::C1 → put_header(0,0x11a) → put_byte(0) → put_byte(failCode)
// → put_int(GetRoutingItemUniqueID) → finalize(true) → send_to_party → cancelTimer →
// SetEndRouting → ~PacketGuard（+EH 岛 _Unwind_Resume）。P167 实测 16=16 call 集归零。语义等价。
// ---------------------------------------------------------------------------
// [b28d-solo-UserCoreDepsB2] 87/86：唯一残差 = N104 跨版本 EH 尾块相对序。ORIG 正常
// 路径在 SetEndRouting 后就地 `lea;mov;call ~PacketGuard` 再 `jmp epilogue`，unwind 岛
// 居中；4.4 把正常路径 `jmp` 到 unwind 岛之后的**共享**析构段（`~PacketGuard` 一份，
// 正常/unwind 两路共用），且 jump-thread 掉 ORIG 的 +0x122 nop 岛。两形实测：
// ①P223 goto 尾标签串（`if(party==0) goto L_nop; …; goto L_ret; L_nop:; L_ret:;`）
//   产 nop 但多 1 条 jmp（88，劣化）；②goto 跳出 PacketGuard 作用域强制正常路径析构点
//   无变化（86，析构仍外提）。属 GCC 4.4 cleanup-sharing 编译器行为，源码不可择。
// P167 16=16 归零；包体/谓词逐条同 ORIG（ORIG 0x0860b332）。语义等价。
void CSpecialItemRoutingManager::ProcessFailEtc(CParty* party, int failCode)
{
    if (party != 0)
    {
        // ORIG（idx102 报告）：IsRoutingState 为假则跳过状态读取；5 或 0xc 任一
        // 命中即可路由。布尔全程寄存器（test %al,%al; je），故谓词内联不具名局部
        // （P0 无槽局部）。
        if (party->IsRoutingState() &&
            (party->GetFirstValidMemberState() == 5 ||
             party->GetFirstValidMemberState() == 0xc))
        {
            PacketGuard packet;
            packet.put_header(0, 0x11a);
            packet.put_byte(0);      // ORIG put_byte(local_18,0)
            packet.put_byte(failCode);
            packet.put_int((int)party->GetRoutingItemUniqueID());
            packet.finalize(true);
            party->send_to_party(packet);
            cancelTimer(party);
            party->SetEndRouting();
        }
    }
}

// ---------------------------------------------------------------------------
// 7. CSpecialItemRoutingManager::processRouting —— ORIG T 0x0860ac6a
// （报告 CSpecialItemRoutingManager/processRouting.md）：路由物品投递主流程。
// ---------------------------------------------------------------------------
// [b28d-solo-UserCoreDepsB2] 435→446（ORIG 442），P167 59=59 CLEAR。本轮修入：
// ①`diceMembers.clear()`（ORIG 构造后紧跟 clear 调用，原缺）；
// ②`itemDice` 副本（ORIG -0x2c=-0x30 拷贝，GetRoutingDiceMemberList 用它）；
// ③getCurCharacInvenW()/itemMap 去具名局部改内联实参（P0，ORIG 无该两槽）；
// ④routingUniqueId 声明 int（ORIG -0x80 槽直传 erase 的 const int&，无 int 临时）；
// ⑤space 声明 int + `is_stackable()` 正极性 + 补 `else space=0`（ORIG movl 写/movsbl 读）；
// ⑥**真语义修复**：掷点循环 ORIG 为 `while(it!=end())`+单次 `operator*` pair 拷贝+`it++`，
//   且 `roll==bestRoll` 时 `je <nop>` **不前进**（重掷）；旧码 `++it` 无条件前进=真 bug。
// 残差（byte 层封顶）：N82 帧 0xd0/0xe0 + 全帧槽号平移（ORIG 对象优先占低槽 vs 4.4
// 标量分区）、N7 routingItemId 4.1 升 %ebx 跨 G_CDataManager（4.4 恒 mem→%edx）、
// 跨版本 nop 尾/`mov+movsbl` vs `movsbl mem`。ORIG 0x0860ac6a，语义等价。
void CSpecialItemRoutingManager::processRouting(CParty* party)
{
    Inven_Item* routingItem = party->GetRoutingInvenItem();
    if (routingItem->m_addInfo == 0)
    {
        ProcessFailEtc(party, 0);
        return;
    }

    int routingItemId = party->GetRoutingItemID();
    CItem* item = G_CDataManager()->find_item(routingItemId);
    if (item == 0)
    {
        ProcessFailEtc(party, 0);
        return;
    }
    CItem* itemDice = item;                           // ORIG -0x2c = -0x30 副本

    std::vector<std::pair<int, int> > diceMembers;    // (成员序号, 权重)
    diceMembers.clear();                              // ORIG 构造后紧跟 clear 调用
    party->GetRoutingDiceMemberList(diceMembers, itemDice);

    CUser* winner = 0;                                // -0x28
    int winnerIdx = 0;                                // -0x24
    PacketGuard packet;
    packet.put_header(0, 0x11a);
    if (diceMembers.empty())
    {
        packet.put_byte(0);
        packet.put_byte(0);
        packet.put_int((int)party->GetRoutingItemUniqueID());
    }
    else if (diceMembers.size() == 1)
    {
        winnerIdx = diceMembers[0].first;
        winner = party->get_user(winnerIdx);
    }
    else
    {
        int bestRoll = -1;                            // -0x20
        std::vector<std::pair<int, int> >::iterator it = diceMembers.begin();
        while (it != diceMembers.end())
        {
            std::pair<int, int> member = *it;         // ORIG 单次 operator* 读双字段
            int roll = get_rand_int(100);             // -0x1c
            if (roll > bestRoll)
            {
                bestRoll = roll;
                winnerIdx = member.first;
                winner = party->get_user(member.first);
            }
            else
            {
                if (roll == bestRoll)
                    goto L_skip_inc;                  // ORIG：roll==best 不前进（重掷）
            }
            it++;                                     // ORIG ppEi 后置
        L_skip_inc:;
        }
    }

    if (winner != 0)
    {
        int space = 0;                                // -0x18 ENUM_ITEMSPACE 结果
        if (item->is_stackable())
        {
            if (item->GetItemType() == 9)             // vptr+0x0c
                space = 7;
        }
        else
        {
            if (item->IsCreatureItem())               // vptr+0x14
                space = 7;
            else if (item->IsAvatarItem())            // vptr+0x10
                space = 1;
            else
                space = 0;
        }

        int result = -1;                              // -0x14
        if (!item->IsAvatarItem())
        {
            result = winner->getCurCharacInvenW()->insertItemIntoInventory(
                *routingItem, (eItemAddReason)4, true, true);
        }
        else
        {
            char ipgAgency[0x18];                     // genIPGNo 输出缓冲（Ghidra local_80[24]）
            WongWork::CGenUniqueNo::genIPGNo(
                (WongWork::CGenUniqueNo::ENUM_IPGNO_TYPE)5,
                (unsigned int)winner->getCurCharacNo(), ipgAgency);
            result = winner->addAvatarItem(
                (unsigned long)routingItem->m_addInfo, 0,
                (char)*(short*)((const char*)routingItem + 0xb), ipgAgency,
                (eAvatarItemAddReason)0xf);
        }

        if (result == -1)
        {
            packet.put_byte(0);
            packet.put_byte(1);
            packet.put_int((int)party->GetRoutingItemUniqueID());
        }
        else
        {
            if (item->IsRandomOption())               // vptr+0x4c
            {
                char* curDate = GlobalData::s_systemTime_.getCurDate();
                int levelSection = winner->get_charac_10_level_section();
                statistc_proxy::add(winner, "log_random_option", "pick_cnt", 1,
                                    "%d,'%s'", levelSection, curDate);
            }
            int routingUniqueId = (int)party->GetRoutingItemUniqueID(); // -0x80
            int deleteResult = 0;                     // -0x10
            int state = winner->get_state();
            if (state == 5)
            {
                deleteResult = party->DeleteDungeonDropItem(routingUniqueId);
            }
            else if (state == 0xc)
            {
                WongWork::CBossTower* tower = winner->getBossTower();
                if (tower != 0)
                {
                    // m_stageData.m_itemMap 位于 CBossTower+0xa18
                    // （CBossStage+0x9f4 的 MapInfo +0x24）。CBossStage 后段字段
                    // 为 private 且无具名访问器，按既有 TU 惯例以地址访问。
                    ((std::map<int, map_item>*)((char*)tower + 0xa18))
                        ->erase(routingUniqueId);
                }
            }
            if (0 < deleteResult)
            {
                LogManager::logFormat(
                    1, "SpecialItemRoutingManager.cpp",
                    __PRETTY_FUNCTION__,
                    0x165, "[EPIC ROUTING] DELETE DUNGEON ITEM FAIL");
            }

            packet.put_byte(1);
            packet.put_byte(winnerIdx);
            packet.put_int(routingUniqueId);
            packet.put_byte((int)(char)space);
            packet.put_short(result);
            winner->SendUpdateItem((CUser::eSendTarget)1,
                                   (ENUM_ITEMSPACE)(int)space, result);
        }
    }

    packet.finalize(true);
    party->send_to_party(packet);
    party->SetEndRouting();
}

// ---------------------------------------------------------------------------
// 依赖叶子（无报告，objdump 直转；地址见各注释）。
// ---------------------------------------------------------------------------

// ORIG T 0x085be9c8（无报告，objdump）：+0xcd8==1 / 无副本(+0xcac) / m_bloodDungeonType!=0 /
// m_villageAttackEventFlag==1 / isNoFatigueDungeon 任一命中 → false。
// [b28d-D43] ORIG 0x085be9c8：前两条件（bloodState==1 / 无副本）je 落
// **outer false**(+0x7b)，后三条件 jne/je 落 **inner false**(+0x6b) 再物化
// `test %al,%al; je outer` ⇒ 源码 = 两条件早返 false + 三条件 return 短路链
// （P0：无槽 = 无具名局部；旧 `bool baseOk` 合写令 4.4 落槽 -0x9 并撑帧）。
// 第三条件 `!m_villageAttackEventFlag`（ORIG xor$1 物化）而非 `!=1`（cmp 形）。
bool CParty::IsUseFatigueDungeon()
{
    // [b28d-D43] P222/P214①：ORIG 两守卫 `je outer_false`（同落尾部）+ 内层三条件
    // 物化 ⇒ 正向包裹形（false 归共享尾）；拆两条独立守卫 4.4 反产各自
    // `jne+mov$0+jmp` 岛（43 条），合并 `||` 产 41 条，本形为最优候选。
    if (m_battleField.m_bloodState != 1 && m_battleField.m_pDungeon != 0)
    {
        if (m_battleField.m_pDungeon->m_bloodDungeonType == 0 &&
            !m_battleField.m_pDungeon->m_villageAttackEventFlag &&
            !m_battleField.m_pDungeon->isNoFatigueDungeon())
            return true;
    }
    return false;
}

// ORIG T 0x085bea4a（无报告，objdump）：al 直接透传 check_grid_clear。
bool CParty::CheckMapClear()
{
    return reinterpret_cast<CBattle_Field*>(&m_battleField)->check_grid_clear();
}

// ORIG T 0x0830a0e8（报告 docs/class_func_reports/CBattle_Field/check_grid_clear.md）：
// 遍历当前图怪物表，存在 dropCnt==100 且 +0x13 标志为 0 的怪 → false，全部通过 → true。
// [b28d-D43] ORIG 形：mob 在**循环外**默认构造（0x830a0f3 一次 C1Ev，循环内
// operator=）；it 在 for 初始化处直构 begin()；条件端**第二次独立** GetCurrentMapInfo
// （P48 无 CSE）；增量段 `it++`（P28 ppEi）。旧重建缓存 mapInfo/monsters 引用
// （-0x10/-0xc 两槽）且循环内构造 mob ⇒ 78→66。
bool CBattle_Field::check_grid_clear()
{
    map_monster mob;
    for (std::map<int, map_monster>::iterator it =
             GetCurrentMapInfo()->m_monsterMap.begin();
         it != GetCurrentMapInfo()->m_monsterMap.end(); it++)
    {
        mob = it->second;                             // operator= 拷贝
        if (mob.m_dropCnt == 0x64 && mob.m_field13 == 0)
            return false;
    }
    return true;
}

// ORIG W 0x0832e036（无报告，objdump）：返回 this+0x1bc。
int CEquipItem::GetUsableEquipmentType() const
{
    return m_field1bc;
}

// ORIG W 0x0822eef6（无报告，objdump）：
//   mov 0xc(%ebp),%eax ; lea -0x65(%eax),%edx ; add $0xf44,%edx
//   mov 0x8(%eax,%edx,4),%eax
// 即 B 表累计计数（与 CHackAnalyzer.cpp hackCntB 寻址一致；
// resetServerHackAccumulatedCnt 0x086950b6 清同一槽位）。按 ORIG 运算次序
// 书写以对齐指令选择。
// [b28d-solo-UserCoreDepsB2] IDENTICAL（9=9）：裸偏移 `*(const int*)((char*)this+8+…)`
// 改具名成员数组 `m_serverHackCntB[(int)type-0x65]`（§4.3 具名化），与同族
// setServerHackAccumulatedCnt（ORIG 0x084eb57c，IDENTICAL）同写法，逐条复现 ORIG
// `lea -0x65(%eax),%edx; add $0xf44,%edx; mov 0x8(%eax,%edx,4),%eax`。
int WongWork::CHackAnalyzer::getServerHackAccumulatedCnt(
    WongWork::ENUM_HACKTYPE type) const
{
    return m_serverHackCntB[(int)type - 0x65];
}

void Secu_MonsterCheck::SetUserPtr(CUser* user)
{
    m_user = user;
}
