// ============================================================================
// df_game_r 还原 —— InitScriptProducers_3.cpp
//
// CDataManager::init（ORIG 0x08358332..0x0835f1d5，报告 init.md 全量转写）
// 依赖的脚本装载自由函数 · 第 3 组（Guild/Power/Quest 参数、称号簿、PvP 频道、
// 旧装备属性、屏蔽字、物品组表）。
//
// 本 TU 实现的符号（均经 df_game_r nm 核实为真实 T 符号；逐指令转写自
// docs/class_func_reports/_lt_global_gt_/ 对应报告，调用点行号引自
// docs/class_func_reports/CDataManager/init.md）：
//   importOldEquipmentStatInfo   _Z26importOldEquipmentStatInfoPKc        0x08a58174（init.md +0x2af）
//   importPVPChannelList         _Z20importPVPChannelListPcP18live_server_info_ti
//                                                                         0x08901f10（init.md +0x1ce1）
//   importGuildParameterScript   _Z26importGuildParameterScriptP20GuildParameterScriptPKc
//                                                                         0x08978d48（init.md +0x1d56）
//   importPowerParameterScript   _Z26importPowerParameterScriptP20PowerParameterScriptPKc
//                                                                         0x08a592bc（init.md +0x1dcb）
//   importTitleBookEtcScript     _Z24importTitleBookEtcScriptPKcP15stTitleBookInfo
//                                                                         0x08aa8110（init.md +0x1f2a）
//   importQuestParameterScript   _Z26importQuestParameterScriptP20QuestParameterScriptPKc
//                                                                         0x08a6b660（init.md +0x207d）
//   importSlangName              _Z15importSlangNamePcP13SlangNameData    0x089081a3（init.md +0x2204）
//   importItmeGroupTable         _Z20importItmeGroupTablePKcS0_RSt3mapISsi..
//                                                                         0x0890bb25（init.md +0x3e40）
//
// 声明唯一出处 DataManagerInitDepends.h（本文件在 CDataManager.h 之后包含之）。
// ============================================================================
// [b28d-solo-InitScriptProducers_3] 本 TU 判词表（verdict = check_tu_routed.sh
// 干净口径 + p167 调用点差集；.o 单函数读数受 N124 假报影响仅作参考）：
//   stTitleSection::C1 0x08aa979e   AE/IDENTICAL —— CTitleBook.h 内联 ctor 按
//     P239 把 6 个标量移入 ctor 体（ORIG「全部 string/vector 构造 → 标量 store」）。
//   importSlangName 0x089081a3      AE/IDENTICAL —— 修法三则：①块级 `bool ok`
//     无预清 + `for(;;){value=ScanInt(&ok); if(ok) push_back((uchar)value); else
//     break;}`（P0 具名 int value 槽 + prvalue 临时槽；具名 ch+static_cast<T&&>
//     恒多物化一条，P270）；②goto 循环复现 cond 前置 + `xor $1; test; jne exit`
//     （P292 bool callee）；③末段 == 形 + P223 尾空标签复现 nop 岛。P167 CLEAR(18/18)。
//   importOldEquipmentStatInfo 0x08a58174 AE/IDENTICAL —— ①find==end 正极性
//     （P220/P222）+ value_type(itemIndex,info) 引用构造（P192）；②指针局部
//     elementTolerance/activeProperty 退役，改 m_view[2][i] / m_view2[3][j]
//     视图寻址（P311/P30，P167 归零）；③itemIndex 预清 + 4 个 dead int 局部
//     补帧 0x10（P31/P232）；④goto 循环复现 cond 前置。P167 CLEAR(38/38)。
//   importPVPChannelList 0x08901f10 AE/IDENTICAL —— goto 循环复现 cond 前置 +
//     `xor $1; test; jne exit`（P292）。P167 CLEAR(35/35)。
//   importTitleBookEtcScript 0x08aa8110 N 封顶 —— 650/650、P167 CLEAR(94/94)；
//     残差 6 处全为 4.1 独占的 switch 叶块间 nop 占位岛（4.4 恒不发射；§6.2
//     三件套：gap=0 / 调用点归零 / 逐块比对无语义差）。
//   importQuestParameterScript 0x08a6b660 N 封顶 —— 447/446（gap 3→1）、
//     P167 CLEAR(66/66)。[b28d-solo3-2 收口] 两处形态修正：
//     ①`[piece reward]` 循环头改**合并短路** `if (!ScanType(sub,true) ||
//     sub=="[/piece reward]") break;` —— ORIG 0x08a6bace 是 `jne→mov $1 /
//     je→mov $0 / test;jne` 物化（探针逐条同构），两个独立 break 恒产两个
//     直跳出口（少 5 条）；
//     ②`found` 具名 bool 改 `while (appendage != -1) { …; break; }`（P225）
//     —— 精确复现 ORIG 0x08a6bb2a `cmpl $0xffffffff; setne; test; je` 且
//     **零槽**，appendage 由 -0x10 归位 -0xc。修法已落：p 形参缓存（-0x20）+
//     各段块级 ok 槽 + isStr 无预清 + memcpy 1 字节 key + dead1c。
//     残余 1 条 = 4.1 独占循环前 `jmp+nop` 岛（O[42:44]）+ 帧差 0x10/槽号平移
//     4（P332/N115）+ [piece reward table]/清理路径块序（difflib 对齐伪差）。
//   importGuildParameterScript 0x08978d48 N 封顶 —— 507/505、P167 CLEAR(76/76)；
//     残差为 4.1 独占 nop 岛 + 两步寻址伪影（N94：`[level-1][1]` 的 0x60 disp
//     4.4 折单条 scaled mov，ORIG 拆 lea+mov，-1 条）。修法已落：7 处
//     `"lit" == token` 方向修正（P167 MISSING/EXTRA ×7 归零）、pair 直构替
//     make_pair ×2、push_back 左值 ×1、p 形参缓存（-0x34）+ dead30、level/cost
//     局部提升、w/key/value 预清；两类型用户空析构删除（P317，.cpp 定义由
//     Main 代落）。[b28d-solo3 二次收口] 删 lv 预清（504）与删 6 处 done 复位
//     （499）均使 gap 反向扩大（ORIG 依赖其形），已回退保持 505。
//   importPowerParameterScript 0x08a592bc N 封顶 —— 638/639、P167 CLEAR(101/101)；
//     [b28d-solo3 二次收口] token 链按 ORIG 汇编顺序重排（noti time/kill count
//     effect range/power up bonus/power war channel/power up point mod/join cost
//     modify/victorious reward time/penalty/break bonus/rejoin panalty 十段归位）
//     + [pc room bonus] 具名局部预清中转（ORIG +0x60a..+0x628）
//     + [straight victory penalty] 改 `while(true){…if(!done)break;}` 四守卫
//     （复现 ORIG 逐处 `movzbl; xor $1; test; je +2; jmp`）+ [break bonus]
//     去 value 预清 —— gap 9→-1（639 vs 638）。唯一残差 = 4.4 各段尾 continue
//     公共回边中继 jmp（+1；ORIG 直跳循环头 +0x56）。
// ============================================================================

