// ============================================================================
// CDataManagerTailB.cpp —— W6_TailB 批次：CDataManager 域 ORIG-only 符号收口
// 转写依据 docs/class_func_reports/CDataManager/ 各报告（## 汇编 + ## 反编译 C
// 已逐份读完）；对象字段以 CDataManager.h 权威布局（偏移注释）具名访问。
// 仅承载本批符号的定义；不修改其它实现。
// ============================================================================

#include <algorithm>
#include <cstring>
#include <cstddef>
#include <vector>

#include "GameTypes.h"      // cMyTrace
#include "CDataManager.h"
#include "CStackableItem.h"   // booster/package 检查的完整类型
#include "STStackableScript.h" // STStackablePackageData（pkg reinterpret 目标）
#include "CMap.h"           // VerifyMap → CMap 完整类型
#include "CDungeon.h"       // VerifyDungeon → CDungeon::VerifyMaze / m_clearItemMap
#include "CSkill.h"         // Verify → CSkill 技能遍历
#include "CMonster.h"       // Verify → CMonster 等级检查 / get_index
#include "Quest.h"          // Verify → RewardItemInfo / Quest 遍历
#include "CDungeonClear.h"  // WongWork::CDungeonClear

// ---------------------------------------------------------------------------
// 原 5 个 ORIG-W 访问器已按 P233 迁回 CDataManager.h 类内 inline 定义
// （IsAvailablePvPChannel 0x0819ede0 / GetMinLevelPvPChannel 0x0819ee2e /
// GetMaxLevelPvPChannel 0x0819ee3c / GetLimitItemUsageInfo 0x8693f16 /
// is_AuctionRegFreeCeraItem 0x84e92a4：ORIG 绑定全 W、地址分散于消费 TU 段
// ARAD_LoginSecurity/InterDispatcher/Inter_LoadEtc_impl/CUserOnlyOrig）；
// 本 TU 不再定义这 5 个符号，由各使用点发射 W 副本。
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// VerifyDungeon(CDungeon const&) const —— ORIG T 0x08362b44
//   遍历 dungeon.m_clearItemMap(+0x1c) 的 value（clear 奖励 itemIdx）逐一调
//   find_item（结果仅用于数据校验，不使用返回值），随后 dungeon.VerifyMaze()；
//   固定返回 1（调用方 Verify 按返回值恒真处理，与 ORIG 一致）。
// ---------------------------------------------------------------------------
int CDataManager::VerifyDungeon(const CDungeon& dungeon) const
{
    // ORIG T 0x08362b44：①后置 `it++`（ORIG @+0x45 发 iterator::operator++(int)
    // 带 0 实参 + 返回临时槽 -0x10）；②it->second 先落 -0xc 槽再回读（P0 具名
    // 局部）；③find_item 结果 `test %eax,%eax` 无消费者（P184 死测，4.4 需
    // goto 岛形才发射，P223）；④循环条件 op!=(it, end)（end 临时 -0x14）。
    for (std::map<int, int>::const_iterator it = dungeon.m_clearItemMap.begin();
         it != dungeon.m_clearItemMap.end(); it++)
    {
        int itemIdx = it->second;
        if (find_item(itemIdx))
            goto ITEM_FOUND;
    ITEM_FOUND:
        ;
    }
    dungeon.VerifyMaze();
    return 1;
}

