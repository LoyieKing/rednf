#include "CTournamentDungeonReward.h"
#include <cstring>
#include <iostream>            // P338：ORIG 本 TU 含 std::ios_base::Init __ioinit
                               //（static_init 0x082854e9 有 InitC1+__cxa_atexit，
                               //  GI 0x08285529 sub $0x18）⇒ 缺此 include 则本 TU
                               //  不发射 GI/__static_init，missing_in_game 多 2。

#include "CDungeon.h"          // TournamentRewardRate/TournamentRewardItemRate 权威定义
#include "CBattle_Field_shared.h"   // WongWork::stGenerateRefData_t/stGenerateResult_t
#include "CDataManager.h"
#include "CItemList.h"
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"
#include "GameRand.h"          // get_rand_int

// ORIG T 0x08284750 (size 0x1f)：外联强符号定义，须先于本 TU 全部 caller
// （ctor 0x8284770 / ResetReward 0x8284820）——P210② 帧继承（4.4 按已见
// callee 定义精确继承 outgoing ⇒ ResetReward sub $0x14 与 ORIG 一致）。
void TournamentRewardType::Clear()
{
    m_id = -1;
    m_field04 = 0;
    m_flag08 = static_cast<char>(0xff);
}

// ORIG ctor 0x8284770：写虚表 0x8c0cbf0；D1 0x82847dc；ResetReward 0x8284820。
// 本 TU 仅提供 CBattle_Field 构造/析构链与 ResetReward 所需符号（ORIG 0x8284820）。
// 其余成员（MakeSelectPacket/_MakeRewardPacket/MakeRewardPacket 等）在头为 G2
// 占位布局时未声明成员，故不在此定义，属后续批次。
CTournamentDungeonReward::CTournamentDungeonReward()
    : m_rewards()
{
    // ORIG 0x8284770：构造 2x2 m_rewards 后调 ResetReward()（各 Clear + m_field04=0）
    ResetReward();
}

CTournamentDungeonReward::~CTournamentDungeonReward() {}

// ORIG 0x8284820：对 2x2 个 TournamentRewardType（+0x8 起）逐个 Clear，
// 最后清零 +0x04。构造函数中的 memset 占位同步移除（成员自动初始化）。
// [b28d-N18a] fresh 复验 36/37：Clear 外联化后帧 0x14 / 槽 -0x8/-0x4 已与 ORIG
// 全等；残 1 行 = 二维索引中间寄存器（ORIG `lea 0x0(,%eax,4),%ecx` 直落 vs
// 4.4 `mov %eax,%edx` 中转，P30/N18a 封顶）。
// [b28d-solo-CTournamentDungeonReward] DIFF 36/37：残 = N18a 二维索引中转×1；
// p167 1=1 CLEAR，语义全等 ⇒ N 封顶。
// [SweepCTournament 2026-09-09] 复核 36/37：探针 12 形（/tmp/probe_r{,2,3}.cpp）
// + 旗标矩阵（i486/regmove/gcse/tune/sched）+ 4.4.6 后端全部同形 ⇒ N 封顶。
// [round5-A17 2026-09-10] 本 TU 10 件 DIFF 全量复核（fresh routed .o + p167 逐件）：
// 残差 100% 为同一族——`m_rewards[2][2]`（元素 0xc、行 0x18）二维索引的**中间结果
// 寄存器角色**（ORIG 4.1：外层索引→%edx、内层→%ecx、`lea 0x0(,%eax,4),%ecx` 直落；
// 4.4.7：外层→%ecx、内层→%eax 并多 1 条 `mov %eax,%edx`），计数 = 每处 +1 条
// （ResetReward/isMemberSelectCard/MakeSelectPacket ×1、SelectCard/_MakeRewardPacket/
// isCardSelectComplete/SetRewardGold ×2、DecisionRewardType ×3、GetItemFromCard ×6），
// 另 _ForceSelectCard(95/96)/SetRewardGold(91/95) 各含 N115 push %edi+帧差。
// 探针（/tmp/probe_tn/a.cpp fA..fJ + b.cpp fK..fQ：引用形/行指针形/一维视图/
// m_rows[i].v[j] 组件形/register 形/unsigned 形/单 case 形）全部产同一 `mov %eax,%edx`
// 形（register 形额外 push %esi/%ebx 反劣）⇒ 无源形可择。
// p167 逐件 **CALLSITE CLEAR**（10/10）⇒ 语义完整。判 N18a 族封顶，勿再试。
void CTournamentDungeonReward::ResetReward()
{
    for (int c = 0; c < 2; ++c)
        for (int r = 0; r < 2; ++r)
            m_rewards[c][r].Clear();
    m_field04 = 0;
}

