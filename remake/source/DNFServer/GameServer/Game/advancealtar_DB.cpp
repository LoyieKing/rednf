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

// ---- AdvanceAltarData 数据模型（advancealtar__AdvanceAltarData 报告）----
AdvanceAltarData::AdvanceAltarData()
    : m_stages(), m_buyItems1(), m_buyItems2(), m_buyItems3(),
      m_slots1(), m_slots2(), m_rewards(), m_update(0)
{
}

AdvanceAltarData::~AdvanceAltarData()
{
}

// ORIG T 0x08899b4e：把 map<uchar,_Slot> 拷入定长槽数组（槽 0 长度 10）。
void AdvanceAltarData::getSlot(const std::map<unsigned char,_Slot>& m, _Slot* out, int count) const
{
    int local_10 = 0;
    for (std::map<unsigned char,_Slot>::const_iterator it = m.begin();
         it != m.end() && local_10 < count; )
    {
        const _Slot& slot = it->second;
        if (slot.m_int1 == 0)               // 源槽 m_int1==0 → 跳过
        {
            ++it;
        }
        else if (out[local_10].m_int1 == 0)  // 目标槽 m_int1==0 才写入
        {
            std::memcpy(&out[local_10], &slot, 10);
            ++it;
            ++local_10;
        }
        else
        {
            ++local_10;
        }
    }
}

// ORIG T 0x08899a3a：把 map<int,_BuyItem> 拷入定长购买项数组（每项 12）。
void AdvanceAltarData::getBuyItem(const std::map<int,_BuyItem>& m, _BuyItem* out, int count) const
{
    int local_10 = 0;
    for (std::map<int,_BuyItem>::const_iterator it = m.begin();
         it != m.end() && local_10 < count; )
    {
        const _BuyItem& bi = it->second;
        if (bi.m_field4 < 1)                 // 源 m_field4 < 1 → 跳过
        {
            ++it;
        }
        else if (out[local_10].m_field4 < 1)  // 目标 m_field4 < 1 才写入
        {
            std::memcpy(&out[local_10], &bi, 0xc);
            ++it;
            ++local_10;
        }
        else
        {
            ++local_10;
        }
    }
}

// ORIG T 0x088996d8：把 AdvanceAltarData 序列化到 _AdvanceAltarDbData（0x518）。
void AdvanceAltarData::get(_AdvanceAltarDbData& out) const
{
    out.m_update = m_update;                          // +0x517 = this[0xcc]
    std::memcpy(&out.m_ridable, this, 0x21);          // 前 0x21（_RidableBase 扁平区）

    int local_10 = 0;
    for (std::map<int,_Stage>::const_iterator it = m_stages.begin();
         it != m_stages.end() && local_10 <= 0x31; )
    {
        const _Stage& st = it->second;
        if (st.m_field0 < 1)                          // value.m_field0 < 1 → 跳过
        {
            ++it;
        }
        else
        {
            // ORIG 目标地址 = &out + 0x21 + local_10*8 → m_stages[local_10]
            std::memcpy(&out.m_stages[local_10], &st, sizeof(_Stage));
            ++it;
            ++local_10;
        }
    }

    // ORIG 目标基址 +0x1b1 / +0x229 → 具名数组成员（元素宽 10 / 12 与 pack(1) 布局一致）。
    getSlot(m_slots1, out.m_slots, 0xc);
    getSlot(m_slots2, out.m_slots, 0xc);
    getBuyItem(m_buyItems1, out.m_buyItems, 0x32);
    getBuyItem(m_buyItems2, out.m_buyItems, 0x32);
    getBuyItem(m_buyItems3, out.m_buyItems, 0x32);

    local_10 = 0;
    for (std::map<int,_Reward>::const_iterator it = m_rewards.begin();
         it != m_rewards.end() && local_10 <= 0x1d; )
    {
        const _Reward& rw = it->second;
        if (rw.m_field0 < 1)
        {
            ++it;
        }
        else
        {
            // ORIG 目标地址 = &out + 0x481 + local_10*5 → m_rewards[local_10]
            std::memcpy(&out.m_rewards[local_10], &rw, sizeof(_Reward));
            ++it;
            ++local_10;
        }
    }
}

// ---- CharacAdvanceAltarManager（CUser.h 声明）----
// ORIG T 0x08131710：把管理器内单个 AdvanceAltarData 拷到 _CharacAdvanceAltarDbData.m_db。
void CharacAdvanceAltarManager::getCharacAdvanceAltarDbData(_CharacAdvanceAltarDbData& out) const
{
    // ORIG 循环 local_10 <= 0（一次），m_data 按 stride 0xd0 取第 i 行
    for (int local_10 = 0; local_10 <= 0; ++local_10)
    {
        m_data[local_10].get(out.m_db[local_10]);
    }
}

// ORIG T 0x08899244（objdump 全文核对）：memset 前 0x21（_RidableBase）→
// m_update=0 → 7 个 map clear（stages/buyItems1..3/slots1/slots2/rewards）。
void AdvanceAltarData::reset()
{
    std::memset((char*)this, 0, 0x21);
    m_update = 0;
    m_stages.clear();
    m_buyItems1.clear();
    m_buyItems2.clear();
    m_buyItems3.clear();
    m_slots1.clear();
    m_slots2.clear();
    m_rewards.clear();
}

