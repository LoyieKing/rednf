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

// ORIG 0x080f49b8。仅转发到 EventUpgradeJarInfo::EventUpgradeJarInfo。
Event_Upgrade_Jar::Event_Upgrade_Jar()
    : EventUpgradeJarInfo()
{
}

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
        // [b28d-solo-EventUpgradeJar] P231/P248：ORIG 0x080f4bf0 每轮循环体首条
        // `mov 0xc(%ebp),%ebx` 把形参升格 callee-saved 跨 operator[] 保活 ⇒ 纯
        // register 存储类提示、声明在循环体内（无 asm 绑定，P0 合规）。残余
        // `cmp %eax,%ebx` vs `cmp %ebx,%eax` 操作数序属 N18b。
        register int midx = materialIdx;
        if (midx == (int)m_materialIndices[i])
        {
            // [b28d-solo-EventUpgradeJar] P157/P183：ORIG 0x080f4c13/0x080f4c3e 两次
            // 重算 this+i*0xc+0x40 全链（无引用缓存槽），逐次表达式访问。
            if (upgrade >= m_failRateByFortune[i].size())
                return 100000;
            return m_failRateByFortune[i][upgrade];
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
int Event_Upgrade_Jar::_getMeterialCount_NewNPC(unsigned int idx)
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
    // [b28d-solo-EventUpgradeJar] N 封顶（verdict=NEAR，助记符序列全等、指令数
    // 95=95）：残余① float base 槽 -0x10 vs ORIG -0x28（4.1 浮点局部落帧最深、
    // 4.4 落浅槽；N82/N95 族，pad/声明序探针反劣）；② kind==1 循环
    // `cmp %eax,%ebx` vs `cmp %ebx,%eax` 操作数序（N18b 4.4 恒折）。
    // [b28d-solo-EventUpgradeJar] P212/N98：ORIG 槽序 i=-0xc（浅）/base=-0x28（深），
    // 全头部声明「先声明深」——base 先声明、i 后声明即复现（块内声明 4.4 反向）。
    float base;
    int i;

    // [b28d-solo-EventUpgradeJar] ORIG 0x080f4dac：kind==0 落 leave 前 nop 岛
    // （0x080f4edb），kind==1 尾 `jmp TAIL`、kind==2 fall-through 到 TAIL，TAIL 写
    // m_materialIdx 后 `jmp END` 跳过 nop 岛 ⇒ 负极性早退 goto + 中间 TAIL 标签
    // （P223 正例；探针 /tmp/eu_probe1.cpp fB）。
    if (m_materialKind == 0)
        goto END;

    if (m_materialKind == 1)
    {
        // ORIG：+0x34(m_discountBase)=0、+0x30(m_meterialCount)=1、
        // +0x2c(m_recipeItem)=m_materialIndices 中命中 materialIdx 的值。
        info.m_discountBase = 0;
        info.m_meterialCount = 1;
        // ORIG 循环比较为有符号 setg（P196：unsigned 生 seta）。
        for (i = 0; i < (int)m_materialIndices.size(); ++i)
        {
            // P231/P248：ORIG 0x080f4df9 每轮循环体首条 `mov 0xc(%ebp),%ebx`。
            register int midx = materialIdx;
            if (midx == (int)m_materialIndices[i])
            {
                info.m_recipeItem = m_materialIndices[i];
                break;
            }
        }
        goto TAIL;
    }
    if (m_materialKind == 2)
    {
        // b28d-lane25：P167 差集实证 ORIG 0x080f4d?? 调 _getDiscountRate_NewNPC()
        // 取折扣率（非直读 m_discountRate 成员）。
        // [b28d-solo-EventUpgradeJar] P252：ORIG 0x080f4e73 先读 m_discountBase 到
        // -0x20、fildl+fstps 存具名 float 局部（-0x28），**之后**才 call 折扣率 getter，
        // 再 fmuls 该槽 ⇒ 原码有具名 float 局部且求值序在 call 之前。
        base = (float)info.m_discountBase;
        info.m_discountBase = (int)(_getDiscountRate_NewNPC() * base);
        info.m_meterialCount = _getMeterialCount_NewNPC(upgrade);
        info.m_recipeItem = 0x28be7b;
    }
TAIL:
    m_materialIdx = upgrade;
END:;
}