// ORIG 0x828502e：type∈{0,1}、maxItems<3 校验后，按 cardNo 在
// m_rewards[type][i].m_flag08 中匹配卡片号：
//   m_id==0  → items[0] 置 add_info=m_field04（固定物品），items[1] 空；
//   m_id!=-1 → items[1].m_addInfo=m_id 后经 CItemList::create_item 生成，
//              items[0] 空；m_id==-1 → 双空返回 0（未命中）。
#include "CDataManager.h"
#include "CItemList.h"   // create_item（G_CDataManager()->getItemList()）

int CTournamentDungeonReward::GetItemFromCard(ENUM_TOURNAMENT_REWARD_CARD_TYPE type,
                                              int cardNo, Inven_Item* items,
                                              unsigned int maxItems)
{
    // b21 verdict=DIFF（N 残余）：168/170。调用点全等（reset×/set_add_info/create_item/
    //   G_CDataManager）。两处大修：1) G_CDataManager()->getItemList() O0 禁内联产 EXTRA
    //   call → m_itemList 直读(+0xc)；2) `TournamentRewardType& slot` 引用局部只算一次地址
    //   （ORIG 每访问重推导 `this+type*0x18+i*0xc`）→ 删引用改 m_rewards[type][i].member
    //   直写（-64 insn）。残余仅索引计算寄存器/leas 调度(N82)与共享退出块布局(N-JMP,+2)。
    //   语义完成，byte 层 N 封顶。
    // [b28d-N82] fresh 复验 168/170：同上（N82 帧/槽 + N18a 索引寄存器）。
    // [b28d-solo-CTournamentDungeonReward] 试形记录：早返拆分（maxItems>2 / type 越界
    // 各自 return 0）实测 168/174（原形 168/170）⇒ 4.4 仍尾部合并三处失败块，
    // 多 4 条，回退为合并守卫形。
    // [b28d-solo-CTournamentDungeonReward] DIFF 168/170：残 = N18a×5 + 三失败块尾部
    // 合并（N-JMP，ORIG 就地 `mov $0,%eax; jmp` ×3）；p167 9=9 CLEAR ⇒ N 封顶。
    // [SweepTournament 2026-09-09] 真语义 bug 修复：原 `(char)m_flag08 == (char)cardNo`
    // 被编译成字节比较（movzbl+mov+cmp %al,%dl）；ORIG 0x8285092 为 movzbl+movsbl+cmp
    // mem（32 位有符号，char 提升 int）⇒ cardNo ∉ [-128,127] 时结果相反。改
    // `m_flag08 == cardNo` 后我方比较段逐条复现 ORIG；168/170 与 p167 9=9 CLEAR 不变。
// [SweepCTournament 2026-09-09] 复核 168/170：残 = N18a 索引中转×5 + 三失败块
// 尾部合并（N-JMP：ORIG 就地 `mov $0,%eax; jmp`×3）；p167 9=9 CLEAR ⇒ N 封顶。
// [round1-L19 2026-09-09] 复修（判据① jcc 落点 + 判据⑨ 旧判复核）：
// 守卫重构 = ① `maxItems > 2` 独立早返（复现 ORIG 0x0828503a `cmpl $0x2,0x18(%ebp);
// jbe <body>` + fall-through fail 块）；② `(int)type < 0 || (int)type > 1` **合并
// 短路**（复现 ORIG 0x08285049 `js <共享 fail>` + `cmp $1; jle <cont>` 两跳共享
// 0x08285055 fail 块）。旧形 `maxItems<3 && type>=0 && type<2` 生 `ja` 异极性链。
// 读数：168/170 gap=-2 → **168/174 gap=-6**、ndiff 118→110、**槽集合差 ∅（全等）**。
// p167 9=9 CLEAR。残余 = 6 条 `mov %eax,%edx` 二维索引中转（ORIG `lea 0x0(,%eax,4),
// %ecx` 直落 vs 4.4 累加器恒 eax ⇒ 多 1 条/访问，N7/N18a 后端族；12 形探针
// /tmp/idx_probe{,2}.cpp + fB..fI 已验证不可择）+ 1 处 jne/je。
    if (maxItems > 2)
    {
        return 0;
    }
    if ((int)type < 0 || (int)type > 1)
    {
        return 0;
    }
    {
        for (int i = 0; i < 2; ++i)
        {
            // ORIG 0x8285092：movzbl 读 + movsbl 符号扩展后 `cmp 0x10(%ebp),%eax`
            // —— 32 位有符号比较（char 提升 int）。旧写法 `(char)cardNo` 强制字节比较，
            // cardNo ∉ [-128,127] 时行为与 ORIG 相反（真语义 bug）。
            if (m_rewards[(int)type][i].m_flag08 == cardNo)
            {
                if (m_rewards[(int)type][i].m_id == 0)
                {
                    items[0].reset();
                    items[0].set_add_info(m_rewards[(int)type][i].m_field04);
                    items[1].reset();
                    return 1;
                }
                if (m_rewards[(int)type][i].m_id != -1)
                {
                    items[0].reset();
                    items[1].reset();
                    items[1].m_addInfo = m_rewards[(int)type][i].m_id;
                    G_CDataManager()->m_itemList->create_item(  // ORIG mov 0xc(%eax) 直读
                        ENUM_ITEM_CREATE_TYPE_1, items[1],
                        m_rewards[(int)type][i].m_field04);
                    return 1;
                }
                items[0].reset();
                items[1].reset();
                return 0;
            }
        }
    }
    return 0;
}

