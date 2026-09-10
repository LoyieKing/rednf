// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StScriptImporters/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// StScriptImporters.cpp —— st* 脚本结构 W 符号实现 + 全局导入函数（OrigStStructs 独占）。
//
// 覆盖 ORIG 真实符号（nm 实证均为 W = 头内联强度，本 TU 以类外定义提供同名符号）：
//   stQPGuideScript::stRecommendQPData  C1 0x891ba22 / C1ERKS_ 0x892f730 / D1 0x891ba6a
//                                      op= 0x892f7ea / clear 0x891ba80
//   stPartyMatchingInfo                 C1 0x891b78e / C1ERKS_ 0x892ead6 / D1 0x891ca12
//                                      op= 0x892ebe0 / clear 0x891b81c
//   stNeoplePCBANG                      C1 0x891acce / C1ERKS_ 0x89276f8 / D1 0x891c6c2
//                                      op= 0x89277be / clear 0x891ac9a
//   stBloodWarBgmTable                  C1 0x891a95c / C1ERKS_ 0x892d3b6 / D1 0x891c9fc
//                                      op= 0x892d464 / clear 0x891a9a4
//   stBloodPoint                        C1 0x891a9c4 / clear 0x891a9d8（importBloodWarEtcScript 依赖）
//   stUltimateWarMonsterInfo            C1 0x891aa04 / clear 0x891aa18（同上）
// 以及 ORIG T 导入函数：
//   importQPGuideScript    0x0890ff93 / importPartyMatchingInfo 0x0890f311
//   importPCBANGScript     0x08905773 / importBloodWarEtcScript 0x0890e3f8
//
// 报告：docs/class_func_reports/_lt_global_gt_/{importQPGuideScript,importPartyMatchingInfo,
//       importPCBANGScript,importBloodWarEtcScript}.md；W 符号无报告，objdump 直转
//       （汇编已逐条核对：clear/C1/C1ERKS_/D1/op= 各成员序与 ORIG 一致）。

#include <utility>
#include <vector>

#include "stQPGuideScript.h"
#include "stPartyMatchingInfo.h"
#include "stNeoplePCBANG.h"
#include "stBloodWarBgmTable.h"

#include "NpcScript.h"          // loadRDARScriptFile / getCharacterJob
#include "DNFLexWrapper.h"      // ScanType / ScanStr / ScanInt

// getGrowTypeIndex 定义于 InitScriptProducers_6.cpp（ORIG T 0x088be63d）；无权威头，
// 照 InitScriptProducers_7.cpp 局部声明模式。
#include "StScriptImporters/getGrowTypeIndex.inc"

// stBloodWarGrowTable / stBloodWarBloodPointTable —— importBloodWarEtcScript 形参类型
// （mangled P19stBloodWarGrowTable / P25stBloodWarBloodPointTable）。GCC 4.4 对
// typedef 到模板实例会展开 typedef 名，唯派生结构体才能产生 ORIG mangled。
// stBloodWarGrowTable 亦定义于 WarRoom.cpp（ImportWarRoomWPTable 形参，布局一致；
// 纯派生 struct 无成员函数，链接无符号冲突）。此处为 importBloodWarEtcScript
// 提供本 TU 视图。
struct stBloodWarGrowTable
    : public std::vector<std::vector<std::pair<int, int> > >
{
};
struct stBloodWarBloodPointTable
    : public std::vector<stBloodPoint>
{
};

// ============================================================================
// stQPGuideScript::stRecommendQPData（报告缺失，objdump 直转）
// ============================================================================

// ORIG C1 0x891ba22（0x48）：vector<int>(+0x8) C1 + clear()。
#include "StScriptImporters/stRecommendQPData.inc"
{
    clear();
}

// ORIG D1 0x891ba6a（0x16）：仅 vector<int> 析构。
#include "StScriptImporters/dtor_stRecommendQPData.inc"
{
}

// ORIG C1ERKS_ 0x892f730（0x36）：两 int + vector<int> 复制。
#include "StScriptImporters/stRecommendQPData_1.inc"
    : m_job(other.m_job), m_grow(other.m_grow), m_items(other.m_items)
{
}