// ORIG T 0x081316a0：检查首个 AdvanceAltarData 的更新标志（manager+0xd0 == m_data.m_update）。
bool CharacAdvanceAltarManager::isUpdated() const
{
    for (int local_8 = 0; local_8 <= 0; ++local_8)
    {
        if (m_data[local_8].m_update != 0)
            return true;
    }
    return false;
}

// ORIG T 0x813159a：槽位坐骑有效时把免费票补为配置值（CDataManager+0x6364
// = m_altarConfig.m_0）并置 m_data.m_update=1。
void CharacAdvanceAltarManager::refillTicketFree()
{
    for (int i = 0; i < 1; ++i)
    {
        if (isValidRidableId(static_cast<advancealtar::RidableId::T>(m_data[0].m_ridableId)))
        {
            m_data[0].m_ticket = G_CDataManager()->m_altarConfig.m_0;
            m_data[0].m_update = 1;
        }
    }
}

// ORIG T 0x08131660（docs/class_func_reports/advancealtar__CharacAdvanceAltarManager/
// reset.md 全文）：current(+0x00)=0；循环 local_10<=0 一次 reset +i*0xd0+4 处 AdvanceAltarData。
void CharacAdvanceAltarManager::reset()
{
    m_currentAdvanceAltarData = 0;
    for (int local_c = 0; local_c <= 0; ++local_c)
    {
        // 单实例布局：m_data@+0x04、stride 0xd0 == ORIG `this + local_c*0xd0 + 4`。
        m_data[local_c].reset();
    }
}

// ----------------------------------------------------------------------------
// +0x00 承载 AdvanceAltarData* current（setCurrentAdvanceAltarData 0x08131254
// 实证：命中槽位写 this+4+i*0xd0，无匹配置 0；reset 置 0）。权威头 advancealtar.h
// 以 union（int m_field0 ∪ AdvanceAltarData* m_currentAdvanceAltarData）承载，
// 以下 const 访问器直接取具名指针成员。
// ----------------------------------------------------------------------------

// ORIG T 0x081312d0（getCurrentRidableId.md 全文）：current 空 → 0；
// 否则 *current（AdvanceAltarData::m_ridableId，_RidableBase+0x00）。
int CharacAdvanceAltarManager::getCurrentRidableId() const
{
    if (m_currentAdvanceAltarData == 0)
        return 0;
    return m_currentAdvanceAltarData->m_ridableId;
}

// ORIG T 0x081312ec（isAvailableTciket.md 全文）：current 空 → 0；
// 否则 *(int*)(cur+4) + *(int*)(cur+8) > 0（免费票 + 现持票）。
bool CharacAdvanceAltarManager::isAvailableTciket() const
{
    if (m_currentAdvanceAltarData == 0)
    {
        return false;
    }
    // ORIG 4 字节读 +0x04 / +0x08 两票池（DB 列对照：+0x04=ticket_free、
    // +0x08=ticket_cera，见 _RidableDbHead；_RidableBase 现有成员名与该列名
    // 相反，属命名口径问题，本批只去裸偏移不改共享成员名）。
    // ORIG 无任何槽：指针逐次重读、和与比较全寄存器直算（P0/P183）。
    return (m_currentAdvanceAltarData->m_ticket +
            m_currentAdvanceAltarData->m_freeTicket) > 0;
}

// ORIG T 0x0813131a（isEnterStage.md 全文）：current 空 → 0；在
// current->m_stages(+0x24, map<int,_Stage>) find(stageId) 命中 → 1。
bool CharacAdvanceAltarManager::isEnterStage(int stageId) const
{
    const AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return false;
    }
    return cur->m_stages.find(stageId) != cur->m_stages.end();
}

// ---- getTowerInfo（ORIG T 0x081328ea，getTowerInfo.md 全文）----
// current(+0x00) 空→0；current->m_buyItems3(+0x6c) find(towerIdx) 未命中→0；
// G_CDataManager()+0x631c（m_altarShop）.getBuyUpgradeDataVec(*current,2) 空→0；
// 向量中 m_nItemId==towerIdx 项未命中→0；getFieldDataPoint(买项.m_short8,
// FT_INDEX=1) 空→0；买项计数 m_short8==0→0；否则 *out1=FieldData+4(m_value)、
// *out2=(int)m_short8，返回 1。
bool CharacAdvanceAltarManager::getTowerInfo(int towerIdx, int& outValue,
                                             int& outCount) const
{
    const AdvanceAltarData* cur = m_currentAdvanceAltarData;
    if (cur == 0)
    {
        return false;
    }
    const std::map<int, _BuyItem>& buys = cur->m_buyItems3;
    std::map<int, _BuyItem>::const_iterator it = buys.find(towerIdx);
    if (it != buys.end())
    {
        std::vector<BuyUpgradeData>* vec =
            G_CDataManager()->m_altarShop.getBuyUpgradeDataVec(
                cur->m_ridableId, AdvanceAltarShopType::T_2);
        if (vec == 0)
        {
            return false;
        }
        BuyUpgradeData* data = 0;
        for (std::vector<BuyUpgradeData>::iterator vi = vec->begin();
             vi != vec->end(); vi++)
        {
            if (vi->m_nItemId == towerIdx)
            {
                data = &*vi;
                break;
            }
        }
        if (data == 0)
        {
            return false;
        }
        const FieldData* point =
            data->getFieldDataPoint((int)it->second.m_short8, FieldType::FT_INDEX);
        if (point == 0)
        {
            return false;
        }
        if (it->second.m_short8 == 0)
        {
            return false;
        }
        outValue = point->m_value;
        outCount = (int)it->second.m_short8;
        return true;
    }
    return false;
}