// ---------------------------------------------------------------------------
// Verify() const —— ORIG T 0x08360d94（0x1359 字节）
// 结构（报告 ## 反编译 C 全文对照）：
//   1) mob reward 单调性检查（m_mobReward[i+1] <= m_mobReward[i] → trace 0x2723）；
//   2) 11 个职业技能表 CSkill+0xc8（required level range tag）==0 → 报错任一命中
//      则 trace 0x2735 并返回 false；
//   3) 全部主地图 VerifyMap(map, -1)，失败返回 false；
//   4) 全部副本 VerifyDungeon(dungeon)；
//   5) 道具表遍历：stackable 的 booster/package 引用完整性检查（缺失即置错）；
//   6) Quest 表奖励引用检查（reward/collision item & collision quest）；
//   7) 怪物等级区间合法性检查；
//   8) WongWork::CDungeonClear 全副本模拟（diff=4）输出二进制串，失败打
//      "Simulate dungeon clear data fail!!!!"；全部通过返回 true。
// 注：CSkill+0xc8、quest+0xb8/+0xc4/+0xd0/+0xdc/+0xec、CMonster+0x58/+0x5c 在
// 当前权威头尚未具名化，按 ORIG 偏移访问并标注【ORIG asm 证据】；待对应头批次
// 收口后替换为具名字段。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] N：N82 —— §6.2 三件套：p167 166=166 CLEAR
//   （MISSING/EXTRA 归零）、gap -49 条（4.4 多具名槽往返）、帧 0x1ac0 vs
//   0x1abc（-4）。语义完成项：空循环照抄（ORIG i∈[1,0xc6) 死体）、
//   __PRETTY_FUNCTION__（12 处）、booster/package 经 G_CDataManager()->find_item、
//   选择列表 vector 拷贝构造+析构、getSkillList 双调用、getQuestList 具名、
//   collision item 用 (*it)、toString 返回值判失败（删 memset 近似）。
//   残余为 4.1/4.4 槽复用策略（N82）与 STL 内联/转换构造发射差（N89 族）。
//   ORIG 0x08360d94。
// [SweepCDataManagerTailB 2026-09-10] 修真缺口（判据㉑ 缺调用 + 判据② call * 虚槽）：
//   ① ORIG 道具循环对 `item->GetItemType()`（vtable+0xc，_ZTV14CStackableItem
//      vptr+0xc=CStackableItem::GetItemType）共 **4 次虚调用**（08361240/08361257/
//      08361467/083617d0，Ghidra C 亦示 || 两臂各一次 + else 一次 + 死比较一次）；
//      旧码把结果缓存进 `const int type` 只调 1 次 ⇒ 缺 2 次 call *。已改逐点重读。
//   ② ORIG is_stackable 守卫为 08361222 `test %al,%al; je 083617d5`（正极性，
//      无 xor $1）；旧码 `if(!is_stackable()) continue;` 4.4 恒发 xor $1（P168a），
//      已改 `if (is_stackable()) { body }` 包裹。
//   读数：raw insns 1154→1172→1167（ORIG 1174）。二轮按 P335/P0 对齐：删 4 个
//   ORIG 无的具名局部（skill/sp、qp、mp；P0 无槽往返 ⇒ 表达式直读），补 ORIG
//   有的 dungeon 局部（08361fab mov %eax,-0x1c; mov -0x1c,%eax）⇒ fn_diff_tu
//   bad 1068→989、store-reload 成对 P 7→3；call * 2→4=ORIG 4；
//   p167 166=166 CLEAR（MISSING 全为 C1/C2、D1/D2 同址别名，P93 豁免）。
//   残余：4.1 多路径 jmp+nop 汇合块（ORIG jcc 103 vs 我方 95）、P197④ setcc 物化、
//   N18a lea/add、N122 bool 取反 xor（我方 2 处 vs ORIG 1 处）、else 分支
//   `cmp $0x22` 被 4.4 DCE（P171 死比较跨版本前端差）。
bool CDataManager::Verify() const
{
    // [ORIG +0x0008..+0x0027] 空循环照抄（ORIG 保留 i∈[1,0xc6) 死循环体）
    for (int k = 1; k < 0xc6; ++k)
    {
    }

    // [ORIG +0x0027..+0x00d7] mob reward 单调性（i ∈ [0,200)）
    for (int i = 0; i < 200; ++i)
    {
        if ((unsigned int)m_mobReward[i + 1] <= (unsigned int)m_mobReward[i])
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x2723, 9, true, true);
            trace("Warning: Unusual mob reward(%d) on mob grade(%d)",
                  (int)(unsigned int)m_mobReward[i], i);
        }
    }

    // [ORIG +0x00d7..+0x0247] 11 个职业的技能 required-level-range tag 检查
    bool skillFail = false;
    for (int job = 0; job <= 0xa; ++job)
    {
        for (__gnu_cxx::hash_map<int, CSkill*>::const_iterator
                 it(m_skillList->getSkillList(job)->begin()),
                 e(m_skillList->getSkillList(job)->end()); it != e; ++it)
        {
            // 【ORIG asm 08360f12/08360f15: mov 0x4(%eax),%eax; mov 0xc8(%eax),%eax】
            // ORIG 无 skill/sp 具名局部（P0：无槽往返 ⇒ 表达式直读；P335 消 P 对）。
            if (*(const int*)((const char*)it->second + 0xc8) == 0)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x272e, 9, true, true);
                trace("Skill [required level range] tag value error "
                      "(Job: %d)(Skill: %d)", job, it->second->get_index());
                skillFail = true;
            }
        }
    }
    if (skillFail)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x2735, 9, true, true);
        trace("Verifying Skills fail");
        return false;
    }

    // [ORIG +0x02a3..+0x0363] 主地图校验（逐张覆盖失败标志，不提前 break；
    // ORIG 同样每次覆写 local_69 后统一判定）
    bool mapFail = false;
    {
        // asm 0836103b: this+0x04 → hash_map 直接作用于该对象首部
        // （CMapList::m_mainMap +0x00）
        const CMapList* ml = m_mapList;
        for (__gnu_cxx::hash_map<int, CMap*>::const_iterator
                 it(ml->m_mainMap.begin());
             it != ml->m_mainMap.end(); it++)
        {
            // ORIG 形态：VerifyMap 返回值 xor 1 存入标志位，逐张覆盖不提前 break
            mapFail = ((VerifyMap(*it->second, -1) ^ 1) & 0xff) != 0;
        }
    }
    if (mapFail)
    {
        return false;
    }

    // [ORIG +0x0363..+0x03f8] 全副本 VerifyDungeon（this+0x00 直接作用 =
    // CDungeonList::m_dungeonMap +0x00）
    for (__gnu_cxx::hash_map<int, CDungeon*>::const_iterator
             it(m_dungeonList->m_dungeonMap.begin());
         it != m_dungeonList->m_dungeonMap.end(); it++)
    {
        VerifyDungeon(*it->second);
    }

    // [ORIG +0x03f8..+0x0994] 道具表 booster/package 引用完整性
    bool itemFail = false;
    {
        // asm 083611c8: this+0x0c(m_itemList) + lea 0x4 → CItemList::m_itemMap
        for (__gnu_cxx::hash_map<int, CItem*>::const_iterator
                 it(m_itemList->m_itemMap.begin());
             it != m_itemList->m_itemMap.end(); ++it)
        {
            CItem* item = it->second;
            // ORIG 正极性守卫（asm 08361222 `test %al,%al; je 083617d5`，无 xor $1）：
            // 4.4 对 `if (!f())` 恒发 xor $1（P168a），故改写为 if (f()) 包裹 body
            // （下列 body 缩进未调整，纯机器码形态对齐）。
            if (item->is_stackable())
            {
            // ORIG 逐点重读 GetItemType()（P48/P157）：|| 短路两臂各一次虚调用
            // （ORIG 08361240 cmp $0xf / 08361257 cmp $0x10），不可缓存到局部。
            if (item->GetItemType() == 0xf || item->GetItemType() == 0x10)
            {
                // booster：CStackableItem+0x240 stStackableBooster_t.m_elements
                CStackableItem* sitem = (CStackableItem*)item;
                for (std::vector<stStackableBoosterElement_t>::const_iterator
                         el = sitem->m_booster.m_elements.begin(),
                         elEnd = sitem->m_booster.m_elements.end();
                     el != elEnd; ++el)
                {
                    for (std::vector<stStackableBoosterItemInfo_t>::const_iterator
                             info = el->m_items.begin(),
                             infoEnd = el->m_items.end();
                         info != infoEnd; ++info)
                    {
                        if (info->m_itemIdx == 1)
                        {
                            continue;
                        }
                        if (G_CDataManager()->find_item(info->m_itemIdx) == 0)
                        {
                            cMyTrace trace(__PRETTY_FUNCTION__, 0x275a, 9, true, true);
                            trace("Booster item element not found (%d)->(%d)",
                                  item->get_index(), info->m_itemIdx);
                            itemFail = true;
                        }
                    }
                }
            }
            else if (item->GetItemType() == 0xe)  // ORIG 08361467 cmp $0xe
            {
                // package：包列表/选择列表逐元素 find_item 校验。
                // 共享头 m_pkg21c/m_pkg234 元素类型暂为 vector<char> 占位
                // （STStackableScript.h 循环依赖无法具名化），沿用
                // CCeraShop_Special.cpp:409 已确立的 reinterpret_cast 模式。
                CStackableItem* sitem = (CStackableItem*)item;
                std::vector<STStackablePackageData>& pkgs =
                    *reinterpret_cast<std::vector<STStackablePackageData>*>(
                        &sitem->m_pkg21c);  // 【CStackableItem+0x21c】
                for (std::vector<STStackablePackageData>::const_iterator p =
                         pkgs.begin(),
                         pEnd = pkgs.end();
                     p != pEnd; ++p)
                {
                    if (p->m_field0 == 1)
                    {
                        continue;
                    }
                    if (G_CDataManager()->find_item(p->m_field0) == 0)
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x276b, 9, true, true);
                        trace("Package item element not found (%d)->(%d)",
                              item->get_index(), p->m_field0);
                        itemFail = true;
                    }
                }

                const std::vector<std::vector<STStackablePackageData> >& sel =
                    *reinterpret_cast<
                        std::vector<std::vector<STStackablePackageData> >*>(
                        &sitem->m_pkg234);  // 【CStackableItem+0x234】
                for (std::vector<std::vector<STStackablePackageData> >::
                         const_iterator group = sel.begin();
                     group != sel.end(); ++group)
                {
                    const std::vector<STStackablePackageData> copy(*group);
                    for (std::vector<STStackablePackageData>::const_iterator p =
                             copy.begin(), pEnd = copy.end();
                         p != pEnd; ++p)
                    {
                        if (p->m_field0 == 1)
                        {
                            continue;
                        }
                        if (G_CDataManager()->find_item(p->m_field0) == 0)
                        {
                            cMyTrace trace(__PRETTY_FUNCTION__, 0x277b, 9, true, true);
                            trace("Package item selection element not found "
                                  "(%d)->(%d)",
                                  item->get_index(), p->m_field0);
                            itemFail = true;
                        }
                    }
                }
            }
            else
            {
                // ORIG 083617d0 虚调用后 `cmp $0x22,%eax` 无消费者（P171 死比较照抄）
                (void)(item->GetItemType() == 0x22);
            }
            }
        }
    }

    if (!itemFail)
    {
        // [ORIG +0x0994..+0x1149] Quest 表奖励引用检查
        bool questFail = false;
        const QuestList* ql = m_questList->getQuestList();
        for (__gnu_cxx::hash_map<int, Quest*>::const_iterator
                 qit(ql->m_questMap.begin()),
                 qe(ql->m_questMap.end()); qit != qe; ++qit)
        {
            Quest* quest = qit->second;

            // 【ORIG asm 083618aa: mov 0xb8(%eax)】quest 类型/tag 字段
            // ORIG 无 qp 具名局部（P0）：按 quest 指针直接偏移访问。
            const int state = *(const int*)((const char*)quest + 0xb8);
            if (state != 0 && state != 0x19)
            {
                continue;
            }

            // 【ORIG +0xc4/+0xd0】两段 RewardItemInfo 奖励向量（当前 Quest.h
            // 未建模此区布局，按 ORIG 偏移访问）
            {
                std::vector<RewardItemInfo>& v =
                    *reinterpret_cast<std::vector<RewardItemInfo>*>(reinterpret_cast<char*>(quest) + 0xc4);
                for (std::vector<RewardItemInfo>::const_iterator it = v.begin();
                     it != v.end(); ++it)
                {
                    const int itemIdx = it->m_itemIdx;
                    if (itemIdx != 0 && itemIdx != 1 && itemIdx != 2 &&
                        itemIdx != 999 &&
                        this->find_item(itemIdx) == 0)
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x27a1, 5);
                        trace("Quest reward item element not found "
                              "(quest: %d)(item: %d)",
                              *(const int*)((const char*)quest + 0x04), it->m_itemIdx);  // 【ORIG asm 08361b7e: quest+0x04】
                        questFail = true;
                    }
                }
            }
            {
                std::vector<RewardItemInfo>& v =
                    *reinterpret_cast<std::vector<RewardItemInfo>*>(reinterpret_cast<char*>(quest) + 0xd0);
                for (std::vector<RewardItemInfo>::const_iterator it = v.begin();
                     it != v.end(); ++it)
                {
                    const int itemIdx = it->m_itemIdx;
                    if (itemIdx != 0 && itemIdx != 1 && itemIdx != 2 &&
                        itemIdx != 999 &&
                        this->find_item(itemIdx) == 0)
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x27ac, 5);
                        trace("Quest reward select item element not found "
                              "(quest: %d)(item: %d)",
                              *(const int*)((const char*)quest + 0x04), it->m_itemIdx);  // 【ORIG asm 08361b7e: quest+0x04】
                        questFail = true;
                    }
                }
            }
            // 【ORIG +0xdc】collision quest 列表（vector<int>）
            {
                std::vector<int>& v =
                    *reinterpret_cast<std::vector<int>*>(reinterpret_cast<char*>(quest) + 0xdc);
                for (std::vector<int>::const_iterator it = v.begin();
                     it != v.end(); ++it)
                {
                    if (find_quest(*it) == 0)
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x27cd, 5);
                        trace("Quest reward collision quest element now found "
                              "(quest: %d)(collision quest: %d)",
                              *(const int*)((const char*)quest + 0x04), *it);  // 【ORIG asm 08361b7e: quest+0x04】
                        questFail = true;
                    }
                }
            }
            // 【ORIG +0xec】collision item 向量（RewardItemInfo）
            {
                std::vector<RewardItemInfo>& v =
                    *reinterpret_cast<std::vector<RewardItemInfo>*>(reinterpret_cast<char*>(quest) + 0xec);
                for (std::vector<RewardItemInfo>::const_iterator it = v.begin();
                     it != v.end(); ++it)
                {
                    const int itemIdx = (*it).m_itemIdx;
                    if (itemIdx != 0 && itemIdx != 1 && itemIdx != 2 &&
                        itemIdx != 999 &&
                        this->find_item(itemIdx) == 0)
                    {
                        cMyTrace trace(__PRETTY_FUNCTION__, 0x27da, 5);
                        trace("Quest reward collision item element not found "
                              "(quest: %d)(item: %d)",
                              *(const int*)((const char*)quest + 0x04), (*it).m_itemIdx);  // 【ORIG asm 08361b7e: quest+0x04】
                        questFail = true;
                    }
                }
            }
        }

        if (!questFail)
        {
            // [ORIG +0x1187..] 怪物等级区间检查（CDataManager+0x08 管理器 +
            // 0x18 monsterMap；CMonster+0x58/+0x5c level min/max）
            for (__gnu_cxx::hash_map<int, CMonster*>::const_iterator
                     it(m_monsterManager->m_monsterMap.begin());
                 it != m_monsterManager->m_monsterMap.end(); it++)
            {
                CMonster* mon = it->second;
                // ORIG 无 mp 具名局部（P0）：按 mon 指针直接偏移访问。
                const int lvA = *(const int*)((const char*)mon + 0x58);  // 【ORIG asm +0x10de】
                const int lvB = *(const int*)((const char*)mon + 0x5c);  // 【ORIG asm +0x10db】
                if (lvA < 1 || 200 < lvA || lvB < 1 || 200 < lvB)
                {
                    cMyTrace trace(__PRETTY_FUNCTION__, 0x2808, 9, true, true);
                    trace("Error: Invalid monster(%d) level(%d-%d)",
                          mon->get_index(), lvA, lvB);
                }
            }

            // [ORIG +0x11a0..+0x1313] 全副本通关模拟（WongWork::CDungeonClear）
            WongWork::CDungeonClear clearSim;
            for (__gnu_cxx::hash_map<int, CDungeon*>::const_iterator
                     it(m_dungeonList->m_dungeonMap.begin());
                 it != m_dungeonList->m_dungeonMap.end(); ++it)
            {
                // ORIG 08361fab: mov %eax,-0x1c(%ebp); mov -0x1c(%ebp),%eax
                // ⇒ 原码有具名局部（P0：有槽往返）；08361fb9 cwtl = (short) 转换。
                CDungeon* dungeon = it->second;
                clearSim.addClearedDungeon((short)dungeon->get_index(), '\x04');
            }
            char buf[0x1800];
            if (clearSim.toString(buf, 0x1800) < 0)
            {
                cMyTrace trace(__PRETTY_FUNCTION__, 0x2815, 9, true, true);
                trace("Simulate dungeon clear data fail!!!!");
            }
        }
    }

    return true;
}