// ORIG op= 0x892f7ea（0x39）：两 int + vector<int> 赋值。
stQPGuideScript::stRecommendQPData& stQPGuideScript::stRecommendQPData::operator=(
    stRecommendQPData const& other)
{
    m_job = other.m_job;
    m_grow = other.m_grow;
    m_items = other.m_items;
    return *this;
}

// ORIG clear 0x891ba80（0x3f）：+0x00/+0x04 int=-1、+0x08 vector clear + reserve(4)。
#include "StScriptImporters/clear.inc"
{
    m_job = -1;
    m_grow = -1;
    m_items.clear();
    m_items.reserve(4);
}

// ============================================================================
// stPartyMatchingInfo（报告缺失，objdump 直转）
// ============================================================================

// ORIG C1 0x891b78e（0x8e）：两 string + vector<int> 构造后 clear()。
#include "StScriptImporters/stPartyMatchingInfo.inc"
    : m_channelName(), m_regionName(), m_dungeonIdx()
{
    clear();
}

// ORIG D1 0x891ca12（0x6d）：vector + 两 string 逆序析构。
#include "StScriptImporters/dtor_stPartyMatchingInfo.inc"
{
}

// ORIG C1ERKS_ 0x892ead6（0x86）：两 string + vector<int> 复制。
#include "StScriptImporters/stPartyMatchingInfo_1.inc"
    : m_channelName(other.m_channelName),
      m_regionName(other.m_regionName),
      m_dungeonIdx(other.m_dungeonIdx)
{
}

// ORIG op= 0x892ebe0（0x4d）：string op= ×2 + vector op=。
stPartyMatchingInfo& stPartyMatchingInfo::operator=(stPartyMatchingInfo const& other)
{
    m_channelName = other.m_channelName;
    m_regionName = other.m_regionName;
    m_dungeonIdx = other.m_dungeonIdx;
    return *this;
}

// ORIG clear 0x891b81c（0x2f）：两 string clear + vector<int> clear。
#include "StScriptImporters/clear_1.inc"
{
    m_channelName.clear();
    m_regionName.clear();
    m_dungeonIdx.clear();
}

// ============================================================================
// stNeoplePCBANG（报告缺失，objdump 直转）
// ============================================================================

// ORIG C1 0x891acce（0x48）：vector<pair>(+0xc) C1 + clear()。
#include "StScriptImporters/stNeoplePCBANG.inc"
{
    clear();
}

// ORIG D1 0x891c6c2（0x16）：仅 vector<pair> 析构。
#include "StScriptImporters/dtor_stNeoplePCBANG.inc"
{
}

// ORIG C1ERKS_ 0x89276f8（0x42）：三 int + vector<pair> 复制。
#include "StScriptImporters/stNeoplePCBANG_1.inc"
    : m_field0(other.m_field0),
      m_level(other.m_level),
      m_peopleCount(other.m_peopleCount),
      m_status(other.m_status)
{
}

// ORIG op= 0x89277be（0x45）：三 int + vector<pair> 赋值。
stNeoplePCBANG& stNeoplePCBANG::operator=(stNeoplePCBANG const& other)
{
    m_field0 = other.m_field0;
    m_level = other.m_level;
    m_peopleCount = other.m_peopleCount;
    m_status = other.m_status;
    return *this;
}

// ORIG clear 0x891ac9a（0x33）：+0x00/+0x04/+0x08 int=0 + vector<pair> clear。
#include "StScriptImporters/clear_2.inc"
{
    m_field0 = 0;
    m_level = 0;
    m_peopleCount = 0;
    m_status.clear();
}

// ============================================================================
// stBloodWarBgmTable（报告缺失，objdump 直转）
// ============================================================================

// ORIG C1 0x891a95c（0x48）：string(+0x4) C1 + clear()。
#include "StScriptImporters/stBloodWarBgmTable.inc"
{
    clear();
}

