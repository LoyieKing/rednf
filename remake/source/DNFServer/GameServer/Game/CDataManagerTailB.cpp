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
// IsAvailablePvPChannel(int) —— ORIG W 0x0819ede0
//   b434 = m_pvpChannel.m_minGrade(+0x04)、b438 = m_pvpChannel.m_maxGrade(+0x08)
//   两界同为 0 → 视作无限制返回 1；否则要求 minGrade <= arg <= maxGrade。
// ---------------------------------------------------------------------------
bool CDataManager::IsAvailablePvPChannel(int grade)
{
    if (m_pvpChannel.m_minGrade == 0 && m_pvpChannel.m_maxGrade == 0)
    {
        return true;
    }
    if (m_pvpChannel.m_minGrade > grade || m_pvpChannel.m_maxGrade < grade)
        goto unavailable;
    return true;
unavailable:
    return false;
}

// GetMinLevelPvPChannel —— ORIG W 0x0819ee2e：读 m_pvpChannel.m_minLevel(+0x14 → CDM+0xb444)
int CDataManager::GetMinLevelPvPChannel()
{
    return m_pvpChannel.m_minLevel;
}

// GetMaxLevelPvPChannel —— ORIG W 0x0819ee3c：读 m_pvpChannel.m_maxLevel(+0x18 → CDM+0xb448)
int CDataManager::GetMaxLevelPvPChannel()
{
    return m_pvpChannel.m_maxLevel;
}

// ---------------------------------------------------------------------------
// GetLimitItemUsageInfo —— ORIG W 0x8693f16
//   add $0x4cf8,%eax 直取并返回；+0x4cf8 即 m_limitItemUsage 成员。
// ---------------------------------------------------------------------------
stLimitItemUsageInfoEx* CDataManager::GetLimitItemUsageInfo()
{
    return &m_limitItemUsage;
}

// ---------------------------------------------------------------------------
// is_AuctionRegFreeCeraItem(unsigned int) const —— ORIG W 0x84e92a4
//   lea 0xb4ac(this)（m_choiceSet）→ set<unsigned int>::find(idx) 后与 end 比较，
//   非 end 返回非零。语义：该道具在"拍卖行注册免费道具"集合内。
// ---------------------------------------------------------------------------
bool CDataManager::is_AuctionRegFreeCeraItem(unsigned int itemIdx) const
{
    return m_choiceSet.find(itemIdx) != m_choiceSet.end();
}