// ---------------------------------------------------------------------------
// _subPunctuationMark(char const*, char*) —— ORIG T 0x08362c22
//   从 src 中剔除"跳过字符"后拷入 dst：
//     - 单字节：不在 m_slangName.m_charList(+0x86ac) 中 → 复制；
//     - 双字节：word = src[i]|src[i+1]<<8 不在 m_slangName.m_shortList(+0x86b8)
//       中 → 复制两字节。
//   （若命中跳过表则不复制——函数名与两分支对称性一致；见报告汇编逐条核对。）
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] AE：删 strlen 缓存（ORIG 每轮调 strlen）、
//   out 用 int（unsigned 会令 dst[out] 折 lea）、w 用 `unsigned char buf[2]={0}`
//   字节写入形（ORIG movw $0 + 两次 movb + movzwl 回读）。ORIG 0x08362c22。
char* CDataManager::_subPunctuationMark(const char* src, char* dst)
{
    int out = 0;
    for (unsigned int i = 0; i < strlen(src); ++i)
    {
        const unsigned char* p = (const unsigned char*)(src + i);
        if (_isOneByteCharacter(*p))
        {
            if (std::find(m_slangName.m_charList.begin(),
                          m_slangName.m_charList.end(),
                          *p) == m_slangName.m_charList.end())
            {
                dst[out++] = (char)*p;
            }
        }
        else
        {
            // ORIG 取连续两字节组 word（低位在前）
            unsigned char buf[2] = { 0 };
            buf[0] = p[1];
            buf[1] = p[0];
            const unsigned short w = *(const unsigned short*)buf;
            if (std::find(m_slangName.m_shortList.begin(),
                          m_slangName.m_shortList.end(),
                          w) == m_slangName.m_shortList.end())
            {
                dst[out++] = (char)*p;
                dst[out++] = (char)p[1];
                ++i;  // 双字节多消耗一个输入位（asm +0x1b4 处额外自增）
            }
        }
    }
    dst[out] = '\0';
    return dst;
}

