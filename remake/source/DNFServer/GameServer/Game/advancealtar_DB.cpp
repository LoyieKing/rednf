// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 advancealtar_DB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
// [b28d-D03 判词 · L2Free4to15.S6 本波更新]（fresh `check_tu_routed.sh`：identical=83 ae=149
//   （合计 232）near=0 diff=5 = _Z41__static_initialization…（GI 假缺口：ORIG 该符号有 493 个
//   实例，工具按名匹配取首个 0x0807ee76（属 PacketTypes）⇒ 逐 TU 读数不可比，我方 ioinit 形
//   正确、零改动）+ 下列 4 件；
//   p167 调用点差集对三件实跑：ORIG calls == OUR calls（31/31、26/26、48/48），
//   MISSING/EXTRA 仅为 C1↔C2、D1↔D2 同址别名伪差（P93）⇒ 语义无缺口）
//   · getFromMemory 0x0813394c —— 32/31：唯一残差 = P211 档③ store 装载序
//     （ORIG `mov %eax,%edx; mov 0xc(%ebp),%eax; mov %edx,(%eax)` RHS-first + edx 中转；
//     我方 LHS-first 直存）。字段侧/callee 侧两旋钮历轮已试（判词见函数体）⇒ 封 N76 族。
//   · requestLoadFromDBAndES 0x08133784 —— **[S6 本波] 122→124（123/124，gap +1→−1）**：
//     尾块 `mov $0x1,%ebx` 差异归零 —— ORIG 两臂各一份 ⇒ 源形为两臂各自 `return true;`
//     （非共享尾块单 return）；改写后该差异消失。唯一残差 = `data->m_characNo` store 的
//     LHS/RHS-first 1 条（4.4 对局部指针字段 store 恒 RHS-first+edx 中转，ORIG LHS-first
//     直存）⇒ N76 族。p167 26/26 且仅 C1/C2、D1/D2 别名差 ⇒ 语义等价封顶。
//   · DB_StatisticAdvanceAltar::dispatch 0x081412b2 —— 312/308：p167 31/31（仅 cMyTrace
//     C1↔C2 别名差）；gap −4 = 跨版本 EH 域形态（sprintf/__cxa_allocate_exception/throw 段
//     的常量地址伪代码化 + 我方少 4 条 cleanup 装载）⇒ N104（EH 尾块相对序）封顶。
//   · DB_Update::update 0x0813ec5e —— **[S6 本波] 469→467（466/467，gap −3→−1）**：
//     `db.m_update==1 && isValidRidableId(...)` 的 4.4 bool 物化岛（`mov $1/$0,%eax; test; je`）
//     已消 —— 拆为两段独立 guard+continue，与 ORIG 0x0813ecb6/0x0813ecce 两处
//     `xor $1; test %al,%al; jne`（各带 nop/jmp continue 中继 +0x6fa/+0x6fd）逐条对齐
//     （探针 t1/t2 实证：`a&&b` 在 4.4 -O0 恒物化，两段 guard 生 ORIG 形）。残差 = a) 帧
//     0xdc vs 0xbc 与全部槽号平移 0x10（N82：4.4 块内对象按序压深 —— insert 分支"重声明 q*"
//     实验使帧 0xbc→0xcc、槽再深 0x10，实测反劣已回滚）；b) `db.m_update != 1` 1 条
//     —— **[Q1P342ScanA 2026-09-11 复核·已落地；原注「现 unsigned char，.h 侧建议改名」
//     系过时表述]** `advancealtar.h:250` 现为 `bool m_update;`（注释带
//     `[类型轴 2026-09-11]` + ORIG 0x0813ecb6 直证），并配
//     `static_assert(offsetof(_CharacAdvanceAltarDbData, m_update) == 0x517)`。
//     实测本函数 ORIG 468 / OURS 470，双方 `xor $0x1` **各 6 条、逐点成对**
//     （ORIG 0x813ecb6/ecce/ee62/eeb3/ef04/ef55 ↔ OURS 0x8798/8822/9226/9307/9388/9469）
//     ⇒ 该项收益已兑现，**不再是未决建议**（残余已归 N82 帧-槽族）。
//     p167 48/48 仅 std::string C1/C2、D1/D2 别名差。
//   · DB_Load::SendResult 0x0813ea46 —— **[S6 本波归零] 71→66（66/66 AE）**：原判 N122/N93附
//     撤销 —— ORIG 0x0813ea54 `cmpb $0x0,-0x1c(%ebp); je +0xe6` 是**正面条件包裹**形
//     `if (ok) { 体 } return true;`（-0x1c 即 bool 形参 spill 槽），非就地取反；探针实证
//     `bool f(bool ok){ if(ok){g();} return true; }` 4.4 生逐条同形（`mov 0x18(%ebp),%eax;
//     mov %al,-0x1c(%ebp); cmpb $0x0,-0x1c(%ebp); je …`），而 `if(!ok) return true;` 生
//     `movzbl; xor $1; test` + return 物化漂移。结构改写后归位（AE 66/66）。
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