// ---- DB_Update（advancealtar__DB_Update 报告）----
// ORIG T 0x0813eb38：opcode 0x1a | uid | memcpy 0x51c → DB_Q。
void DB_Update::makeRequest(int uid, const _CharacAdvanceAltarDbData& data)
{
    // ORIG 无 stream 具名槽：Acquire 结果经 eax 直传 CStreamGuard（P0/P89）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localglobal/global_AdvanceAltar_Dispatcher.cpp", 0x1d9),
                       true);
    guard.operator*()->operator<<(0x1a);
    guard.operator*()->operator<<(uid);
    _CharacAdvanceAltarDbData* sig =
        guard.operator->()->GetInBuffer<_CharacAdvanceAltarDbData>();
    std::memcpy(sig, &data, 0x51c);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

// ---- DBAndExchangeServerProc（advancealtar__DBAndExchangeServerProc 报告）----
// ORIG T 0x0813394c：characNo→data.m_characNo；拷 AdvanceAltarData；isUpdated 则 DB_Update。
void DBAndExchangeServerProc::getFromMemory(const CUser& user, _CharacAdvanceAltarDbData& data)
{
    data.m_characNo = (unsigned int)user.getCurCharacNo();
    user.m_advanceAltar.getCharacAdvanceAltarDbData(data);
    if (user.m_advanceAltar.isUpdated() == 0)
        goto L_done;
    DB_Update::makeRequest((int)user.GetUID(), data);
L_done:;
}

// ---- requestLoadFromDBAndES（advancealtar__DBAndExchangeServerProc 报告全文）----
// ORIG T 0x08133784：reset 管理器 → Acquire stream(opcode 0x19) → uid →
// DisableCharacInfo(10) →（ValidLastLogin 时补 channelNo/characNo）→
// GetInBuffer<_CharacAdvanceAltarDbData> 清零 0x51c 后写 m_characNo →
// ValidLastLogin ? put 队列5 : put DB_Q(2) → 返回 1。
bool DBAndExchangeServerProc::requestLoadFromDBAndES(CUser& user, MSG_CHARAC_SELECT& msg)
{
    user.m_advanceAltar.reset();  // ORIG CharacAdvanceAltarManager::reset @0x08131660

    Stream* stream = GlobalData::s_stream_pool->Acquire(
        "localglobal/global_AdvanceAltar.cpp", 0xc3a);
    CStreamGuard guard(stream, true);
    guard.operator*()->operator<<(0x19);
    guard.operator*()->operator<<(static_cast<int>(user.GetUID()));
    user.DisableCharacInfo(CACHE_ADVANCE_ALTAR);  // ORIG push 0xa（枚举名 Prod_DispatcherHelpers 落地）

    if (user.IsValidLastLoginCharac())
    {
        guard.operator*()->operator<<(user.GetLastLoginChannelNo());
        guard.operator*()->operator<<(user.GetLastLoginCharacNo());
    }

    _CharacAdvanceAltarDbData* data =
        guard.operator->()->GetInBuffer<_CharacAdvanceAltarDbData>();
    std::memset(data, 0, 0x51c);
    data->m_characNo = static_cast<unsigned int>(user.get_charac_no(static_cast<int>(msg.m_slot)));

    if (!user.IsValidLastLoginCharac())
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    else
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(5), guard);  // 第 6 槽（ORIG 无枚举名）
    return true;
}

} // namespace advancealtar

namespace APSystem
{

// ORIG T 0x08121616（纯空函数）：行动力从内存缓存加载——本版本 ORIG 体为空。
void CDBAndExchangeServerProc::GetActionPointFromMemory(const CUser&, _SIG_LOAD_ACTION_POINT&)
{
}

} // namespace APSystem

// ---- CUser 跨类叶子助手（Prod_GlobalAdv；全局作用域，置于文件尾避免命名空间混淆）----

// ORIG W 0x081348ce _ZN5CUser17DisableCharacInfoE13CACHE_SECTION：
// movb $0,(this+0x8d0fd+section) —— m_pad8d0fd[section] 清零。
void CUser::DisableCharacInfo(CACHE_SECTION section)
{
    m_pad8d0fd[static_cast<int>(section)] = 0;
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
bool convertStringToZip(char* out, unsigned int outCap, const char* src,
                        unsigned int srcLen, unsigned int& realLen);
bool convertZipToString(MySQL* mysql, int col, char* zipBuf, unsigned int zipCap,
                        char* strBuf, unsigned int strCap);
}  // namespace advancealtar

#include "SigTypes.h"  // STAdvanceAltarEntranceData / STAdvanceAltarLayerStatData（GetOutBuffer 实例化需要 sizeof）

// ORIG _Z19__GetCurrentDNFDateR2tm（T 0x86b1fc0，定义 GameUtils.cpp）——仓库暂无头声明。
void __GetCurrentDNFDate(struct tm& out);

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

