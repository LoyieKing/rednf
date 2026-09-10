// ============================================================================
// df_game_r Game/ —— CDataManager::init() 脚本装载自由函数（第 7 批）。
//
// 本 TU 承载 DataManagerInitDepends.h 声明的以下 12 个 import*（全部为 ORIG nm
// 真实 T 符号；逐函数按 objdump 反汇编忠实转写，调用点见
// docs/class_func_reports/CDataManager/init.md）：
//
//   importEventAvatarListScript        0x0895e1ed  init.md +0x688b
//   importEventEtcScript               0x0895e30f  init.md +0x6998
//   importFairPVPEquipment             0x088ffbfc  init.md +0x2328
//   importGrowthEquipmentScript        0x0895eb3f  init.md +0x0576
//   importHellPartyScript              0x08909857  init.md +0x0d30
//   importInGameAdvertisementEtcScript 0x0897e9c4  init.md +0x048c
//   importInGameEventInfoList          0x0897eb59  init.md +0x674d
//   importLimitItemUsageInfo           0x0890d871  init.md +0x562b
//   importMercenarySystemEtcScript     0x089f6c50  init.md +0x6b19
//   importNPCRelationEventScript       0x0890d44f  init.md +0x5a5f
//   importPcBangEquipmentRendar        0x08915606  init.md +0x6be8
//   importQuestShopScript              0x0890fa3d  init.md +0x10d4
//
// 另提供上述函数直接依赖、当前无任何定义的 ORIG 小符号：
//   makeFairPVPEquipmentPackageKey 由 InitScripts_2 提供（勿在此重复定义）
//   tokenizeStr / ConvertStringToTime / getGrowTypeIndex 由 InitScripts_6 提供
//   loadAppendageType / loadValueType 沿用 extern 声明（定义归其报告域批次）
//   stLimitItemUsageInfo::Add 0x891b4a8 / stResetItemInfo::Add 0x891b4dc /
//   stRefillItemInfo::Add 0x891b502
//   stQuestShopStatus C1 0x891b948 / clear 0x891b8cc / D1 0x83cd826
//   stStatusLevelInfo C1 0x891b8b4；stQuestShoptInitCost C1 0x837109a
//   stNPCRelationEventInfo C1 0x891c946
//   stMercenaryRewardGroup C1 0x89f7a9c / stMercenaryCompetitionArea C1 0x89f7b28
//   stHellPartyMonsterGroupList C1 0x891b1d6 / stHellParty_t C1 0x891b242 D1 0x891c7e2
// ============================================================================

#include "CDataManager.h"        // DataManagerInitDepends.h 形参类型宿主，必须先包含

#include "DataManagerInitDepends.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#include "DNFLexWrapper.h"
#include "EventAvatarListScript.h"

#include "CEventScriptMng.h"   // InGameEventInfo / InGameEventInfoList

// ---- 外部脚本助手（定义在兄弟 TU，此处仅引用）----
int loadAppendageType(const std::string& token);                       // 0x0889253b
int loadValueType(const std::string& token);                           // 0x08892ac4
void tokenizeStr(const std::string& str, std::vector<std::string>& out,
                 const std::string& delim);                            // 0x088945e2（InitScripts_6）
int getCharacterJob(const char* name);                                 // 0x088be57e（NpcScript.h 域）
bool ConvertStringToTime(std::string str, long& out);                  // 0x088c2341（InitScripts_6）
int getGrowTypeIndex(const char* name);                                // 0x088be63d（InitScripts_6）

// （原 namespace 内 kInGameEventTokenIds/lookupTokenId 发明查找表已删：
//  importInGameEventInfoList 按 ORIG 函数级静态 __lookup 真形重写，见该函数。）

// ============================================================================
// importMercenarySystemEtcScript — ORIG 0x089f6c50（init.md +0x6b19，
// "Etc/mercenary.etc" → m_mercenarySystem）。返回 0=成功，否则错误码
// （9 装载失败 / 0xc 空指针 / 0x29..0xd1 各段 Scan 失败，见 case 注释）。
// ORIG 双相派发（函数级静态 __lookup/__init_lock/__inited 三件套，case 贯穿
// 注册链 0x25/0x32/0x46/0x57/0x69/0x7f）已按汇编逐段还原；错误路径累加/
// 检查次序（+0x96d 先 Σ groupId 后查 ok、+0xa61 先 Σ count）与
// [mob/item index] make_pair 实参序（+0xa72：first=count 槽 -0x144）逐条对齐。
// 残余 N 类豁免（orig=ours=838，助记符序列仅剩以下跨编译器结构差）：
//  - +0x3ce `mov -0x13c,%edx` 值直达 vs 4.4 `mov…,%eax; mov %eax,%edx` 两段
//    （N18a regmove 不可逆，+0x555 同理，2 处）；
//  - +0x545 16 位 jle vs 4.4 同操作数折为 jbe（setcc-fold 比较方向）；
//  - +0xa61/+0xb51 累加 4.1 `mov mem,%eax; mov %eax,%edx` 两段物化，4.4 恒折
//    单步（N18a 反向，2 处）；
//  - +0xbe2/+0xcd6 及组错误出口：4.1 将 group/area 析构 merge 为 esi 标志共享
//    epilogue（+0xc04/+0xcf5），4.4 生成中继 jmp（EH 双份 dtor epilogue vs
//    merge 跨版本差异）。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=DIFF [N18a/N7] ORIG 0x089f6c50
//  p167 102/102 CLEAR；orig/ours=838/838。残余 65 行归一化差异全部为 N18a
//  regmove（4.1 `mov mem,%edx` vs 4.4 `mov mem,%eax; mov %eax,%edx`，5 处）
//  + EH 尾岛 2 处（跨版本，语义完成）。
int importMercenarySystemEtcScript(const char* path, stMercenarySystemInfo* out)
{
    if (!loadRDARScriptFile("", path))
        return 9;
    if (out == NULL)
        return 0xc;

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;                 // ORIG -0x11c
    std::string sub;                   // ORIG -0x120
    std::string sub2;                  // ORIG -0x124
    std::string unused;                // ORIG -0x128（构造/析构但零引用槽）
    bool ok = false;                   // ORIG -0x129
    short lv = 0;                      // ORIG -0x12c（movw $0；%ax 写）
    unsigned int gold = 0;             // ORIG -0x130
    unsigned int item = 0;             // ORIG -0x134
    unsigned int avatarIdx = 0;        // ORIG -0x138
    unsigned int period = 0;           // ORIG -0x13c
    float rate = 0;                    // ORIG -0x140
    unsigned int count = 0;            // ORIG -0x144
    unsigned int index = 0;            // ORIG -0x148

    for (;;)
    {
        if (!ScanType(token, true))
            break;

        // ORIG 重派发跳回 +0xf0（__lookup guard）：静态声明位于主循环体内。
        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        for (;;)
        {
            int id;
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                id = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    continue;
                }
                id = 0;
            }

            switch (id)
            {
            case 0x25:                      // [base time unit]
            case 0:
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[base time unit]", 0x25));
                }
                else
                {
                    period = ScanInt(&ok);
                    if (!ok) return 0x29;
                    if (period == 0) return 0x2c;
                    out->m_field0 = period;
                    break;
                }
                /* fall through（ORIG +0x383 jmp +0x3d5） */
            case 0x32:                      // [level base GoldItem Prob]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[level base GoldItem Prob]", 0x32));
                }
                else
                {
                    lv = ScanInt(&ok);
                    if (!ok) return 0x36;
                    gold = ScanInt(&ok);
                    if (!ok) return 0x3a;
                    item = ScanInt(&ok);
                    if (!ok) return 0x3e;
                    out->m_vec8.push_back(std::make_pair(lv, std::make_pair(gold, item)));
                    if (out->m_field4 > (unsigned short)lv)   // ORIG 16 位 cmp 形：保留最小值
                        out->m_field4 = (unsigned short)lv;
                    break;
                }
                /* fall through（+0x430 jmp +0x55e） */
            case 0x46:                      // [mercenary reward per period]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[mercenary reward per period]", 0x46));
                }
                else
                {
                    while (true)
                    {
                        period = ScanInt(&ok);
                        if (!ok) break;
                        rate = ScanFloat(&ok);
                        if (!ok) return 0x50;
                        out->m_vec14.push_back(std::make_pair(period, rate));
                    }
                    break;
                }
                /* fall through（+0x5b9 jmp +0x66c） */
            case 0x57:                      // [mercenary reward avatar bonus]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[mercenary reward avatar bonus]", 0x57));
                }
                else
                {
                    while (true)
                    {
                        avatarIdx = ScanInt(&ok);
                        if (!ok) break;
                        rate = ScanFloat(&ok);
                        if (!ok) return 0x61;
                        out->m_vec20.push_back(std::make_pair(avatarIdx, rate));
                    }
                    break;
                }
                /* fall through（+0x6c7 jmp +0x774） */
            case 0x69:                      // [mercenary reward critical]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[mercenary reward critical]", 0x69));
                }
                else
                {
                    if (!ScanStr(&out->m_str2c))
                        return 0x6c;
                    out->m_field3c = 0;
                    while (true)
                    {
                        gold = ScanInt(&ok);
                        if (!ok) break;
                        rate = ScanFloat(&ok);
                        if (!ok) return 0x77;
                        out->m_field3c += gold;      // ORIG 累计和
                        out->m_vec30.push_back(std::make_pair(gold, rate));
                    }
                    break;
                }
                /* fall through（+0x7c0 jmp +0x8a9） */
            case 0x7f:                      // [competition area]
                if (!__inited)
                {
                    __lookup.insert(
                        LookupMap::value_type("[competition area]", 0x7f));
                }
                else
                {
                    stMercenaryCompetitionArea area;
                    int groupIdSum = 0;              // ORIG -0x1c
                    while (true)
                    {
                        if (!ScanType(sub, true))
                            break;
                        if (sub == "[reward group]")
                        {
                            stMercenaryRewardGroup group;
                            group.m_groupId = ScanInt(&ok);
                            groupIdSum += group.m_groupId;   // ORIG 先累加后检查 ok
                            if (!ok) return 0x8f;
                            if (!ScanStr(&group.m_name))
                                return 0x92;
                            while (true)
                            {
                                if (!ScanType(sub2, true))
                                    return 0x97;
                                if (sub2 == "[mob index]")
                                {
                                    index = ScanInt(&ok);
                                    if (!ok) continue;
                                    count = ScanInt(&ok);
                                    if (!ok) return 0xa3;
                                    group.m_field20 = group.m_field20 + count;   // group+0x20 累计（ORIG 两段物化）
                                    group.m_mobList.push_back(std::make_pair(count, index));
                                    continue;
                                }
                                if (sub2 == "[item index]")
                                {
                                    index = ScanInt(&ok);
                                    if (!ok) continue;
                                    count = ScanInt(&ok);
                                    if (!ok) return 0xb3;
                                    group.m_field24 = group.m_field24 + count;   // group+0x24 累计（ORIG 两段物化）
                                    group.m_itemList.push_back(std::make_pair(count, index));
                                    continue;
                                }
                                if (sub2 == "[/reward group]")
                                {
                                    area.m_groups.push_back(group);
                                    break;
                                }
                            }
                            continue;
                        }
                        if (sub == "[world map]")
                        {
                            area.m_worldMapIdx = ScanInt(&ok);  // ORIG 无 ok 检查
                            continue;
                        }
                        if (sub == "[area level]")
                        {
                            area.m_areaLevel = ScanInt(&ok);    // ORIG 无 ok 检查
                            continue;
                        }
                        if (sub == "[/competition area]")
                        {
                            area.m_groupIdSum = groupIdSum;
                            out->m_vec40.push_back(area);
                            break;
                        }
                        return 0xd1;
                    }
                    break;
                }
                /* fall through */
            default:
                break;
            }

            // ORIG LAB_089f795a 公共尾：注册链走完释放锁置位重派发；正常态下一轮。
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
            }
            else
                break;
        }
    }
    return 0;
}