#include "CDataManager.h"
#include "sync_script.h"         // CSyncScript/STEquipmentPartSetPiece（须先于 deps 头，与 CDataManager.cpp 口径一致）
#include "DataManagerInitDepends.h"
#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt/ScanFloat/ScanStr/ScanTypeOrStr
#include "LogManager.h"          // LogManager::logFormat

#include <map>
#include <string>
#include <string.h>              // memcpy（ORIG importQuestParameterScript [difficulty] 段）
#include <utility>
#include <vector>

// ---- 外部依赖声明（ORIG T 符号，定义归属各自批次；此处仅引用）----

// _Z13importRewardsPSt6vectorIP18stTitleRewardBasisSaIS1_EESs 0x08aa8b79
int importRewards(std::vector<stTitleRewardBasis*>* out, std::string token);
// _Z19importTitleSectionP14stTitleSectioni 0x08aa8d65（返回 bool）
bool importTitleSection(stTitleSection* section, int idx);
// _Z18getTitleCollectionP15stTitleBookInfoi 0x08aa94e1
std::vector<stTitleElement>* getTitleCollection(stTitleBookInfo* book, int tab);
// _Z26convertTabTypeStringToEnumPKc 0x08aa7d98
int convertTabTypeStringToEnum(const char* text);
// _Z17loadAppendageTypeRKSs 0x0889253b / _Z13loadValueTypeRKSs 0x08892ac4
int loadAppendageType(const std::string& token);
int loadValueType(const std::string& token);

// g_equipmentStatInfoMap —— ORIG 未修饰名 B 0x095000a0（RDARScriptOldEquip 域
// 全局，本 TU 为其唯一定义点；消费方经同名 extern "C" 引用）。
// 类型证据：报告 _lt_global_gt_/__static_initialization_and_destruction_0_
// 08a588b8.md —— ORIG 静态初始化为 map<unsigned int, STEquipmentStatInfo>
// 默认构造并 __cxa_atexit 注册析构（本定义的动态初始化与之等价）。
extern "C"
{
std::map<unsigned int, STEquipmentStatInfo> g_equipmentStatInfoMap;
}

// ============================================================================
// importOldEquipmentStatInfo —— ORIG 0x08a58174（../RDARScriptOldEquip.cpp）
// 报告 _lt_global_gt_/importOldEquipmentStatInfo.md。
// 逐条读取 itemIndex + STEquipmentStatInfo 全部 26 个 int 字段；任一缺失即
// logFormat 并返回 false；重复 index 打印 Redundant 后跳过插入；
// ScanInt 收尾失败（正常 EOF）返回 true。
// ============================================================================
bool importOldEquipmentStatInfo(const char* dir)
{
    if (!loadRDARScriptFile(dir, "equipment/OldEquipmentStatInfoList.dat"))
    {
        LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                              __FUNCTION__, 0x12,
                              "CRITICAL ERROR : old equipment stat info load failed!");
        return false;
    }

    unsigned int itemIndex = 0;
    bool ok;                       // ORIG 无预清（ScanInt 首写）
    // ORIG 帧头 `movl $0,-0x20/-0x1c/-0x18/-0x14`：四个 dead int 局部
    // （仅初始化、全程不读）——P31/P232 复现 ORIG sub $0x194 帧（我们少 0x10）。
    int dead0 = 0;
    int dead1 = 0;
    int dead2 = 0;
    int dead3 = 0;

    // ORIG 0x08a581f4：cond 前置（`call ScanInt; mov %eax,itemIndex;
    // movzbl ok; xor $1; test; jne exit`）+ body 后置 + 尾部 `jmp` 回边。
eq_loop:
    if (!(itemIndex = ScanInt(&ok), ok))
        goto eq_end;
    {
        STEquipmentStatInfo info;
        info.m_hpMax = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x29,
                                  "Invalid Old Equipment Stat Info(missing hpMax)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_mpMax = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x30,
                                  "Invalid Old Equipment Stat Info(missing mpMax)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_phyAtt = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x37,
                                  "Invalid Old Equipment Stat Info(missing physical attack)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_phyDef = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x3d,
                                  "Invalid Old Equipment Stat Info(missing physical defense)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_magAtt = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x43,
                                  "Invalid Old Equipment Stat Info(missing magical attack)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_magDef = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x49,
                                  "Invalid Old Equipment Stat Info(missing magical defense)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_equipPhyAtt = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x50,
                                  "Invalid Old Equipment Stat Info(missing equipment physical attack)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_equipPhyDef = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x57,
                                  "Invalid Old Equipment Stat Info(missing equipment physical defense)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_equipMagAtt = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x5e,
                                  "Invalid Old Equipment Stat Info(missing magical attack)! itemIndex = %d", itemIndex);
            return false;
        }
        info.m_equipMagDef = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x65,
                                  "Invalid Old Equipment Stat Info(missing magical defense)! itemIndex = %d", itemIndex);
            return false;
        }
        // 元素耐性 4 项（+0x28..+0x34）：ORIG 以 base+(i+8)*4 寻址
        // （m_view 基址 +0x08、索引 8+i；P311 成员数组视图）。
        for (int i = 0; i < 4; ++i)
        {
            info.m_view[2][i] = ScanInt(&ok);
            if (!ok)
            {
                LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                      __FUNCTION__, 0x6e,
                                      "Invalid Old Equipment Stat Info(missing element tolerance %d)! itemIndex = %d", i, itemIndex);
                return false;
            }
        }
        info.m_refAll = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x76,
                                  "Invalid Old Equipment Stat Info(missing all element tolerance)! itemIndex = %d", itemIndex);
            return false;
        }
        // 状态异常耐性 17 项（+0x3c..+0x7c）：ORIG 以 base+(j+12)*4 寻址
        // （m_view2 基址 +0x0c、索引 12+j；j>3 越界照抄 ORIG = P171）。
        for (int j = 0; j < 0x11; ++j)
        {
            info.m_view2[3][j] = ScanInt(&ok);
            if (!ok)
            {
                LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                      __FUNCTION__, 0x7f,
                                      "Invalid Old Equipment Stat Info(missing active property %d)! itemIndex = %d", j, itemIndex);
                return false;
            }
        }
        info.m_refAllState = ScanInt(&ok);
        if (!ok)
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x87,
                                  "Invalid Old Equipment Stat Info(missing all activeStatus tolerance)! itemIndex = %d", itemIndex);
            return false;
        }

        // b28d-55（P220/P222 + P167 差集实证）：ORIG 用**正极性** `find == end`
        // （差集：ORIG 有 _Rb_tree_iterator::eq 而无 ne）+ 插入用
        // value_type(itemIndex, info) 引用构造（pair C1IRjRS1_），make_pair 形是
        // 我们多出的调用（P192/P178）。
        if (g_equipmentStatInfoMap.find(itemIndex) == g_equipmentStatInfoMap.end())
        {
            g_equipmentStatInfoMap.insert(
                std::map<unsigned int, STEquipmentStatInfo>::value_type(itemIndex, info));
        }
        else
        {
            LogManager::logFormat(0, "../RDARScriptOldEquip.cpp",
                                  __FUNCTION__, 0x92,
                                  "Redundant Old Equipment Stat Info! itemIndex = %d", itemIndex);
        }
    }
    goto eq_loop;

