// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 advancealtar_misc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ advancealtar 杂项批次（Wave5 AltarMisc）。覆盖 ORIG 符号：
//   BuyUpgradeData::{operator= 0x88a5784 W, getFieldData 0x88a2c62, getFieldType
//     0x88a3012, getMaxLevel 0x88a3322, getMinLevel 0x88a33c0,
//     getMinLevelFieldDataPoint 0x88a2f1a, getStatFirstPos 0x88a2f7a,
//     getStatIconIndex 0x88a3056, getStatStringIndex 0x88a3106,
//     isValidLevel 0x88a345e}
//   BuyShopData::reset                                  0x088a1090 T
//   AchievementRewardData::reset                        0x0889ae56 T
//   SurvivalRewardData::getRoundRewardData              0x0889aebe T(const)
//   RewardParameter::getAchievementRewardData           0x0889b544 / 0x0889b5a8
//   AdvanceAltarShopParameter::getMaxStarResetGoldFee   0x088a2bc0 T
//   StageDifficulty::getNextDifficulty                  0x0889900a T
//   isOnEvent                                           0x0812cfa6 T
//   isOpenAdvanceAltar                                  0x0812cfea T
//   putRidableInfoToPacket                              0x08141781 T
//   AdvanceAltarData::{set 0x88992da, getBuyItemMap 0x88999d8,
//                      getSlotMap 0x8899a0c}
//   StageTimeLineParameter::{importPhase 0x889a930,
//                            checkAndSetActionDefine 0x889a032}
// 以及使用点发射的生命周期弱符号：
//   _StarRestFeeByGold C1/C2 0x88a3760（inline ctor，头内定义）
//   _ActionAssign C1/C2      0x889b99c（inline ctor，头内定义）
//   _PhaseSummary C1/C2/D1/D2 0x889b954 / 0x0889ebc0（inline ctor/dtor，头内定义）
//   _BuyItem D1/D2           0x0813b4cc（inline 空析构，随使用点发射）
// 逐函数对照 docs/class_func_reports/ 对应报告；报告缺处以 objdump 转写并标
// [objdump ADDR]。需要 CMake 登记。
#include "CDataManager.h"            // advancealtar 共享类型权威段（含 AdvanceAltarField.h）
#include "advancealtar.h"            // AdvanceAltarData/_AdvanceAltarDbData/_Slot/_Stage 等
#include "advancealtar_StageDifficulty.h"
#include "CDungeon.h"                // SurvivalRewardData/RoundRewardData
#include "GlobalData.h"              // s_event_manager / s_DailyScheduleManager
#include "CGuildServerProxy.h"       // CEventManager::GetRepeatEvent / CEventBase::IsEventing
#include "PacketGuard.h"             // PacketGuard : InterfacePacketBuf
#include "DNFLexWrapper.h"           // ScanType/ScanInt(bool*)/ScanStr
#include "StringResource.h"          // CStringManager（全局域类，LinkFinal 链接收口批）

#include <cstdio>
#include <cstring>

