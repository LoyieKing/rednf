// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 advancealtar_DB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ advancealtar DBAndExchangeServerProc + APSystem CDBAndExchangeServerProc
// 独立注册 TU（2026-08-23；2026-08-27 AltarDB 批追加 DB_Load/DB_Update/DB_StatisticAdvanceAltar/
// convertStringToZip/convertZipToString/g_*Buffer 全局）。实现：
//   advancealtar::DBAndExchangeServerProc::getFromMemory          ORIG T 0x0813394c
//   advancealtar::CharacAdvanceAltarManager::getCharacAdvanceAltarDbData ORIG T 0x08131710
//   advancealtar::CharacAdvanceAltarManager::isUpdated           ORIG T 0x081316a0
//   advancealtar::AdvanceAltarData::{get,getSlot,getBuyItem}     ORIG T 0x088996d8 / ...
//   advancealtar::DB_Update::makeRequest                         ORIG T 0x0813eb38
//   APSystem::CDBAndExchangeServerProc::GetActionPointFromMemory ORIG T 0x08121616（纯空函数）
//   （追加段清单见文件尾注释块）
// ----------------------------------------------------------------------------
// [b28d-D03 本波 判词]（fresh `check_tu_routed.sh`：identical=83 ae=148（合计 231）
//   near=0 diff=6 = _Z41__static_initialization…（N123 豁免）+ 下列 5 件；
//   p167 调用点差集对三件实跑：ORIG calls == OUR calls（31/31、26/26、48/48），
//   MISSING/EXTRA 仅为 C1↔C2、D1↔D2 同址别名伪差（P93）⇒ 语义无缺口）
//   · getFromMemory 0x0813394c —— 32/31：唯一残差 = P211 档③ store 装载序
//     （ORIG `mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,(%eax)` RHS-first + edx 中转；
//     我方 LHS-first 直存）。字段侧/callee 侧两旋钮历轮已试（判词见函数体）⇒ 封 N76 族。
//   · requestLoadFromDBAndES 0x08133784 —— 123/122：同 P211 store 序 1 条 + EH 段
//     `_Unwind_Resume` 我方缺失（CStreamGuard dtor 的 cleanup 合并差异，P216/P93 别名）；
//     p167 26/26 且仅 C1/C2、D1/D2 别名差 ⇒ 语义等价封顶。
//   · DB_StatisticAdvanceAltar::dispatch 0x081412b2 —— 312/308：p167 31/31（仅 cMyTrace
//     C1↔C2 别名差）；gap −4 = 跨版本 EH 域形态（sprintf/__cxa_allocate_exception/throw 段
//     的常量地址伪代码化 + 我方少 4 条 cleanup 装载）⇒ N104（EH 尾块相对序）封顶。
//   · DB_Update::update 0x0813ec5e —— 468/469：宽度直方图差 ext_movzwl orig=4/our=6
//     （我方多 2 条 16 位零扩展）+ 帧 0xdc vs 0xbc（我方紧凑 0x20）；p167 48/48 仅
//     std::string C1/C2、D1/D2 别名差 ⇒ N82（帧/槽）+ P19/N87 形态封顶。
//   · DB_Load::SendResult 0x0813ea46 —— 66/71：ORIG 0x0813ea54 `cmpb $0x0,-0x1c(%ebp);
//     je`（bool 形参截断槽直比）vs 4.4 `movzbl; xor $1; test; je` ⇒ N122（4.4 bool 取反
//     恒 xor；h1..h5 五探针全败）+ N93附（尾块 `mov $1,%ebx` 物化位置）封顶。
#include <cstring>
#include "CUser.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "advancealtar.h"
#include "PacketTypes.h"  // MSG_CHARAC_SELECT 完整类型（requestLoadFromDBAndES 读 m_slot）
#include <cstdio>
#include "DBDispatcher.h"
#include "AdvanceAltarZip.h"    // compress_zip/uncompress_zip 共享声明（唯一实现在 APSystemOnlyOrig.cpp）
#include "LogManager.h"
#include "GameTypes.h"          // cMyTrace
#include "CEnvironment.h"       // ENUM_DB_HANDLE_IDX / ENUM_SERVER_GROUP

#include <cstdlib>
#include <ctime>
#include <string>
#include "DBMgr.h"          // DBMgr 完整类型（GetDBHandle）+ MySQL.h
#include "CStreamGuard.h"