// ============================================================================
// importEventAvatarListScript — ORIG 0x0895e1ed（init.md +0x688b 调用，
// "Event/EventAvatarList.evt" → m_eventAvatarList）。
// 语义：loadRDARScriptFile("",path)；clear；扫描 "[avatar list]" 后连续
// ScanInt 入 set（ScanInt 失败回主循环）；"[event avatar list end]" 置返回
// 标记。返回该标记（未见到 end 段返回 false）。
// ============================================================================
bool importEventAvatarListScript(EventAvatarListScript* s, const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    // [b28c-lane24] ORIG 0x0895e1ed 序：p(-0x10)=s 的 store 先于 s->clear() 的 call；
    // 死变量 unused0(-0xc) 的 movl 先于 seenEnd(-0x1d) 的 movb ⇒ 按 ORIG 声明序。
    EventAvatarListScript* p = s;              // ORIG -0x10 arg 缓存（insert 用）
    s->clear();
    std::string token;
    int unused0 = 0; (void)unused0;   // ORIG 栈槽 -0xc 同款死变量（-O0 保留存储）
    bool seenEnd = false;
    int value;                                 // ORIG -0x24（prologue 无初始化）

    for (;;)                                   // ORIG 4.1 while 落点形（check 在头）
    {
        if (!ScanType(token, true))
            break;
        if (token == "[avatar list]")
        {
            value = -1;
            for (;;)
            {
                value = ScanInt(&seenEnd);          // 复用同一 bool（ORIG 同栈槽）
                if (!seenEnd)
                {
                    break;
                }
                p->m_avatars.insert(value);
            }
        }
        else if (token == "[event avatar list end]")
        {
            seenEnd = true;
            // [b28d-47] ORIG 0x0895e1ed +0x0e3：置位后 jmp +0xea（直接退出主循环），
            // 非回主循环头（+0x56）。缺 break 会多扫一轮，语义差异。
            break;
        }
    }
    return seenEnd;
}

// ============================================================================
// importEventEtcScript — ORIG 0x0895e30f（init.md +0x6998，"Event/EventEtc.evt"
// → m_eventEtc）。"[seria room tree material list]" 连续 int 入 m_vec0；
// "[seria room deco material list]" 按 (值, 序号) 写 m_mapC（序号自增）；
// "[christmas event end]" 结束并返回 true。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=AE（已修）ORIG 0x0895e30f
//   P164 分离形 `while(run){ if(!ScanType) break; }` + p 缓存槽(-0x10) +
//   make_pair<int&,int&> 左值对（P61）。orig/ours=128/128。
bool importEventEtcScript(EventEtcScript* s, const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    // [b28d-solo-InitScriptProducers_7] ORIG -0x10 具名 p 缓存（store 先于 clear
    // call；后续 push_back/mapC insert 的 this 均从该槽取）。
    EventEtcScript* p = s;
    s->clear();

    std::string token;
    bool run = true;

    // [b28d-solo-InitScriptProducers_7] ORIG 0x0895e30f 分离形：条件头仅测 run
    //（cmpb $0,-0x9; jne 体），ScanType 检查落体内首（P164 同族）。
    while (run)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[seria room tree material list]")
        {
            int value = -1;
            for (;;)
            {
                if (!ScanInt(&value))
                {
                    break;
                }
                p->m_vec0.push_back(value);
            }
        }
        else if (token == "[seria room deco material list]")
        {
            int key = 0;
            int index = 0;
            for (;;)
            {
                if (!ScanInt(&key))
                {
                    break;
                }
                // ORIG 0x0895e402：make_pair<int&,int&>(key,index)（P61 左值性）
                // → value_type 转换构造 pair<const unsigned,unsigned>(pair<int,int>)
                // → map::insert(value_type&)。直构 value_type(u,u) 形不同。
                p->m_mapC.insert(std::make_pair(key, index));
                ++index;
            }
        }
        else if (token == "[christmas event end]")
        {
            run = false;
        }
    }
    return !run;
}