eq_end:
    return true;
}

// ============================================================================
// importPVPChannelList —— ORIG 0x08901f10
// 报告 _lt_global_gt_/importPVPChannelList.md；调用点 init.md +0x1cce：
//   importPVPChannelList("Etc/pvpChannelList.etc", this+0xb4ec, 0)。
// [PVP CHANNEL INFO] 段：grade=ScanInt、count=ScanInt；每条 ScanStr(名字) +
// pvp_channel_info_t 的 9 个 int（扫描序→偏移见 CDataManager.h 注释）+
// 尾随 ScanStr；完成后把 map[grade]=vector insert 进 live_server_info_t 的
// m_pvpChannelMap。第三形参 flag 在 ORIG 函数体内未使用。
// 任一 channel 字段缺失 → 清理后返回 false。
// ============================================================================
bool importPVPChannelList(char* path, live_server_info_t* info, int flag)
{
    (void)flag;
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    bool ok = false;

    // ORIG 0x08901f10：cond 前置（`if (!ScanType) goto end` ⇒
    // `call; xor $1; test; jne exit`，P292 bool callee）、body 后置、尾部
    // `jmp` 回边。4.4 对 while 恒旋转为「body 前置 + cond 尾部 `test; je`」。
pvp_loop:
    if (!ScanType(token, true))
        goto pvp_end;
    {
        if (token == "[PVP CHANNEL INFO]")
        {
            while (true)
            {
                int grade;
                if (!(grade = ScanInt(&ok), ok))
                    break;
                int count;
                if (!(count = ScanInt(&ok), ok))
                    break;

                std::vector<pvp_channel_info_t> channels;
                std::string name;

                for (int i = 0; i < count; ++i)
                {
                    ScanStr(&name);

                    pvp_channel_info_t channel;
                    channel.m_field0 = ScanInt(&ok);           // 扫描序 1 → +0x00
                    if (!ok)
                        return false;
                    channel.m_minGrade = ScanInt(&ok);          // 扫描序 2 → +0x04
                    if (!ok)
                        return false;
                    channel.m_maxGrade = ScanInt(&ok);         // 扫描序 3 → +0x08
                    if (!ok)
                        return false;
                    channel.m_channelType = ScanInt(&ok);      // 扫描序 4 → +0x10
                    if (!ok)
                        return false;
                    channel.m_upgradeRevision = ScanInt(&ok);  // 扫描序 5 → +0x0c
                    if (!ok)
                        return false;
                    channel.m_minLevel = ScanInt(&ok);         // 扫描序 6 → +0x14
                    if (!ok)
                        return false;
                    channel.m_maxLevel = ScanInt(&ok);        // 扫描序 7 → +0x18
                    if (!ok)
                        return false;
                    channel.m_field1c = ScanInt(&ok);          // 扫描序 8 → +0x1c
                    if (!ok)
                        return false;
                    channel.m_field20 = ScanInt(&ok);          // 扫描序 9 → +0x20
                    if (!ok)
                        return false;

                    channels.push_back(channel);
                    ScanStr(&name);
                }

                info->m_pvpChannelMap.insert(std::make_pair(grade, channels));
            }
        }
    }
    goto pvp_loop;

pvp_end:
    return true;
}