namespace advancealtar
{

// [objdump 0x813dc48] b28d-26: AE。修法=两处无符号比较取「否定早返」正向形
//   （zipCap <= zlen / strCap <= rawLen）+ zlen 同型化 unsigned（P209 姊妹：4.4 对
//   unsigned 形参比较以形参为 dst 直发 ja）。ORIG 0x813dc73 / 0x813dcda 实证。
#include "advancealtar_DB/convertZipToString.inc"
}

// ---- AdvanceAltarData 数据模型（advancealtar__AdvanceAltarData 报告）----
#include "advancealtar_DB/AdvanceAltarData.inc"
}

#include "advancealtar_DB/dtor_AdvanceAltarData.inc"
}

// ORIG T 0x08899b4e：把 map<uchar,_Slot> 拷入定长槽数组（槽 0 长度 10）。
#include "advancealtar_DB/getSlot.inc"
}

// ORIG T 0x08899a3a：把 map<int,_BuyItem> 拷入定长购买项数组（每项 12）。
// [b28d-solo-advancealtar_DB] AE（orig 83 / ours 83，clean resid=0）。同 getSlot：
//   语义 ORIG 0x08899a75 `test; setle %al; test; je <+0x5d>` ⇒ **x<=0 才 ++it**。
//   归零钥匙：① 第一检查 `< 1` 直写（setle 形）；② 第二检查按 P222 正极性 `>0`
//   写（ORIG 0x08899ab0 jle 落 memcpy）。两处联动才归零。
#include "advancealtar_DB/getBuyItem.inc"
}

// ORIG T 0x088996d8：把 AdvanceAltarData 序列化到 _AdvanceAltarDbData（0x518）。
#include "advancealtar_DB/get.inc"
}

// ---- CharacAdvanceAltarManager（CUser.h 声明）----
// ORIG T 0x08131710：把管理器内单个 AdvanceAltarData 拷到 _CharacAdvanceAltarDbData.m_db。
#include "advancealtar_DB/getCharacAdvanceAltarDbData.inc"
}

// ORIG T 0x08899244（objdump 全文核对）：memset 前 0x21（_RidableBase）→
// m_update=0 → 7 个 map clear（stages/buyItems1..3/slots1/slots2/rewards）。
#include "advancealtar_DB/reset.inc"
}

// ORIG T 0x081316a0：检查首个 AdvanceAltarData 的更新标志（manager+0xd0 == m_data.m_update）。
#include "advancealtar_DB/isUpdated.inc"
}

// ORIG T 0x813159a：槽位坐骑有效时把免费票补为配置值（CDataManager+0x6364
// = m_altarConfig.m_0）并置 m_data.m_update=1。
#include "advancealtar_DB/refillTicketFree.inc"
}

// ORIG T 0x08131660（docs/class_func_reports/advancealtar__CharacAdvanceAltarManager/
// reset.md 全文）：current(+0x00)=0；循环 local_10<=0 一次 reset +i*0xd0+4 处 AdvanceAltarData。
#include "advancealtar_DB/reset_1.inc"
}

// ----------------------------------------------------------------------------
// +0x00 承载 AdvanceAltarData* current（setCurrentAdvanceAltarData 0x08131254
// 实证：命中槽位写 this+4+i*0xd0，无匹配置 0；reset 置 0）。权威头 advancealtar.h
// 以 union（int m_field0 ∪ AdvanceAltarData* m_currentAdvanceAltarData）承载，
// 以下 const 访问器直接取具名指针成员。
// ----------------------------------------------------------------------------

// ORIG T 0x081312d0（getCurrentRidableId.md 全文）：current 空 → 0；
// 否则 *current（AdvanceAltarData::m_ridableId，_RidableBase+0x00）。
#include "advancealtar_DB/getCurrentRidableId.inc"
}

// ORIG T 0x081312ec（isAvailableTciket.md 全文）：current 空 → 0；
// 否则 *(int*)(cur+4) + *(int*)(cur+8) > 0（免费票 + 现持票）。
#include "advancealtar_DB/isAvailableTciket.inc"
}

// ORIG T 0x0813131a（isEnterStage.md 全文）：current 空 → 0；在
// current->m_stages(+0x24, map<int,_Stage>) find(stageId) 命中 → 1。
#include "advancealtar_DB/isEnterStage.inc"
}