// ============================================================================
// importFairPVPEquipment — ORIG 0x088ffbfc（init.md +0x2328，
// "Etc/FairPVPEquipment.etc" → m_stackableMap）。
// "[group]"：ScanStr 职业→getCharacterJob；ScanInt((bool*)0) 段位；
// key=makeFairPVPEquipmentPackageKey(job,grade)。随后收集若干
// "[package selection]" 包（成对 itemId/count 直至 ScanInt 失败），整组赋给
// (*out)[key]。[/fair pvp package] 或流结束均返回 true。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=AE（已修）ORIG 0x088ffbfc
//   P164 外层/内层 `for(;;)+break` 双改 + itemId 去预清（槽 -0x2c 对齐）；
//   p167 95/95 CLEAR。
bool importFairPVPEquipment(char* path, std::map<int, std::vector<std::vector<STStackablePackageData> > >* out)
{
    if (out == NULL)
    {
        return false;
    }
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;
    std::string jobName;

    // [b28d-solo-InitScriptProducers_7] ORIG 0x088ffbfc：循环头 ScanType 检查
    //（P164 `for(;;){ if(!ScanType) break; }`），内层 items 循环亦 `for(;;)+break`
    //（ORIG `call ScanInt; xor$1; test; jne 退出`）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[group]")
        {
            if (!ScanStr(&jobName))
            {
                return false;
            }
            const int job = getCharacterJob(jobName.c_str());
            const int grade = ScanInt((bool*)NULL);
            const int key = makeFairPVPEquipmentPackageKey(job, grade);

            std::vector<std::vector<STStackablePackageData> > groups;
            ScanType(token, true);
            while (token == "[package selection]")
            {
                int itemId;                      // ORIG -0x2c（无预清 store）
                std::vector<STStackablePackageData> pack;
                for (;;)
                {
                    if (!ScanInt(&itemId))
                        break;
                    STStackablePackageData data;
                    data.m_field0 = itemId;
                    data.m_field4 = ScanInt((bool*)NULL);
                    pack.push_back(data);
                }
                groups.push_back(pack);
                ScanType(token, true);
            }
            (*out)[key] = groups;
        }
        else if (token == "[/fair pvp package]")
        {
            break;
        }
    }
    return true;
}

// ============================================================================
// importGrowthEquipmentScript — ORIG 0x0895eb3f（init.md +0x0576，
// "Event/GrowthWeapon.evt" → m_growthWeapon）。五个标量段直写字段（任一
// ScanInt 失败提前结束但返回 true）；"[growth equipment reward list]"
// 成对 (idx, reward) 入 m_rewardMap。
// 字段偏移与 growthEquipmentScript 布局一致（+0x18/+0x1c/+0x20/+0x22/+0x24）。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=AE（已修）ORIG 0x0895eb3f
//   P164 循环头 ScanType + P223 多源 goto 返回岛（done1..done5）+ 独立 if
//   序列（非 else-if）。orig/ours=183/183。
bool importGrowthEquipmentScript(growthEquipmentScript* s, const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    if (s == NULL)
    {
        return false;
    }

    std::string token;
    bool ok = false;

    // [b28d-solo-InitScriptProducers_7] ORIG 0x0895eb3f：循环头 ScanType 检查
    //（P164 `for(;;){ if(!ScanType) break; }`），五处 ok 失败各发独立 nop 岛
    // + `jmp 0895ed68` 共享 `mov $1,%ebx` 返回岛（P223 多源 goto 尾标签）。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[gift growth weapon box]")
        {
            s->m_giftBox = ScanInt(&ok);
            if (!ok) goto done1;             // ORIG：失败跳出解析但仍返回 1
        }
        if (token == "[growth material item]")
        {
            s->m_materialItem = ScanInt(&ok);
            if (!ok) goto done2;
        }
        if (token == "[growth material item max]")
        {
            s->m_materialMax = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok) goto done3;
        }
        if (token == "[event bubble max]")
        {
            s->m_eventBubbleMax = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok) goto done4;
        }
        if (token == "[event sand max]")
        {
            s->m_eventSandMax = static_cast<unsigned short>(ScanInt(&ok));
            if (!ok) goto done5;
        }
        if (token == "[growth equipment reward list]")
        {
            for (;;)
            {
                std::pair<int, int> entry;
                entry.first = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                entry.second = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                s->m_rewardMap.insert(entry);
            }
        }
    }
done1:;
done2:;
done3:;
done4:;
done5:;
    return true;
}

// ============================================================================
// importInGameAdvertisementEtcScript — ORIG 0x0897e9c4（init.md +0x048c，
// "Etc/ingame_advertisement.etc" → m_inGameAd）。clear() 将前 151 字节置 1；
// "[in game advertisement expose condition of teen]" 后每个 idx（ScanInt
// 成功且 <=150）清 0 一字节，idx>150 结束本段。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=AE（已修）ORIG 0x0897e9c4
//   P164 分离形 `while (run) { if (!ScanType(token, true)) break; }`（条件头
//   仅测 run、ScanType 检查落体内首）。
bool importInGameAdvertisementEtcScript(InGameAdvertisementScript* s, const char* path)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }
    InGameAdvertisementScript* p = s;          // ORIG -0x14 arg 缓存
    s->clear();                                // ORIG：p 存槽先于 clear call

    std::string token;                         // ORIG -0x18
    bool run = true;                           // ORIG -0xd（token ctor 后写 1）
    int idx = 0;                               // ORIG -0xc（声明序 run→idx→ok，[b28d-47]）
    bool ok = false;                           // ORIG -0x19

    // [b28d-solo-InitScriptProducers_7] ORIG 0x0897e9c4 分离形：条件头仅测 run
    //（cmpb $0,-0xd; jne 体），ScanType 检查落体内首（call;xor$1;test;jne nop 岛）
    // —— 合并 `while (run && ScanType(...))` 在 4.4 产短路段物化 + je 体，形异。
    while (run)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[in game advertisement expose condition of teen]")
        {
            for (;;)
            {
                idx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                if (idx > 150)
                {
                    break;
                }
                p->m_exposeConditionOfTeen[idx] = 0;   // ORIG movb$0,(edx,eax)
            }
        }
    }
    return true;
}

// ============================================================================
// importLimitItemUsageInfo — ORIG 0x0890d871（init.md +0x562b，
// "Etc/Chn_Server_LimititemUsageinfo.etc" → m_limitItemUsage）。
// 三段分别填充 Ex 结构的三个子表；日期串经 ConvertStringToTime 转秒。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=AE（已修）ORIG 0x0890d871
// 按 ORIG 逐段重排：
//  ① [Limit Item Usage Info] 首个 ScanInt 存 char 槽(-0x1d) ⇒ 源码首参是 char kind，
//    第二/三参是 int a(-0x1c)/b(-0x18)（原重建把 kind/a 类型写反）；
//  ② 循环头 ScanType 检查（P164）+ ok 失败 break/return 双出口；
//  ③ itemIdx(-0x14)/itemId(-0x10)/count(-0xc) 循环外声明并预清（ORIG movl $0）；
//  ④ ORIG -0x24 循环外 dead int 预清（P171/P232）补齐帧。
bool importLimitItemUsageInfo(char* path, stLimitItemUsageInfoEx* out)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;                         // ORIG -0x30
    std::string dateText;                      // ORIG -0x34
    int unused24 = 0; (void)unused24;          // ORIG -0x24（循环外预清，全程无读）
    bool ok = false;                           // ORIG -0x35

    for (;;)                                   // ORIG 4.1 while 落点形（check 在头）
    {
        if (!ScanType(token, true))
            break;
        if (token == "[Limit Item Usage Info]")
        {
            for (;;)
            {
                const char kind = (char)ScanInt(&ok);   // ORIG mov%al,-0x1d（char 槽）
                if (!ok)
                {
                    break;                                   // 回主循环
                }
                const int a = ScanInt(&ok);              // ORIG -0x1c
                if (!ok)
                {
                    return false;
                }
                const int b = ScanInt(&ok);              // ORIG -0x18
                if (!ok)
                {
                    return false;
                }
                out->m_info0.Add(kind, a, b);
            }
        }
        else if (token == "[reset item]")
        {
            int itemIdx = 0;                     // ORIG -0x14（块首预清）
            for (;;)
            {
                itemIdx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                if (!ScanStr(&dateText))
                {
                    return false;
                }
                long endTime;                    // ORIG -0x3c（无预清）
                if (!ConvertStringToTime(dateText, endTime))
                {
                    return false;
                }
                out->m_resetC.Add(itemIdx, endTime);
            }
        }
        else if (token == "[refill item]")
        {
            int itemId = 0;                      // ORIG -0x10（块首预清）
            int count = 0;                       // ORIG -0xc（块首预清）
            for (;;)
            {
                itemId = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                count = ScanInt(&ok);
                if (!ok)
                {
                    return false;
                }
                if (!ScanStr(&dateText))
                {
                    return false;
                }
                long endTime;                    // ORIG -0x40（无预清）
                if (!ConvertStringToTime(dateText, endTime))
                {
                    return false;
                }
                out->m_refill18.Add(itemId, count, endTime);
            }
        }
    }
    return true;
}