// ---- POD inline ctor W 符号实例化触发器（ORIG 使用点在 DBAndExchangeServerProc/
//      CUser 缓存序列化链；本 TU 保留发射点保证 ABI 集齐）----
__attribute__((unused)) static void instantiateAdvanceAltarDbPOD()
{
    _Stage a; _Slot b; _Reward c; _BuyItem d;
    _AdvanceAltarDbData e; _CharacAdvanceAltarDbData f;
    (void)a; (void)b; (void)c; (void)d; (void)e; (void)f;

    // DB_Update C1/C2（ORIG W 0x844de06）：类无数据成员，隐式 inline ctor/dtor
    // 仅展开基类 vptr 初始化；局部对象保证弱符号随本 TU 发射。
    DB_Update u;
    (void)u;
}

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
DB_Load::DB_Load() {}
DB_Load::~DB_Load() {}

// [objdump 0x813ddb2]
bool DB_Load::dispatch(int uid, int uniqueId, Stream* stream)
{
    if (!DBDispatcher::dispatch(uid, uniqueId, stream))
        return false;
    _CharacAdvanceAltarDbData* data = stream->GetOutBuffer<_CharacAdvanceAltarDbData>();
    if (data == 0)
        return false;
    bool ok = Select(*data);
    return SendResult(uid, uniqueId, *data, ok);   // ORIG eax = SendResult 返回值
}

// [report Select.md / objdump 0x813de40]
bool DB_Load::Select(_CharacAdvanceAltarDbData& data)
{
    MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
    mysql->set_query(
                     "select ridable_id, ticket_free, ticket_cera, \t\t\t\t  star_game, star_cera, "
                     "star_usable, \t\t\t\t  survival_best, star_reset_count, is_unlock_stage_effect, "
                     "\t\t\t\t  stage_list, slot_list, buy_item_list, reward_list \t\t\t\t  from "
                     "charac_advance_altar\t\t\t\t\t  where charac_no = %u",
                     data.m_characNo);
    if (!mysql->exec(true))
        return false;

    const int nRows = mysql->get_n_rows();
    for (int row = 0; row < nRows && row < 1; ++row)
    {
        if (!mysql->fetch())
            return false;

        // ORIG 行基址 local_14 = &data + row*0x518 + 4 → m_db 起
        //（行距 = sizeof(_AdvanceAltarDbData) = 0x518）。
        _AdvanceAltarDbData* dbRow = reinterpret_cast<_AdvanceAltarDbData*>(
            reinterpret_cast<char*>(&data.m_db) +
            static_cast<unsigned>(row) * sizeof(_AdvanceAltarDbData));
        _RidableDbHead& view = dbRow->m_head();
        int col = 0;

        int ridableId = 0;
        if (!mysql->get_int(col++, ridableId))                       // [0x0811692c]
            return false;
        if (!isValidRidableId(static_cast<RidableId::T>(ridableId)))
        {
            LogManager::logFormat(1, "localglobal/global_AdvanceAltar_Dispatcher.cpp",
                                  __PRETTY_FUNCTION__,
                                  0xb1, "invalid RidableId=%d", ridableId);
            return false;
        }
        view.m_ridableId = ridableId;

        if (!mysql->get_short(col++, view.m_ticketFree))              // [0x0814201c get_short(int,int&)]
            return false;
        if (!mysql->get_short(col++, view.m_ticketCera))
            return false;
        if (!mysql->get_int(col++, view.m_starGame))                  // [0x0811692c]
            return false;
        if (!mysql->get_int(col++, view.m_starCera))
            return false;
        if (!mysql->get_int(col++, view.m_starUsable))
            return false;
        if (!mysql->get_short(col++, view.m_survivalBest))            // [0x080fddec get_short(int,short&)]
            return false;
        if (!mysql->get_short(col++, view.m_starResetCount))
            return false;
        short unlockFlag = 0;
        if (!mysql->get_short(col++, unlockFlag))                     // [0x080fddec get_short(int,short&)]
            return false;
        // ORIG：`movzwl` 零扩展 unlockFlag → `test %ax,%ax; jle`（有符号16位 ≤0→0，
        // 即 0x8000..0xFFFF 负值判 0，1..0x7fff 判 1）→ `movb $0x1/$0x0,0x20`。
        view.m_isUnlockStageEffect = (unlockFlag > 0) ? 1 : 0;

        std::memset(g_StrStageBuffer, 0, kMaxStrBuffer);
        std::memset(g_StrSlotBuffer, 0, kMaxStrBuffer);
        std::memset(g_StrBuyItemBuffer, 0, kMaxStrBuffer);
        std::memset(g_StrRewardBuffer, 0, kMaxStrBuffer);

        if (!convertZipToString(mysql, col++, g_ZipStageBuffer, kMaxZipBuffer, g_StrStageBuffer, kMaxStrBuffer))
            return false;
        if (!convertZipToString(mysql, col++, g_ZipSlotBuffer, kMaxZipBuffer, g_StrSlotBuffer, kMaxStrBuffer))
            return false;
        if (!convertZipToString(mysql, col++, g_ZipBuyItemBuffer, kMaxZipBuffer, g_StrBuyItemBuffer, kMaxStrBuffer))
            return false;
        if (!convertZipToString(mysql, col++, g_ZipRewardBuffer, kMaxZipBuffer, g_StrRewardBuffer, kMaxStrBuffer))
            return false;

        if (!parseStage(g_StrStageBuffer, dbRow->m_stages, kMaxStageCount))
            return false;
        if (!parseSlot(g_StrSlotBuffer, dbRow->m_slots, kMaxSlotCount))
            return false;
        if (!parseBuyItem(g_StrBuyItemBuffer, dbRow->m_buyItems, kMaxBuyItemCount))
            return false;
        if (!parseReward(g_StrRewardBuffer, dbRow->m_rewards, kMaxRewardCount))
            return false;
    }
    return true;
}