// ---------------------------------------------------------------------------
// rewriteUsableEmoticon(CUser const&, char*) —— ORIG T 0x0835fba2
//   扫描文本中 "\x1e <emotcode>" 三字节模式，表情 code 以 (signed char) 取得
//   后查 m_emoticonList(+0x28)；未找到或 emoticon->isUsable(user)（vtbl+0x08,
//   asm 0835fc31..0x9c 虚调用）为假则把第 3 字节改写 0xff 作废。
//   循环条件 strlen(text)-2 > i（有符号）。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] AE：emo 具名指针提前到函数头声明（ORIG 槽
//   emo(-0x18)/len(-0x14)/i(-0x10)/emoCode(-0xc) 按「先声明深」复现）；
//   循环下标用 int（size_t 会令 text[i] 折成 lea 三连）。ORIG 0x0835fba2。
void CDataManager::rewriteUsableEmoticon(const CUser& user, char* text)
{
    ChattingEmoticonBase* emo;
    if (text == 0)
    {
        return;
    }
    const size_t len = strlen(text);
    for (int i = 0; (int)len - 2 > i; ++i)
    {
        if ((unsigned char)text[i] != 0x1e ||
            (unsigned char)text[i + 1] != ' ')
        {
            continue;
        }
        const int emoCode = (signed char)text[i + 2];  // asm movsbl
        emo = m_emoticonList->find_emoticon(emoCode);
        if (emo == 0)
        {
            text[i + 2] = (char)0xff;
        }
        else if (!emo->isUsable(user))
        {
            text[i + 2] = (char)0xff;
        }
    }
}