// ============================================================================
// importGuildParameterScript —— ORIG 0x08978d48
// 报告 _lt_global_gt_/importGuildParameterScript.md；调用点 init.md +0x1d53：
//   importGuildParameterScript(this+0xa50c, "Etc/Guild.etc")。
// 返回 bool：仅当读到 [guild parameter end] 为 true；加载失败 / ScanType 失败 /
// [guild exp weight] 遇 m_field8==0 均返回 false（ORIG 一致）。
// ============================================================================
// [b28d-solo-InitScriptProducers_3 N 封顶] 507/505、P167 CLEAR(76/76)；残差 =
// 4.1 nop 占位岛 + 两步寻址伪影（详见文件头判词表）。
bool importGuildParameterScript(GuildParameterScript* script, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    // ORIG 0x08978d77：`mov 0x8(%ebp),%eax; mov %eax,-0x34(%ebp)` —— 形参缓存
    // 进具名局部 p（P0），全部访问走 p->。
    GuildParameterScript* p = script;
    p->clear();

    std::string token;
    int dead30 = 0;                // ORIG -0x30 `movl $0`（dead 局部，P31/P232）
    bool done = false;

    while (true)
    {
        while (true)
        {
            while (true)
            {
                while (true)
                {
                    while (true)
                    {
                        if (!ScanType(token, true))
                            goto scan_end;

                        if (token == "[guild create cost]")
                        {
                            p->m_field0 = ScanInt((bool*)0);
                            continue;
                        }
                        break;
                    }
                    if (token == "[guild levelup cost]")
                    {
                        // 最多 17 行：level cost1 cost2；level>0x11 放弃余下行。
                        // ORIG 写序（asm +0x107/+0x126）：cost1→[level-1][1]、
                        // cost2→[level-1][0]，m_levelUpCost 首址 +0x5c。
                        int level = 0;      // ORIG -0x2c：循环外声明 + 预清
                        for (int count = 0; count < 0x11; ++count)
                        {
                            level = ScanInt((bool*)0);
                            if (level > 0x11)
                                break;
                            // ORIG 0x08978e3a..：ScanInt 结果直存数组（无 cost 具名局部）。
                            p->m_levelUpCost[level - 1][1] = ScanInt((bool*)0);
                            p->m_levelUpCost[level - 1][0] = ScanInt((bool*)0);
                        }
                        continue;
                    }
                    break;
                }
                if (token == "[guild point acquirement ratio]")
                {
                    p->m_field4 = ScanInt((bool*)0);
                    continue;
                }
                break;
            }
            if (token == "[guild exp prob]")
            {
                p->m_field8 = static_cast<char>(ScanInt((bool*)0));
                continue;
            }
            break;
        }

        if (token == "[guild exp weight]")
        {
            // asm +0x1c7：prob==0 时直接以 false 结束整个函数。
            if (p->m_field8 == 0)
                goto scan_end;
            // ORIG 0x08978e??：`addl $1,-0x18(%ebp)` + `movsbl` ⇒ 循环变量是
            // int（char 变量恒产 addb，与 ORIG 不符）。
            // ORIG 0x08978ffa 前 `movl $0,-0x24/-0x20/-0x8c/-0x1c`：rangeMin/
            // rangeMax/ratio/lv 均在 w 循环外声明并预清 0，循环内再赋值。
            int rangeMin = 0;
            int rangeMax = 0;
            int ratio = 0;
            int lv = 0;            // 保留预清：实测 505 vs 504（ORIG 507）不回退
            for (int w = 0; w < p->m_field8; ++w)
            {
                rangeMin = ScanInt((bool*)0);
                rangeMax = ScanInt((bool*)0);
                ratio = ScanInt((bool*)0);
                // asm +0x24d..+0x295：insert(make_pair(lv,ratio))，
                // .second==false 时终止内层循环。
                for (lv = rangeMin; lv <= rangeMax; ++lv)
                {
                    if (!p->m_map44.insert(std::make_pair(lv, ratio)).second)
                        break;
                }
            }
            continue;
        }

        if (token == "[guild mail]")
        {
            p->m_mail = ScanInt((bool*)0);
            continue;
        }
        if (token == "[guild level demand for agit]")
        {
            p->m_fieldC = ScanInt((bool*)0);
            continue;
        }
        if (token == "[guild exp book]")
        {
            // 固定 3 对 key/value 插入 m_map10（asm +0x36f..+0x3f3）。
            // ORIG 0x08979132 前 `movl $0,-0x94/-0x98`：key/value 循环外预清。
            int key = 0;
            int value = 0;
            for (int i = 0; i < 3; ++i)
            {
                key = ScanInt((bool*)0);
                value = ScanInt((bool*)0);
                p->m_map10.insert(std::make_pair(key, value));
            }
            continue;
        }
        if ("[guild level demand for buying guild exp book]" == token)
        {
            p->m_field28 = ScanInt((bool*)0);
            continue;
        }
        if ("[agit upgrade info]" == token)
        {
            STAgitUpgradeInfo entry;
            for (int i = 0; i < 5; ++i)
            {
                entry.m_field0 = ScanInt((bool*)0);
                entry.m_field4 = ScanInt((bool*)0);
                entry.m_field8 = ScanInt((bool*)0);
                entry.m_fieldC = ScanInt((bool*)0);
                p->m_vec2c.push_back(entry);
            }
            continue;
        }
        if ("[guild storage upgrade info]" == token)
        {
            STGuildStorageUpgradeInfo entry;
            while ((entry.m_field0 = ScanInt(&done), done))
            {
                done = false;
                entry.m_field4 = ScanInt((bool*)0);
                entry.m_field8 = ScanInt((bool*)0);
                entry.m_fieldC = ScanInt((bool*)0);
                entry.m_field10 = ScanInt((bool*)0);
                p->m_vec38.push_back(entry);
            }
            done = false;
            continue;
        }
        if ("[guild exp member weight]" == token)
        {
            int memberCount;
            double weight;
            while (((memberCount = ScanInt(&done)), done))
            {
                done = false;
                weight = ScanFloat((bool*)0);
                // ORIG 0x08979363：`pair<int,double>::pair<int&,double&>` 直构
                // （C1IRiRdEEOT_OT0_）而非 make_pair（P167 差集 EXTRA make_pair）。
                p->m_vecE4.push_back(std::pair<int, double>(memberCount, weight));
            }
            done = false;
            continue;
        }
        if ("[guild levelup price]" == token)
        {
            int guildLevel;
            while (((guildLevel = ScanInt(&done)), done))
            {
                done = false;
                std::vector<int> prices;
                prices.reserve(0x11);
                for (int j = 0; j < 0x11; ++j)
                {
                    // ORIG 0x0897942e：push_back(const int&) —— 实参是具名左值
                    // 局部（先 `mov %eax,-0xd0(%ebp)` 再取址），非右值 prvalue。
                    int price = ScanInt((bool*)0);
                    prices.push_back(price);
                }
                // ORIG 0x089793??：pair<int,vector<int>>::pair<int&,vector<int>&> 直构。
                p->m_vecF0.push_back(
                    std::pair<int, std::vector<int> >(guildLevel, prices));
            }
            done = false;
            continue;
        }
        if ("[today guild member weight]" == token)
        {
            p->m_fieldFC = ScanFloat((bool*)0);
            continue;
        }
        if ("[guild fund condition]" == token)
        {
            p->m_field104 = ScanInt((bool*)0);
            p->m_field108 = ScanInt((bool*)0);
            p->m_field10c = ScanInt((bool*)0);
            p->m_field110 = ScanFloat((bool*)0);
            continue;
        }
        if (token == "[guild parameter end]")
        {
            done = true;
            break;
        }
    }

scan_end:
    return done;
}