// ============================================================================
// importNPCRelationEventScript — ORIG 0x0890d44f（init.md +0x5a5f，
// "Event/NPCRelationEvent.evt" → m_npcRelation）。
// 顶层仅接受 "[NPC]"（其他 token 直接成功返回）；块内 "[index]" new 0x48 节点
// 并 AddNPCInfo；"[good]/[soso]" 进入好感段：每 kind token 先 GetFavorIndex，
// "[item]" 成对入 AddFavorItemInfo、"[reward]" 单条入 AddRewardItemInfo；
// "[can not give item]" 连续 id 入 AddDislikeItemInfo。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=DIFF [N82/N112] ORIG 0x0890d44f
// 按 ORIG 重排（已修）：
//  ① sub(-0x4c) 移入 [NPC] 内层循环内声明（ORIG 每轮 C1/D1，非函数级）；
//  ② favorKind 改裸短路 if 条件（ORIG eax 物化 mov$1/mov$0，非 int 槽）；
//  ③ 内层循环改 `for(;;)`；curIdx 循环内预清（去函数头重复 store）；
//  ④ 真 bug：`token != "[NPC]"`/`sub != "[can not give item]"` 改 operator==
//     正判（p167 实测 MISSING eq×2 / EXTRA ne×2，ne 选错重载）；
//  ⑤ [NPC] 非匹配与循环 EOF 共享 done 返回岛（goto done）。
// 残余 DIFF 归因（N 封顶，p167 40/40 CLEAR、orig/ours=309/314）：
//  sub 槽 -0x4c vs -0x54（4.4 块内 string 槽序）+ 4.4 continue 标志编码
//  `mov $0x2,%ebx` vs 4.1 `mov $0x1,%ebx` + AddFavor/RewardItemInfo 实参
//  寄存器 edx↔ecx 角色差（N82/N112/N7 族）。
bool importNPCRelationEventScript(const char* path, CNPCRelationEventManager* out)
{
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    std::string token;                         // ORIG -0x40（ctor 首位）
    std::string deadStr;                       // ORIG -0x44（prologue ctor、尾 dtor，全程无用的死串）
    int dead3c = 0; (void)dead3c;              // ORIG -0x3c prologue $0 写
    bool ok = false;                           // ORIG -0x45
    if (out == NULL)                           // ORIG 判空在 ok 初始化之后
    {
        return false;
    }
    int curIdx;                                // ORIG -0x38（循环内每轮预清）

    for (;;)                                   // ORIG 4.1 while 落点形（check 在头）
    {
        if (!ScanType(token, true))
            break;
        curIdx = 0;
        // ORIG 用 operator== 正判（p167：eq 2 处；写 != 会误选 ne 重载）
        if (token == "[NPC]")
        {
        }
        else
        {
            goto done;                          // ORIG：非 [NPC] 顶层 token 即结束（共享返回岛）
        }

        for (;;)
        {
            std::string sub;                   // ORIG -0x4c（每轮 C1/D1）
            if (!ScanType(sub, true))
            {
                break;
            }

            if (sub == "[index]")
            {
                curIdx = ScanInt(&ok);
                if (!ok)
                {
                    break;
                }
                stNPCRelationEventInfo* info = new stNPCRelationEventInfo;
                out->AddNPCInfo(static_cast<unsigned short>(curIdx), info);
                continue;
            }

            // ORIG 0x0890d5aa：裸短路链物化（mov $1,%eax / mov $0,%eax + test），
            // 非具名 int 局部（具名形产 setne 槽化，见旧 DIFF）。
            if (sub == "[good]" || sub == "[soso]" || sub == "[bad]")
            {
                // ---- 好感段：kind token 循环 ----
                for (;;)
                {
                    std::string kindToken;
                    if (!ScanType(kindToken, true))
                    {
                        break;
                    }
                    const ENUM_NPC_RELATION_EVENT_FAVOR_KIND favor =
                        out->GetFavorIndex(kindToken.c_str());

                    if (kindToken == "[item]")
                    {
                        for (;;)
                        {
                            const int itemId = ScanInt(&ok);
                            if (!ok)
                            {
                                break;
                            }
                            const int count = ScanInt(&ok);
                            if (!ok)
                            {
                                break;
                            }
                            out->AddFavorItemInfo(static_cast<unsigned short>(curIdx), favor,
                                                  static_cast<unsigned int>(itemId),
                                                  static_cast<unsigned int>(count));
                        }
                    }
                    else if (kindToken == "[reward]")
                    {
                        const int a = ScanInt(&ok);
                        if (!ok)
                        {
                            break;
                        }
                        const int b = ScanInt(&ok);
                        if (!ok)
                        {
                            break;
                        }
                        out->AddRewardItemInfo(static_cast<unsigned short>(curIdx), favor,
                                               static_cast<unsigned int>(a),
                                               static_cast<unsigned int>(b));
                        continue;                 // ORIG LAB_0890d75a：bVar2=true 回内层
                    }
                    else
                    {
                        break;                  // 未知 kind token 结束好感段
                    }
                }
                continue;                       // 好感段结束 → 回 [NPC] 内层循环头
            }

            // ORIG 0x0890d78a：operator== 正判（p167：eq 补 2 处 / ne 多 2 处）
            if (sub == "[can not give item]")
            {
                int dislikeId = 0;              // ORIG -0x1c（块首预清）
                for (;;)
                {
                    dislikeId = ScanInt(&ok);
                    if (!ok)
                    {
                        break;
                    }
                    out->AddDislikeItemInfo(static_cast<unsigned short>(curIdx),
                                            static_cast<unsigned int>(dislikeId));
                }
            }
        }
    }
done:
    return true;
}