// [report SendResult.md / objdump 0x813ea46]
bool DB_Load::SendResult(int uid, int uniqueId, _CharacAdvanceAltarDbData& data, bool ok)
{
    if (!ok)          // 残差 cmpb mem 直判 vs movzbl+xor+test 属 P209 镜像 N（4.4 bool 直判恒载寄存器）
        return true;
    // ORIG 无 stream 具名槽：Acquire 结果经 eax 直传（P0/P89）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                           "localglobal/global_AdvanceAltar_Dispatcher.cpp", 0x1bf),
                       true);
    guard.operator*()->operator<<(uid);
    guard.operator*()->operator<<(uniqueId);
    guard.operator->()->put_binary(&data, 0x51c);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);   // ORIG QUEUE_IDX=1
    return true;
}

// 以下四个 parse* 共同模式：外层 strtok_r(str,"|")，内层 strtok_r(0,",")，
// 字段序按结构布局写入 [字节偏移 per 报告 asm shl 表]。

// [report parseStage.md / objdump 0x813e3e8]
bool DB_Load::parseStage(char* str, _Stage* out, int count)
{
    if (str == 0 || out == 0)
        return false;
    char* saveOuter = 0;
    char* line = strtok_r(str, "|", &saveOuter);
    int row = 0;   // 4.4 将该零初始化提升至 strtok_r 前（P112/N82 族），源形无解
    for (; line != 0 && row < count;
         line = strtok_r(0, "|", &saveOuter), ++row)
    {
        char* saveInner = 0;
        char* field = strtok_r(line, ",", &saveInner);
        for (int f = 0; field != 0 && f < 2; ++f)
        {
            if (f == 0) out[row].m_field0 = atoi(field);
            else        out[row].m_field4 = atoi(field);   // [objdump 0x813e494 → +4]
            field = strtok_r(0, ",", &saveInner);
        }
    }
    return true;
}

// [report parseSlot.md / objdump 0x813e542]
bool DB_Load::parseSlot(char* str, _Slot* out, int count)
{
    if (str == 0 || out == 0)
        return false;
    char* saveOuter = 0;
    int row = 0;
    for (char* line = strtok_r(str, "|", &saveOuter);
         line != 0 && row < count; line = strtok_r(0, "|", &saveOuter), ++row)
    {
        char* saveInner = 0;
        char* field = strtok_r(line, ",", &saveInner);
        for (int f = 0; field != 0 && f < 4; ++f)
        {
            // ORIG：atoi(field) 在 switch 各 case 内各自调用（4 处静态 call），
            // 勿前置 `const int v = atoi(...)`（会合成单 in-loop call）。
            switch (f)
            {
            case 0: out[row].m_byte0 = static_cast<unsigned char>(atoi(field) > 0 ? 1 : 0); break; // [0x813e47a]
            case 1: out[row].m_int1 = atoi(field); break;                                   // [0x813e48f +1]
            case 2: out[row].m_byte5 = static_cast<unsigned char>(atoi(field) != 0 ? 1 : 0); break;// [0x813e4a7 +5]
            case 3: out[row].m_int6 = atoi(field); break;                                   // [0x813e4bb +6]
            }
            field = strtok_r(0, ",", &saveInner);
        }
    }
    return true;
}

// [report parseBuyItem.md / objdump 0x813e708]
bool DB_Load::parseBuyItem(char* str, _BuyItem* out, int count)
{
    if (str == 0 || out == 0)
        return false;
    char* saveOuter = 0;
    int row = 0;
    for (char* line = strtok_r(str, "|", &saveOuter);
         line != 0 && row < count; line = strtok_r(0, "|", &saveOuter), ++row)
    {
        char* saveInner = 0;
        char* field = strtok_r(line, ",", &saveInner);
        for (int f = 0; field != 0 && f < 4; ++f)
        {
            // ORIG：atoi(field) 在 switch 各 case 内各自调用（4 处静态 call）。
            switch (f)
            {
            case 0: out[row].m_field0 = atoi(field); break;                              // [0x813e76c]
            case 1: out[row].m_field4 = atoi(field); break;
            case 2: out[row].m_short8 = static_cast<short>(atoi(field)); break;          // [0x813e79a +8 short]
            case 3: out[row].m_short10 = static_cast<short>(atoi(field)); break;         // [0x813e7ab +10 short]
            }
            field = strtok_r(0, ",", &saveInner);
        }
    }
    return true;
}

// [report parseReward.md / objdump 0x813e8cc]
bool DB_Load::parseReward(char* str, _Reward* out, int count)
{
    if (str == 0 || out == 0)
        return false;
    char* saveOuter = 0;
    int row = 0;
    for (char* line = strtok_r(str, "|", &saveOuter);
         line != 0 && row < count; line = strtok_r(0, "|", &saveOuter), ++row)
    {
        char* saveInner = 0;
        char* field = strtok_r(line, ",", &saveInner);
        for (int f = 0; field != 0 && f < 2; ++f)
        {
            if (f == 0) { out[row].m_field0 = atoi(field); }                             // [0x813e934]
            else
            {
                const int v = atoi(field);
                out[row].m_byte4 = static_cast<unsigned char>(v != 0 ? 1 : 0);           // [0x813e94d]
            }
            field = strtok_r(0, ",", &saveInner);
        }
    }
    return true;
}