#include "advancealtar_DB/convertZipToString.inc"


#include "advancealtar_DB/AdvanceAltarData.inc"


#include "advancealtar_DB/dtor_AdvanceAltarData.inc"


#include "advancealtar_DB/getSlot.inc"


#include "advancealtar_DB/getBuyItem.inc"


#include "advancealtar_DB/get.inc"


#include "advancealtar_DB/getCharacAdvanceAltarDbData.inc"


#include "advancealtar_DB/reset.inc"


#include "advancealtar_DB/isUpdated.inc"


#include "advancealtar_DB/refillTicketFree.inc"


#include "advancealtar_DB/reset_1.inc"


// ----------------------------------------------------------------------------
// +0x00 承载 AdvanceAltarData* current（setCurrentAdvanceAltarData 0x08131254
// 实证：命中槽位写 this+4+i*0xd0，无匹配置 0；reset 置 0）。权威头 advancealtar.h
// 以 union（int m_field0 ∪ AdvanceAltarData* m_currentAdvanceAltarData）承载，
// 以下 const 访问器直接取具名指针成员。
// ----------------------------------------------------------------------------

#include "advancealtar_DB/getCurrentRidableId.inc"


#include "advancealtar_DB/isAvailableTciket.inc"


#include "advancealtar_DB/isEnterStage.inc"


#include "advancealtar_DB/getTowerInfo.inc"


#include "advancealtar_DB/makeRequest.inc"


#include "advancealtar_DB/getFromMemory.inc"


#include "advancealtar_DB/requestLoadFromDBAndES.inc"


} // namespace advancealtar

namespace APSystem
{

#include "advancealtar_DB/GetActionPointFromMemory.inc"


} // namespace APSystem

// ---- CUser 跨类叶子助手（Prod_GlobalAdv；全局作用域，置于文件尾避免命名空间混淆）----

#include "advancealtar_DB/DisableCharacInfo.inc"


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
                        unsigned int srcLen, unsigned int& realLen);
#include "advancealtar_DB/convertZipToString_1.inc"
                        char* strBuf, unsigned int strCap);
}  // namespace advancealtar

#include "SigTypes.h"  // STAdvanceAltarEntranceData / STAdvanceAltarLayerStatData（GetOutBuffer 实例化需要 sizeof）

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

#include "advancealtar_DB/DB_Load.inc"
#include "advancealtar_DB/dtor_DB_Load.inc"

#include "advancealtar_DB/dispatch.inc"


#include "advancealtar_DB/Select.inc"


#include "advancealtar_DB/SendResult.inc"


// 以下四个 parse* 共同模式：外层 strtok_r(str,"|")，内层 strtok_r(0,",")，
// 字段序按结构布局写入 [字节偏移 per 报告 asm shl 表]。

#include "advancealtar_DB/parseStage.inc"


#include "advancealtar_DB/parseSlot.inc"


#include "advancealtar_DB/parseBuyItem.inc"


#include "advancealtar_DB/parseReward.inc"



#include "advancealtar_DB/dispatch_1.inc"


#include "advancealtar_DB/makeStageString.inc"


#include "advancealtar_DB/makeSlotString.inc"


#include "advancealtar_DB/makeRewardString.inc"


#include "advancealtar_DB/makeBuyItemString.inc"


#include "advancealtar_DB/update.inc"


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