// ---------------------------------------------------------------------------
// hasPreventPrefixCharacName(char const*, int) —— ORIG T 0x0836360c sz 0x10a
// （objdump 0x0836360c..0x08363716 逐指令转写；报告缺失标 [报告缺失]）。
//   count = m_serverParameter.m_preventPrefixCount（CDM+0x194）；
//   entry(i) = m_serverParameter.m_preventPrefixEntries + i*0x1e（CDM+0x190+8 起）。
//   对每条：entryLen=strlen(entry)；21 字节栈缓存清零（P4 gcc 内建展开形态）
//   → memcpy(prefix, name, entryLen) → strcmp(entry, prefix)；==0 → 返回 1。
//   循环条件 count > i（setg）；扫完返回 0。第二参 unused ORIG 亦未读。
//   依 §8.3 已将 ServerParameterScript.m_pad12c 头部拆具名（sizeof 0x49f4 不变）。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] N：P30/N18a —— 删 sps 引用/entry 局部后帧与
//   槽位全对齐（sub $0x30、prefix -0x25、i -0x10、entryLen -0xc），仅余 4.4.6
//   对 i*0x1e 索引展开的寄存器选择（`mov %edx,%ecx; sub; mov` vs `sub %eax,%edx`）
//   与 `lea 0x190(%edx)` 折成 `mov+add` 共 2 处 4 条；p167 3=3 CLEAR。
//   ORIG 0x0836360c。
int CDataManager::hasPreventPrefixCharacName(const char* name, int /*unused*/)
{
    for (int i = 0; m_serverParameter.m_preventPrefixCount > i; ++i)
    {
        const size_t entryLen =
            strlen(&m_serverParameter.m_preventPrefixEntries[i * 0x1e]);
        char prefix[0x15] = { '\0' };
        memcpy(prefix, name, entryLen);
        if (strcmp(&m_serverParameter.m_preventPrefixEntries[i * 0x1e],
                   prefix) == 0)
            return 1;
    }
    return 0;
}