// [report dispatch.md / objdump 0x813ec24]：不调基类前置，直接取 OutBuffer → update。
bool DB_Update::dispatch(int /*uid*/, int /*uniqueId*/, Stream* stream)
{
    _CharacAdvanceAltarDbData* data = stream->GetOutBuffer<_CharacAdvanceAltarDbData>();
    if (data == 0)
        return false;
    update(*data);
    return true;
}

void DB_Update::makeStageString(std::string& out, const _Stage* arr, int count)   // [objdump 0x813f380]
{
    if (arr == 0)
        return;
    out.clear();
    char buf[128];
    for (int i = 0; i < count; ++i)
    {
        if (arr[i].m_field0 != 0 || arr[i].m_field4 != 0)
        {
            std::sprintf(buf, "%d%s%d%s", arr[i].m_field0, kDelimterField,
                         arr[i].m_field4, kDelimterItem);   // ORIG 经 DAT ","/"|"
            out += buf;
        }
    }
}

void DB_Update::makeSlotString(std::string& out, const _Slot* arr, int count)    // [objdump 0x813f442]
{
    if (arr == 0)
        return;
    out.clear();
    char buf[128];
    for (int i = 0; i < count; ++i)
    {
        // ORIG 0x0813f4ef fmt="%d%s%d%s%d%s%d%s"，"," ×3(0xc/0x14/0x1c esp)+
        // "|"@0x24 为独立 rodata 实参（9 实参槽 outgoing ⇒ 帧 0xc0）。同
        // makeRewardString 配方（batch-13 帧差根因；实证装载序列 0x0813f483..0x0813f4fd，
        // 值序 byte0,int1,byte5,int6 与现 cast 一致）。
        // 【§6.2 标注】gap 5→1（o=73 n=68→72）：残余 1 = ORIG int1 装载两段
        // `mov 0x1(%eax),%eax; mov %eax,%ecx`（0x0813f4a5/4a8，4.1 先载后转），
        // 4.4 直出目标寄存器单段，寄存器分配形态差 N82，不可用 C++98 表达。
        std::sprintf(buf, "%d%s%d%s%d%s%d%s",
                     static_cast<int>(arr[i].m_byte0), ",", arr[i].m_int1, ",",
                     static_cast<int>(arr[i].m_byte5), ",", arr[i].m_int6, "|");
        out += buf;
    }
}

void DB_Update::makeRewardString(std::string& out, const _Reward* arr, int count) // [objdump 0x813f66a]
{
    if (arr == 0)
        return;
    out.clear();
    char buf[128];
    for (int i = 0; i < count; ++i)
    {
        if (0 < arr[i].m_field0)
        {
            // ORIG 0x0813f6e2 fmt="%d%s%d%s"(@0x8b6db4d)，逗号/竖线为独立 rodata 串常量
            // （","@0x8b6d831 / "|"@0x8b6d82f，readelf 实证）→ 6 实参槽 outgoing 区
            // 48B，帧 0xb8；先前 "%d,%d|" 2 参形态使帧缩为 0xa8（Main 指令之缓冲
            // 16B 维度差的真实来源 = sprintf 实参集，非 buf 本体：buf 基址 -0x8c
            // 两侧本就一致）。
            std::sprintf(buf, "%d%s%d%s", arr[i].m_field0, ",",
                         static_cast<int>(arr[i].m_byte4), "|");
            out += buf;
        }
        // 【§6.2 标注】残余 gap=2：ORIG if 体尾 `jmp +0xa6` + jle 目标空块 `nop@0xa5`
        // （同族 makeBuyItemString 0x0813f644 同形）。系 ORIG 编译器块放置产物，
        // gcc4.4 -O0 对空 else/尾分号均折叠，无 C++98 等价写法 ⇒ 控制流形态
        // N 类豁免；语义与行为（帧 0xb8、buf@-0x8c、i@-0xc、6 实参、rodata fmt
        // "%d%s%d%s"）已全部对齐。
    }
}

void DB_Update::makeBuyItemString(std::string& out, const _BuyItem* arr, int count) // [objdump 0x813f53c]
{
    if (arr == 0)
        return;
    out.clear();
    char buf[128];
    for (int i = 0; i < count; ++i)
    {
        if (arr[i].m_field4 != 0 || arr[i].m_short8 != 0)
        {
            // ORIG 0x0813f619 fmt="%d%s%d%s%d%s%d%s"，"," ×3 + "|"（实证装载序列
            // 0x0813f5d5..0x0813f615；双 short 为 movzwl+movswl 符号扩展，与
            // static_cast<int> 同形）。同 makeRewardString 配方。
            // 【§6.2 标注】gap 6→2（o=92 n=86→90）：残余 2 = if 体尾 `jmp+nop`
            // 空块放置对（ORIG 0x0813f644/646，同 makeRewardString 0x0813f70d/0f
            // 形），gcc4.4 折叠空块、无 C++98 等价写法 ⇒ 控制流形态 N 类豁免。
            std::sprintf(buf, "%d%s%d%s%d%s%d%s", arr[i].m_field0, ",",
                         arr[i].m_field4, ",",
                         static_cast<int>(arr[i].m_short8), ",",
                         static_cast<int>(arr[i].m_short10), "|");
            out += buf;
        }
    }
}