// ORIG 0x080f4ee2。按 m_materialKind 取失败概率写入 outProb：kind==1 → _getFailProb_Fortune，
// kind==2 → _getFailProb_NewNPC。kind==0 或其它不改 outProb。
void Event_Upgrade_Jar::GetProbability(int materialIdx, unsigned int& outProb) const
{
    // [b28d-solo-EventUpgradeJar] ORIG 0x080f4ee2：kind==0 的 je 落到 leave 前的
    // 空 then nop 岛（0x080f4f5e），两分支各自 `jmp leave`。4.4 的复现形为
    // 「负极性早退 goto + 尾标签」（P223 正例），外层 if(kind!=0) 包裹会被
    // jump-thread 掉 nop 岛（探针 /tmp/eu_probe1.cpp fB/fD 实证）。
    if (m_materialKind == 0)
        goto END;

    if (m_materialKind == 1)
    {
        outProb = _getFailProb_Fortune(materialIdx, (unsigned int)m_materialIdx);
        goto END;
    }
    if (m_materialKind == 2)
    {
        outProb = _getFailProb_NewNPC((unsigned int)m_materialIdx);
        goto END;
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
    // b28d-lane25：P167 差集实证 ORIG 用非 const `_Rb_tree_iterator`（ne/ptEv）且
    // make_pair(low,high) 为 pair<short,short>→pair<int,int> 转换（P96/P270/P192）。
    std::map<unsigned long, short>::iterator itCard = m_boosterIndexMap.find(cardIdx);
    std::map<unsigned long, short>::iterator itBooster = m_boosterIndexMap.find(boosterIdx);
    // [b28d-solo-EventUpgradeJar] ORIG 0x080f4a78/0x080f4aa6 两条件各 test/je 后
    // `mov $1/%eax … mov $0` 再 `test %al,%al; je`（bool 短路物化、值无槽往返）⇒
    // P226 语句表达式包裹 && 短路（嵌套 if 会被 4.4 直跳、缺物化岛）。
    // P74 补充②：ORIG 0x080f4a66/0x080f4a94 的 ne 调用 this=end 临时、实参=it
    // ⇒ 源码把 end() 写在表达式左侧（`end() != it`）。
    if (({ m_boosterIndexMap.end() != itCard && m_boosterIndexMap.end() != itBooster; }))
    {
        // ORIG 槽序：low=-0x2c（深）/high=-0x2a（浅）。用 short[2] 数组：ORIG
        // 0x080f4b1e 的 make_pair 第二实参 `lea -0x2c(%ebp),%edx; add $0x2,%edx`
        // 只在数组形下复现（独立标量恒直接 lea -0x2a；探针 /tmp/eu_probe8.cpp
        // fA 独立标量/fB 数组对照）。
        short lh[2];
        lh[0] = itCard->second;
        lh[1] = itBooster->second;
        // ORIG 0x080f4af1..0x080f4b17：三次 `movzwl; xor; mov %ax` ⇒ 原码 XOR 交换
        // （tmp 局部产 mov 三连，非 ORIG 形）。
        // ORIG 0x080f4ae4 先装 low（-0x2c）后装 high（-0x2a），`cmp %ax,%dx; jle`
        // ⇒ 跳过条件为 low<=high，写成 `low > high` 才得同装载序/极性。
        if (lh[0] > lh[1])
        {
            lh[0] ^= lh[1];
            lh[1] ^= lh[0];
            lh[0] ^= lh[1];
        }

        std::map<std::pair<int, int>, unsigned long>::iterator itRecipe =
            m_recipeMap.find(std::make_pair(lh[0], lh[1]));
        if (m_recipeMap.end() != itRecipe)
        {
            return itRecipe->second;
        }
    }
    return 0;
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
// [b28d-solo-EventUpgradeJar] N 封顶：ORIG 405 insn / 我方 404 insn，p167 调用点
// 43/43 CLEAR（--demangle），残余为跨版本结构性差异：① 4.4 对本函数**大 body**
// 的段分支反转布局（`jne <body>; jmp 循环头` vs ORIG `je <下一段>; body; jmp 循环头`；
// 小 body 探针 /tmp/eu_probe9.cpp f/g/h 三形均产 ORIG 形 ⇒ 属 4.4 reorder-blocks
// 启发式，非源形可控；__builtin_expect 实测反劣 404→407）；② 槽位分配
// （ORIG cardIdx=-0x28/boosterIdx=-0x24/pairCount=-0x20/i=-0x1c vs 4.4 反向，
// N82/N98 族）。已修项：id==-1 只 break 内层 while（真 bug）、string 声明序、
// id/recipeIndex 头部初始化、ScanType EOF 尾岛（P164）、(n+1)*n 书写序（P243 姊妹）。
bool importEventUpgradeJarScript(EventUpgradeJarInfo* info, const char* path)
{
    // [b28d-solo-EventUpgradeJar] P140/P171：ORIG 0x089687d1 首条即
    // `loadRDARScriptFile("", path)`，无 info/path 空指针防御 ⇒ 删除多余守卫。
    if (!loadRDARScriptFile("", path))
        return false;

    // b28d-lane25：ORIG 0x089687d1 全文重写。要点：
    // ① 比较形 `"字面量" == token`（const char* 在左，P167 差集实证）；
    // ② 全部 ScanInt/ScanFloat 走 bool* 重载 `x = ScanInt(&ok); if (!ok)`；
    // ③ 两个 string 局部（ORIG C1Ev/D1Ev 各 1）；
    // ④ 主循环 for(;;) 各段尾回头部；失败 return false / EOF return true。
    // [b28d-solo-EventUpgradeJar] ORIG 扫描/比较用的是**第二个** string（-0x70，
    // 0x08968841/0x08968857 实证）；第一个 string（-0x6c）全程未被读取 ⇒ 声明序
    // unusedToken 在前、token 在后。id/recipeIndex 在函数头初始化（0x0896881a
    // `movl $0,-0x78`、0x08968821 `movl $0,-0x2c`），循环内 id 再显式重置。
    // [round5 复测] N95 反推：ORIG 全帧槽号比我方**统一深 4**（token -0x6c vs
    // -0x68、ok -0x71 vs -0x6d、id -0x78 vs -0x74、npc -0x80 vs -0x7c），而两侧
    // `sub` 同为 $0x90 ⇒ 差在槽基（4.1 槽基可顶到 ebp / 4.4 恒留 gap），非帧长。
    // 按 P232（4.4-O0 对未使用局部照样占栈）补 4B 死局部试把槽基整体下移 4。
    int slotBasePad;
    std::string unusedToken;   // ORIG 第一 string 局部（0x08968806 C1Ev / -0x6c，未读）
    std::string token;         // ORIG 第二 string 局部（0x08968811 C1Ev / -0x70）
    (void)unusedToken;
    bool ok = false;
    unsigned int id = 0;       // ORIG -0x78
    // [round5 反例] 同法在标量区（id 与 recipeIndex 之间）再补 4B pad 实测**反效**：
    // 深区被推深 4（ok -0x71→-0x75、id -0x78→-0x7c 偏离 ORIG），而 recipeIndex
    // 仍留 -0x28（不动）⇒ 4.4 的标量槽序非声明序可移，已回退（P212/N98 族）。
    int recipeIndex = 0;       // ORIG -0x2c
    // [P212] 全头部声明：ORIG 卡槽 cardIdx/boosterIdx 深于 pairCount/i
    // （-0x28/-0x24 vs -0x20/-0x1c）；块内声明在 4.4 下先声明浅 ⇒ 反序。
    int cardIdx;               // ORIG 无初始化（无 movl $0）⇒ 裸声明
    int boosterIdx;
    ST_Event_Upgrade_Jar_EventNPCs_Script npc;   // ORIG 函数级对象（0x08968828 C1，-0x80）

    for (;;)
    {
        // [b28d-solo-EventUpgradeJar] P164：ORIG 0x08968851 `jne 08968cfd`（尾部
        // EOF 块 `nop; mov $1,%ebx; jmp 清理`）⇒ 用 break + 循环外统一 return true
        // 复现尾部共享块（就地 `return true` 反产 je 跳过形）。
        if (!ScanType(token, true))
            break;

        if ("[group info]" == token)
        {
            // ORIG 0x08968878：先写 m_npcCount 字段，后查 ok。
            info->m_npcCount = (unsigned short)ScanInt(&ok);
            if (!ok)
                return false;

            // 每组连续读入若干 id，以 -1 结束；insert(make_pair(id, g))（ORIG insert）。
            for (unsigned short g = 0; g < info->m_npcCount; ++g)
            {
                // ORIG make_pair 模板参 <unsigned int&, unsigned short&> ⇒ id 为 unsigned。
                id = 0;
                // [b28d-solo-EventUpgradeJar] 真 bug 修正：ORIG 0x089688d8 `je +0x15b`
                // 落 g++（0x0896892c）——id==-1 只 break **内层 while**、继续下一轮 g；
                // 原码 break 出 for 会终止整个 [group info] 扫描。
                while (true)
                {
                    id = ScanInt(&ok);
                    if (!ok)
                        return false;
                    if (id == -1)
                        break;
                    info->m_boosterIndexMap.insert(std::make_pair(id, g));
                }
            }

            // 配方对：`<cardIdx> <boosterIdx> <recipeIndex>`，共 npcCount*(npcCount+1)/2 组。
            // [b28d-solo-EventUpgradeJar] ORIG 0x08968959 先 `lea 0x1(%eax),%edx`
            // （算 n+1）再读 n 入 eax、`imul %edx,%eax` ⇒ 源把 (n+1) 写在左侧。
            int pairCount = (int)((unsigned int)(info->m_npcCount + 1U) * info->m_npcCount) >> 1;
            for (int i = 0; i < pairCount; ++i)
            {
                cardIdx = ScanInt(&ok);
                if (!ok)
                    return false;
                boosterIdx = ScanInt(&ok);
                if (!ok)
                    return false;
                recipeIndex = ScanInt(&ok);
                if (!ok)
                    return false;
                info->m_recipeMap[std::make_pair(cardIdx - 1, boosterIdx - 1)] =
                    (unsigned long)recipeIndex;
            }
            // [b28d-solo-EventUpgradeJar] ORIG 段尾 `jmp 主循环头`（0x08968a48）、
            // 失败比较 `je 下一段` ⇒ 独立 if + continue（else-if 链被 4.4 排成
            // 「jne body; jmp 循环头」反形）。
            continue;
        }
        if ("[fortune index]" == token)
        {
            // ORIG 0x08968a6e..0x08968b60：固定 4 次（直线，无循环、无 -1 终止），
            // 每次 `push_back((unsigned long)ScanInt(&ok))`（右值临时槽）。
            int idx;
            idx = ScanInt(&ok);
            if (!ok)
                return false;
            info->m_materialIndices.push_back((unsigned long)idx);
            idx = ScanInt(&ok);
            if (!ok)
                return false;
            info->m_materialIndices.push_back((unsigned long)idx);
            idx = ScanInt(&ok);
            if (!ok)
                return false;
            info->m_materialIndices.push_back((unsigned long)idx);
            idx = ScanInt(&ok);
            if (!ok)
                return false;
            info->m_materialIndices.push_back((unsigned long)idx);
            continue;
        }
        if ("[fortune fail rate]" == token)
        {
            // ORIG 0x08968b86：逗号条件 while((groupIdx=ScanInt(&ok)), ok)；
            // 第二个值丢弃、第三个为 rate；首值失败仅回主循环（break），
            // 后两值失败 return false。无 -1 终止检查。
            int groupIdx;
            // [round5 复测] ORIG 0x08968b9x 形为 `movzbl ok; xor $0x1,%eax; test
            // %al,%al; je <body>` + fall-through `nop; jmp 主循环头`（bool 取反
            // 物化 + je 直落 body）；`ok == true` 写法在 4.4 恒产 `test; jne body`
            // **无 xor 段**（P168a②/P292 档①）⇒ 改 `if (!ok) break;` 裸取反形。
            for (;;)
            {
                groupIdx = ScanInt(&ok);
                if (!ok)
                    break;
                ScanInt(&ok);                 // 丢弃（ORIG 0x08968ba5 结果不入字段）
                if (!ok)
                    return false;
                int rate;
                rate = ScanInt(&ok);
                if (!ok)
                    return false;
                info->m_failRateByFortune[groupIdx - 1].push_back((unsigned int)rate);
            }
            continue;
        }
        if ("[event upgrade table]" == token)
        {
            // ORIG 0x08968c35：ScanFloat(&ok) 直存 m_discountRate（fstps 序）。
            // [P0] ORIG 无 float 局部槽（`fstps 0x70(%eax)` 直存成员，无
            // `mov -0xN(%ebp),%edx` 中转）⇒ 表达式内联，勿声明具名局部。
            info->m_discountRate = ScanFloat(&ok);
            if (!ok)
                return false;
            for (;;)
            {
                ScanInt(&ok);                 // 丢弃（ORIG 0x08968c5b 结果不入字段）
                npc.m_failRate = (unsigned int)ScanInt(&ok);
                if (!ok)
                    break;                    // 序列终止：回主循环
                npc.m_meterialCount = (unsigned int)ScanInt(&ok);
                if (!ok)
                    return false;
                info->m_npcScripts.push_back(npc);
            }
            continue;
        }
        if ("[upgrade max]" == token)
        {
            // ORIG 0x08968cd7：先写 m_upgradeMax 字段，后查 ok。
            info->m_upgradeMax = ScanInt(&ok);
            if (!ok)
                return false;
        }
    }
    return true;
}

// ORIG 0x08968d59。把 EventUpgradeJarInfo 的 NPC 脚本填进 upgrade_table_t 的 m_infos
// （m_recipeItem=0x28be7b、m_meterialCount=npc.m_meterialCount），并对 m_infos[0..99]
// 的折扣基数应用 m_discountRate。任一指针为空返回 0。
bool updateUpgradeTableFromEventUpgradeJarScript(upgrade_table_t* table, EventUpgradeJarInfo* info)
{
    // [b28d-solo-EventUpgradeJar] P133：ORIG 0x08968d60/0x08968d70 两条独立守卫
    // 各发 `cmpl $0; jne <下一检查>` + fall-through 早退块（合并 `||` 反产 je 跳早退）。
    if (table == 0)
        return false;
    if (info == 0)
        return false;

    for (unsigned int i = 0; i < info->m_npcScripts.size(); ++i)
    {
        if (i < table->m_infos.size())
        {
            // b28d-lane25：P167 差集实证 ORIG 对 m_infos[i] 发两次 operator[]
            // （P48 无 CSE，禁引用缓存）。
            table->m_infos[i].m_recipeItem = 0x28be7b;
            table->m_infos[i].m_meterialCount = (int)info->m_npcScripts[i].m_meterialCount;
        }
    }

    // 对 upgrade_table_t 主体表（m_body）的 100 项应用折扣：
    // 绝对偏移 (i+0x28)*4+4 处为 int 计数值，乘 m_discountRate 后四舍五入回写。
    // （ORIG： *(int*)(param_1 + (i+0x28)*4+4) = round(旧值 * discount)）
    // [b28d-solo-EventUpgradeJar] 用 dword 下标视图：ORIG 0x08968e0b 每轮
    // `mov i,%ecx/edx; add $0x28,%edx; mov 0x4(%eax,%edx,4),%eax` 两次重算
    // （P157 无缓存、P48 双访问）；m_i32 相对 m_body(+0xc) 的等效下标 = i+0x26。
    // N 封顶（P30/N81 族）：ORIG 基址 table+4/索引 i+0x28；我方视图基址只能
    // m_bodyView(+0xc) ⇒ `lea 0x26(%eax),%edx; mov 0xc(%eax,%edx,4)` —— 同一地址
    // 0xa4+4i，仅 4.1/4.4 对「前导成员偏移」的 add/disp 拆分常数不同
    // （探针 /tmp/eu_probe7.cpp fA 复证）。指令数 83=83 全等、调用点 p167 全清。
    // 乘法书写序按 P243：ORIG 先 fildl 旧值（-0x1c）后 flds 折扣率 ⇒ 旧值在左。
    for (unsigned int i = 0; i < 100; ++i)
    {
        // [round1-L19 2026-09-09] 基址 table+4/索引 i+0x28 形已实测**反劣**
        // （83/83 gap=0 → 83/87 gap=-4、ndiff 12→20：4.4 折成
        // `add $0x29; shl $2; lea`，不产出 ORIG 的 `add $0x28;disp 4` 两步形）
        // ⇒ 维持 m_bodyView 视图（P30/N81/N4 封顶，仅前导偏移拆分常数不同）。
        UPG_TBL_I32(table, (int)i + 0x26) =
            (int)((float)UPG_TBL_I32(table, (int)i + 0x26) * info->m_discountRate);
    }

    return true;
}