// ============================================================================
// importQuestShopScript — ORIG 0x0890fa3d（init.md +0x10d4，
// "Etc/questshop.etc" → m_questShop）。返回 0=成功，否则错误码：
//   0x2815 装载失败 / 0x2829 [quest status] 序号 ScanInt 失败 /
//   0x284f loadAppendageType==-1 / 0x286d [quest point] ScanInt 失败。
// "[initialization costs]" 三值写 m_initCost（失败按 ORIG 返回 0）。
// ============================================================================
// [b28d-solo-InitScriptProducers_7] verdict=DIFF [N82/P71] ORIG 0x0890fa3d
//  已修：status 提函数头（-0xcc）；[quest status]/[initialization costs] 改
//  if/else 链（p167 MISSING eq → == 正判）；map 插入改 make_pair
//  （p167 MISSING make_pair/pair C1 OS_ 变体，P61/P192）。
//  残余：p167 仅 EXTRA `stStatusLevelInfoD1Ev` ×2 —— P71 用户空析构跨版本
//  trivial 判定差（CDataManager.h 共享头，不改）；槽/临时分配 N82。
// [b28f-SweepDeps 复核] 主循环 `while (ScanType(token,true))`：ORIG 0x0890fa9d
//  为**头置条件**（`movl $0x1,0x4(%esp); call ScanType; xor $0x1,%eax; test;
//  jne exit`，入口 0x0890fa9a `jmp +2` 跳过对齐 nop），4.4 恒旋转为底部条件
//  （entry `jmp <尾条件>`）。P292/P283 形（for(;;)+`if(!ScanType) break;`）实测
//  **反劣**：our 370→372、gap −27→−29（diff 行 525→517 不改变 verdict）⇒ 已回退，
//  按 P164 环旋转（4.4 固有）封顶。同族 importPcBangEquipmentRendar 内层
//  items 循环已记「三形穷举不可同时保 xor$1 与头置」。
// [b28f-SweepDeps 线索] 另 2 处 EXTRA `stStatusLevelInfoD1Ev` 可试 P334 trivial
//  镜像视图（本 TU anonymous namespace 同字段序 POD 镜像 + 容器实参位
//  reinterpret_cast<const stStatusLevelInfo*>），未实测（超本波预算）。
int importQuestShopScript(const char* path, stQuestShop& out)
{
    if (!loadRDARScriptFile("", path))
    {
        return 0x2815;
    }

    // [b28d-solo-InitScriptProducers_7] ORIG 0x0890fa3d 声明序/槽：
    // token -0x80 / ok -0x81 / text -0x88 / status -0xcc（status 函数头 C1，
    // 每 [quest status] 记录调 clear；旧版块内声明致帧 0xb0 vs ORIG 0xe0）。
    std::string token;                         // ORIG -0x80
    bool ok = false;                           // ORIG -0x81
    std::string text;                          // ORIG -0x88
    stQuestShopStatus status;                  // ORIG -0xcc

    while (ScanType(token, true))
    {
        // ORIG 0x0890fac9：[quest status] 用 operator== 正判、else 落
        // [initialization costs]（p167：MISSING eq / EXTRA ne）；map 插入用
        // make_pair（p167：MISSING make_pair/pair C1<...>OS_ 变体，P61/P192）。
        if (token == "[quest status]")
        {
            // ---- 单个 [quest status] 记录 ----
            status.clear();
            const int idx = ScanInt(&ok);
            if (!ok)
            {
                return 0x2829;
            }
            if (out.m_field18 < idx)
            {
                out.m_field18 = idx;
            }

            bool closeRecord = false;
            while (!closeRecord)
            {
                if (!ScanType(token, true))
                {
                    closeRecord = true;         // ORIG：流结束也先入库再回主循环
                    break;
                }
                if (token == "[/quest status]")
                {
                    closeRecord = true;
                    break;
                }
                if (token == "[name]")
                {
                    ScanStr(&status.m_name);
                }
                else if (token == "[name2]")
                {
                    ScanStr(&status.m_name2);
                }
                else if (token == "[explain]")
                {
                    ScanStr(&status.m_explain);
                }
                else if (token == "[icon]")
                {
                    ScanStr(&status.m_icon);
                    status.m_field10 = ScanInt((bool*)NULL);
                }
                else if (token == "[quest status info]")
                {
                    if (!ScanStr(&text))
                    {
                        return 0x284f;
                    }
                    const int appendage = loadAppendageType(text);
                    if (appendage == -1)
                    {
                        return 0x284f;
                    }
                    if (!ScanStr(&text))
                    {
                        return 0x284f;
                    }
                    const int valueType = loadValueType(text);
                    (void)valueType;
                    const float value = ScanFloat((bool*)NULL);
                    (void)value;
                    // ORIG：out+0x28+appendage = (char)idx（appendage 槽位标记）
                    out.m_pad28[appendage] = static_cast<char>(idx);
                }
                else if (token == "[level info]")
                {
                    stStatusLevelInfo info;
                    info.m_second = static_cast<unsigned char>(ScanInt(&ok));
                    if (!ok)
                    {
                        continue;
                    }
                    info.m_first = static_cast<unsigned char>(ScanInt(&ok));
                    if (!ok)
                    {
                        continue;
                    }
                    status.m_levelInfos.push_back(info);
                }
                else if (token == "[quest point]")
                {
                    const int point = ScanInt(&ok);
                    if (!ok)
                    {
                        return 0x286d;
                    }
                    status.m_questPoint = static_cast<unsigned char>(point);
                }
            }

            out.m_map0.insert(std::make_pair(idx, status));
        }
        else if (token == "[initialization costs]")
        {
            stQuestShoptInitCost cost;
            cost.m_cost0 = ScanInt(&ok);
            if (!ok) { return 0; }
            cost.m_cost1 = ScanInt(&ok);
            if (!ok) { return 0; }
            cost.m_cost2 = ScanInt(&ok);
            if (!ok) { return 0; }
            out.m_initCost.m_cost0 = cost.m_cost0;
            out.m_initCost.m_cost1 = cost.m_cost1;
            out.m_initCost.m_cost2 = cost.m_cost2;
        }
    }
    return 0;
}

// ============================================================================
// importPcBangEquipmentRendar — ORIG 0x08915606（init.md +0x6be8，
// "Etc/PcBangRentalService.etc" → m_pcBangRentar）。
// "[section]"：EventRewardSection 三字节 + count 个 uchar 入 m_vec0；
// "[group]"：job/grade→key，收集 "[package selection]" 包（itemId、count、
// short、"逗号串"→tokenize/getGrowTypeIndex 逐项 push_back），[/group] 时
// insert 进 m_mapC。
// ============================================================================
// [SweepInitProd7 复核] verdict=DIFF [N78/N7/N104] ORIG 0x08915606
//  逐 jcc/调用点核对：无语义缺口。jcc 27:27 一一对应，唯一极性差 = [section]
//  do-while 的 more 标志（ORIG 0x089159e7 `test %ebx,%ebx; je` 寄存器化 vs
//  我方 `cmpb $0,-0x29(%ebp); jne` 槽化，do-while 语义等价）；内层 items 循环
//  4.4 旋转（条件落尾 0x2b46 vs ORIG 落头 0x08915b02）；growText/delim 临时
//  析构的 EH 双份 epilogue vs 4.1 merge（N104）。
//  p167 别名口径 CLEAR（MISSING C2/D2 ↔ EXTRA C1/D1 一一配对，P93）；
//  无 call *（无虚调用）；orig/ours=582/581。
bool importPcBangEquipmentRendar(const char* path, PcBangItemRentarData* out)
{
    // ORIG 双相派发（T 0x08915606）：out 判空先于 loadRDAR（+0x0c）；__lookup
    // 三件套位于主循环体内；注册链 [section]0x33f6→[group]0x3406+?（0x3409）。
    // [section] 执行形 = do-while：一次 token 命中后连续解析多节直至
    // ScanInt(&ok) 断流（**bVar2=false 仅退出该 do-while 回主循环，非 return**）；
    // [group] 执行形 = 内层 ScanType 循环：[package selection] 组 items，
    // [/group] 提交 make_pair+map insert，内层 EOF 无提交直接回主循环。
    // 收敛口径：orig=582 ours=581，语义/槽位/调用序已逐段对齐；残余 1 条及
    // 零散助记符差为跨编译器形态：[section] do-while 的 more 标志 4.1 全程走
    // %ebx（本侧 bool 槽 spill，N78 register-bool 寄存器选择不控）、
    // 内层 items 循环 4.4 旋转（scan 落底 vs ORIG 落头，穷举 for/while/
    // while(true)+break 三形均不能同时保住 xor$1 物化与头置）、growText/
    // delim/tokenize 临时析构的 EH 双份 epilogue vs 4.1 merge（N 在册）。
    if (out == NULL)
    {
        return false;
    }
    if (!loadRDARScriptFile("", path))
    {
        return false;
    }

    typedef std::map<const char*, int, less_TCHAR> LookupMap;

    std::string token;                    // ORIG -0xa4
    std::string nameText;                 // ORIG -0xa8
    bool ok = false;                      // ORIG -0xa9
    int count = 0;                        // ORIG -0x30（prologue 初始化，跨段复用）

    for (;;)
    {
        if (!ScanType(token, true))
            break;

        static LookupMap __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        for (;;)
        {
            int id;
            if (__inited)
            {
                LookupMap::const_iterator it = __lookup.find(token.c_str());
                id = (it != __lookup.end()) ? it->second : -1;
            }
            else
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    continue;
                }
                id = 0;
            }

            switch (id)
            {
            case 0x33f6:                   // [section]
            case 0:
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[section]", 0x33f6));
                }
                else
                {
                    bool more = false;
                    do
                    {
                        EventRewardSection section;   // 每轮构造/析构（ORIG -0xc0）
                        section.m_field0 = ScanInt(&ok);
                        if (ok == true)
                        {
                            section.m_field1 = ScanInt((bool*)NULL);
                            section.m_field2 = ScanInt((bool*)NULL);
                            count = ScanInt((bool*)NULL);
                            for (int i = 0; i < count; ++i)
                            {
                                // ORIG 0x08915c??：push_back(EOh) 右值形
                                //（p167：MISSING push_back(EOh) / EXTRA (RKh)）
                                section.m_items.push_back(
                                    static_cast<unsigned char>(ScanInt((bool*)NULL)));
                            }
                            out->m_vec0.push_back(section);
                            more = true;
                        }
                        else
                        {
                            more = false;
                        }
                    } while (more);
                    break;
                }
                /* fall through（ORIG +0x2e9 jmp +0x3ee） */
            case 0x3409:                   // [group]
                if (!__inited)
                {
                    __lookup.insert(LookupMap::value_type("[group]", 0x3409));
                }
                else
                {
                    if (!ScanStr(&nameText))
                        return false;      // ORIG +0x454 ebx=0 → 直接出口 return false
                    int job = getCharacterJob(nameText.c_str());
                    int grade = ScanInt((bool*)NULL);
                    int key = makeFairPVPEquipmentPackageKey(job, grade);
                    std::vector<std::vector<EventRewardItemInfo> > groups;
                    for (;;)
                    {
                        if (!ScanType(token, true))
                            goto group_no_submit;   // ORIG +0x4ca→+0x850 EOF 无提交
                        if (token == "[package selection]")
                        {
                            std::vector<EventRewardItemInfo> items;
                            int itemIdx;
                            while (ScanInt(&itemIdx) == 1)
                            {
                                EventRewardItemInfo item;
                                item.m_field0 = itemIdx;
                                item.m_short8 = ScanInt((bool*)NULL);
                                item.m_field4 = ScanInt((bool*)NULL);
                                ScanStr(&nameText);
                                std::string growText(nameText.c_str());
                                std::vector<std::string> parts;
                                std::string delim(",");
                                tokenizeStr(growText, parts, delim);
                                unsigned int p = 0;
                                while ((int)parts.size() > (int)p)
                                {
                                    int grow = getGrowTypeIndex(parts.at(p).c_str());
                                    item.m_growTypes.push_back(grow);
                                    p = p + 1;
                                }
                                items.push_back(item);
                            }
                            groups.push_back(items);
                            continue;
                        }
                        if (token == "[/group]")
                            break;
                    }
                    out->m_mapC.insert(std::make_pair(key, groups));
                group_no_submit:
                    ;
                    break;
                }
                /* fall through */
            default:
                break;
            }

            // ORIG LAB_08915e80 公共尾。
            if (!__inited)
            {
                __init_lock.leave();
                __inited = true;
            }
            else
                break;
        }
    }
    return true;
}