// [report update.md / objdump 0x813ec5e]
bool DB_Update::update(const _CharacAdvanceAltarDbData& data)
{
    MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3, (ENUM_SERVER_GROUP)0);
    bool result = true;
    for (int row = 0;; ++row)
    {
        if (row > 0)
            return result;

        // ORIG 基址 local_20 = &data + 4（= m_db）；各列以 m_db 内具名成员寻址
        //（update.md 0x0813ec5e：`movzbl 0x517` 取 m_update、`mov 0x4/0x8/0xc/0x10/0x14`
        //  取 m_head 前 5 列、`movswl 0x18`、`movzwl 0x1a`、`movzbl 0x20`、
        //  数组指针 `add $0x21/0x1b1/0x229/0x481`）。
        const _AdvanceAltarDbData& db = data.m_db[0];
        const _RidableDbHead& view = db.m_head();
        const _Stage* stages = db.m_stages;
        const _Slot* slots = db.m_slots;
        const _BuyItem* buyItems = db.m_buyItems;
        const _Reward* rewards = db.m_rewards;

        if (db.m_update == 1 && isValidRidableId(static_cast<RidableId::T>(view.m_ridableId)))
        {
            std::string stageStr;
            std::string slotStr;
            std::string buyItemStr;
            std::string rewardStr;
            makeStageString(stageStr, stages, 0x32);
            makeSlotString(slotStr, slots, 0xc);
            makeBuyItemString(buyItemStr, buyItems, 0x32);
            makeRewardString(rewardStr, rewards, 0x1e);

            unsigned int zipStageLen = 0, zipSlotLen = 0, zipBuyItemLen = 0, zipRewardLen = 0;
            std::memset(g_ZipStageBuffer, 0, 0x800);
            std::memset(g_ZipSlotBuffer, 0, 0x800);
            std::memset(g_ZipBuyItemBuffer, 0, 0x800);
            std::memset(g_ZipRewardBuffer, 0, 0x800);

            if (!convertStringToZip(g_ZipStageBuffer, 0x800, stageStr.data(),
                                    static_cast<unsigned>(stageStr.size()), zipStageLen))
                return false;
            if (!convertStringToZip(g_ZipSlotBuffer, 0x800, slotStr.data(),
                                    static_cast<unsigned>(slotStr.size()), zipSlotLen))
                return false;
            if (!convertStringToZip(g_ZipBuyItemBuffer, 0x800, buyItemStr.data(),
                                    static_cast<unsigned>(buyItemStr.size()), zipBuyItemLen))
                return false;
            if (!convertStringToZip(g_ZipRewardBuffer, 0x800, rewardStr.data(),
                                    static_cast<unsigned>(rewardStr.size()), zipRewardLen))
                return false;

            char* qStage = mysql->blob_to_str(0, g_ZipStageBuffer, static_cast<int>(zipStageLen));
            char* qSlot = mysql->blob_to_str(1, g_ZipSlotBuffer, static_cast<int>(zipSlotLen));
            char* qBuyItem = mysql->blob_to_str(2, g_ZipBuyItemBuffer, static_cast<int>(zipBuyItemLen));
            char* qReward = mysql->blob_to_str(3, g_ZipRewardBuffer, static_cast<int>(zipRewardLen));
            mysql->set_query(
                             "update charac_advance_altar \t\t\t\t  set ticket_free = %d, ticket_cera = %d, "
                             "\t\t\t\t  star_game = %d, star_cera = %d, star_usable = %d, \t\t\t\t  "
                             "survival_best = %d, star_reset_count = %d, is_unlock_stage_effect = %d, "
                             "\t\t\t\t  stage_list = '%s', \t\t\t\t  slot_list = '%s', \t\t\t\t  "
                             "buy_item_list = '%s', \t\t\t\t  reward_list = '%s' \t\t\t\t  where "
                             "charac_no = %u and ridable_id = %d",
                             view.m_ticketFree, view.m_ticketCera, view.m_starGame, view.m_starCera,
                             view.m_starUsable, static_cast<int>(view.m_survivalBest),
                             static_cast<int>(static_cast<unsigned short>(view.m_starResetCount)),
                             static_cast<int>(view.m_isUnlockStageEffect),
                             qStage, qSlot, qBuyItem, qReward,
                             data.m_characNo, view.m_ridableId);
            result = mysql->exec(true);
            if (!(result && mysql->getAffectedRowCount() != 0))
            {
                qStage = mysql->blob_to_str(0, g_ZipStageBuffer, static_cast<int>(zipStageLen));
                qSlot = mysql->blob_to_str(1, g_ZipSlotBuffer, static_cast<int>(zipSlotLen));
                qBuyItem = mysql->blob_to_str(2, g_ZipBuyItemBuffer, static_cast<int>(zipBuyItemLen));
                qReward = mysql->blob_to_str(3, g_ZipRewardBuffer, static_cast<int>(zipRewardLen));
                mysql->set_query(
                                 "insert into charac_advance_altar \t\t\t\t\t\t  (charac_no, ridable_id, "
                                 "\t\t\t\t\t\t  ticket_free, ticket_cera, \t\t\t\t\t\t  star_game, "
                                 "star_cera, star_usable, \t\t\t\t\t\t  survival_best, star_reset_count, "
                                 "is_unlock_stage_effect, \t\t\t\t\t\t  stage_list, \t\t\t\t\t\t  "
                                 "slot_list, \t\t\t\t\t\t  buy_item_list, \t\t\t\t\t\t  reward_list) "
                                 "\t\t\t\t\t\t  values \t\t\t\t\t\t  (%u, %d, \t\t\t\t\t\t  %d, %d, "
                                 "\t\t\t\t\t\t  %d, %d, %d,\t\t\t\t\t\t  %d, %d, %d, \t\t\t\t\t\t  '%s', "
                                 "\t\t\t\t\t\t  '%s', \t\t\t\t\t\t  '%s', \t\t\t\t\t\t  '%s')",
                                 data.m_characNo, view.m_ridableId, view.m_ticketFree, view.m_ticketCera,
                                 view.m_starGame, view.m_starCera, view.m_starUsable,
                                 static_cast<int>(view.m_survivalBest),
                                 static_cast<int>(static_cast<unsigned short>(view.m_starResetCount)),
                                 static_cast<int>(view.m_isUnlockStageEffect),
                                 qStage, qSlot, qBuyItem, qReward);
                result = mysql->exec(true);
            }
            result = true;   // ORIG movl $1（处理完成位）
        }
    }
}