// ORIG D1 0x891c9fc（0x16）：仅 string 析构。
#include "StScriptImporters/dtor_stBloodWarBgmTable.inc"
{
}

// ORIG C1ERKS_ 0x892d3b6（0x2a）：int + string 复制。
#include "StScriptImporters/stBloodWarBgmTable_1.inc"
    : m_id(other.m_id), m_name(other.m_name)
{
}

// ORIG op= 0x892d464（0x2d）：int + string 赋值。
stBloodWarBgmTable& stBloodWarBgmTable::operator=(stBloodWarBgmTable const& other)
{
    m_id = other.m_id;
    m_name = other.m_name;
    return *this;
}

// ORIG clear 0x891a9a4（0x1f）：+0x00 int=-1 + string clear。
#include "StScriptImporters/clear_3.inc"
{
    m_id = -1;
    m_name.clear();
}

// ============================================================================
// stBloodPoint / stUltimateWarMonsterInfo（importBloodWarEtcScript 依赖，报告缺失）
// ============================================================================

// ORIG C1 0x891a9c4（0x13）= clear()；D1 0x893f604 trivial（5B）。
#include "StScriptImporters/stBloodPoint.inc"
{
    clear();
}

// ORIG clear 0x891a9d8（0x2c）：+0x00=4、+0x04/+0x08/+0x0c=0。
#include "StScriptImporters/clear_4.inc"
{
    m_type = 4;
    m_v1 = 0;
    m_v2 = 0;
    m_v3 = 0;
}

// ORIG C1 0x891aa04（0x13）= clear()；D1 0x893f858 trivial（5B）。
#include "StScriptImporters/stUltimateWarMonsterInfo.inc"
{
    clear();
}

// ORIG clear 0x891aa18（0x22）：三 int=0。
#include "StScriptImporters/clear_5.inc"
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
}