// ---- getTowerInfo（ORIG T 0x081328ea，getTowerInfo.md 全文）----
// current(+0x00) 空→0；current->m_buyItems3(+0x6c) find(towerIdx) 未命中→0；
// G_CDataManager()+0x631c（m_altarShop）.getBuyUpgradeDataVec(*current,2) 空→0；
// 向量中 m_nItemId==towerIdx 项未命中→0；getFieldDataPoint(买项.m_short8,
// FT_INDEX=1) 空→0；买项计数 m_short8==0→0；否则 *out1=FieldData+4(m_value)、
// *out2=(int)m_short8，返回 1。
#include "advancealtar_DB/getTowerInfo.inc"
}

// ---- DB_Update（advancealtar__DB_Update 报告）----
// ORIG T 0x0813eb38：opcode 0x1a | uid | memcpy 0x51c → DB_Q。
#include "advancealtar_DB/makeRequest.inc"
}

// ---- DBAndExchangeServerProc（advancealtar__DBAndExchangeServerProc 报告）----
// ORIG T 0x0813394c：characNo→data.m_characNo；拷 AdvanceAltarData；isUpdated 则 DB_Update。
// [b28d-solo-advancealtar_DB] N76 封顶（orig 31 / ours 30，clean 归一 resid=5）：
//   ORIG 0x0813395d `mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,(%eax)`（RHS-first
//   +edx 中转）；4.4 对 `data.m_characNo = (int)getCurCharacNo()` 恒 LHS-first
//   `mov 0xc(%ebp),%edx; mov %eax,(%edx)`。已试形：(unsigned)cast/(int)cast/裸形/
//   P226 语句表达式（后者升格 ebx 破帧、`push %ebx; sub $0x14`）——字段与 callee
//   返回同为 unsigned 无可动旋钮（P211 反例）⇒ 无 C++98 源形可复现。语义等价。
//   if/else 空 else 复现 ORIG 体尾 jmp+nop 岛（P285）保留。
#include "advancealtar_DB/getFromMemory.inc"
}

// ---- requestLoadFromDBAndES（advancealtar__DBAndExchangeServerProc 报告全文）----
// ORIG T 0x08133784：reset 管理器 → Acquire stream(opcode 0x19) → uid →
// DisableCharacInfo(10) →（ValidLastLogin 时补 channelNo/characNo）→
// GetInBuffer<_CharacAdvanceAltarDbData> 清零 0x51c 后写 m_characNo →
// ValidLastLogin ? put 队列5 : put DB_Q(2) → 返回 1。
// [b28d-solo-advancealtar_DB] N76+N93附+N104 封顶（orig 123 / ours 122，clean resid=25）：
//   (1) `data->m_characNo = get_charac_no(...)`：ORIG 0x081338d6 LHS-first 直存，
//       4.4 对**局部指针**字段 store 恒 RHS-first+edx 中转（探针 f2 形参指针=LHS-first，
//       局部指针形恒值先）⇒ N76 族；
//   (2) 尾块 `mov $0x1,%ebx`：ORIG 两臂各一份（4.1 把 return true 内联到两臂），
//       4.4 只在共享尾块一份 ⇒ N93附/跨版本 CFG；
//   (3) EH landing 块相对序（ORIG 居中、4.4 压尾）⇒ N104。
//   P167 CLEAR（26/26）、语义等价。
#include "advancealtar_DB/requestLoadFromDBAndES.inc"
}

} // namespace advancealtar

namespace APSystem
{

// ORIG T 0x08121616（纯空函数）：行动力从内存缓存加载——本版本 ORIG 体为空。
#include "advancealtar_DB/GetActionPointFromMemory.inc"
}

} // namespace APSystem

// ---- CUser 跨类叶子助手（Prod_GlobalAdv；全局作用域，置于文件尾避免命名空间混淆）----

// ORIG W 0x081348ce _ZN5CUser17DisableCharacInfoE13CACHE_SECTION：
// movb $0,(this+0x8d0fd+section) —— m_pad8d0fd[section] 清零。
#include "advancealtar_DB/DisableCharacInfo.inc"
}