// ============================================================================
// importHellPartyScript — ORIG 0x08909857（init.md +0x0d30，
// "Etc/HellParty.etc" → *Get_instance_stHellParty_t()）。
// 返回 0=成功；错误码段：0x1a24 装载 / 0x1a2e 首.ScanType /
// 0x1a37..0x1aa3 基础值各 Scan / 0x1aba 组下标 / 0x1acd..0x1aed 组名 /
// 0x1aed 未知组名 / 0x1b08 组下标重复 / 0x1b1a..0x1b30 difficulty 段。
// ============================================================================
// [SweepInitProd7 复核] verdict=DIFF [N112/N121/N82] ORIG 0x08909857
//  本波逐 jcc/字段偏移核对，修真语义 bug（判据⑮ 多余门禁 / 写入位置 / P0 / P171 /
//  P293）：
//   ①[group] 怪物循环原写 `for (i < slotCount)`，slotCount=out.m_field72[tag] ——
//     ORIG 0x08909f9e..0x08909fee 是**无上界** for(;;)，仅 ScanInt 后 ok 失败
//     break（越界照抄）；我方多读 out.m_field72[tag] 并提前退出 ⇒ 删门禁；
//   ②[group index] 原先判 ok 再写 out.m_groupIndex —— ORIG 0x08909dc2 先
//     `mov %dx,0x50(%eax)` 写字段、0x08909dc6 才判 ok；
//   ③groupName/rowName 的 ScanStr 改 `ok = ScanStr(...); if (!ok)`，复现 ORIG
//     0x08909e2d/0x0890a197 的 `mov %al,-0xd9; movzbl -0xd9` ok 槽往返（P0）；
//   ④补 tag 分支死写入 out.m_field6c[tag]/out.m_field72[tag] → 两个死局部
//     （ORIG 0x08909e6a..0x08909f86，P171 死写入照抄）+ tag 改 uchar（movb）；
//   ⑤m_groupMap.find 改具名 iterator ⇒ 复现 ORIG 先 find(0x0890a011) 后
//     end(0x0890a02c) 的求值序（P293）；
//   ⑥[hellparty script end] 与 [/hellparty basic values]/[/hellparty monster
//     group] 收尾比较移至链末（ORIG 0x0890a2ae / 0x08909d21 / 0x0890a0f5）；
//     difficulty 循环改 `i < rows+1` 复现 ORIG `add $1; cmp; setg`。
//  残余（N 封顶）：4.4 把 11 个 else-if 回边 jmp 集中并各带 1 字节 nop 对齐
//  （N112/N121 块重排，源码不可控）+ 全帧槽号平移（N82/N126）。
//  p167 别名口径 CLEAR（MISSING 全 C2/D2 与 EXTRA C1/D1 一一配对，P93）；
//  无 call *（无虚调用）；ratio 0.9357、gap=-10（全部为上述 nop/块序）。
int importHellPartyScript(const char* path, stHellParty_t& out)
{
    bool ok = false;                  // ORIG -0xd9：prologue 后即 movb $0
    std::string token;
    std::string sub;

    if (!loadRDARScriptFile("", path))
    {
        return 0x1a24;
    }

    for (;;)
    {
        // ---- 顶层分派（ORIG 外层头：每次取一 token，流结束 → 0x1a2e）----
        if (!ScanType(token, true))
        {
            return 0x1a2e;
        }

        // ================= [hellparty basic values] =================
        if (token == "[hellparty basic values]")
        {
            for (;;)
            {
                if (!ScanType(sub, true))
                {
                    return 0x1a37;
                }

                if (sub == "[max base value]")
                {
                    out.m_maxBaseValue = ScanInt(&ok);
                    if (out.m_maxBaseValue == 0) { return 0x1a41; }
                    if (!ok) { return 0x1a43; }
                }
                else if (sub == "[min base value]")
                {
                    out.m_minBaseValue = ScanInt(&ok);
                    if (!ok) { return 0x1a4c; }
                }
                else if (sub == "[base value]")
                {
                    out.m_baseValue = ScanInt(&ok);
                    if (!ok) { return 0x1a53; }
                }
                else if (sub == "[sub base rates]")
                {
                    out.m_subBaseRates = ScanFloat(&ok);
                    if (!ok) { return 0x1a5a; }
                }
                else if (sub == "[pass sub base rates]")
                {
                    out.m_passSubBaseRates = ScanFloat(&ok);
                    if (!ok) { return 0x1a61; }
                }
                else if (sub == "[base rising value]")
                {
                    out.m_baseRisingValue = ScanInt(&ok);
                    if (!ok) { return 0x1a68; }
                }
                else if (sub == "[level difference rates]")
                {
                    // 三连读，仅以最后一次 ScanInt 结果判定（ORIG 同栈槽）。
                    out.m_levelDiffRates[1] = ScanFloat(&ok);
                    out.m_levelDiffRates[2] = ScanFloat(&ok);
                    out.m_levelDiffRates[3] = ScanFloat(&ok);
                    if (!ok) { return 0x1a72; }
                }
                else if (sub == "[difficulty rates]")
                {
                    out.m_difficultyRates[0] = ScanFloat(&ok);
                    out.m_difficultyRates[1] = ScanFloat(&ok);
                    out.m_difficultyRates[2] = ScanFloat(&ok);
                    out.m_difficultyRates[3] = ScanFloat(&ok);
                    out.m_difficultyRates[4] = ScanFloat(&ok);
                    if (!ok) { return 0x1a88; }
                }
                else if (sub == "[party rates]")
                {
                    // 首值经 ScanFloat(3,&ok) 以刻度 3 装载（ORIG _Z9ScanFloatiPb）。
                    out.m_partyRates[0] = ScanFloat(3, &ok);
                    out.m_partyRates[1] = ScanFloat(&ok);
                    out.m_partyRates[2] = ScanFloat(&ok);
                    out.m_partyRates[3] = ScanFloat(&ok);
                    if (!ok) { return 0x1a93; }
                }
                else if (sub == "[ancient dungeon add rate]")
                {
                    out.m_ancientDungeonAddRate = ScanFloat(&ok);
                    if (!ok) { return 0x1a9b; }
                }
                else if (sub == "[guide line value]")
                {
                    out.m_guideLineValue = ScanInt(&ok);
                    if (!ok) { return 0x1aa3; }
                }
                else if (sub == "[/hellparty basic values]")
                {
                    break;                          // 回顶层（ORIG 0x08909d21：
                                                     // 收尾比较位于链末）
                }
                // 其余子 token 忽略
            }
            continue;
        }

        // ================ [hellparty monster group] ================
        if (token == "[hellparty monster group]")
        {
            for (;;)
            {
                if (!ScanType(sub, true))
                {
                    return 0x1ab2;
                }

                if (sub == "[group index]")
                {
                    // ORIG 0x08909db8：ScanInt 结果先存字段（mov %dx,0x50(%eax)），
                    // 之后才检查 ok（无具名 idx 局部，P0）。
                    out.m_groupIndex = static_cast<unsigned short>(ScanInt(&ok));
                    if (!ok)
                    {
                        return 0x1aba;
                    }
                }
                else if (sub == "[group]")
                {
                    // ORIG 0x08909dfc：i=0 的 store 先于 list C1（for 初始化
                    // 前置为块头声明）。
                    int i = 0;
                    stHellPartyMonsterGroupList list;
                    std::string groupName;
                    // ORIG 0x08909e28：返回值先写 ok 槽再回读（P0：原码有具名
                    // ok 局部往返），不是直判调用结果。
                    ok = ScanStr(&groupName);
                    if (!ok)
                    {
                        return 0x1ac9;
                    }

                    // 'A'..'E' 选择难度槽 tag 1..5。ORIG 每分支先读
                    // out.m_field6c[tag] / out.m_field72[tag] 存入紧随 list 之后的
                    // 两个死局部（P171 死写入照抄；读字段无副作用、值不再被读），
                    // 最后写 tag 字节（ORIG movb ⇒ uchar）。
                    unsigned char deadField6c;
                    unsigned short deadField72;
                    unsigned char tag;
                    if (groupName == "A")
                    { deadField6c = out.m_field6c[1]; deadField72 = out.m_field72[1]; tag = 1; }
                    else if (groupName == "B")
                    { deadField6c = out.m_field6c[2]; deadField72 = out.m_field72[2]; tag = 2; }
                    else if (groupName == "C")
                    { deadField6c = out.m_field6c[3]; deadField72 = out.m_field72[3]; tag = 3; }
                    else if (groupName == "D")
                    { deadField6c = out.m_field6c[4]; deadField72 = out.m_field72[4]; tag = 4; }
                    else if (groupName == "E")
                    { deadField6c = out.m_field6c[5]; deadField72 = out.m_field72[5]; tag = 5; }
                    else
                    {
                        return 0x1aed;
                    }
                    (void)deadField6c;
                    (void)deadField72;
                    (void)tag;
                    // ORIG 0x08909f9e..0x08909fee：无上界门禁的 for(;;)，仅以 ok
                    // 失败 break 退出（越界行为照抄，P171）；ScanInt 结果直存数组
                    // （P0：ORIG 无 monsterIdx/rate 中转局部，store 紧跟 call）。
                    for (; ; ++i)
                    {
                        list.m_monsterIdx[i] = ScanInt(&ok);
                        if (!ok) { break; }
                        list.m_rate[i] = static_cast<unsigned char>(ScanInt(&ok));
                        if (!ok) { break; }
                    }

                    // ORIG 0x0890a011 先 call find、0x0890a02c 后 call end
                    //（P293：具名 it 形复现 find 先/end 后）。
                    std::map<unsigned short, stHellPartyMonsterGroupList>::iterator it =
                        out.m_groupMap.find(out.m_groupIndex);
                    if (it != out.m_groupMap.end())
                    {
                        return 0x1b08;              // 组下标重复
                    }
                    // ORIG make_pair<unsigned short&, stHellPartyMonsterGroupList&>
                    // → value_type 转换构造（p167：MISSING make_pair/pair C1 OS_ 变体）
                    out.m_groupMap.insert(std::make_pair(out.m_groupIndex, list));
                }
                else
                {
                    // ORIG 0x0890a0f5：收尾比较位于 [group] 之后。
                    if (sub == "[/hellparty monster group]")
                    {
                        break;                      // 回顶层
                    }
                    return 0x1aed;                  // 未知子 token
                }
            }
            continue;
        }

        // ======================= [difficulty] =======================
        if (token == "[difficulty]")
        {
            const int rows = ScanInt(&ok);
            if (rows > 6)
            {
                return 0x1b1a;
            }
            if (!ok)
            {
                return 0x1b1d;
            }
            // ORIG 0x0890a295：`mov rows; add $1; cmp i; setg` ⇒ 原码 `i < rows+1`。
            for (int i = 1; i < rows + 1; ++i)
            {
                std::string rowName;
                ok = ScanStr(&rowName);
                if (!ok)
                {
                    return 0x1b25;
                }
                // 行名后五值：tag 字节 / 计数 ushort / 三字节列。
                out.m_field6c[i] = static_cast<unsigned char>(ScanInt(&ok));
                out.m_field72[i] = static_cast<unsigned short>(ScanInt(&ok));
                out.m_field7e[i] = static_cast<unsigned char>(ScanInt(&ok));
                out.m_field84[i] = static_cast<unsigned char>(ScanInt(&ok));
                out.m_field8a[i] = static_cast<unsigned char>(ScanInt(&ok));
                if (!ok)
                {
                    return 0x1b30;
                }
            }
            continue;
        }

        // ORIG 0x0890a2ae：[hellparty script end] 比较位于全部分派之后
        // （basic → monster group → difficulty → script end）。
        if (token == "[hellparty script end]")
        {
            return 0;
        }

        // 其他顶层 token：忽略并继续外层扫描
    }
}