namespace advancealtar
{



// ============================================================================
// BuyUpgradeData 查询组（docs/class_func_reports/advancealtar__BuyUpgradeData/）
// ============================================================================

// ORIG W 0x88a5784（objdump 全文）：+0/+4 直拷、+8 仅低 1 字节（movzbl）、
// +0xc string 赋值、+0x10 vector<FieldType::T> 赋值、+0x1c LevelFieldDataMap
// 赋值；返回 *this。
BuyUpgradeData& BuyUpgradeData::operator=(const BuyUpgradeData& other)
{
    m_nItemId = other.m_nItemId;                                    // [objdump 0x88a578a]
    m_field4 = other.m_field4;                                      // [objdump 0x88a5794]
    m_field8 = (int)(unsigned char)other.m_field8;                  // [objdump 0x88a57a3 movzbl]
    m_strDesc = other.m_strDesc;                                    // [objdump 0x88a57c0 Ss::operator=]
    m_fieldTypes = other.m_fieldTypes;                              // [objdump 0x88a57d8 vector::operator=]
    m_listMap = other.m_listMap;                                    // [objdump 0x88a57f0 map::operator=]
    return *this;
}

// ORIG T 0x088a2c62：入口恒记 logFormat(line 0x26e)；m_listMap 命中 level 且
// 其行命中 ft 时返回 &FieldData；否则记 line 0x27b 错误日志后再以 key=1 做一次
// 相同两段查询并同式返回（继承 ORIG 尾部指令序 [objdump 0x88a2d5a..2e41]）。
// [b28d-solo-advancealtar_misc 判词] NEAR（134:134，助记符序列全等、仅槽号差）
//   已修：①返回类型 int→FieldData*（ORIG 两路 `call operator-> ; add $0x4,%eax`）；
//   ②第二段 find key 常量 1（P171 照抄）；③typeName 独立块（析构先于第二段 find）
//   使返回路径回归 ORIG `add $0x4,%eax`（块外声明时为 lea+ebx 中转）。
//   残余（N 封顶，P212/N93/N82附）：4.1 给 typeName 浅槽 -0x10、lit 深 -0x1c；
//   4.4 相反（typeName -0x24、lit -0x18），其余槽随动 4B（end2 -0x10↔-0x14、
//   end1 -0x14↔-0x18、fit -0x20↔-0x24、fit2 -0x1c↔-0x20；key -0xc / sret -0x2c
//   两侧相同）。P212 全头部声明法对非平凡类型不可用（提前声明 string 会引入
//   C1Ev + operator= 两处 call，实测 missing +2、更劣）；嵌套块/块内先声明 fit2/
//   迭代器先声明后赋值（P50）三形实测均不闭合。语义/调用点/指令数全等。
#include "advancealtar_misc/getFieldData.inc"
}

// ORIG T 0x088a3012：idx 越界 → -1；否则 m_fieldTypes[idx]。
#include "advancealtar_misc/getFieldType.inc"
}

// ORIG T 0x088a3322：初值 0，取 m_listMap 最大 key。
#include "advancealtar_misc/getMaxLevel.inc"
}

// ORIG T 0x088a33c0：初值 0，取 m_listMap 最小 key。
#include "advancealtar_misc/getMinLevel.inc"
}

// ORIG T 0x088a2f1a：out=0 起、失败再以 out=1 试一次，返回末次
// getFieldDataPoint 结果。
#include "advancealtar_misc/getMinLevelFieldDataPoint.inc"
}

// ORIG T 0x088a2f7a：首个落在 [FT_HP(0xa), FT_ETC2(0x18)] 的字段下标，否则 -1。
#include "advancealtar_misc/getStatFirstPos.inc"
}

// ORIG T 0x088a3056：ft∈[10,24] → 序号 0..14，缺省 -1。
#include "advancealtar_misc/getStatIconIndex.inc"
}

// ORIG T 0x088a3106（sret 返回 std::string）：FieldType→字符串表条目
// CStringManager::getStr(id)；区间外的 ft 返回空串。
#include "advancealtar_misc/getStatStringIndex.inc"
}

// ORIG T 0x088a345e：!(level<min || max<level)。
#include "advancealtar_misc/isValidLevel.inc"
}

// ============================================================================
// BuyShopData::reset（ORIG T 0x088a1090：+0=0 + 三 vector clear）
// ============================================================================
#include "advancealtar_misc/reset.inc"
}

// ============================================================================
// AchievementRewardData::reset（ORIG T 0x0889ae56 报告全文）
// ============================================================================
#include "advancealtar_misc/reset_1.inc"
}

// ============================================================================
// SurvivalRewardData::getRoundRewardData（ORIG T const 0x0889aebe 报告全文）：
// 首个满足 m_0<=round<=m_4 的 RoundRewardData → &元素.m_vec8，未命中 → 0。
// ============================================================================
#include "advancealtar_misc/getRoundRewardData.inc"
}

// ============================================================================
// RewardParameter::getAchievementRewardData 两个重载
// ============================================================================

// ORIG T 0x0889b544：m_rewardMap.find(type) 命中 → 元素指针，否则 0。
advancealtar::AchievementRewardData*
#include "advancealtar_misc/getAchievementRewardData.inc"
}