// ---------------------------------------------------------------------------
// VerifyDungeon(CDungeon const&) const —— ORIG T 0x08362b44
//   遍历 dungeon.m_clearItemMap(+0x1c) 的 value（clear 奖励 itemIdx）逐一调
//   find_item（结果仅用于数据校验，不使用返回值），随后 dungeon.VerifyMaze()；
//   固定返回 1（调用方 Verify 按返回值恒真处理，与 ORIG 一致）。
// ---------------------------------------------------------------------------
int CDataManager::VerifyDungeon(const CDungeon& dungeon) const
{
    for (std::map<int, int>::const_iterator it = dungeon.m_clearItemMap.begin();
         it != dungeon.m_clearItemMap.end(); ++it)
    {
        find_item(it->second);
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
int CDataManager::Verify() const
{
    static const char PRETTY[] = "bool CDataManager::Verify() const";

    // [ORIG +0x0027..+0x00d7] mob reward 单调性（i ∈ [0,200)）
    for (int i = 0; i < 200; ++i)
    {
        unsigned int cur = (unsigned int)m_mobReward[i];
        unsigned int next = (unsigned int)m_mobReward[i + 1];
        if (next <= cur)
        {
            cMyTrace trace(PRETTY, 0x2723, 9, true, true);
            trace("Warning: Unusual mob reward(%d) on mob grade(%d)",
                  (int)cur, i);
        }
    }

    // [ORIG +0x00d7..+0x0247] 11 个职业的技能 required-level-range tag 检查
    bool skillFail = false;
    for (int job = 0; job <= 0xa; ++job)
    {
        __gnu_cxx::hash_map<int, CSkill*>* map =
            m_skillList->getSkillList(job);
        for (__gnu_cxx::hash_map<int, CSkill*>::const_iterator
                 it(map->begin()), e(map->end()); it != e; ++it)
        {
            CSkill* skill = it->second;
            // 【ORIG asm 08360f15: mov 0xc8(%eax),%eax】required level range tag
            const char* sp = (const char*)skill;
            if (*(const int*)(sp + 0xc8) == 0)
            {
                cMyTrace trace(PRETTY, 0x272e, 9, true, true);
                trace("Skill [required level range] tag value error "
                      "(Job: %d)(Skill: %d)", job, skill->get_index());
                skillFail = true;
            }
        }
    }
    if (skillFail)
    {
        cMyTrace trace(PRETTY, 0x2735, 9, true, true);
        trace("Verifying Skills fail");
        return false;
    }

    // [ORIG +0x02a3..+0x0363] 主地图校验（逐张覆盖失败标志，不提前 break；
    // ORIG 同样每次覆写 local_69 后统一判定）
    bool mapFail = false;
    {
        // asm 0836103b: this+0x04 → hash_map 直接作用于该对象首部
        // （CMapList::m_mainMap +0x00）
        for (__gnu_cxx::hash_map<int, CMap*>::const_iterator
                 it(m_mapList->m_mainMap.begin()),
                 e(m_mapList->m_mainMap.end()); it != e; ++it)
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
             it(m_dungeonList->m_dungeonMap.begin()),
             e(m_dungeonList->m_dungeonMap.end()); it != e; ++it)
    {
        VerifyDungeon(*it->second);
    }

    // [ORIG +0x03f8..+0x0994] 道具表 booster/package 引用完整性
    bool itemFail = false;
    {
        // asm 083611c8: this+0x0c(m_itemList) + lea 0x4 → CItemList::m_itemMap
        for (__gnu_cxx::hash_map<int, CItem*>::const_iterator
                 it(m_itemList->m_itemMap.begin()),
                 e(m_itemList->m_itemMap.end()); it != e; ++it)
        {
            CItem* item = it->second;
            if (!item->is_stackable())
            {
                continue;
            }
            const int type = item->GetItemType();
            if (type == 0xf || type == 0x10)
            {
                // booster：CStackableItem+0x240 stStackableBooster_t.m_elements
                CStackableItem* sitem = (CStackableItem*)item;
                for (std::vector<stStackableBoosterElement_t>::const_iterator
                         el = sitem->m_booster.m_elements.begin();
                     el != sitem->m_booster.m_elements.end(); ++el)
                {
                    for (std::vector<stStackableBoosterItemInfo_t>::const_iterator
                             info = el->m_items.begin();
                         info != el->m_items.end(); ++info)
                    {
                        if (info->m_itemIdx == 1)
                        {
                            continue;
                        }
                        if (find_item(info->m_itemIdx) == 0)
                        {
                            cMyTrace trace(PRETTY, 0x275a, 9, true, true);
                            trace("Booster item element not found (%d)->(%d)",
                                  item->get_index(), info->m_itemIdx);
                            itemFail = true;
                        }
                    }
                }
            }
            else if (type == 0xe)
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
                         pkgs.begin();
                     p != pkgs.end(); ++p)
                {
                    if (p->m_field0 == 1)
                    {
                        continue;
                    }
                    if (find_item(p->m_field0) == 0)
                    {
                        cMyTrace trace(PRETTY, 0x276b, 9, true, true);
                        trace("Package item element not found (%d)->(%d)",
                              item->get_index(), p->m_field0);
                        itemFail = true;
                    }
                }

                std::vector<std::vector<STStackablePackageData> >& sel =
                    *reinterpret_cast<
                        std::vector<std::vector<STStackablePackageData> >*>(
                        &sitem->m_pkg234);  // 【CStackableItem+0x234】
                for (std::vector<std::vector<STStackablePackageData> >::
                         const_iterator group = sel.begin();
                     group != sel.end(); ++group)
                {
                    for (std::vector<STStackablePackageData>::const_iterator p =
                             group->begin();
                         p != group->end(); ++p)
                    {
                        if (p->m_field0 == 1)
                        {
                            continue;
                        }
                        if (find_item(p->m_field0) == 0)
                        {
                            cMyTrace trace(PRETTY, 0x277b, 9, true, true);
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
                item->GetItemType();  // ORIG 其余类型空检（虚调用后弃值）
            }
        }
    }

    if (!itemFail)
    {
        // [ORIG +0x0994..+0x1149] Quest 表奖励引用检查
        bool questFail = false;
        for (__gnu_cxx::hash_map<int, Quest*>::const_iterator
                 qit(m_questList->m_questMap.begin()),
                 qe(m_questList->m_questMap.end()); qit != qe; ++qit)
        {
            Quest* quest = qit->second;
            const char* qp = (const char*)quest;

            // 【ORIG asm 083618aa: mov 0xb8(%eax)】quest 类型/tag 字段
            const int state = *(const int*)(qp + 0xb8);
            if (state != 0 && state != 0x19)
            {
                continue;
            }

            // 【ORIG +0xc4/+0xd0】两段 RewardItemInfo 奖励向量（当前 Quest.h
            // 未建模此区布局，按 ORIG 偏移访问）
            {
                std::vector<RewardItemInfo>& v =
                    *reinterpret_cast<std::vector<RewardItemInfo>*>(const_cast<char*>(qp) + 0xc4);
                for (std::vector<RewardItemInfo>::const_iterator it = v.begin();
                     it != v.end(); ++it)
                {
                    const int itemIdx = it->m_itemIdx;
                    if (itemIdx != 0 && itemIdx != 1 && itemIdx != 2 &&
                        itemIdx != 999 &&
                        this->find_item(itemIdx) == 0)
                    {
                        cMyTrace trace(PRETTY, 0x27a1, 5);
                        trace("Quest reward item element not found "
                              "(quest: %d)(item: %d)",
                              *(const int*)(qp + 0x04), itemIdx);  // 【ORIG asm 08361b7e: quest+0x04】
                        questFail = true;
                    }
                }
            }
            {
                std::vector<RewardItemInfo>& v =
                    *reinterpret_cast<std::vector<RewardItemInfo>*>(const_cast<char*>(qp) + 0xd0);
                for (std::vector<RewardItemInfo>::const_iterator it = v.begin();
                     it != v.end(); ++it)
                {
                    const int itemIdx = it->m_itemIdx;
                    if (itemIdx != 0 && itemIdx != 1 && itemIdx != 2 &&
                        itemIdx != 999 &&
                        this->find_item(itemIdx) == 0)
                    {
                        cMyTrace trace(PRETTY, 0x27ac, 5);
                        trace("Quest reward select item element not found "
                              "(quest: %d)(item: %d)",
                              *(const int*)(qp + 0x04), itemIdx);  // 【ORIG asm 08361b7e: quest+0x04】
                        questFail = true;
                    }
                }
            }
            // 【ORIG +0xdc】collision quest 列表（vector<int>）
            {
                const std::vector<int>& v =
                    *reinterpret_cast<const std::vector<int>*>(qp + 0xdc);
                for (std::vector<int>::const_iterator it = v.begin();
                     it != v.end(); ++it)
                {
                    if (find_quest(*it) == 0)
                    {
                        cMyTrace trace(PRETTY, 0x27cd, 5);
                        trace("Quest reward collision quest element now found "
                              "(quest: %d)(collision quest: %d)",
                              *(const int*)(qp + 0x04), *it);  // 【ORIG asm 08361b7e: quest+0x04】
                        questFail = true;
                    }
                }
            }
            // 【ORIG +0xec】collision item 向量（RewardItemInfo）
            {
                std::vector<RewardItemInfo>& v =
                    *reinterpret_cast<std::vector<RewardItemInfo>*>(const_cast<char*>(qp) + 0xec);
                for (std::vector<RewardItemInfo>::const_iterator it = v.begin();
                     it != v.end(); ++it)
                {
                    const int itemIdx = it->m_itemIdx;
                    if (itemIdx != 0 && itemIdx != 1 && itemIdx != 2 &&
                        itemIdx != 999 &&
                        this->find_item(itemIdx) == 0)
                    {
                        cMyTrace trace(PRETTY, 0x27da, 5);
                        trace("Quest reward collision item element not found "
                              "(quest: %d)(item: %d)",
                              *(const int*)(qp + 0x04), itemIdx);  // 【ORIG asm 08361b7e: quest+0x04】
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
                     it(m_monsterManager->m_monsterMap.begin()),
                     e(m_monsterManager->m_monsterMap.end()); it != e; ++it)
            {
                CMonster* mon = it->second;
                const char* mp = (const char*)mon;
                const int lvA = *(const int*)(mp + 0x58);  // 【ORIG asm +0x10de】
                const int lvB = *(const int*)(mp + 0x5c);  // 【ORIG asm +0x10db】
                if (lvA < 1 || 200 < lvA || lvB < 1 || 200 < lvB)
                {
                    cMyTrace trace(PRETTY, 0x2808, 9, true, true);
                    trace("Error: Invalid monster(%d) level(%d-%d)",
                          mon->get_index(), lvA, lvB);
                }
            }

            // [ORIG +0x11a0..+0x1313] 全副本通关模拟（WongWork::CDungeonClear）
            WongWork::CDungeonClear clearSim;
            for (__gnu_cxx::hash_map<int, CDungeon*>::const_iterator
                     it(m_dungeonList->m_dungeonMap.begin()),
                     e(m_dungeonList->m_dungeonMap.end()); it != e; ++it)
            {
                clearSim.addClearedDungeon(
                    (short)(*it->second).get_index(), '\x04');
            }
            char buf[6144];
            memset(buf, 0, sizeof(buf));
            clearSim.toString(buf, 0x1800);
            // ORIG toString(char*,unsigned) 返回字节数并在 <0 时打
            // "Simulate dungeon clear data fail!!!!"（0x2815）。当前权威头
            // WongWork::CDungeonClear::toString 为 void，无法取得写回长度；
            // 空串判失败仅为最保守近似（buf 未写入则仍会判定）。
            if (buf[0] == '\0')
            {
                cMyTrace trace(PRETTY, 0x2815, 9, true, true);
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
char* CDataManager::_subPunctuationMark(const char* src, char* dst)
{
    unsigned int out = 0;
    const size_t len = strlen(src);
    for (size_t i = 0; i < len; ++i)
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
            const unsigned short w =
                (unsigned short)((unsigned short)*p |
                                 ((unsigned short)p[1] << 8));
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
void CDataManager::rewriteUsableEmoticon(const CUser& user, char* text)
{
    if (text == 0)
    {
        return;
    }
    const size_t len = strlen(text);
    for (size_t i = 0; (int)((int)len - 2) > (int)i; ++i)
    {
        if ((unsigned char)text[i] != 0x1e ||
            (unsigned char)text[i + 1] != ' ')
        {
            continue;
        }
        const int emoCode = (signed char)text[i + 2];  // asm movsbl
        ChattingEmoticonBase* emo = m_emoticonList->find_emoticon(emoCode);
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
int CDataManager::hasPreventPrefixCharacName(const char* name, int /*unused*/)
{
    ServerParameterScript& sps = m_serverParameter;
    for (int i = 0; sps.m_preventPrefixCount > i; ++i)
    {
        const char* entry = sps.m_preventPrefixEntries + i * 0x1e;
        const size_t entryLen = strlen(entry);
        char prefix[0x15] = { '\0' };
        memcpy(prefix, name, entryLen);
        if (strcmp(entry, prefix) == 0)
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
bool CDataManager::hasSpecialCharacterExceptSpace(const char* name)
{
    const size_t len = strlen(name);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char* p = (const unsigned char*)(name + i);
        if (_isOneByteCharacter(*p))
        {
            {
                for (std::vector<InvalidCharData::stRangeElement<
                         unsigned char> >::const_iterator it =
                         m_invalidChar.m_charRange.begin();
                     it != m_invalidChar.m_charRange.end(); ++it)
                {
                    if (it->m_min <= *p && *p < it->m_max)
                    {
                        return true;
                    }
                }
            }
            {
                if (std::find(m_invalidChar.m_charList.begin(),
                              m_invalidChar.m_charList.end(),
                              *p) != m_invalidChar.m_charList.end())
                {
                    return true;
                }
            }
        }
        else
        {
            const unsigned short w =
                (unsigned short)((unsigned short)*p |
                                 ((unsigned short)p[1] << 8));
            {
                for (std::vector<InvalidCharData::stRangeElement<
                         unsigned short> >::const_iterator it =
                         m_invalidChar.m_shortRange.begin();
                     it != m_invalidChar.m_shortRange.end(); ++it)
                {
                    if (it->m_min <= w && w <= it->m_max)
                    {
                        return true;
                    }
                }
            }
            {
                if (std::find(m_invalidChar.m_shortList.begin(),
                              m_invalidChar.m_shortList.end(),
                              w) != m_invalidChar.m_shortList.end())
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

    const int defaultKey = 0xb;
    it = m_createChrac.m_map0.find(defaultKey);
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
int CDataManager::get_useJobFatigueMatchGift(char job, int a, int b)
{
    const int diff = a - b;
    std::map<int, useCharacterJobFatigue::fatigueMatchGift>::iterator it =
        m_useJobFatigue.m_giftMap.find((int)job);
    if (it == m_useJobFatigue.m_giftMap.end())
    {
        return 0;
    }
    std::map<int, int>& rewardMap = it->second.m_rewardMap;
    std::map<int, int>::iterator uEnd = rewardMap.end();
    std::map<int, int>::iterator u1 = rewardMap.upper_bound(diff);
    std::map<int, int>::iterator u2 = rewardMap.upper_bound(a);
    if (u1 == uEnd && u2 == rewardMap.end())
    {
        return 0;
    }
    if (u1 == u2)
    {
        return 0;
    }
    return u1->second;
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