// ============================================================================
// ORIG 0x8284a24：两张卡槽各按 rate 掷点决定奖励（未过首档则 m_id=0）。
// [b28d-N82] fresh 复验 118/130：残差 = 4.4 多 push %edi + 帧 0x30→0x3c
// （callee-saved 需求/帧策略差）+ 二维索引寄存器（N18a）⇒ 语义等价封顶。
// [b28d-solo-CTournamentDungeonReward] DIFF 118/121：P272 德摩根块序（else 体直落、
// 双跳同落 then）+ `pick+1` 初始化物化 + operator[] 引用缓存（ORIG 槽 -0xc）三项修后
// 130→121；残 = 寄存器角色互换（card↔slotIdx，N7/N18a 族）；p167 4=4 CLEAR。
// [SweepCTournament 2026-09-09] 复核 118/121：残 = N18a/N7 索引中转×3；p167 4=4
// CLEAR；jcc（je/jl/jg/seta/setle）与 ORIG 逐位全等 ⇒ N 封顶。
// ============================================================================
void CTournamentDungeonReward::DecisionRewardType(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE card, TournamentRewardRate& rate,
    const TournamentRewardItemRate* itemRate)
{
    for (int slotIdx = 0; slotIdx < 2; ++slotIdx)
    {
        int roll = get_rand_int(rate.m_c + 1);
        // ORIG 双跳同落 then（je/jl 0x8284a92）、else 体直落（P272 德摩根形）。
        if (rate.m_0 != 0 && rate.m_0 >= roll)
        {
            m_rewards[(int)card][slotIdx].m_id = 0;
        }
        else if (roll <= rate.m_0 + rate.m_4)
        {
            // ORIG `add $1,%eax; mov %eax,-0x18` ⇒ pick 在初始化处 +1 物化
            int pick = get_rand_int(itemRate->m_c) + 1;
            int accum = 0;
            for (size_t i = 0; i < itemRate->m_items.size(); ++i)
            {
                // ORIG 缓存 operator[] 返回的引用（槽 -0xc），两处赋值复用
                const TournamentRewardItem& item = itemRate->m_items[i];
                accum += item.m_data[1];
                if (pick <= accum)
                {
                    m_rewards[(int)card][slotIdx].m_id = item.m_data[0];
                    m_rewards[(int)card][slotIdx].m_field04 = item.m_data[2];
                    break;
                }
            }
        }
    }
}

// ============================================================================
// ORIG 0x8284890：round=victory?round:round-1（夹取 0..4）；查 rates 表命中
// 后掷卡槽 0；终局轮（round==4 且胜利）再以表项 5 掷卡槽 1。
// ============================================================================
void CTournamentDungeonReward::MakeReward(
    const std::map<int, TournamentRewardRate>* rates,
    const TournamentRewardItemRate* itemRate, int round, bool victory)
{
    if (rates == 0)
        return;

    // [b28c-16] P198①：ORIG 先发 `movl $0x0,-0x18`（预清）再按 victory 分支
    // 各自 `mov 0x14(%ebp),%eax; [sub $1]; mov %eax,-0x18` 两段写；
    // 三目合并形 4.4 丢预清行并改装载序。
    int curRound = 0;
    if (victory)
        curRound = round;
    else
        curRound = round - 1;
    if (curRound < 0)
        curRound = 0;
    else if (4 < curRound)
        curRound = 4;

    std::map<int, TournamentRewardRate>::const_iterator it = rates->find(curRound);
    if (it == rates->end())
        return;

    TournamentRewardRate rate = it->second;
    DecisionRewardType(ENUM_TOURNAMENT_REWARD_CARD_TYPE_0, rate, itemRate);
    if (round == 4 && victory)
    {
        std::map<int, TournamentRewardRate>::const_iterator itFinal = rates->find(5);
        if (itFinal == rates->end())
            return;
        {
            TournamentRewardRate rateFinal = itFinal->second;
            DecisionRewardType(ENUM_TOURNAMENT_REWARD_CARD_TYPE_1, rateFinal,
                               itemRate);
        }
    }
}