// ORIG T 0x0889b5a8：线性找 m_difficulty==diff 且 m_stageClearCount==count 的
// 条目（命中判断 [objdump 0x889b61c +0x8/+0xc]）；未命中 → 0。
advancealtar::AchievementRewardData*
#include "advancealtar_misc/getAchievementRewardData_1.inc"
}

// ============================================================================
// AdvanceAltarShopParameter::getMaxStarResetGoldFee（ORIG T 0x088a2bc0 报告全
// 文）：maxKey 初值 0 取 m_starRestMap 最大 key，返回 operator[](maxKey) 元素
// 值（空表时会插入默认元素，行为保留）。
// ============================================================================
#include "advancealtar_misc/getMaxStarResetGoldFee.inc"
}

// ============================================================================
// StageDifficulty::getNextDifficulty（ORIG T 0x0889900a 报告全文）
// ============================================================================
#include "advancealtar_misc/getNextDifficulty.inc"
}

// ============================================================================
// isOnEvent（ORIG T 0x0812cfa6，objdump 全文）：
//   eax=s_event_manager(0x0941f730)；GetRepeatEvent(0x77)；虚槽 vptr+0x34
//   （IsEventing(CUser*)）以参数 0 调用；返回 IsEventing!=false。
#include "advancealtar_misc/isOnEvent.inc"
}
// ============================================================================
#include "advancealtar_misc/isOpenAdvanceAltar.inc"
}

// ============================================================================
// putRidableInfoToPacket（ORIG T 0x08141781，报告全文）：前 0x21 字节直拷 +
// 七个 map 各 put_int(size) 后逐元素 put_binary(&value, 元素尺寸)。
// 去发明模板：ORIG nm 无 putMapEntries 实例（§4.1），七段逐循环内联，
// 循环体形态按 ORIG 汇编（begin 前置 0x0851417d2 区、end 每轮尾构、
// postfix it++ 带 dest 临时 0x08135f38、operator-> 后 +4/+1 直传 put_binary）。
// 残余 DIFF 归因（终态 insns 350 vs 349，槽图 -0x10/-0xc/-0x14..-0x64 与
// begin/end/++temp 三件套逐段对齐）：唯一真实指令差 = 首条 put_binary 的
// data 装载 ORIG 两段 `mov 0xc(%ebp),%eax; mov %eax,%edx`（0x08141787/8a），
// 4.4.7 恒折单段（N18a 负例，probe 不可翻）；其余 diff 行为该 1 行错位引发
// 的对齐伪差与 ORIG nearest-symbol call 显示行（put_binary 等公共 W 符号两侧
// 同名，relink 归一后消）。
// [b28d-solo-advancealtar_misc 判词] N 封顶（N18a 单段 regmove）：
//   fresh .o 350:349（gap 1），p167 调用点差集 CLEAR（57=57，0 missing/extra）；
//   唯一残差 = 首条 put_binary 的 `&data` 实参装载：ORIG 两段 `mov 0xc(%ebp),%eax;
//   mov %eax,%edx`，4.4.6 恒折单段 `mov 0xc(%ebp),%edx`。P248 三档旋钮已试：
//   `const_cast<char*>(reinterpret_cast<const char*>(&data))` 与
//   `reinterpret_cast<char*>(const_cast<AdvanceAltarData*>(&data))` 两 cast 形
//   产物相同（均单段）；形参类型受 mangled 约束（RKNS_16AdvanceAltarDataE）不可异型化；
//   ORIG 的 `mov mem,%eax; mov %eax,%reg` 属 4.1 冗余 regmove（N18a）不可造。
// ============================================================================
#include "advancealtar_misc/putRidableInfoToPacket.inc"
}

// ============================================================================
// AdvanceAltarData::{getBuyItemMap,getSlotMap,set}
// ============================================================================

// ORIG T 0x088999d8（非 const，nm 实证 _ZN 无 K）：0/1/2 →
// +0x3c/+0x54/+0x6c，其它 → 0。
#include "advancealtar_misc/getBuyItemMap.inc"
}