// ============================================================================
// importPowerParameterScript —— ORIG 0x08a592bc
// 报告 _lt_global_gt_/importPowerParameterScript.md；调用点 init.md +0x1dc8：
//   importPowerParameterScript(this+0xa628, "Etc/Power.etc")。
// 返回 bool：仅当读到 [power parameter end] 为 true；加载失败 / ScanType 失败 /
// [power war channel] 第二值缺失 返回 false（ORIG 一致）。
// ============================================================================
// [b28d-solo-InitScriptProducers_3 N 封顶] 638/629、P167 CLEAR(101/101)；残差 =
// 4.1/4.4 break 块序（N122/P168a）+ nop 岛（详见文件头判词表）。
// [2026-09-10 D06 · N] 残差 = 我方多 1 条**不可达中继** `jmp <while_head>`（位于
//   `jmp <return done>` 之后的死区，ORIG 该处为 `nop`）⇒ 4.4 的循环出口中继
//   （A28 边界）；实测 `while(true)`↔`for(;;)` 同值（631 vs 630），`break`↔`goto
//   scan_end` 同值 ⇒ 源码层不可控，封顶。
bool importPowerParameterScript(PowerParameterScript* script, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    // ORIG 0x08a592bc：`mov 0x8(%ebp),%eax; mov %eax,-0x14(%ebp)` —— 形参缓存
    // 进具名局部 p（P0），全部访问走 p->。
    PowerParameterScript* p = script;
    p->clear();

    std::string token;
    int dead10 = 0;                // ORIG -0x10 `movl $0`（dead 局部，P31/P232）
    bool done = false;

    while (true)
    {
        if (!ScanType(token, true))
            goto scan_end;

        if (token == "[power join cost]")                      { p->m_powerJoinCost = ScanInt((bool*)0); continue; }
        if (token == "[power join guild level]")               { p->m_powerJoinGuildLevel = ScanInt((bool*)0); continue; }
        if (token == "[power join quest]")                     { p->m_powerJoinQuest = ScanInt((bool*)0); continue; }
        if (token == "[kill point to end power war]")          { p->m_killPointToEndPowerWar = ScanInt((bool*)0); continue; }
        if (token == "[exp reward]")                           { p->m_expReward = ScanInt((bool*)0); continue; }
        if (token == "[easy assault range]")                   { p->m_easyAssaultRange = ScanInt((bool*)0); continue; }
        if (token == "[ghost revival base time]")              { p->m_ghostTime = ScanInt((bool*)0); continue; }
        if (token == "[respon penalty time]")                  { p->m_penaltyThreshold = ScanInt((bool*)0); continue; }
        if (token == "[powerwar point multiple number]")       { p->m_powerWarPointMultiple = ScanInt((bool*)0); continue; }
        if (token == "[refresh power war process info]")       { p->m_refreshProcessInfo = ScanInt((bool*)0); continue; }
        if (token == "[abuser ban time]")                      { p->m_abuserBanTime = ScanInt((bool*)0); continue; }
        if (token == "[power up ratio]")                       { p->m_powerUpRatio = ScanInt((bool*)0); continue; }
        if (token == "[power up assault count]")               { p->m_powerUpAssaultCount = ScanInt((bool*)0); continue; }
        if (token == "[power up assault victory ratio]")       { p->m_powerUpAssaultVictoryRatio = ScanInt((bool*)0); continue; }
        if (token == "[power up generate period]")             { p->m_powerUpGeneratePeriod = ScanInt((bool*)0); continue; }
        // ORIG 0x08a595db：`movl $0,-0x28` 预清 + cond 前置
        // （`call ScanInt; movzbl done; xor $1; je break`）+ body 后置。
        if (token == "[power up noti time]")
        {
            int value = 0;
            while (true)
            {
                value = ScanInt(&done);
                if (!done)
                    break;
                p->m_vec20.push_back(value);
            }
            continue;
        }
        if (token == "[straight victories]")                   { p->m_straightVictories = ScanInt((bool*)0); continue; }
        if (token == "[item initial cooltime]")                { p->m_itemInitialCooltime = ScanInt((bool*)0); continue; }
        // asm +0x3e6..+0x434：首值带 ok 校验，次值不校验。
        if (token == "[kill count effect range]")
        {
            while (true)
            {
                std::pair<int, int> range;
                range.first = ScanInt(&done);
                if (!done)
                    break;
                range.second = ScanInt((bool*)0);
                p->m_vec70.push_back(range);
            }
            continue;
        }
        if (token == "[basic powerwar point]")                 { p->m_basicPowerWarPoint = ScanInt((bool*)0); continue; }
        if (token == "[first grade bonus powerwar point]")     { p->m_firstGradeBonusPoint = ScanInt((bool*)0); continue; }
        if (token == "[ranking bonus grade]")                  { p->m_rankingBonusGrade = ScanInt((bool*)0); continue; }
        if (token == "[ranking bonus decrease point]")         { p->m_rankingBonusDecreasePoint = ScanInt((bool*)0); continue; }
        if (token == "[decrease powerwar point per day]")      { p->m_decreasePointPerDay = ScanInt((bool*)0); continue; }
        if (token == "[required user powerwar point 4 dungeon]") { p->m_requiredUserPoint4Dungeon = ScanInt((bool*)0); continue; }
        if (token == "[power war dungeon info]")
        {
            p->m_powerWarDungeonInfo.first = ScanInt((bool*)0);   // ORIG +0x94
            p->m_powerWarDungeonInfo.second = ScanInt((bool*)0); // ORIG +0x98
            continue;
        }
        if (token == "[power up bonus]")
        {
            int value = 0;
            while (true)
            {
                value = ScanInt(&done);
                if (!done)
                    break;
                p->m_vec38.push_back(value);
            }
            continue;
        }
        if (token == "[pc room bonus]")
        {
            // asm +0x60a..+0x628：`movl $0,-0xc` 预清 + 具名局部中转，
            // ok 标志被忽略。
            int value = 0;
            value = ScanInt(&done);
            p->m_pcRoomBonus = value;
            continue;
        }
        if (token == "[time assault super power state on join powerwar]")
        {
            p->m_timeAssaultSuperPowerStateOnJoin = ScanInt((bool*)0);
            continue;
        }
        if (token == "[time assault super power state after powerwar]")
        {
            p->m_timeAssaultSuperPowerStateAfter = ScanInt((bool*)0);
            continue;
        }
        if (token == "[powerwar enter level limit]")           { p->m_minEnterLevel = ScanInt((bool*)0); continue; }
        if (token == "[powerwar enter age limit]")             { p->m_minEnterAge = ScanInt((bool*)0); continue; }
        if (token == "[power war channel]")
        {
            while (true)
            {
                int channel;
                if (!(channel = ScanInt(&done), done))
                    break;
                int serverGroup = ScanInt(&done);
                if (!done)
                    return false;                    // asm +0x739：第二值缺失即整体失败
                p->m_powerWarServerList.push_back(
                    std::make_pair(channel, serverGroup));
            }
            continue;
        }
        if (token == "[power up point mod]")                   { p->m_powerUpPointMod = ScanInt((bool*)0); continue; }
        if (token == "[power join cost modify]")
        {
            int value = 0;
            while (true)
            {
                value = ScanInt(&done);
                if (!done)
                    break;
                p->m_vecAC.push_back(value);
            }
            continue;
        }
        if (token == "[victorious reward time]")               { p->m_victoriousRewardTime = ScanInt((bool*)0); continue; }
        if (token == "[straight victory penalty]")
        {
            stVictoryPenalty penalty;
            while (true)
            {
                penalty.m_field0 = ScanInt(&done);
                if (!done)
                    break;
                penalty.m_field4 = ScanInt(&done);
                if (!done)
                    break;
                penalty.m_field8 = ScanInt(&done);
                if (!done)
                    break;
                penalty.m_fieldC = ScanInt(&done);
                if (!done)
                    break;
                p->m_vecBC.push_back(penalty);
            }
            continue;
        }
        if (token == "[straight victory break bonus]")
        {
            int value;                             // ORIG 无预清（直接 store）
            while (true)
            {
                value = ScanInt(&done);
                if (!done)
                    break;
                p->m_vecC8.push_back(value);
            }
            continue;
        }
        if (token == "[power rejoin panalty]")                 { p->m_powerWarCooldownMinutes = ScanInt((bool*)0); continue; }
        if (token == "[power parameter end]")
        {
            done = true;
            // [2026-09-10 D06] ORIG 该处只有**一条** `jmp <return done>`；`break`
            // 跳出 while 会多一条（4.4 的 while 出口 + epilogue 跳板），改单 goto。
            goto scan_end;
        }
    }

scan_end:
    return done;
}