// ============================================================================
// ORIG 0x8284b8e：空奖励槽（m_id==0）以生成结果首个掉落物的 add_info2
// （金币额）乘金币率写入 m_field04。
// [b28d-N82] fresh 复验 91/94：同 DecisionRewardType（多 push %edi、帧 0x30→0x3c、
// 循环槽 -0x10/-0xc → -0x20/-0x1c）+ N18a 索引差 ⇒ 语义等价封顶。
// [b28d-solo-CTournamentDungeonReward] DIFF 91/95：修 empty()→begin()!=end()（P197④
// 反向：4.4 empty 是 out-of-line call+bool 物化岛）与 get_add_info()→m_addInfo2
// 直读（ORIG `mov 0x7(%eax),%eax`）后调用点 4=4 与 ORIG 全等；残 = N18a + push %edi/
// N115 帧 0x10；p167 4=4 CLEAR。
// [SweepCTournament 2026-09-09] 复核 91/95：残 = N115 多 push %edi + N18a 中转×2；
// p167 4=4 CLEAR；jcc（jne/je/setle）与 ORIG 逐位全等 ⇒ N 封顶。
// ============================================================================
void CTournamentDungeonReward::SetRewardGold(
    WongWork::stGenerateResult_t& result, float goldRate)
{
    for (int card = 0; card < 2; ++card)
    {
        for (int slotIdx = 0; slotIdx < 2; ++slotIdx)
        {
            if (m_rewards[card][slotIdx].m_id == 0)
            {
                // ORIG：具名 iterator 局部（槽 -0x18）+ end() 临时（-0x14）
                // + operator!=/operator-> 实例；4.4 的 empty() 是 out-of-line
                // call 且带 bool 物化岛（P197④ 反向），故写 begin()!=end() 形。
                std::vector<Inven_Item>::iterator it =
                    result.m_dropItems.begin();
                if (it != result.m_dropItems.end())
                {
                    // ORIG 直读 +0x07（m_addInfo2），无 get_add_info call；
                    // x87 RC=truncate 直接截断（fildl+fmuls+fistpl，无 +0.5）
                    m_rewards[card][slotIdx].m_field04 =
                        (int)((float)it->m_addInfo2 * goldRate);
                }
            }
        }
    }
}

// ORIG 0x82852e4：单卡型奖励包体（byte2 + 每卡槽 int/int/short0）
// [b28d-N18a] fresh 复验 65/66：残 1 行 = 二维索引中间寄存器 + 块尾 nop 差。
// [b28d-solo-CTournamentDungeonReward] DIFF 65/66：残 = N18a×2 + ORIG 空 then 占位
// nop 岛（§6.1 剥离）；p167 4=4 CLEAR。
void CTournamentDungeonReward::_MakeRewardPacket(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE type, PacketGuard* packet)
{
    // [SweepCTournament 2026-09-09] P223：ORIG 0x082853ae 尾 `jmp +0xcd` +
    // 0x082853b0 `nop`（jg 的落点）＝4.1 空 then 占位岛；`if((int)type<2){...}`
    // 被 4.4 jump-thread（少 jmp+nop，diff 实证 -2）。复现形＝否定 goto + 尾空标签
    // （探针 /tmp/probe_isl.cpp F/G：F 无岛、G 产 `jne; body; jmp; nop` 逐条同形）。
    if ((int)type >= 2)
        goto L_done;
    {
        packet->put_byte(2);
        for (int slotIdx = 0; slotIdx < 2; ++slotIdx)
        {
            packet->put_int(m_rewards[(int)type][slotIdx].m_id);
            packet->put_int(m_rewards[(int)type][slotIdx].m_field04);
            packet->put_short(0);
        }
    }
L_done:;
}