// ---------------------------------------------------------------------------
// hasSpecialCharacterExceptSpace(char const*) —— ORIG T 0x08362e0e
//   对 name 每个位置：
//     单字节（_isOneByteCharacter）：先查 InvalidCharData.m_charRange(+0x0c →
//       CDM+0x86d0) 区间表（lo <= c 且 c < hi 命中），再查 m_charList(+0x00 →
//       CDM+0x86c4) 精确表；
//     双字节：word = c[i]|c[i+1]<<8，先查 m_shortRange(+0x24 → CDM+0x86e8)
//       （lo <= word 且 word <= hi 双闭），再查 m_shortList(+0x18 → CDM+0x86dc)
//       精确表；任一命中返回 1，否则继续，扫完返回 0。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] AE：四处循环改「begin 先、end 后并缓存」
//   （for 初始化逗号声明 it, itEnd）；range 循环经 `&(*it)` 具名指针读
//   m_min/m_max（ORIG -0x14/-0xc 指针槽）；精确表查找由 std::find 改手写迭代器
//   循环；第二条件写 `r->m_max > *p`（P196 方向）。ORIG 0x08362e0e。
bool CDataManager::hasSpecialCharacterExceptSpace(const char* name)
{
    for (size_t i = 0; i < strlen(name); ++i)
    {
        const unsigned char* p = (const unsigned char*)(name + i);
        if (_isOneByteCharacter(*p))
        {
            for (std::vector<InvalidCharData::stRangeElement<
                     unsigned char> >::const_iterator it =
                     m_invalidChar.m_charRange.begin(),
                     itEnd = m_invalidChar.m_charRange.end();
                 it != itEnd; ++it)
            {
                const InvalidCharData::stRangeElement<unsigned char>* r =
                    &(*it);
                if (r->m_min <= *p && r->m_max > *p)
                {
                    return true;
                }
            }
            for (std::vector<unsigned char>::const_iterator it =
                     m_invalidChar.m_charList.begin(),
                     itEnd = m_invalidChar.m_charList.end();
                 it != itEnd; ++it)
            {
                if (*it == *p)
                {
                    return true;
                }
            }
        }
        else
        {
            unsigned char buf[2] = { 0 };
            buf[0] = p[1];
            buf[1] = p[0];
            const unsigned short w = *(const unsigned short*)buf;
            for (std::vector<InvalidCharData::stRangeElement<
                     unsigned short> >::const_iterator it =
                     m_invalidChar.m_shortRange.begin(),
                     itEnd = m_invalidChar.m_shortRange.end();
                 it != itEnd; ++it)
            {
                const InvalidCharData::stRangeElement<unsigned short>* r =
                    &(*it);
                if (r->m_min <= w && w <= r->m_max)
                {
                    return true;
                }
            }
            for (std::vector<unsigned short>::const_iterator it =
                     m_invalidChar.m_shortList.begin(),
                     itEnd = m_invalidChar.m_shortList.end();
                 it != itEnd; ++it)
            {
                if (*it == w)
                {
                    return true;
                }
            }
            ++i;  // 双字节消耗两位（asm +0x30a 公共 +1 之外再 +1）
        }
    }
    return false;
}