// ============================================================================
// importQuestParameterScript —— ORIG 0x08a6b660
// 报告 _lt_global_gt_/importQuestParameterScript.md；调用点 init.md +0x207a：
//   importQuestParameterScript(this+0x8cc8, "n_Quest/questParameter.etc")。
// [difficulty] 用 ScanTypeOrStr：类型令牌取 sub[0] 为键、ScanInt 为值插入
// m_map0；字符串令牌仅识别 [/difficulty] 终止符。[piece reward] 中首个
// ScanInt 失败即整体返回 false（asm +0x445）。
// ============================================================================
// [b28d-solo3-2 收口] 447/446、P167 CLEAR(66/66)、gap 3→1；两处形态修正
// （合并短路 break / while(appendage!=-1){…break;} 零槽 setne）见文件头判词表。
bool importQuestParameterScript(QuestParameterScript* script, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    // ORIG 0x08a6b660：`mov 0x8(%ebp),%eax; mov %eax,-0x20(%ebp)` —— 形参缓存
    // 进具名局部 p（P0：该槽后续 12 处重读），全部访问走 p->。
    QuestParameterScript* p = script;
    p->clear();

    std::string token;
    int dead1c = 0;                // ORIG -0x1c `movl $0`（dead 局部，P31/P232）
    bool done = false;

    while (true)
    {
        if (!ScanType(token, true))
            goto scan_end;

        if (token == "[difficulty]")
        {
            std::string sub;
            while (true)
            {
                bool isStr;                // ORIG 无预清（ScanTypeOrStr 首写）
                if (!ScanTypeOrStr(isStr, sub, true))
                    break;
                if (isStr)
                {
                    if (sub == "[/difficulty]")
                        break;
                    continue;
                }
                int value = ScanInt((bool*)0);
                // ORIG 0x08a6b7xx：`call c_str` 后 `movl $1,0x8(%esp)` +
                // `call memcpy@plt`（1 字节拷贝到 key 槽）⇒ 原码是显式
                // `memcpy(&key, sub.c_str(), 1)`，不是 `sub.c_str()[0]`
                //（P167 差集：ORIG-only memcpy ×1）。
                char key;
                memcpy(&key, sub.c_str(), 1);
                p->m_map0.insert(std::make_pair(key, value));
            }
            continue;
        }
        if (token == "[green level penalty]")   { p->m_field18 = ScanInt((bool*)0); continue; }
        if (token == "[grey level penalty]")    { p->m_field1c = ScanInt((bool*)0); continue; }
        if (token == "[bal kun penalty]")       { p->m_field20 = ScanInt((bool*)0); continue; }
        if (token == "[exp reward table]")
        {
            p->m_vec24.reserve(200);
            bool ok;                   // ORIG 块级 ok 槽 -0x87（各段独立，不复用）
            int value;
            for (int i = 0; i < 200; ++i)
            {
                if (!(value = ScanInt(&ok), ok))
                    break;
                p->m_vec24.push_back(value);
            }
            continue;
        }
        if (token == "[gold reward table]")
        {
            p->m_vec30.reserve(200);
            bool ok;                   // ORIG 块级 ok 槽 -0x8d
            int value;
            for (int i = 0; i < 200; ++i)
            {
                if (!(value = ScanInt(&ok), ok))
                    break;
                p->m_vec30.push_back(value);
            }
            continue;
        }
        if (token == "[piece reward table]")
        {
            bool ok;                   // ORIG 块级 ok 槽 -0x95
            while (true)
            {
                int key = ScanInt(&ok);
                if (!ok)
                    break;
                int value = ScanInt(&ok);
                if (!ok)
                    break;
                p->m_map3c.insert(std::make_pair(key, value));
            }
            continue;
        }
        if (token == "[piece reward]")
        {
            bool ok;                   // ORIG 块级 ok 槽 -0xa1
            int questIdx = ScanInt(&ok);
            if (!ok)
                return false;

            std::vector<pieceQuestRewardStatus> statuses;
            std::string sub;
            while (true)
            {
                // ORIG 0x08a6bace..0x08a6bb13：合并短路 `||` + bool 物化
                // （jne→mov $1 / je→mov $0 / test; jne break）。探针实测
                // `if (!ScanType(...) || sub == "[/piece reward]") break;`
                // 逐条复现；独立 break 恒产两个直跳出口，少 5 条。
                if (!ScanType(sub, true) || sub == "[/piece reward]")
                    break;

                int appendage = loadAppendageType(sub);
                // ORIG 0x08a6bb2a：`cmpl $0xffffffff,-0xc(%ebp); setne %al;
                // test %al,%al; je <循环头>` —— P225 实证 `while(cond){body;break;}`
                // 精确复现该 setne/test/je 且**零槽**（具名 bool 恒落槽 -0x9，把
                // appendage 由 -0xc 推到 -0x10、整帧 +0x10）。
                while (appendage != -1)
                {
                    pieceQuestRewardStatus status;
                    status.m_appendageType = appendage;
                    std::string typeText;
                    ScanStr(&typeText);
                    status.m_valueType = loadValueType(typeText);
                    status.m_value = ScanFloat((bool*)0);
                    statuses.push_back(status);
                    break;
                }
            }
            p->m_map54.insert(std::make_pair(questIdx, statuses));
            continue;
        }
        if (token == "[quest parameter end]")
        {
            done = true;
            break;
        }
    }

scan_end:
    return done;
}