// ============================================================================
// 2026-08-27 AltarDB 批追加段（ORIG 全真转写；报告 docs/class_func_reports/
// advancealtar__DB_Load|DB_Update|DB_StatisticAdvanceAltar/ + objdump）
//   DB_Load      : C1/C2 W 0x844ddea、D1/D2 W 0x1420a0、D0 W 0x1420d0、
//                  dispatch T 0x813ddb2、Select T 0x813de40、SendResult T 0x813ea46、
//                  parseStage/parseSlot/parseBuyItem/parseReward T 0x813e3e8/...
//   DB_Update    : C1/C2 W 0x844de06、D1/D2 0x1420ee、D0 0x14211e、
//                  dispatch 0x813ec24、update 0x813ec5e、make*String 0x813f380..
//   DB_Statistic : C1/C2 W 0x844fc3a、D1/D2 0x142952、D0 0x142982、dispatch T 0x81412b2
//   convertStringToZip  T 0x813dd3e   convertZipToString T 0x813dc48
//   g_Zip*/g_Str* 8 个 B 全局（各 0x800，ORIG @0x09400380..0x09403b80）
// convert* 前向声明（实现于本 TU 文件尾 namespace advancealtar 内；Select/update 先用）
namespace advancealtar
{
#include "advancealtar_DB/convertStringToZip.inc"
#include "advancealtar_DB/convertZipToString_1.inc"
}  // namespace advancealtar

#include "SigTypes.h"  // STAdvanceAltarEntranceData / STAdvanceAltarLayerStatData（GetOutBuffer 实例化需要 sizeof）

// ORIG _Z19__GetCurrentDNFDateR2tm（T 0x86b1fc0，定义 GameUtils.cpp）——仓库暂无头声明。
#include "advancealtar_DB/__GetCurrentDNFDate.inc"

namespace advancealtar
{

// ---- ORIG namespace 内 static 常量副本（本 TU 使用处引用；各 TU 独立发射
//      _ZN12advancealtarL...E，与 ORIG 多副本形态一致）----
static const int kMaxStrBuffer = 2048;          // 0x08b6d3d0
static const int kMaxZipBuffer = 2048;          // 0x08b6d3cc
static const char* const kDelimterItem = "|";   // 0x08b6d3d8 指针 → '|'(0x7c)
static const char* const kDelimterField = ",";  // 0x08b6d3dc 指针 → ','(0x2c)
static const int kMaxStageCount = 50;          // 0x08af2e6c
static const int kMaxSlotCount = 12;           // 0x08af2e70
static const int kMaxBuyItemCount = 50;        // 0x08af2e74
static const int kMaxRewardCount = 30;         // 0x08af2e78

// ---- 8 个全局扁平缓冲区（ORIG B 符号，各 0x800 字节）----
char g_ZipStageBuffer[0x800];     // 0x09400380
char g_StrStageBuffer[0x800];     // 0x09400b80
char g_ZipSlotBuffer[0x800];      // 0x09402380
char g_StrSlotBuffer[0x800];      // 0x09402b80
char g_ZipBuyItemBuffer[0x800];   // 0x09401380
char g_StrBuyItemBuffer[0x800];   // 0x09401b80
char g_ZipRewardBuffer[0x800];    // 0x09403380
char g_StrRewardBuffer[0x800];    // 0x09403b80

// charac_advance_altar 行头字段视图：改用共享头 advancealtar.h 的 _RidableDbHead
// （union 叠加在 _AdvanceAltarDbData::m_ridable 上，字段偏移/宽度按 ORIG
//  Select.md/update.md 指令实证）。原 TU 本地视图 DbHeadView 因自然对齐与
// ORIG 指令宽度不符（+0x04/+0x08 ORIG 为 get_short(int,int&) 的 4 字节槽，
// 且 +0x0c 起三列 ORIG 间隔 4 字节）而删除。

// ---- DB_Load ----
class DB_Load : public DBDispatcher
{
public:
    DB_Load();
    ~DB_Load();
    virtual bool dispatch(int uid, int uniqueId, Stream* stream);
    bool Select(_CharacAdvanceAltarDbData& data);
    bool SendResult(int uid, int uniqueId, _CharacAdvanceAltarDbData& data, bool ok);
    bool parseStage(char* str, _Stage* out, int count);
    bool parseSlot(char* str, _Slot* out, int count);
    bool parseBuyItem(char* str, _BuyItem* out, int count);
    bool parseReward(char* str, _Reward* out, int count);
};

// [objdump 0x844ddea] base ctor + 设派生 vptr —— 编译器生成。
#include "advancealtar_DB/DB_Load.inc"
#include "advancealtar_DB/dtor_DB_Load.inc"
#include "advancealtar_DB/dispatch.inc"
}

// [report Select.md / objdump 0x813de40]
#include "advancealtar_DB/Select.inc"
}