// ORIG 0x8284caa：卡型 0 必发；flag 时补卡型 1，否则 byte0 占位
void CTournamentDungeonReward::MakeRewardPacket(PacketGuard* packet, bool flag)
{
    _MakeRewardPacket(ENUM_TOURNAMENT_REWARD_CARD_TYPE_0, packet);
    if (flag)
        _MakeRewardPacket(ENUM_TOURNAMENT_REWARD_CARD_TYPE_1, packet);
    else
        packet->put_byte(0);
}
// ============================================================================
// ORIG 0x08284ee4（报告 SelectCard.md）：cardA/cardB ∈ [0,1]、slotNo ∈ [0,3]
// 校验后，m_rewards[(int)cardA][(int)cardB].m_flag08（movzbl 零扩展读）== 0xff（未选）
// 才写入 slotNo 低字节并返回 1；否则返回 0。
// [b28d-N18a] fresh 复验 66/68：残差 = 二维索引中间寄存器（movsbl 装载序 +
// `mov %eax,%edx` 中转，P30/N18a）。
// [b28d-solo-CTournamentDungeonReward] DIFF 66/68：残 = N18a×2（读/写各一）；
// p167 0=0 CLEAR。
// [SweepCTournament 2026-09-09] 复核 66/68；jcc 逐位核对（js/jle×3 + cmp $0xff
// /jne）与 ORIG 全等 ⇒ N 封顶。
// ============================================================================
bool CTournamentDungeonReward::SelectCard(int slotNo, char cardA, char cardB)
{
    if (cardA < 0 || cardA > 1)
        return false;
    if (cardB < 0 || cardB > 1)
        return false;
    if (slotNo < 0 || slotNo > 3)
        return false;

    // ORIG cmp $0xff; jne fail —— 命中未选槽（==0xff）直落写入体，失败块居尾
    if (static_cast<unsigned char>(
            m_rewards[(int)cardA][(int)cardB].m_flag08) == 0xff)
    {
        m_rewards[(int)cardA][(int)cardB].m_flag08 = static_cast<char>(slotNo);
        return true;
    }
    return false;
}
// ============================================================================
// ORIG 0x08285284（报告 CTournamentDungeonReward/isMemberSelectCard.md）：
// 遍历该卡型两槽，m_flag08（char 符号扩展）== cardNo 即 1。
// [b28c-16 定论 N18a] 残差仅二维寻址中间结果寄存器：ORIG `lea 0x0(,%eax,4),%ecx`
// 直落 ecx（type 在 edx/slot 在 ecx），4.4 恒 slot*12→eax 后 `mov %eax,%edx`
// 多 1 条并改寄存器名（type→ecx/slot→eax）。探针 /tmp/l16p/p8.cpp 两形不可择。
// [b28d-N18a] fresh 复验 34/35：维持 N18a。
// [b28d-solo-CTournamentDungeonReward] DIFF 34/35：残 = N18a×1；p167 0=0 CLEAR。
// [SweepCTournament 2026-09-09] 复核 34/35；jcc（jne/setle）与 ORIG 全等 ⇒ N。
// ============================================================================
bool CTournamentDungeonReward::isMemberSelectCard(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE type, int cardNo)
{
    for (int slot = 0; slot < 2; ++slot)
    {
        if (static_cast<char>(m_rewards[(int)type][slot].m_flag08) == cardNo)
            return true;
    }
    return false;
}

// ============================================================================
// ORIG 0x082853b8（报告 _ForceSelectCard.md）：type>1 直接返回；
// 对两槽：m_flag08==0xff（未选）时遍历 members，凡 !isMemberSelectCard(type,*it)
// 者写入 m_flag08（低字节）。持续遍历不提前退出（ORIG 形态）。
// [b28d-N82] fresh 复验 95/96：残差 = 多 push %edi + 帧 0x20→0x2c + 循环槽平移。
// [b28d-solo-CTournamentDungeonReward] DIFF 95/101：修 `++it`→`it++`（P28 增量侧：
// ORIG 增量段 `operator++(it,0)` + 独立 sret 临时 -0x10）；修后结构/槽序与 ORIG
// 全等，残 = N18a×1 + push %edi/N115 帧 0x10；p167 7=7 CLEAR。
// [SweepCTournament 2026-09-09] 复核 96/101：残 = N115 push %edi + N18a 中转×3；
// p167 7=7 CLEAR；jcc（jg/jne/je/setle）与 ORIG 逐位全等 ⇒ N 封顶。
// ============================================================================
void CTournamentDungeonReward::_ForceSelectCard(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE type, const std::vector<int>& members)
{
    if ((int)type > 1)
        return;

    for (int slot = 0; slot < 2; ++slot)
    {
        if (static_cast<unsigned char>(m_rewards[(int)type][slot].m_flag08) == 0xff)
        {
            // ORIG 增量段调 operator++(it,0)（后置，带独立 sret 临时槽 -0x10）⇒
            // 源形必写 it++（P28 增量侧推论）。显式 itEnd 缓存实测 ndiff 63→73
            // 反劣（4.4 不按 P13 缓存 end），故维持 `it != members.end()`。
            for (std::vector<int>::const_iterator it = members.begin();
                 it != members.end(); it++)
            {
                if (!isMemberSelectCard(type, *it))
                    m_rewards[(int)type][slot].m_flag08 = *it;
            }
        }
    }
}