// ============================================================================
// importTitleBookEtcScript —— ORIG 0x08aa8110
// 报告 _lt_global_gt_/importTitleBookEtcScript.md；调用点 init.md +0x1f23：
//   importTitleBookEtcScript("Etc/TitleBook.etc", this+0xa788)。
// 双遍结构（ORIG __lookup/__init_lock/__inited 三个函数级静态）：
//   首令牌以 id=0 进入 switch，case 0 与首个 case 0x39 合并，之后逐 case
//   **贯穿**登记 6 个 (键,id)（0x39/0x4d/0x7f/0xc6/0xe4/0xf4），落到 default
//   后释放锁、置位 __inited 并 continue，以当前令牌重新查表派发——
//   因此首令牌同样会被正常处理。
// ============================================================================
// [b28d-solo-InitScriptProducers_3 N 封顶] 650/650、P167 CLEAR(94/94)；残差 =
// 4.1 switch 叶块 nop 占位岛 ×6（4.4 恒不发射；详见文件头判词表）。
// [2026-09-10 D06 · N 封顶复核] 本轮三形实测（读 `fnx.py` 归一化口径）：
//  ① 原形（内层 `for(;;)` + `continue`）：643 vs ORIG 642（我方多 1 条 jmp）；
//  ② 内层+外层全 goto 形（`titlebook_retry:`/`outer_retry:`）：642 = 642（gap 0）
//     但残差变为「入口 `jmp <outer_head>`（我方多）↔ 置位处 `jmp +0x61`（我方少）」
//     两处位置互换；③ 单 goto 出口形：同 ②。⇒ 属 4.4 循环头/出口块布局（P223/A28
//     epilogue-threading 族），源码层不可分裂；保留 ② 形（gap 0 优于 ① 的 +1）。
bool importTitleBookEtcScript(const char* path, stTitleBookInfo* out)
{
    if (!loadRDARScriptFile("", path))
        return false;

    // ORIG 每条键注册只有两次 call：pair<const char* const,int>::pair<char const(&)[N],int>
    // （即 map::value_type 就地构造，符号 _ZNSt4pairIKPKciEC1IRA<N>_S0_iEEOT_OT0_）
    // 再 map::insert(value_type const&)。用 make_pair 会多出
    // pair<const char*,int> → value_type 的转换临时（第三次 call），与 ORIG 不符。
    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;
    std::string tok2;   // ORIG [ebp-0xdc]：构造/析构却从未使用的空串槽
    std::string tok3;
    bool ok = false;

    // ORIG +0x61 处条件在循环顶部（call ScanType; xor eax,1; test; jne +0x9d8），
    // 退出标号 +0x9d8 是一个 nop（break 出口）后接 return true。
    // [2026-09-10 D06] 外层同样改 goto 形（P164/P283）：`for (;;)` 会让 4.4 在
    // 循环头前多插一条入口 `jmp <head>; nop`（实测 241b `jmp 241e`），ORIG 是
    // `movb $0x0,-0xe1` 后**顺序流入** +0x61。
outer_retry:;
    if (!ScanType(token, true))
        goto outer_done;

        // ORIG 的重派发跳回 +0x82（__lookup 的 __cxa_guard 检查），
        // 说明三个函数级静态声明位于外层 token 循环体内部。
        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;   // ORIG !__inited 用 xor eax,1 → bool

        // ORIG 重派发是一个内层循环：continue 回到 +0x82（__lookup guard）。
        // [2026-09-10 D06] 内层循环用 goto 形（P164/P283）：`for (;;)` 会让 4.4 在
        // 循环头前多插一条入口 `jmp`（实测 241b `jmp 241e`），goto 形则顺序流入 guard。
    titlebook_retry:;                       // ORIG +0x82（__lookup guard 头）
        {
            int id;
            if (__inited)
            {
                std::map<const char*, int, less_TCHAR>::const_iterator it =
                    __lookup.find(token.c_str());
                id = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    goto titlebook_retry;
                }
                id = 0;
            }

            // ORIG +0x26e 起是 cmp/je 判定树：case 0（未初始化态）与首个
            // case 0x39 合并；每个 case 在 !__inited 时登记一条键值后**贯穿**
            // 到下一 case 继续登记，最终落到 default（=switch 末尾 +0x9ac）；
            // 已初始化态执行段体后 break 退出 switch。
            switch (id)
            {
            case 0x39:                           // [default reward]
            case 0:
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[default reward]", 0x39));
                }
                else
                {
                    while (true)
                    {
                        if (!ScanType(tok3, true))
                            return true;
                        if (tok3 == "[/default reward]")
                            break;
                        importRewards(&out->m_vec48, tok3);
                    }
                    break;
                }
            }
            /* fall through */
            case 0x4d:                           // [title collection info]
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[title collection info]", 0x4d));
                }
                else
                {
                    ScanStr(&tok3);
                    int tab = convertTabTypeStringToEnum(tok3.c_str());
                    int count = ScanInt((bool*)0);
                    for (int i = 0; i < count; ++i)
                    {
                        ScanInt((bool*)0);              // ORIG 丢弃值（asm +0x468）
                        stTitleElement element;
                        element.m_index = ScanInt((bool*)0);
                        if (element.m_index != -1)
                        {
                            element.m_achievementIdx = ScanInt((bool*)0);
                            int jobCount = ScanInt((bool*)0);
                            for (int j = 0; j < jobCount; ++j)
                                element.m_fitJobs.push_back(ScanInt((bool*)0));
                        }
                        out->m_map18[tab].push_back(element);
                    }
                    break;
                }
            }
            /* fall through */
            case 0x7f:                           // [section]
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[section]", 0x7f));
                }
                else
                {
                    ScanStr(&tok3);
                    int tab = convertTabTypeStringToEnum(tok3.c_str());
                    for (;;)
                    {
                        stTitleSection section;
                        section.m_category = tab;

                        // 统计同 tab 已有章节数作为段内索引（asm +0x622..+0x668）。
                        int sameCount = 0;
                        for (std::vector<stTitleSection>::size_type k = 0;
                             k < out->m_vecC.size(); ++k)
                        {
                            if (out->m_vecC[k].m_category == tab)
                                ++sameCount;
                        }

                        // ORIG +0x5f5 是 call/test al,al/je（无 keepGoing 中间变量），
                        // 且 then 分支内联、break 落到循环之后 → if(...){段体} else break。
                        if (importTitleSection(&section, sameCount))
                        {
                            // 收缩区间到首个空槽（asm +0x69d..+0x70b）：
                            // endIdx(+0x10) > i 且 coll[i].m_index==-1 时 endIdx=i。
                            std::vector<stTitleElement>* collection =
                                getTitleCollection(out, tab);
                            if (collection != 0)
                            {
                                for (int i = section.m_startIdx;
                                     section.m_endIdx > i; ++i)
                                {
                                    // ORIG 两个条件各自 setg/sete + test al,al（嵌套 if），非 &&。
                                    if ((int)collection->size() > i)
                                    {
                                        if ((*collection)[i].m_index == -1)
                                        {
                                            section.m_endIdx = i;
                                        }
                                    }
                                }
                            }
                            out->m_vecC.push_back(section);
                        }
                        else
                        {
                            break;
                        }
                    }
                    break;
                }
            }
            /* fall through */
            case 0xc6:                           // [custom condition message]
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[custom condition message]", 0xc6));
                }
                else
                {
                    stCustomConditionMsg entry;
                    out->m_vec30.clear();
                    for (;;)
                    {
                        // ORIG +0x6cd：条件在循环顶部（call ScanStr; xor eax,1; test; jne）。
                        if (!ScanStr(&tok3))
                            break;
                        int tab = convertTabTypeStringToEnum(tok3.c_str());
                        entry.m_tabType = tab;
                        entry.m_value = ScanInt((bool*)0);
                        ScanStr(&entry.m_msg);
                        out->m_vec30.push_back(entry);
                    }
                    break;
                }
            }
            /* fall through */
            case 0xe4:                           // [adjust list]
            {
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[adjust list]", 0xe4));
                }
                else
                {
                    stAdjust entry;
                    for (;;)
                    {
                        // ORIG +0x8c2：顶部取值后判 ok；循环体内外均无 ok = false。
                        entry.m_questIdx = ScanInt(&ok);
                        if (!ok)
                            break;
                        entry.m_field4 = ScanInt((bool*)0);
                        entry.m_achievementIdx = ScanInt((bool*)0);
                        out->m_vec3c.push_back(entry);
                    }
                    break;
                }
            }
            /* fall through */
            case 0xf4:                           // [view titlebook eventmark]
            {
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[view titlebook eventmark]", 0xf4));
                }
                else
                {
                    int value = 0;
                    // ORIG +0x98e：xor eax,1 / test / jne → if (!ScanInt(...)) break。
                    if (!ScanInt(&value))        // ORIG 使用 ScanInt(int*) 重载
                        break;
                    out->m_viewEventMark = (value != 0);
                    break;
                }
            }
            /* fall through */
            default:
                break;
            }

            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
                goto titlebook_retry;            // 以当前令牌重新派发（ORIG `jmp +0x82`）
            }
            // [2026-09-10 D06] ORIG 0x08aa8ae3 `jmp 08aa8171 <+0x61>`：已初始化路径
            // **一跳直落外层循环头**；写 `break`（跳出内层）会多出一条内层出口 jmp
            // （4.4 把内层出口与外层回边分成两条，实测 ours 643 vs orig 642 = +1）。
            goto outer_retry;
        }