// ============================================================================
// importInGameEventInfoList — ORIG 0x0897eb59（报告 _lt_global_gt_/
// importInGameEventInfoList.md 汇编+Ghidra C 双段转写；init.md +0x674d）。
// 双相派发真形：函数级静态 __lookup map<char const*,int,less_TCHAR>
// （BSS 0x094f7ba8）+ __init_lock SpinLock（0x094f7bc0）+ __inited（0x094f7bc4），
// guard 每轮穿（P177：static 位于重派发循环头）；注册轮持锁 id=0 → switch
// 全 miss → 公共尾 0x0897f0b3 leave+置位+jmp 0x0897ebca（guard 重查同 token）。
// 分派 = `cmpl $0x37; ja; shl $2; jmp *table` 跳表（P163：case 0 并入
// [/event_list] leaf 撑 range）。token→id 逐例 insert（非链式，每块独立）：
// [/event_list]=0x24（命中→result=true 经共享退出岛 0x0897f0fe 返回）、
// [idx]=0x27（ScanInt(int*)→info+0x00 + ScanStr→info+0x08，返回值不判）、
// [record type]=0x2b(+0x04)、[start]=0x2e(+0x0c)、[end]=0x31(+0x10)、
// [state]=0x34(+0x14)、[/event]=0x37（insertEventInfo(info.m_field4, info)
// 0x0897f0ae）。info/token 为函数级临时（-0xd8/-0xc0，循环外构造一次）；
// result(-0x1d) 仅 ScanType EOF / [/event_list] 两退出路径读取（P164 scan_exit
// 岛共享尾：`movzbl -0x1d,%ebx` + dtor 统一块）。
//
// 本波已修（P 范式命中）：
//  P309 key 提函数头声明（`const char* key;` 无 init）⇒ token 槽 -0xc0 对齐；
//  P168a② 公共尾负极性 `if (!__inited) { leave; 置位; goto redo; }` ⇒ 复现
//   ORIG `movzbl __inited; xor $1; test; je <nop 岛>`；
//  [/event_list] 路径改裸 `return true;` ⇒ 4.4 直 `mov $0x1,%ebx; jmp 共享退出岛`
//   （与 ORIG 0x0897ee2b 同形，旧 `result=true; goto` 的槽写已消）。
//
// 残余 DIFF 归因（N 类封顶，语义完成：p167 50/50 CLEAR、orig/ours=364/363）：
//  ① 4.4 循环入口对齐填充 `jmp +2; nop`（info C1 之后、ScanType 之前），ORIG
//     4.1 直接 fall-through（N112 变体，jmp 非 nop 不被 strip_nop_padding 剥离）；
//  ② find 的 sret 临时槽与 key 槽互换：ORIG key -0xb8 / temp -0xbc，4.4 编译器
//     临时恒优先占最浅槽 ⇒ key -0xbc / temp -0xb8（N82/N86 槽分配族；穷举
//     具名 iterator/声明序互换/内联实参三形均不闭合）。
// ============================================================================
// [SweepInitProd7 复核] verdict=DIFF [N112/N82] orig/ours=364/363
//   jcc 20:20 逐位同构（含 0x37 跳表前置 `cmpl $0x37; ja`），仅 3 处 jmp/nop
//   岛位置差（我方循环入口多 `jmp +2; nop`、ORIG 尾多 `jmp; nop`；N112/N121）；
//   p167 别名口径 CLEAR（MISSING C2/D2 ↔ EXTRA C1/D1 配对，P93）；
//   无 call *（无虚调用）；无语义缺口。
bool importInGameEventInfoList(InGameEventInfoList* list, const char* path)
{
    if (!loadRDARScriptFile("", path))
        return false;

    const char* key;                  // ORIG -0xb8（函数头声明，P309 先声明浅；无 init）
    std::string token;                // ORIG -0xc0
    bool result = false;              // ORIG -0x1d（0x0897eb97 movb $0）
    InGameEventInfo info;             // ORIG -0xd8（循环外 C1 0x0897eba4）

    while (true)
    {
        if (!ScanType(token, true))
        {
            break;                    // EOF → return result(false)（0x0897f0df）
        }

    redo_lookup:
        static std::map<const char*, int, less_TCHAR> __lookup;
        static SpinLock __init_lock;
        static bool __inited = false;

        int id;
        if (__inited)
        {
            key = token.c_str();                     // 0x0897ecd6
            std::map<const char*, int, less_TCHAR>::const_iterator it =
                __lookup.find(key);
            id = (it != __lookup.end()) ? it->second : -1;
        }
        else
        {
            __init_lock.enter();                     // 0x0897ed8d
            if (__inited)
            {
                __init_lock.leave();                 // 0x0897eda4
                goto redo_lookup;                    // 0x0897edaa → guard
            }
            id = 0;                                  // 0x0897edaf
        }

        switch (id)
        {
        case 0:
        case 0x24:                       // [/event_list]（0x0897edce leaf）
            // ORIG 各 leaf `movzbl __inited; xor$1; test; je parse` —— 取反
            // 短路物化形（P32：!bool 判产生 xor$1），注册块落 if 体。
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[/event_list]", 0x24));
                break;                               // → 公共尾
            }
            return true;                             // 0x0897ee2b mov$1,%ebx
        case 0x27:                       // [idx]
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[idx]", 0x27));
                break;
            }
            ScanInt(&info.m_eventId);                // 0x0897ee9b（返回值弃）
            ScanStr(&info.m_name);                   // 0x0897eeac
            break;
        case 0x2b:                       // [record type]
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[record type]", 0x2b));
                break;
            }
            ScanInt(&info.m_field4);
            break;
        case 0x2e:                       // [start]
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[start]", 0x2e));
                break;
            }
            ScanInt(&info.m_startTime);
            break;
        case 0x31:                       // [end]
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[end]", 0x31));
                break;
            }
            ScanInt(&info.m_endTime);
            break;
        case 0x34:                       // [state]
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[state]", 0x34));
                break;
            }
            ScanInt(&info.m_isEventing);             // +0x14 int（头注实证）
            break;
        case 0x37:                       // [/event]
            if (!__inited)
            {
                __lookup.insert(
                    std::map<const char*, int, less_TCHAR>::value_type(
                        "[/event]", 0x37));
                break;
            }
            list->insertEventInfo(info.m_field4, info);   // 0x0897f0ae
            break;
        default:
            break;                                   // 未命中静默（含 "[event_list]" 开标签）
        }

        // ORIG 公共尾取反物化形：movzbl __inited; xor $1; test; je <nop 岛>
        // ⇒ 源码写负极性 `if (!__inited) { leave; 置位; 重派发; }`（P168a②）。
        if (!__inited)
        {
            __init_lock.leave();                     // 0x0897f0c8
            __inited = true;                         // 0x0897f0cd
            goto redo_lookup;                        // 0x0897f0d4 → guard 重派发
        }

    scan_next:;
    }
    return result;                                   // 共享退出岛 0x0897f0fe
}

