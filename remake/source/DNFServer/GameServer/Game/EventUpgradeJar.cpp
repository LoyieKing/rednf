// ============================================================================
// df_game_r 还原 —— Event_Upgrade_Jar / EventUpgradeJarInfo。
// 逐函数依据 ORIG objdump/Ghidra 反汇编转写（见 docs/class_func_reports/Event_Upgrade_Jar/）。
// 布局与字段见 EventUpgradeJarInfo.h。
// ============================================================================

#include "EventUpgradeJarInfo.h"

#include <algorithm>

#include "DNFLexWrapper.h"   // loadRDARScriptFile/ScanType/ScanInt/ScanFloat（真实符号）

// ---- ST_Event_Upgrade_Jar_EventNPCs_Script 已内联 ctor（头文件）。----

// ============================================================================
// EventUpgradeJarInfo 生命周期
// ============================================================================

// vector<ST_...>(+0x74)。标量成员（m_npcCount/m_discountRate/m_upgradeMax/
// m_materialKind/m_materialIdx）ORIG 一律不初始化（工作状态运行时才写）。
EventUpgradeJarInfo::EventUpgradeJarInfo()
{
}

// ORIG 0x080f51d2（W）。dtor 依构造逆序析构全部 STL 成员（vector/map 自动释放）。
EventUpgradeJarInfo::~EventUpgradeJarInfo()
{
}

// ============================================================================
// Event_Upgrade_Jar 生命周期
// ============================================================================

// ORIG 0x080f49b8。仅转发到 EventUpgradeJarInfo::EventUpgradeJarInfo。
Event_Upgrade_Jar::Event_Upgrade_Jar()
    : EventUpgradeJarInfo()
{
}

// ORIG 0x080f49cc。仅转发到 EventUpgradeJarInfo::~EventUpgradeJarInfo。
Event_Upgrade_Jar::~Event_Upgrade_Jar()
{
}

// ============================================================================
// Event_Upgrade_Jar 方法
// ============================================================================

// ORIG 0x080f49e0。转发到 importEventUpgradeJarScript((EventUpgradeJarInfo*)this, path)。
// 第一个参数 scriptDir 未使用（ORIG 仅用第二个）。
// ORIG 返回 int（call 后 test %al,%al;je;mov$1;jmp;mov$0 的无 xor 物化，P9）：
// 直接 return bool 生 movzbl %al; 需三元 ?1:0 强制分支化转换。
int Event_Upgrade_Jar::LoadScript(const char* /*scriptDir*/, const char* path)
{
    return importEventUpgradeJarScript(this, path) ? 1 : 0;
}

// ORIG 0x080f4bb0。条件：item[+0x02]==0x28be7d 且 item[+0x07]>=1。
bool Event_Upgrade_Jar::check_requiredRecipe(const Inven_Item& item) const
{
    if (!(item.m_addInfo == 0x28be7d))
        return false;
    if (item.m_addInfo2 <= 0)
        return false;
    return true;
}

 // ORIG 0x080f4be0。Fortune 分支：在 m_materialIndices 中查 materialIdx，命中则在对应的
// m_failRateByFortune[i] 段中按 upgrade 取概率；越界/未命中返回 100000。
unsigned int Event_Upgrade_Jar::_getFailProb_Fortune(int materialIdx, unsigned int upgrade) const
{
    // ORIG：i 为 int（size() 与 i 有符号 setg 比较）。
    for (int i = 0; i < static_cast<int>(m_materialIndices.size()); ++i)
    {
        if (materialIdx == (int)m_materialIndices[i])
        {
            const std::vector<unsigned int>& failRates = m_failRateByFortune[i];
            if (upgrade >= failRates.size())
                return 100000;
            return failRates[upgrade];
        }
    }
    return 100000;
}

// ORIG 0x080f4c8e。NewNPC 分支：按 m_npcScripts[idx].m_failRate 取值；越界返回 100000。
unsigned int Event_Upgrade_Jar::_getFailProb_NewNPC(unsigned int idx) const
{
    if (idx >= m_npcScripts.size())
        return 100000;
    return m_npcScripts[idx].m_failRate;
}

// ORIG 0x080f4ccc。NewNPC 分支：按 m_npcScripts[idx].m_meterialCount 取值；越界返回 1000000。
unsigned int Event_Upgrade_Jar::_getMeterialCount_NewNPC(unsigned int idx)
{
    if (idx >= m_npcScripts.size())
        return 1000000;
    return m_npcScripts[idx].m_meterialCount;
}