outer_done:;

    return true;
}

// ============================================================================
// importSlangName —— ORIG 0x089081a3
// 报告 _lt_global_gt_/importSlangName.md；调用点 init.md +0x21fd：
//   importSlangName("Etc/slangname.etc", this+0x86a0)。
// [SLANG]/[one byte skip char]/[two byte skip char] 三段分别回填
// SlangNameData 的 vector<string>(+0x00)/vector<uchar>(+0x0c)/
// vector<ushort>(+0x18)。
// ============================================================================
bool importSlangName(char* path, SlangNameData* out)
{
    if (!loadRDARScriptFile("", path))
        return false;

    std::string token;
    std::string text;
    int value;   // ORIG [ebp-0xc]：两段共用的具名 int 中转（ScanInt 结果先落此槽）

    // ORIG 循环头在函数前部（cond 前置、body 后置、尾部 `jmp` 回边）：
    // `while` 在 4.4 恒旋转为「body 前置 + cond 尾部」，故按 P283/P164 改
    // goto 循环；`if (!ScanType(...)) goto end` 复现 ORIG 的
    // `call ScanType; xor $0x1,%eax; test %al,%al; jne exit`（P292：bool callee）。
slang_loop:
    if (!ScanType(token, true))
        goto slang_end;
    {
        if (token == "[SLANG]")
        {
            while (ScanStr(&text))
                out->m_slangList.push_back(text);
        }
        if (token == "[one byte skip char]")
        {
            // ORIG 形（asm +0xb7..+0xe8）：块级 `bool ok`（**无预清**）+
            // `for(;;){ value=ScanInt(&ok); if(ok) push_back((uchar)value);
            //  else break; }`——ScanInt 结果先落 int 槽 -0xc；push_back 实参是
            // **prvalue 临时**（`mov -0xc,%eax; mov %al,-0xf; lea -0xf`），
            // 具名 ch 左值恒选 RKh 且 4.4 对 `static_cast<T&&>(ch)` 会多物化一次。
            bool ok;
            for (;;)
            {
                value = ScanInt(&ok);
                if (ok)
                    out->m_charList.push_back(static_cast<unsigned char>(value));
                else
                    break;
            }
        }
        // ORIG 089081a3 +0x100：末段 if 的**不匹配**出口落 089081e7(nop) →
        // 089081e8(jmp 089081eb 循环头)——用 == 形（ORIG 符号 _ZSteqIc…）。
        if (token == "[two byte skip char]")
        {
            // 同 [one byte] 段：ORIG 第二段 ok 槽 -0x1a、临时槽 -0xe。
            bool ok;
            for (;;)
            {
                value = ScanInt(&ok);
                if (ok)
                    out->m_shortList.push_back(static_cast<unsigned short>(value));
                else
                    break;
            }
        }
    slang_token_end:;
    }
    goto slang_loop;

slang_end:
    return true;
}

// ============================================================================
// importItmeGroupTable —— ORIG 0x0890bb25
// 报告 _lt_global_gt_/importItmeGroupTable.md；调用点 init.md +0x3e40：
//   importItmeGroupTable("Etc/", "ItemGroupName.tbl", getItemGroupNameTable())。
// 循环读 (id, 名称) 对插入 table；返回 0=成功、0x1ebd=ScanStr 失败、
// 0x1eb2=加载失败（错误码与 ORIG 一致）。
// ============================================================================
int importItmeGroupTable(const char* dir, const char* file,
                         std::map<std::string, int>& table)
{
    if (!loadRDARScriptFile(dir, file))
        return 0x1eb2;

    std::string name;
    bool ok = false;

    while (true)
    {
        int id = ScanInt(&ok);
        if (!ok)
            break;
        // b28c-l23：ORIG @0x0890bb5x 循环体内**无** `movb $0x0,-0x25(%ebp)` 预清
        // （ok 仅由 ScanInt 出参写、判后不复位）⇒ 该复位是原码没有的死写入，删。
        if (!ScanStr(&name))
            return 0x1ebd;
        table.insert(std::make_pair(name, id));
    }
    return 0;
}

// stCustomConditionMsg::stCustomConditionMsg —— ORIG C1 0x8aa9928：string 成员
// 默认构造后两 int 置 -1（importTitleBookEtcScript [custom condition message]）。
stCustomConditionMsg::stCustomConditionMsg()
{
    m_tabType = -1;
    m_value = -1;
}