// ---------------------------------------------------------------------------
// get_createCharacterGift(int, createChracScript::rewardInfoList&) const
//   ORIG T 0x08365668：m_createChrac.m_map0(+0x4bd4) 先 find(paramIdx)、再
//   find(0xb)（默认角色创建礼包 key），各命中则将该条目 vector<rewardInfo>
//   追加到 out.m_list 尾部（insert(end, begin, end)）。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] AE：默认礼包 key 用字面量直传（ORIG +0xce
//   `movl $0xb,-0x1c` 槽物化 = P244 字面量→槽→取址形；具名 int 局部会使
//   it/defaultKey 槽互换，DIFF）。ORIG 0x08365668。
void CDataManager::get_createCharacterGift(
    int idx, createChracScript::rewardInfoList& out) const
{
    std::map<int, createChracScript::rewardInfoList>::const_iterator it =
        m_createChrac.m_map0.find(idx);
    if (it != m_createChrac.m_map0.end())
    {
        out.m_list.insert(out.m_list.end(),
                          it->second.m_list.begin(),
                          it->second.m_list.end());
    }

    it = m_createChrac.m_map0.find(0xb);
    if (it != m_createChrac.m_map0.end())
    {
        out.m_list.insert(out.m_list.end(),
                          it->second.m_list.begin(),
                          it->second.m_list.end());
    }
}