// ORIG 0x080f4d0c。读 m_discountRate（+0x70）。
float Event_Upgrade_Jar::_getDiscountRate_NewNPC() const
{
    return m_discountRate;
}

// ORIG 0x080f4d20。设 m_materialKind：index==0x28be7b → 2（newNPC）；在 m_materialIndices
// 中命中 → 1（fortune）；否则 0。m_materialIdx 清 0。
void Event_Upgrade_Jar::CheckMaterialIndex(unsigned long index)
{
    m_materialKind = 0;
    m_materialIdx = 0;

    if (index == 0x28be7b)
    {
        m_materialKind = 2;
        return;
    }

    // ORIG 0x080f4d20：循环比较为 (int)i < size()（Ghidra 明示），setg 有符号。
    // 原 unsigned 生 seta 反符号（P196）。
    for (int i = 0; i < (int)m_materialIndices.size(); ++i)
    {
        if (m_materialIndices[i] == index)
        {
            m_materialKind = 1;
            return;
        }
    }
}

// ORIG 0x080f4dac。按 m_materialKind 填充 upgrade_info_t：
//   kind==1（fortune）：info.m_meterialCount=1，m_recipeItem 在 m_materialIndices 中查
//     materialIdx 命中则取对应索引值；kind==2（newNPC）：m_meterialCount=打折后材料数、
//     m_recipeItem=0x28be7b、m_discountBase=round(discountRate*原值)。末写 m_materialIdx=upgrade。
void Event_Upgrade_Jar::SettingRecipeNUpConunt(int materialIdx, upgrade_info_t& info, unsigned char upgrade)
{
    if (m_materialKind != 0)
    {
        if (m_materialKind == 1)
        {
            // ORIG：+0x34(m_discountBase)=0、+0x30(m_meterialCount)=1、
            // +0x2c(m_recipeItem)=m_materialIndices 中命中 materialIdx 的值。
            info.m_discountBase = 0;
            info.m_meterialCount = 1;
            for (unsigned int i = 0; i < m_materialIndices.size(); ++i)
            {
                if (materialIdx == (int)m_materialIndices[i])
                {
                    info.m_recipeItem = m_materialIndices[i];
                    break;
                }
            }
        }
        else if (m_materialKind == 2)
        {
            info.m_discountBase = (int)(m_discountRate * (float)info.m_discountBase);
            info.m_meterialCount = _getMeterialCount_NewNPC(upgrade);
            info.m_recipeItem = 0x28be7b;
        }
        m_materialIdx = upgrade;
    }
}

// ORIG 0x080f4ee2。按 m_materialKind 取失败概率写入 outProb：kind==1 → _getFailProb_Fortune，
// kind==2 → _getFailProb_NewNPC。kind==0 或其它不改 outProb。
void Event_Upgrade_Jar::GetProbability(int materialIdx, unsigned int& outProb) const
{
    // [b28c-35] N：ORIG 0x080f4f2b/0x080f4f5c 两分支各自 `jmp <尾>` + 尾前 nop 岛
    // （4.1 无 jump-threading）；4.4 对 else-if / return / 多源 goto 三形均把 case2 的
    // jmp 折成 fall-through（P223 单源 goto 尾岛合并负例）。语义全等。
    // ORIG 0x080f4ee2：先显式 test kind;je 直跳到共同尾（m_materialKind!=0 外guard），
    // 再 ==1 / ==2 分支。原 if==1 else-if==2 缺外 guard 的 test/je 5 条。
    if (m_materialKind != 0)
    {
        if (m_materialKind == 1)
        {
            outProb = _getFailProb_Fortune(materialIdx, (unsigned int)m_materialIdx);
            // [b28c-35] ORIG 0x080f4f2b/0x080f4f5c 两分支各自 `jmp <尾>` 且尾前有
            // 空块 nop 岛（P223）；else-if 形 4.4 把 case2 的 jmp 折成 fall-through。
            goto END;
        }
        if (m_materialKind == 2)
        {
            outProb = _getFailProb_NewNPC((unsigned int)m_materialIdx);
            goto END;
        }
    }
END:;
}