// ---- DB_StatisticAdvanceAltar ----
class DB_StatisticAdvanceAltar : public DBDispatcher
{
public:
    DB_StatisticAdvanceAltar();
    ~DB_StatisticAdvanceAltar();
    virtual bool dispatch(int uid, int uniqueId, Stream* stream);
};

DB_StatisticAdvanceAltar::DB_StatisticAdvanceAltar() {}
DB_StatisticAdvanceAltar::~DB_StatisticAdvanceAltar() {}

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

// [report dispatch.md / objdump 0x81412b2]
bool DB_StatisticAdvanceAltar::dispatch(int /*uid*/, int /*uniqueId*/, Stream* stream)
{
    static const char* s_func =
        "virtual bool advancealtar::DB_StatisticAdvanceAltar::dispatch(int, int, Stream*)";
    try
    {
        MySQL* mysql = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)4, (ENUM_SERVER_GROUP)0);
        struct tm now;
        __GetCurrentDNFDate(now);
        char date[12];
        std::sprintf(date, "%04d-%02d-%02d", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday);

        int entranceCount = 0;
        *stream >> entranceCount;                                   // [0x861c5c4 operator>>(int&)]
        for (int i = 0; i < entranceCount; ++i)
        {
            void* rawE = stream->GetOutBuffer<STAdvanceAltarEntranceData>();
            EntranceView* e = reinterpret_cast<EntranceView*>(rawE);
            if (rawE == 0)
                throw 0x624;
            cMyTrace(s_func, 0x626, 0)("[AdvanceAltar] %u,%u,%u,%u",
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
            void* rawL = stream->GetOutBuffer<STAdvanceAltarLayerStatData>();
            LayerView* l = reinterpret_cast<LayerView*>(rawL);
            if (rawL == 0)
                throw 0x64d;
            cMyTrace(s_func, 0x64f, 0)("[AdvanceAltar] %u,%u,%u,%u,%u",
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
        LogManager::logFormat(1, "localglobal/global_AdvanceAltar_Dispatcher.cpp", s_func,
                              0x674, "[AdvanceAltar] query error line(%u)", err);   // [0x814174e]
        return false;
    }
}

// ---- 压缩转换自由函数 ----

// [objdump 0x813dd3e]
bool convertStringToZip(char* out, unsigned int outCap, const char* src,
                        unsigned int srcLen, unsigned int& realLen)
{
    if (srcLen == 0)
    {
        realLen = 0;
        return true;
    }
    realLen = outCap;                       // ORIG 0x813dd5d：写入 outCap（容量占位）
    std::memset(out, 0, outCap);
    register unsigned char c asm("eax") = static_cast<unsigned char>(
        compress_zip(out, realLen, src, srcLen));
    c ^= 1;
    if (c)
        return false;
    return true;
}

// [objdump 0x813dc48]
bool convertZipToString(MySQL* mysql, int col, char* zipBuf, unsigned int zipCap,
                        char* strBuf, unsigned int strCap)
{
    const int zlen = mysql->get_binary_length(col);
    if (zlen == 0)
        return true;
    // ORIG 0x813dc73：mov 0x14(%ebp),%eax; cmp -0x10(%ebp),%eax（左=zipCap 参数载寄存器）。
    // 反向 (unsigned)zlen >= zipCap 试图让 4.4 同向装载（P209 姊妹探针）。
    if (static_cast<unsigned int>(zlen) >= zipCap)
        return false;
    std::memset(zipBuf, 0, zipCap);
    if (!mysql->get_binary(col, zipBuf, zlen))
        return false;
    const unsigned int rawLen = (*reinterpret_cast<unsigned int*>(zipBuf)) & 0x3fffffff;
    if (rawLen >= strCap)
        return false;
    std::memset(strBuf, 0, strCap);
    unsigned int len = rawLen;
    // P168a①：bool callee 裸负形 = ORIG `call; xor$1,%eax; test%al; je` choose。
    if (!uncompress_zip(strBuf, len, zipBuf, static_cast<unsigned int>(zlen)))
        return false;
    return true;
}

} // namespace advancealtar