// ============================================================================
// ORIG 0x0828523c（报告 ForceSelectCard.md）：_ForceSelectCard(0)；victory 时
// 再 _ForceSelectCard(1)。
// ============================================================================
void CTournamentDungeonReward::ForceSelectCard(std::vector<int>& members, bool victory)
{
    _ForceSelectCard(ENUM_TOURNAMENT_REWARD_CARD_TYPE_0, members);
    if (victory)
        _ForceSelectCard(ENUM_TOURNAMENT_REWARD_CARD_TYPE_1, members);
}

// ============================================================================
// ORIG 0x08284d08（报告 isCardSelectComplete.md）：members 空 → 0。
// type==2（双卡型）：2x2 全部槽的 m_flag08（char 引用，std::find<char> 实证
// _ZSt4findI...cET_S8_S8_RKT0_）都必须出现在 members 中；否则查该行两槽。
// 全命中 → 1。
// [b28d-N82] fresh 复验 137/139：残差 = 帧/槽平移 + find 块尾 jmp 岛（N104）。
// [b28d-solo-CTournamentDungeonReward] DIFF 137/139：修具名 found 迭代器后求值序
// 与槽位（end -0x2c / begin -0x28 / find -0x30 / end -0x24；else 支 -0x20/-0x1c/
// -0x34/-0x18）与 ORIG 逐槽全等；残 = N18a×2；p167 11=11 CLEAR。
// [SweepCTournament 2026-09-09] 复核 137/139：残 = N18a 中转×2；p167 11=11 CLEAR
//（empty/find/end/begin/eq 全命中）⇒ N 封顶。
// ============================================================================
bool CTournamentDungeonReward::isCardSelectComplete(
    ENUM_TOURNAMENT_REWARD_CARD_TYPE type, const std::vector<int>& members)
{
    if (members.empty())
        return false;

    if ((int)type == 2)
    {
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                // ORIG 求值序 = &flag → end() → begin() → find → 比较侧 end()；
                // 具名 found 使 4.4 的 find 结果先落槽再调 end（P293 同族）。
                std::vector<int>::const_iterator found =
                    std::find(members.begin(), members.end(),
                              m_rewards[i][j].m_flag08);
                if (found == members.end())
                    return false;
            }
        }
    }
    else
    {
        for (int j = 0; j < 2; ++j)
        {
            std::vector<int>::const_iterator found =
                std::find(members.begin(), members.end(),
                          m_rewards[(int)type][j].m_flag08);
            if (found == members.end())
                return false;
        }
    }
    return true;
}

// ============================================================================
// ORIG 0x08284fa4（报告 MakeSelectPacket.md）：两轮 put_byte(2)；内轮对两槽
// put_byte((char)m_flag08 符号扩展)。
// [b28c-16 定论 N18a] 同上（二维寻址中间寄存器单段直落不可造）。
// [b28d-N18a] fresh 复验 44/45：维持 N18a。
// [b28d-solo-CTournamentDungeonReward] DIFF 44/45：残 = N18a×1；p167 2=2 CLEAR。
// [SweepCTournament 2026-09-09] 复核 44/45；put_byte 调用点 2=2 全等 ⇒ N。
// ============================================================================
void CTournamentDungeonReward::MakeSelectPacket(PacketGuard* packet)
{
    for (int type = 0; type < 2; ++type)
    {
        packet->put_byte(2);
        for (int slot = 0; slot < 2; ++slot)
        {
            packet->put_byte(static_cast<char>(m_rewards[type][slot].m_flag08));
        }
    }
}