// ORIG 0x080f4f62。条件：m_materialIndices.size()>1 且 [1]==upgrade 且 item.GetUpgrade()>=m_upgradeMax
// → 返回 false；否则 true。
bool Event_Upgrade_Jar::CheckMaxLuckyLevel(Inven_Item& item, unsigned long upgrade)
{
    // [b28c-35] P0 清绑：删 3 枚 `register bool asm("al")` 显式绑定（本波定稿：
    // 任何情形不得保留显式绑定，宁 AE→DIFF 也要清）。改自然嵌套 if（P225 拆嵌套
    // if 复现逐条件 setcc 物化）。
    if (m_materialIndices.size() > 1)
    {
        if (m_materialIndices[1] == upgrade)
        {
            if ((unsigned int)item.GetUpgrade() >= (unsigned int)m_upgradeMax)
            {
                return false;
            }
        }
    }
    return true;
}

// ORIG 0x080f4fd0。m_materialIndices.size()>=2 时返回 [1]，否则 0。
unsigned long Event_Upgrade_Jar::GetIndexLuckyStrengtheningDeed()
{
    if (m_materialIndices.size() >= 2)
        return m_materialIndices[1];
    return 0;
}

// ORIG 0x080f4a0a。在 m_boosterIndexMap 查 cardIdx/boosterIdx，两键都存在时返回对应
// m_recipeMap[(min,max)] 的值（min/max 为两键对应的 short 值）；否则 0。
unsigned long Event_Upgrade_Jar::getBoosterIndex_byUpgradeCardIndex(unsigned long& cardIdx, unsigned long& boosterIdx)
{
    std::map<unsigned long, short>::const_iterator itCard = m_boosterIndexMap.find(cardIdx);
    if (itCard == m_boosterIndexMap.end())
        return 0;
    std::map<unsigned long, short>::const_iterator itBooster = m_boosterIndexMap.find(boosterIdx);
    if (itBooster == m_boosterIndexMap.end())
        return 0;

    short low = itCard->second;
    short high = itBooster->second;
    if (high < low)
        std::swap(low, high);

    std::map<std::pair<int, int>, unsigned long>::const_iterator itRecipe =
        m_recipeMap.find(std::make_pair((int)low, (int)high));
    if (itRecipe == m_recipeMap.end())
        return 0;
    return itRecipe->second;
}

// ORIG 0x0854b57c（W）。m_materialKind==1。
bool Event_Upgrade_Jar::isFortune() const
{
    return m_materialKind == 1;
}

// ORIG 0x0854b590（W）。m_materialKind==2。
bool Event_Upgrade_Jar::isNewNPC() const
{
    return m_materialKind == 2;
}

// ============================================================================
// 自由函数
// ============================================================================