// ============================================================================
// 三个子类型构造器为头文件内联（CDataManager.h，ORIG 具名 C1 0x837c6be/6e6/70e）。
// ORIG 0x891b4a8 W：临时元素 {kind,a,b} 后 push_back。
void stLimitItemUsageInfo::Add(char kind, int a, int b)
{
    LimitInfo entry;
    entry.m_kind = kind;
    entry.m_a = a;
    entry.m_b = b;
    m_list.push_back(entry);
}

// ORIG 0x891b4dc W。
void stResetItemInfo::Add(int space, long resetSec)
{
    ResetItem entry;
    entry.m_space = space;
    entry.m_resetSec = resetSec;
    m_list.push_back(entry);
}

// ORIG 0x891b502 W。
void stRefillItemInfo::Add(int itemId, int count, long endSec)
{
    RefillItem entry;
    entry.m_itemId = itemId;
    // ORIG @0x891b502：装载序 itemId、endSec、count（槽值不变，仅发射序）
    entry.m_endSec = endSec;
    entry.m_count = count;
    m_list.push_back(entry);
}

// ORIG 0x837109a W：三 int 逆序清零（全局类型，mangled 无 stQuestShop 前缀）。
stQuestShoptInitCost::stQuestShoptInitCost()
{
    m_cost2 = 0;
    m_cost1 = m_cost2;
    m_cost0 = m_cost1;
}

// ORIG stNPCRelationEventInfo C1 0x891c946 W：两个 FavorAndReward 子对象 +
// vector<int> @+0x3c 默认构造。
stNPCRelationEventInfo::stNPCRelationEventInfo() {}

// ORIG stMercenaryRewardGroup C1 0x89f7a9c W：int 清零 + string/vector 构造，
// 尾部两 dword 清零（对齐 pad）。
stMercenaryRewardGroup::stMercenaryRewardGroup()
    : m_groupId(0), m_name(), m_mobList(), m_itemList(),
      m_field20(0), m_field24(0)
{
}

// ORIG stMercenaryCompetitionArea C1 0x89f7b28 W：ushort 清零 + vector 构造
// + int@0x10 清零 + ushort@0x14 清零。
stMercenaryCompetitionArea::stMercenaryCompetitionArea()
    : m_worldMapIdx(0), m_groups(), m_groupIdSum(0), m_areaLevel(0)
{
}

// ORIG stHellPartyMonsterGroupList C1 0x891b1d6 W：memset(-1) 两段 + 三标量清零。
stHellPartyMonsterGroupList::stHellPartyMonsterGroupList()
{
    memset(m_monsterIdx, -1, sizeof(m_monsterIdx));
    memset(m_rate, -1, sizeof(m_rate));
    m_field50 = 0;
    m_field52 = 0;
    m_field54 = 0;
}

// ORIG stHellParty_t C1 0x891b242 W：map@0x54 构造；标量清零；三段 memset；
// 难度表五组循环清零；+0x90=0。
// [b28d-solo-InitScriptProducers_7] verdict=AE（已修）ORIG 0x0891b242
// 4.1 发射序 = map C1(+0x54) → +0x50 →
// +0x0..0xc → +0x4c → +0x10 → +0x14 → memset 0x18/0x28/0x3c → +0x90 → 清零环
//（P239：类成员 meminit 前置、其余标量 store 段按 ORIG 序落 ctor 体）；
// 原全 meminit 形在 4.4 按声明序混合发射且 m_difficultyRates()/m_partyRates()
// 另发 value-init 循环（重复清零），删除并把标量移入体内。
stHellParty_t::stHellParty_t()
    : m_groupMap()
{
    m_groupIndex = 0;                        // +0x50
    m_maxBaseValue = 0;                      // +0x00
    m_minBaseValue = 0;                      // +0x04
    m_baseValue = 0;                         // +0x08
    m_baseRisingValue = 0;                   // +0x0c
    m_ancientDungeonAddRate = 0.f;           // +0x4c
    m_subBaseRates = 0.f;                    // +0x10
    m_passSubBaseRates = 0.f;                // +0x14
    memset(m_levelDiffRates, 0, sizeof(m_levelDiffRates));     // +0x18
    memset(m_difficultyRates, 0, sizeof(m_difficultyRates));   // +0x28
    memset(m_partyRates, 0, sizeof(m_partyRates));             // +0x3c
    m_guideLineValue = 0;                    // +0x90
    for (int i = 0; i <= 5; ++i)
    {
        m_field6c[i] = 0;
        m_field72[i] = 0;
        m_field7e[i] = 0;
        m_field84[i] = 0;
        m_field8a[i] = 0;
    }
}

// ORIG D1 0x891c7e2 W：仅析构 map 成员（其余 POD 无动作）。
stHellParty_t::~stHellParty_t() {}