// [report SendResult.md / objdump 0x813ea46]
// [b28d-solo-advancealtar_DB] N122+N93附 封顶（orig 66 / ours 71，clean resid=15）：
//   (1) ORIG 0x0813ea54 `cmpb $0x0,-0x1c(%ebp); je <mov $1 尾岛>`（bool 形参截断槽
//       直比）；4.4 对 `if (!ok)` 恒 `movzbl; xor $0x1; test; je`（N122，探针
//       h1..h5 五形实测全败，唯一 cmpb 形 h2 的落点/块序相反）；
//   (2) 尾块 `mov $0x1,%ebx` 物化位置差异（N93附）。语义等价。
#include "advancealtar_DB/SendResult.inc"
}

// 以下四个 parse* 共同模式：外层 strtok_r(str,"|")，内层 strtok_r(0,",")，
// 字段序按结构布局写入 [字节偏移 per 报告 asm shl 表]。

// [report parseStage.md / objdump 0x813e3e8]
#include "advancealtar_DB/parseStage.inc"
}

// [report parseSlot.md / objdump 0x813e542]
#include "advancealtar_DB/parseSlot.inc"
}

// [report parseBuyItem.md / objdump 0x813e708]
#include "advancealtar_DB/parseBuyItem.inc"
}

// [report parseReward.md / objdump 0x813e8cc]
#include "advancealtar_DB/parseReward.inc"
}


// [report dispatch.md / objdump 0x813ec24]：不调基类前置，直接取 OutBuffer → update。
#include "advancealtar_DB/dispatch_1.inc"
}

#include "advancealtar_DB/makeStageString.inc"
}

#include "advancealtar_DB/makeSlotString.inc"
}

#include "advancealtar_DB/makeRewardString.inc"
}

#include "advancealtar_DB/makeBuyItemString.inc"
}

// [report update.md / objdump 0x813ec5e]
// [b28d-solo-advancealtar_DB] N82/P212 封顶（orig 466 / ours 469，clean resid=525 全为
//   槽号平移；帧 0xdc vs 0xbc）：
//   (1) ORIG 槽序 mysql(-0x28)/result(-0x21)/row(-0x20)/db(-0x1c)/strings(-0x2c..-0x38)；
//       4.4 块内对象恒更深（strings -0x50..-0x5c）⇒ 全帧平移，P212 全头部声明对该件
//       实测不消（strings 提头后帧仍差 0x20，且改变 string 生命周期）；
//   (2) `db.m_update == 1`：ORIG 0x0813f2xx `movzbl 0x517; xor $0x1; test; jne`（bool
//       化），4.4 恒 `cmp $0x1`；m_update 为 uchar 且 AdvanceAltarData::m_update/
//       get/reset 多消费点共用（跨函数联动），本件不改型；
//   (3) `&&` 短路链物化岛（P225 推广需拆嵌套 if；槽序未定时不单独翻）。
//   P167 CLEAR（48/48；4 处 `data()`→`c_str()` 已修正 EXTRA）、语义等价。
#include "advancealtar_DB/update.inc"
}

// ---- DB_StatisticAdvanceAltar ----
class DB_StatisticAdvanceAltar : public DBDispatcher
{
public:
    DB_StatisticAdvanceAltar();
    ~DB_StatisticAdvanceAltar();
    virtual bool dispatch(int uid, int uniqueId, Stream* stream);
};

#include "advancealtar_DB/DB_StatisticAdvanceAltar.inc"
#include "advancealtar_DB/dtor_DB_StatisticAdvanceAltar.inc"
struct EntranceView { unsigned int stageId, entranceCount, successCount, obtainStar; };  // 0x10
struct LayerView                                        // 0xe 有效 + 2 pad = 0x10
{
    unsigned short itemType;   // +0x00
    unsigned short m_pad02;
    unsigned int itemId;       // +0x04
    unsigned short itemLevel;  // +0x08
    unsigned short useCount;   // +0x0a
    unsigned short useStar;    // +0x0c
};
}  // namespace

// [report dispatch.md / objdump 0x81412b2]
// [b28d-solo-advancealtar_DB] dead-test 族封顶（orig 312 / ours 308，clean resid=4）：
//   ORIG 两处 insert 分支 `mysql->exec(true)` 后各留 `xor $0x1,%eax; test %al,%al`
//   （4.1 对未使用 bool 返回值的归一化残留，P168a③ dead test）；4.4 tree-DCE 删除。
//   P167 CLEAR（31/31）、语义等价。
#include "advancealtar_DB/dispatch_2.inc"
}

// ---- 压缩转换自由函数 ----

// [objdump 0x813dd3e]
#include "advancealtar_DB/convertStringToZip_1.inc"
}

} // namespace advancealtar