// ORIG 0x089687d1。解析 EventUpgradeJar.etc 脚本，填充 EventUpgradeJarInfo 字段。
// 段：`[group info]`（npcCount + 索引 map + 配方 map）、`[fortune index]`（材料索引）、
// `[fortune fail rate]`（分段失败率）、`[event upgrade table]`（折扣率 + NPC 脚本）、
// `[upgrade max]`。任一 Scan 失败返回 false（0）。
bool importEventUpgradeJarScript(EventUpgradeJarInfo* info, const char* path)
{
    if (info == 0 || path == 0)
        return false;
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    bool ok = true;
    bool consumed = true;

    while (consumed && ScanType(token, true))
    {
        if (token == "[group info]")
        {
            int npcCount = 0;
            if (!ScanInt(&npcCount))
            {
                ok = false;
                break;
            }
            info->m_npcCount = (unsigned short)npcCount;

            // ORIG：每组连续读入若干 id（`local_7c`），以 `-1` 结束；将
            // `{id, 组索引 g}` 插入 m_boosterIndexMap（value = 0-based 组号）。
            for (unsigned short g = 0; g < info->m_npcCount; ++g)
            {
                for (;;)
                {
                    int id = 0;
                    if (!ScanInt(&id))
                    {
                        ok = false;
                        break;
                    }
                    if (id == -1)
                        break;
                    info->m_boosterIndexMap[(unsigned long)id] = (short)g;
                }
                if (!ok)
                    break;
            }
            if (!ok)
                break;

            // 配方对：`<cardIdx> <boosterIdx> <recipeIndex>`，共 npcCount*(npcCount+1)/2 组。
            int pairCount = (int)((unsigned int)info->m_npcCount * (info->m_npcCount + 1U)) >> 1;
            for (int i = 0; i < pairCount; ++i)
            {
                int cardIdx = 0, boosterIdx = 0, recipeIndex = 0;
                if (!ScanInt(&cardIdx) || !ScanInt(&boosterIdx) || !ScanInt(&recipeIndex))
                {
                    ok = false;
                    break;
                }
                int lo = cardIdx - 1;
                int hi = boosterIdx - 1;
                info->m_recipeMap[std::make_pair(lo, hi)] = (unsigned long)recipeIndex;
            }
            if (!ok)
                break;
        }
        else if (token == "[fortune index]")
        {
            for (;;)
            {
                int idx = 0;
                if (!ScanInt(&idx))
                {
                    ok = false;
                    break;
                }
                if (idx == -1)
                    break;
                info->m_materialIndices.push_back((unsigned long)idx);
            }
            if (!ok)
                break;
        }
        else if (token == "[fortune fail rate]")
        {
            // ORIG：每次读三值 `groupIdx(=1-based) <discard> <failRate>`，将 failRate
            // 压入 m_failRateByFortune[groupIdx-1]；groupIdx==-1 终止。
            for (;;)
            {
                int groupIdx = 0;
                if (!ScanInt(&groupIdx))
                {
                    ok = false;
                    break;
                }
                if (groupIdx == -1)
                    break;
                int dummy = 0, rate = 0;
                if (!ScanInt(&dummy) || !ScanInt(&rate))
                {
                    ok = false;
                    break;
                }
                info->m_failRateByFortune[groupIdx - 1].push_back((unsigned int)rate);
            }
            if (!ok)
                break;
        }
        else if (token == "[event upgrade table]")
        {
            float discount = 0.0f;
            if (!ScanFloat(&discount))
            {
                ok = false;
                break;
            }
            info->m_discountRate = discount;
            for (;;)
            {
                int dummy = 0;
                if (!ScanInt(&dummy))     // 丢弃（占位，无读取字段）
                {
                    ok = false;
                    break;
                }
                int failRate = 0;
                if (!ScanInt(&failRate))
                    break;                // 序列终止
                int meterialCount = 0;
                if (!ScanInt(&meterialCount))
                {
                    ok = false;
                    break;
                }
                ST_Event_Upgrade_Jar_EventNPCs_Script npc;
                npc.m_failRate = (unsigned int)failRate;
                npc.m_meterialCount = (unsigned int)meterialCount;
                info->m_npcScripts.push_back(npc);
            }
            if (!ok)
                break;
        }
        else if (token == "[upgrade max]")
        {
            int max = 0;
            if (!ScanInt(&max))
            {
                ok = false;
                break;
            }
            info->m_upgradeMax = max;
            consumed = true;
        }
        else
        {
            // 未知段：跳过（保持 ScanType 循环继续）。
        }
    }

    return ok;
}

// ORIG 0x08968d59。把 EventUpgradeJarInfo 的 NPC 脚本填进 upgrade_table_t 的 m_infos
// （m_recipeItem=0x28be7b、m_meterialCount=npc.m_meterialCount），并对 m_infos[0..99]
// 的折扣基数应用 m_discountRate。任一指针为空返回 0。
bool updateUpgradeTableFromEventUpgradeJarScript(upgrade_table_t* table, EventUpgradeJarInfo* info)
{
    if (table == 0 || info == 0)
        return false;

    for (unsigned int i = 0; i < info->m_npcScripts.size(); ++i)
    {
        if (i < table->m_infos.size())
        {
            upgrade_info_t& infoRow = table->m_infos[i];
            infoRow.m_recipeItem = 0x28be7b;
            infoRow.m_meterialCount = (int)info->m_npcScripts[i].m_meterialCount;
        }
    }

    // 对 upgrade_table_t 主体表（m_body）的 100 项应用折扣：
    // 绝对偏移 (i+0x28)*4+4 处为 int 计数值，乘 m_discountRate 后四舍五入回写。
    // （ORIG： *(int*)(param_1 + (i+0x28)*4+4) = round(discount * 旧值)）
    for (unsigned int i = 0; i < 100; ++i)
    {
        int off = (i + 0x28) * 4 + 4;
        UPG_TBL_INT(table, off) = (int)(info->m_discountRate * (float)UPG_TBL_INT(table, off));
    }

    return true;
}