// ORIG T 0x08899a0c（非 const，nm 实证 _ZN 无 K）：1/2 → +0x84/+0x9c，
// 其它（含 0）→ 0。
#include "advancealtar_misc/getSlotMap.inc"
}

// ORIG T 0x088992da（objdump 全文，四段循环逐条转写）。序列化侧 get()
// （advancealtar_DB.cpp）仅落 value 域，因此本逆序列化中各记录的 map key 均
// 取自记录自身的 m_field0/m_byte0/m_int1 字段（与 make_pair 的同址取参一致，
// 见函数体注释地址）。
#include "advancealtar_misc/set.inc"
}

// ============================================================================
// StageTimeLineParameter::{importPhase,checkAndSetActionDefine}
// ============================================================================

// ORIG T 0x889a930（报告反编译 + 关键偏移 objdump 复核：Phase 局部对象 -0x24，
// [duration]→Phase+0、[type]→Phase+4、_ActionAssign 局部 -0x3c 基址，
// 3 次 ScanInt 分别写 +0 int/-8→short/-6→short，push 到 Phase+0x08）。
#include "advancealtar_misc/importPhase.inc"
}

// ORIG T 0x889a032（报告反编译全文）：回填 m_define；找不到 → printf + 0；
// 全部成功则写 summary（(phaseIdx, 累计时长起点, phase type)）并把累计时长存
// TimeLineSummary::m_field0（TimeLine+0xc），返回 1。
// [b28d-solo-advancealtar_misc 判词] DIFF→N 封顶（语义/调用点/控制流全等）：
//   已修①**static 成员**（ORIG 首参 timeLine 读 0x8(%ebp)、无 this；原非 static 声明
//   使 timeLine/defines 落在 0xc/0x10、整帧 +4 错位，见 CDataManager.h 声明处）；
//   ②_PhaseSummary::m_pad 字段化 + 逐字段直写（原 fields[3]+memcpy 多一次 memcpy 调用）；
//   ③`vector<_ActionAssign>& assigns = pit->m_assigns;` 复现 ORIG -0xc 地址缓存；
//   ④两循环后置 `++`（ORIG ppEi 0x0889bede/0x0889bf48 带 sret 临时）与**体内
//   `pit++` + 体末 `++phaseIndex`**（ORIG 0x0889a1d0 位于 add 之后、addl 之前）。
//   残余（N82附/跨版本）：dit(-0x34) 与 summary(-0x40) 槽互换——4.4 聚合对象优先占
//   低槽（summary 最深、dit 次深），4.1 按生命周期穿插（dit -0x40、summary -0x3c）；
//   其余槽（pit -0x2c/end -0x28/m_assigns.end -0x24/defines.end -0x20/ait ppEi -0x1c/
//   pit ppEi -0x18/totalDuration -0x14/phaseIndex -0x10/assigns -0xc/ait -0x30）全等；
//   全头部声明法对非平凡 summary 不可用。
//   [C07ExpertHeroAssault 2026-09-10 · P317 落地] 旧判词「EXTRA =
//   `_PhaseSummary::~_PhaseSummary()`×2 + `_Unwind_Resume`×1，ORIG 该 dtor 符号
//   0x0889ebc0 存在，按 P12 不可删」**已作废**：按 P317 删 `advancealtar.h` 的
//   用户空析构后，`nm /tmp/tu_advancealtar_misc.o` 证 `~_PhaseSummary()` 仍为
//   W ×2（使用点发射）⇒ 未丢符号。读数改前→改后：our_insns 155→**138**
//   （ORIG 138，gap −17→**0**），verdict DIFF→**NEAR**（残余 = dit(-0x34) 与
//   summary(-0x40) 槽互换 + 4.4 聚合对象优先占低槽 = N82附；无 push esi、无
//   `_Unwind_Resume`、无 dtor call 残差）。p167：MISSING 0 / EXTRA 0。
#include "advancealtar_misc/checkAndSetActionDefine.inc"
}

// ---- 布局断言（本批次相关）-------------------------------------------------
typedef char assert_slotfee_0x4[(sizeof(_StarRestFeeByGold) == 4) ? 1 : -1];

}  // namespace advancealtar