namespace
{
// 入场统计行视图（SigTypes.h 目前 pad(0x10)；字段化读取按报告偏移）
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

#include "advancealtar_DB/dispatch_2.inc"
{
    try
    {
        MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)4, (ENUM_SERVER_GROUP)0);
        struct tm now;
        __GetCurrentDNFDate(now);
        // ORIG 0x0814128c 段三条 `movl $0x0,-0x5c/-0x58/-0x54` = date[12] 预清。
        char date[12] = {0};
        std::sprintf(date, "%04d-%02d-%02d", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday);

        int entranceCount = 0;
        *stream >> entranceCount;                                   // [0x861c5c4 operator>>(int&)]
        for (int i = 0; i < entranceCount; ++i)
        {
            // [b28d-solo-advancealtar_DB] P89：ORIG 单槽 -0x28（GetOutBuffer 结果
            //   即视图指针），旧 rawE+e 双局部多一槽并致全帧平移 8。
            EntranceView* e = reinterpret_cast<EntranceView*>(
                stream->GetOutBuffer<STAdvanceAltarEntranceData>());
            if (e == 0)
                throw 0x624;
            // P118/P139：ORIG 0x081412d5 `movl $<rodata>,0x4(%esp)` 直接传函数签名串
            //   （_ZZ…E19__PRETTY_FUNCTION__），非 static 指针变量（多一条装载）。
            cMyTrace(__PRETTY_FUNCTION__, 0x626, 0)("[AdvanceAltar] %u,%u,%u,%u",
                                                     e->stageId, e->entranceCount, e->successCount, e->obtainStar);
            mysql->set_query(
                             " upDate log_advance_alter_entrance  set entrance_count = entrance_count + %u"
                             "  , success_count = success_count + %u  , obtain_star = obtain_star + %u"
                             "  where occ_date = '%s' and stage_id = %u",
                             e->entranceCount, e->successCount, e->obtainStar, date, e->stageId);
            if (!(mysql->exec(true) && mysql->getAffectedRowCount() != 0))
            {
                mysql->set_query(
                                 " inSert into log_advance_alter_entrance(occ_date,  stage_id,"
                                 " entrance_count, success_count, obtain_star)"
                                 "  values('%s', %u, %u, %u, %u) ",
                                 date, e->stageId, e->entranceCount, e->successCount, e->obtainStar);
                mysql->exec(true);
            }
        }

        int layerCount = 0;
        *stream >> layerCount;
        for (int i = 0; i < layerCount; ++i)
        {
            LayerView* l = reinterpret_cast<LayerView*>(
                stream->GetOutBuffer<STAdvanceAltarLayerStatData>());
            if (l == 0)
                throw 0x64d;
            cMyTrace(__PRETTY_FUNCTION__, 0x64f, 0)("[AdvanceAltar] %u,%u,%u,%u,%u",
                                                     static_cast<unsigned>(l->itemType), l->itemId,
                                                     static_cast<unsigned>(l->itemLevel),
                                                     static_cast<unsigned>(l->useCount),
                                                     static_cast<unsigned>(l->useStar));
            mysql->set_query(
                             " upDate log_advance_alter_layer_stat  set use_count = use_count + %u,"
                             " use_star = use_star + %u  where occ_date = '%s' and item_type = %u and"
                             " item_id = %u and item_level = %u",
                             static_cast<unsigned>(l->useCount), static_cast<unsigned>(l->useStar),
                             date, static_cast<unsigned>(l->itemType), l->itemId,
                             static_cast<unsigned>(l->itemLevel));
            if (!(mysql->exec(true) && mysql->getAffectedRowCount() != 0))
            {
                mysql->set_query(
                                 " inSert into log_advance_alter_layer_stat(occ_date,  item_type,"
                                 " item_id, item_level, use_count, use_star)"
                                 " values('%s', %u, %u, %u, %u, %u)",
                                 date, static_cast<unsigned>(l->itemType), l->itemId,
                                 static_cast<unsigned>(l->itemLevel),
                                 static_cast<unsigned>(l->useCount),
                                 static_cast<unsigned>(l->useStar));
                mysql->exec(true);
            }
        }
        return true;
    }
    catch (int err)
    {
        LogManager::logFormat(1, "localglobal/global_AdvanceAltar_Dispatcher.cpp", __PRETTY_FUNCTION__,
                              0x674, "[AdvanceAltar] query error line(%u)", err);   // [0x814174e]
        return false;
    }
}

// ---- 压缩转换自由函数 ----

#include "advancealtar_DB/convertStringToZip_1.inc"


} // namespace advancealtar