// ============================================================================
// importQPGuideScript —— ORIG T 0x0890ff93（报告全文转写）
// ============================================================================
#include "StScriptImporters/importQPGuideScript.inc"
{
    // [SweepStScriptImporters 2026-09-10] 判词：AE（本件 DIFF→AE；TU 四数 227/448/0/3）。
    // 对照 ORIG 0x0890ff93 逐块修复（本波，原 388/396）：
    //  ① 推荐段 ScanStr(&jobStr) 成功路径补 `bRec2 = true;`（ORIG 0x08910092 movb $1,-0x16）；
    //  ② ScanStr(&growStr) 改否定形 if(!...)（复现 ORIG 0x089100ea xor$1;test;je <成功>）；
    //  ③ data.m_job=job 前置于 int grow=-1（ORIG 0x089100ca/0x089100cd）；
    //  ④ data.m_grow=grow 前置于 int count=0（ORIG 0x0891012e/0x08910134）；
    //  ⑤ tooltip 段 key/val 声明移出 while 体 + 体内 key=-1 重置（ORIG 0x089102ba/c1/d1）；
    //  ⑥ tooltip empty 检查改否定 if/else（ORIG 0x089102f8 xor$1;test;je <return0>）；
    //  ⑦ val.compare("") 改「空 then + goto 尾标签」形（P223，复现 ORIG 0x08910367
    //     sete;test;jne <nop 岛 0x0891043c>）；
    //  ⑧ map find 改具名左操作数 iterator it=find(key); if(it==end())（P219：翻求值序
    //     end→find 为 ORIG 的 find→end，sret 槽 -0x6c/-0x38 与 ORIG 全等）；
    //  ⑨ 尾部 3×if-return 改单一 return(!bRec&&!bRec2&&!bTool)（ORIG 0x0891047f 三连
    //     movzbl;xor$1;test;je + movzbl %al,%ebx）。
    // 验证：insns 396=396；**jmp+jcc 67/67 目标下标逐位全等**（含无条件 jmp，mismatch=0；
    // 回边 4=4 方向一致 ⇒ 非 clean() 目标归一假匹配，无 goto-退出）；结构化 diff 仅剩
    // 4 处字符串立即数（ORIG 报告 $"..."/我方 .o 重定位占位，AE 归一化）；p167 48/48
    // （MISSING/EXTRA 仅 C1/C2、D1/D2 同址别名，P93）；槽位集全等（token -0x3c / data -0x50 /
    // jobStr -0x54 / growStr -0x58 / count -0x5c / itemIdx -0x60 / key -0x64 / val -0x68 /
    // find -0x6c / end -0x38 / job -0x14 / grow -0x10 / i -0xc / bRec2 -0x16 / bRec -0x17）。
    if (!loadRDARScriptFile("", path))
        return 0x28b6;
    std::string token;
    bool bRec = true;    // [qp recommendation] 段可解析标志（-0x17）
    bool bRec2 = true;   // [qp recommendation] 记录循环标志（-0x16）
    bool bTool = true;   // [qp tooltip mapping] 循环标志（-0x15）
    stQPGuideScript::stRecommendQPData data;

    while (bRec)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[qp recommendation]")
        {
                while (bRec2)
                {
                    data.clear();
                    int job = -1;
                    std::string jobStr;
                    if (!ScanStr(&jobStr))
                    {
                        // ScanStr 失败：out[10] 已填充则结束记录段，否则返回 0。
                        if (!out.m_vec0.at(10).empty())
                        {
                            bRec2 = false;
                            break;
                        }
                        else
                        {
                            return 0;
                        }
                    }
                    else
                    {
                        bRec2 = true;
                        job = getCharacterJob(jobStr.c_str());
                        if (job < 0 || job > 10)
                            return 0;
                        data.m_job = job;
                        int grow = -1;
                        std::string growStr;
                        if (!ScanStr(&growStr))
                        {
                            return 0;
                        }
                        else
                        {
                            grow = getGrowTypeIndex(growStr.c_str());
                            if (grow == 0xffff)
                                return 0;
                            data.m_grow = grow;
                            int count = 0;
                            if (!ScanInt(&count))
                                return 0;
                            if (count <= 0)
                                return 0;
                            int itemIdx = -1;
                            for (int i = 0; i < count; ++i)
                            {
                                if (!ScanInt(&itemIdx))
                                    return 0;
                                data.m_items.push_back(itemIdx);
                            }
                            if (job < 0 || job > 10)
                                return 0;
                            if (out.m_vec0.size() != 0xb)
                                return 0;
                            out.m_vec0[job].push_back(data);
                        }
                    }
                }
            }
            else if (token == "[qp tooltip mapping]")
            {
                int key = -1;
                std::string val;
                while (bTool)
                {
                    key = -1;
                    if (!ScanInt(&key))
                    {
                        if (!out.m_tooltipMapC.empty())
                        {
                            bTool = false;
                            bRec = false;
                            break;
                        }
                        else
                        {
                            return 0;
                        }
                    }
                    if (key < 0)
                        return 0;
                    if (!ScanStr(&val))
                        return 0;
                    if (!val.compare(""))
                        goto tooltip_skip;
                    {
                        std::map<int, std::string>::iterator it = out.m_tooltipMapC.find(key);
                        if (it == out.m_tooltipMapC.end())
                        {
                            out.m_tooltipMapC.insert(std::make_pair(key, val.c_str()));
                        }
                    }
tooltip_skip:;    // [P223] 空 then 占位岛锚点（复现 ORIG 0x0891043c 的 nop 岛）
                }
            }
    }
    return (!bRec && !bRec2 && !bTool);
}