// ---------------------------------------------------------------------------
// get_useJobFatigueMatchGift(char, int, int) —— ORIG T 0x0836580e
//   gift = m_useJobFatigue.m_giftMap.find((int)job)，未找到返回 0；
//   u1 = gift.m_rewardMap.upper_bound(arg2 - arg3)
//   u2 = gift.m_rewardMap.upper_bound(arg2)（asm -0x20+arg3 即还原 arg2）
//   若 u1==end 且 u2==end → 0；u1==u2 → 0；否则返回 u1->second。
// ---------------------------------------------------------------------------
// [b28d-solo-CDataManagerTailB] AE：声明序 jobType→diff→it 使
//   diff(-0x20)/jobType(-0x1c)/it(-0x24) 全对齐；第二次 upper_bound 的 key 是
//   匿名临时 `diff + b`（非具名局部，ORIG -0x14 为引用实参物化）；
//   单出口 `if (it != end) { ... } return 0;` 复现共享尾块。ORIG 0x0836580e。
int CDataManager::get_useJobFatigueMatchGift(char job, int a, int b)
{
    int jobType;
    const int diff = a - b;
    jobType = (int)job;
    std::map<int, useCharacterJobFatigue::fatigueMatchGift>::iterator it =
        m_useJobFatigue.m_giftMap.find(jobType);
    if (it != m_useJobFatigue.m_giftMap.end())
    {
        std::map<int, int>::iterator u1 =
            it->second.m_rewardMap.upper_bound(diff);
        std::map<int, int>::iterator u2 =
            it->second.m_rewardMap.upper_bound(diff + b);
        if (u1 == it->second.m_rewardMap.end() &&
            u2 == it->second.m_rewardMap.end())
        {
            return 0;
        }
        if (u1 != u2)
        {
            return u1->second;
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// get_useJobFatigueMatchGiftMsg(int, std::string&, std::string&) ——
//   ORIG T 0x0836598e _ZN12CDataManager29get_useJobFatigueMatchGiftMsgEiRSsS0_
//   （报告缺失，按 nm 定址 + objdump 转写）。
//   反汇编逐条转写：
//     it = m_useJobFatigue.m_giftMap.find(job)（map::find 直构 -0x10）；
//     end = m_useJobFatigue.m_giftMap.end()（直构 -0xc，P13 缓存 end）；
//     it != end 时 msg1 = it->second.m_msg1（pair+0x20 = value+0x1c）、
//     msg2 = it->second.m_msg2（pair+0x24 = value+0x20）。
// ---------------------------------------------------------------------------
void CDataManager::get_useJobFatigueMatchGiftMsg(int job, std::string& msg1,
                                                std::string& msg2)
{
    std::map<int, useCharacterJobFatigue::fatigueMatchGift>::iterator it =
        m_useJobFatigue.m_giftMap.find(job);
    if (it != m_useJobFatigue.m_giftMap.end())
    {
        msg1 = it->second.m_msg1;
        msg2 = it->second.m_msg2;
    }
}

// ---------------------------------------------------------------------------
// get_server_name(int) const —— ORIG T 0x08365f00
//   边界外或未知 idx 返回 "ERROR_SERVER"；服务器名字面量取自 .rodata
//   （EUC-KR 字节序列，以 \x 转义保真，提取地址见 case 注释）。
// ---------------------------------------------------------------------------
const char* CDataManager::get_server_name(int idx) const
{
    // ORIG 0x08365f03/07：`cmpl $0,idx; js`（idx<0）+ `cmpl $0x12,idx; jle`
    // 显式越界守卫 → ERROR_SERVER。跳表按 idx 直索引（0x8c332f8 表），
    // case 值 1..0x12（Ghidra C + 表[0]=default 双实证：idx=0 → ERROR_SERVER）；
    // case 0x64 落 default（ORIG 表 0..0x64 共 101 项、cmpl $0x64 上界，
    // 项 19..99 与 0、100 均填 default 块 0x8365fb9）。
    if (idx < 0 || idx > 0x12)
        return "ERROR_SERVER";
    switch (idx)
    {
    case 1:  return "\xc4\xab\xc0\xce";                      // 카인      8c3327b
    case 2:  return "\xb5\xf0\xb7\xb9\xc1\xf6\xbf\xa1";      // 디레지에  8c33280
    case 3:  return "\xbd\xc3\xb7\xce\xc4\xda";              // 시로코    8c33289
    case 4:  return "\xc7\xc1\xb7\xb9\xc0\xcc";              // 프레이    8c33290
    case 5:  return "\xc4\xab\xbd\xc3\xbe\xdf\xbd\xba";      // 카시야스  8c33297
    case 6:  return "\xc8\xfa\xb4\xf5";                      // 힐더      8c332a0
    case 7:  return "\xb7\xe7\xc5\xa9";                      // 루크      8c332a5
    case 8:  return "\xbc\xbc\xb8\xae\xbe\xc6";              // 세리아    8c332aa
    case 9:  return "\xbe\xc8\xc5\xe6";                      // 안톤      8c332b1
    case 10: return "\xb7\xcf\xbd\xc3";                      // 록시      8c332b6
    case 11: return "\xb9\xd9\xc4\xae";                      // 바칼      8c332bb
    case 12: return "\xbf\xc0\xc1\xee\xb8\xb6";              // 오즈마    8c332c0
    case 13: return "\xc4\xab\xc0\xdc";                      // 카잔      8c332c7
    case 14: return "\xc0\xcc\xba\xa5\xc6\xae""1";           // 이벤트1   8c332cc
    case 15: return "\xc0\xcc\xba\xa5\xc6\xae""2";           // 이벤트2   8c332d4
    case 16: return "\xc0\xcc\xba\xa5\xc6\xae""3";           // 이벤트3   8c332dc
    case 17: return "\xbd\xba\xc5\xb8\xc6\xae";              // 스타트    8c332e4
    case 18: return "\xc6\xdb\xbc\xb7""2";                   // 퍼섭2     8c332f1
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
    case 50:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 63:
    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
    case 71:
    case 72:
    case 73:
    case 74:
    case 75:
    case 76:
    case 77:
    case 78:
    case 79:
    case 80:
    case 81:
    case 82:
    case 83:
    case 84:
    case 85:
    case 86:
    case 87:
    case 88:
    case 89:
    case 90:
    case 91:
    case 92:
    case 93:
    case 94:
    case 95:
    case 96:
    case 97:
    case 98:
    case 99:
    case 0x64:            // ORIG 跳表覆盖 0..0x64（cmpl $0x64 上界 + 101 项表，
                          // 项 19..0x64 全填 default 块 0x8365fb9；独立 return 块
                          // 防 4.4 将空 case 折叠回 default 致跳表收缩）
        return "ERROR_SERVER";
    default:
        break;
    }
    return "ERROR_SERVER";
}