// ============================================================================
// importPartyMatchingInfo —— ORIG T 0x0890f311（报告全文转写）
// ============================================================================
#include "StScriptImporters/importPartyMatchingInfo.inc"
{
    // [b28d-solo-StScriptImporters] AE：指令数 180=180、p167 26/26 CLEAR。
    // [SweepStScriptImporters 2026-09-10 复核] jmp+jcc 28/28 目标下标逐位全等（回边 8=8）。
    // 修复：①补 ORIG -0x14 死 string 局部（槽序/析构序列照抄，P232/P171）；
    // ②外层改 for(;;){ if(!ScanType) break; } 破 4.4 循环旋转；
    // ③do-while 改 do{...if(!ok)break;}while(true) 复现 `movzbl;xor$1;test;je` + 入口 jmp；
    // ④[dungeon index] 的 while 改 for(;;){ if(!(idx=ScanInt(&ok),ok)) break; ... }。
    if (!loadRDARScriptFile("", path))
        return 0;
    std::string token;
    std::string str;    // [b28d-solo-StScriptImporters] ORIG -0x14 死 string 局部（槽序/析构序列照抄，P232/P171）
    bool ok = false;
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[common dungeon index]")
        {
            int v;
            do
            {
                v = ScanInt(&ok);
                if (!ok)
                    break;
            } while (true);
        }
        else if (token == "[channel name]")
        {
            stPartyMatchingInfo info;
            ScanStr(&info.m_channelName);
            while (true)
            {
                if (ScanType(token, true) && token == "[/channel name]")
                    break;
                if (token == "[region name]")
                {
                    ScanStr(&info.m_regionName);
                }
                while (true)
                {
                    if (ScanType(token, true) && token == "[/region name]")
                        break;
                    if (token == "[dungeon index]")
                    {
                        int idx;
                        for (;;)
                        {
                            if (!(idx = ScanInt(&ok), ok))
                                break;
                            info.m_dungeonIdx.push_back(idx);
                        }
                        out->push_back(info);
                    }
                }
            }
        }
    }
    return 1;
}

// ============================================================================
// importPCBANGScript —— ORIG T 0x08905773（报告全文转写）
// ============================================================================
#include "StScriptImporters/importPCBANGScript.inc"
{
    // [b28d-solo-StScriptImporters] AE：指令数 393=393、p167 69/69 CLEAR。
    // [SweepStScriptImporters 2026-09-10 复核] jmp+jcc 46/46 目标下标逐位全等（回边 21=21）。
    // 修复：①外层改 for(;;){ if(!ScanType) break; } 破 4.4 循环旋转（ORIG 入口 jmp cond +
    // 各 case 尾 jmp cond）；②补 ORIG -0x10/-0xc 两个死 int 局部（P232 槽序，令
    // token/data/statusPair 槽位 -0x14/-0x2c/-0x34 与 ORIG 精确对齐）。
    if (!loadRDARScriptFile("", path))
        return 0x1417;
    std::string token;
    int dead0 = 0;    // [b28d-solo-StScriptImporters] ORIG -0x10 死局部（P232 槽序）
    int dead1 = 0;    // [b28d-solo-StScriptImporters] ORIG -0xc 死局部
    out.clear();
    stNeoplePCBANG data;
    std::pair<ENUM_CHANGE_STATUS_TYPE, int> statusPair;
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[level]")
        {
            data.m_level = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[people count]")
        {
            data.m_peopleCount = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[physical attack]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[physical defense]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(3);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[magical attack]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(1);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[magical defense]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(2);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[fire resistance]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x14);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[water resistance]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x15);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[dark resistance]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x16);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[light resistance]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x17);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[HP regen speed]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0xe);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[MP regen speed]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x1d);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[move speed]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0xb);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[attack speed]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(10);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[cast speed]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0xc);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[hit recovery]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x22);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[jump power]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(8);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[HP MAX]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0xd);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[MP MAX]")
        {
            statusPair.first = static_cast<ENUM_CHANGE_STATUS_TYPE>(0x30);
            statusPair.second = ScanInt(static_cast<bool*>(0));
            data.m_status.push_back(statusPair);
        }
        else if (token == "[/level]")
        {
            out.push_back(data);
            data.clear();
        }
    }
    return 1;
}

// ============================================================================
// importBloodWarEtcScript —— ORIG T 0x0890e3f8（报告全文转写）
// ============================================================================
#include "StScriptImporters/importBloodWarEtcScript.inc"
                            stBloodWarGrowTable* growTable,
                            stBloodWarBloodPointTable* bloodPoint,
                            std::vector<stBloodWarBgmTable>* bgm,
                            // [b28d-solo-StScriptImporters] 判词：指令数 338=338、p167 43/43 CLEAR。
                            // [SweepStScriptImporters 2026-09-10 复核] 跳转 62/60：回边 11=11
                            // 方向一致（循环多轮语义完好，无 goto-退出 假匹配），仅少 2 个
                            // 前向 jmp（do-while 入口岛，N 族）。
                            // 本波修复：①register int cont（P231 纯 register，非绑定）复现 ORIG
                            // 全程 %ebx + test %ebx；②if(!ok)/else if(!ScanStr)/else 结构复现
                            // `xor $1;test;je` 物化；③外层与 [BLOOD POINT] 改 for(;;)+break 复现
                            // ORIG 条件头在顶（破 4.4 循环旋转）。
                            // 残留 N：do-while 出口 lowering 极性 + 回边岛位置——ORIG 4.1 发
                            // `test %ebx,%ebx; je <顶部岛 nop;jmp cond>; jmp body`（每分支独立回边岛，
                            // 3 岛在循环头前），4.4 恒 `test %ebx,%ebx; jne body;` 落空合并回边
                            // （岛移至尾部）。for(;;)+break / do-while(true)+break / while(cont!=0)
                            // / goto 尾标签四形实测 348/348/338(setne 劣化)/339，均不可翻 ⇒ N 封顶，
                            // 语义等价（p167 CLEAR）。
                            std::vector<stUltimateWarMonsterInfo>* monsterInfo)
{
    if (!loadRDARScriptFile("", path))
        return 0;
    std::string token;
    std::string str;
    bool ok = false;
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[BGM]")
        {
            register int cont;    // [b28d-solo-StScriptImporters] ORIG 全程 %ebx + test %ebx（P231 纯 register，非绑定）
            do
            {
                stBloodWarBgmTable bgmTable;
                bgmTable.m_id = ScanInt(&ok);
                if (!ok)
                    cont = false;
                else if (!ScanStr(&str))
                    cont = false;
                else
                {
                    bgmTable.m_name = str;
                    if (bgm)
                        bgm->push_back(bgmTable);
                    cont = true;
                }
            } while (cont);
        }
        else if (token == "[BP TABLE]")
        {
            register int cont;    // [b28d-solo-StScriptImporters] ORIG 全程 %ebx（P231）
            do
            {
                std::vector<std::pair<int, int> > rows;
                for (int i = 0; i < 5; ++i)
                {
                    std::pair<int, int> p;
                    p.first = ScanInt(&ok);
                    if (!ok)
                        break;
                    p.second = ScanInt(&ok);
                    if (!ok)
                        break;
                    rows.push_back(p);
                }
                if (!ok)
                    cont = false;
                else
                {
                    if (growTable)
                        growTable->push_back(rows);
                    cont = true;
                }
            } while (cont);
        }
        else if (token == "[BLOOD POINT]")
        {
            for (;;)
            {
                if (!ScanStr(&str))
                    break;
                int type = 4;
                if (str == "named")
                    type = 0;
                else if (str == "crazy")
                    type = 1;
                else if (str == "round")
                    type = 2;
                else if (str == "nomal")
                    type = 3;
                int v1 = ScanInt(&ok);
                if (!ok)
                    break;
                int v2 = ScanInt(&ok);
                if (!ok)
                    break;
                int v3 = ScanInt(&ok);
                if (!ok)
                    break;
                if (bloodPoint && type != 4)
                {
                    stBloodPoint bp;
                    bp.m_type = type;
                    bp.m_v1 = v1;
                    bp.m_v2 = v2;
                    bp.m_v3 = v3;
                    bloodPoint->push_back(bp);
                }
            }
        }
        else if (token == "[BLOOD INFO]")
        {
            stUltimateWarMonsterInfo info;
            info.m_field0 = ScanInt(&ok);
            if (!ok)
                break;
            info.m_field4 = ScanInt(&ok);
            if (!ok)
                break;
            info.m_field8 = ScanInt(&ok);
            if (!ok)
                break;
            monsterInfo->push_back(info);
        }
    }
    return 1;
}
