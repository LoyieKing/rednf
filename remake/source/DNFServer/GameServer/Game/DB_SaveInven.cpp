// ============================================================================
// df_game_r 还原 —— DB_SaveInven 实现（ORIG 9 符号）
// 逐符号转写自 docs/class_func_reports/DB_SaveInven/（汇编+反编译 C 全文）。
// SQL 字符串、压缩区偏移（equipslot +0x19 0x2dc / inventory +0x557 0x3b55 /
// creature +0x710b 0x184e / cargo +0x895d 0x2438 / jewel +0x40ac 0xf03）、
// 异常码（0x4427/0x4429/0x4430，catch(unsigned int) 后 cMyTrace 记日志返 0）
// 与 ORIG 指令一一对应；SIG_SAVE_INVENTORY 字段语义见 SigTypes_E.h
// （pack(1)，sizeof 0xad99，2026-08-23 字段化）。
// 唯一 owner：本 TU。ORIG 地址核对（nm --defined-only df_game_r）：
//   C1/C2 0x08416a42 | D1/D2 0x08416a5e | D0 0x08416a8e | dispatch 0x08416aac
//   _saveCharacInvenExpand 0x08416b38 | SaveInven 0x08416cfa | SendResult 0x08417204
// ============================================================================

#include "DB_SaveInven.h"
#include "ARAD_Database.h"          // ARAD::DATABASE::source*Size（各快照长度）

#include "GlobalData.h"             // s_db_mgr
#include "DBMgr.h"                  // DBMgr::GetDBHandle
#include "MySQL.h"                  // MySQL::set_query/exec/blob_to_str
#include "GameTypes.h"              // cMyTrace
#include "AdvanceAltarZip.h"        // compress_zip（唯一实现归 APSystemOnlyOrig.cpp）
#include "SigTypes.h"               // SIG_SAVE_INVENTORY
#include "Stream.h"                 // Stream::GetOutBuffer
#include "CEnvironment.h"           // DB_HANDLE_3 / E_SERVER_GROUP_NONE

#include <string.h>                 // memset

// NumberToString 权威声明在 CInventory.h（0x0810904b u32 版），本 TU 仅前向
// 声明同名全局函数，与权威签名一致（W8LT_DBTask.cpp 同款做法）。
char* NumberToString(unsigned int value, int radix);

// ===========================================================================
// C1/C2 — DB_SaveInven.md @0x08416a42：基类 ctor + 设 vptr。
// D1/D2/D0 — ~.md @0x08416a5e/@0x08416a8e：设 vptr + 基类析构；D0 另 delete。
// 非 inline 定义发射全套生命周期符号。
// ===========================================================================
DB_SaveInven::DB_SaveInven() : DBDispatcher()
{
}

DB_SaveInven::~DB_SaveInven()
{
}

// ===========================================================================
// SendResult — SendResult.md @0x08417204：恒返 1（0x0a 字节）。
// ===========================================================================
int DB_SaveInven::SendResult(int userIdx, int slot, SIG_SAVE_INVENTORY* sig)
{
    return 1;
}

// ===========================================================================
// dispatch — dispatch.md @0x08416aac
// GetOutBuffer 判空返 0；SaveInven 失败返 0；m_saveFlag==0/2 时
// _saveCharacInvenExpand 失败返 0；否则 1。
// ===========================================================================
bool DB_SaveInven::dispatch(int userIdx, int slot, Stream* stream)
{
    bool r;
    SIG_SAVE_INVENTORY* sig = stream->GetOutBuffer<SIG_SAVE_INVENTORY>();
    if (sig == 0)
        return false;
    r = SaveInven(sig);
    if (r != 1)
        return false;
    if (sig->m_saveFlag == 0 || sig->m_saveFlag == 2)
    {
        r = _saveCharacInvenExpand(sig);
        if (r != 1)
            return false;
    }
    return true;
}

// ===========================================================================
// _saveCharacInvenExpand — 报告 @0x08416b38（const）
// cargo(+0x895d,0x2438)/jewel(+0x40ac,0xf03) 压缩失败返 0；
// upDate charac_inven_expand 后直接返回 exec 结果。
// ===========================================================================
bool DB_SaveInven::_saveCharacInvenExpand(SIG_SAVE_INVENTORY* sig) const
{
    static const int handleIdx = 3;
    // §4.3 裸偏移清零：五段压缩快照的起点全部落在 SIG_SAVE_INVENTORY 的具名
    // 成员上（SigTypes_E.h:106-112，sizeof(Inven_Item)==0x3d 由
    // SIG_SAVE_EXPAND_EQUIPSLOT 0x24×0x3d==0x894 实证）：
    //   cargo  +0x895d = m_cargo（长度 sourceCargoSize 0x2438 → +0xad95 =
    //          m_avatarCoin，首尾皆具名闭合）
    //   jewel  +0x40ac = m_invenType1 + sourceInvenSize(0x3b55)/0x3d = 下标 249
    //          （珠宝区紧接背包快照之后；ORIG T 0x08416b38 @报告 const 版）
    char cargo[0x2b82] = { 0 };
    unsigned int cargoLen = 0x2b82;
    if (compress_zip(cargo, cargoLen, (char*)sig->m_cargo,
                     ARAD::DATABASE::sourceCargoSize) != 1)
        return false;
    char jewel[0x120f] = { 0 };
    unsigned int jewelLen = 0x120f;
    if (compress_zip(jewel, jewelLen,
                     (char*)&sig->m_invenType1[
                         ARAD::DATABASE::sourceInvenSize / sizeof(Inven_Item)],
                     ARAD::DATABASE::sourceJewelSize) != 1)
        return false;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    db->set_query("upDate charac_inven_expand set cargo_capacity=%d,cargo='%s',jewel='%s' where charac_no=%u",
                  sig->m_cargoCapacity,
                  db->blob_to_str(0, cargo, cargoLen),
                  db->blob_to_str(1, jewel, jewelLen),
                  sig->m_characNo);
    return db->exec(true);
}

// ===========================================================================
// SaveInven — SaveInven.md @0x08416cfa（函数主体）
// m_saveFlag==0/2 时压缩 equipslot/inventory/creature 三区（失败抛 unsigned int
// 异常码并 catch 记日志返 0）；按 flag 分支 set_query；exec 成功后再
// inSert member_avatar_coin（m_id 用 NumberToString 十进制）。
// ===========================================================================
bool DB_SaveInven::SaveInven(SIG_SAVE_INVENTORY* sig)
{
    static const int handleIdx = 3;
    bool r;
    MySQL* db;
    char equipslot[0x37a] = { 0 };
    char inventory[0x473e] = { 0 };
    char creature[0x1d36] = { 0 };
    unsigned int equipslotLen = 0x37a;
    unsigned int inventoryLen = 0x473e;
    unsigned int creatureLen = 0x1d36;

    // §4.3 续：equipslot +0x019 = m_invenType0、inventory +0x557 =
    // m_invenType1、creature +0x710b = m_invenType3[140]
    //（(0x710b-0x4faf)/0x3d == 140 整除实证；sourceCreatureSize 0x184e =
    //  102×0x3d ⇒ 覆盖 m_invenType3[140..237] + m_creatureTail[0..3]，
    //  末址 0x8959 == &m_cargoCapacity，三段首尾与具名字段精确闭合）。
    if (sig->m_saveFlag == 0 || sig->m_saveFlag == 2)
    {
        try
        {
            if (compress_zip(equipslot, equipslotLen, (char*)sig->m_invenType0,
                             ARAD::DATABASE::sourceEquipSize) != 1)
                throw (unsigned int)0x4427;
            if (compress_zip(inventory, inventoryLen, (char*)sig->m_invenType1,
                             ARAD::DATABASE::sourceInvenSize) != 1)
                throw (unsigned int)0x4429;
            if (compress_zip(creature, creatureLen,
                             (char*)&sig->m_invenType3[140],
                             ARAD::DATABASE::sourceCreatureSize) != 1)
                throw (unsigned int)0x4430;
        }
        catch (unsigned int e)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x4438, 5)("Compress failed. charac_no(%d)(%d)", sig->m_characNo, e);
            return 0;
        }
    }

    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, E_SERVER_GROUP_NONE);
    if (sig->m_saveFlag == 0)
    {
        db->set_query("upDate inventory set money=%u,coin=%d,pay_coin=%d,event_coin=%d,equipslot='%s',inventory='%s',creature='%s' where charac_no=%u",
                      sig->m_money, sig->m_coin, sig->m_payCoin, sig->m_eventCoin,
                      db->blob_to_str(0, equipslot, equipslotLen),
                      db->blob_to_str(1, inventory, inventoryLen),
                      db->blob_to_str(2, creature, creatureLen),
                      sig->m_characNo);
    }
    else if (sig->m_saveFlag == 1)
    {
        db->set_query("upDate inventory set money=%u,coin=%d,pay_coin=%d,event_coin=%d  where charac_no=%u",
                      sig->m_money, sig->m_coin, sig->m_payCoin, sig->m_eventCoin,
                      sig->m_characNo);
    }
    else
    {
        db->set_query("upDate inventory set money=%u,equipslot='%s',inventory='%s',creature='%s' where charac_no=%u",
                      sig->m_money,
                      db->blob_to_str(0, equipslot, equipslotLen),
                      db->blob_to_str(1, inventory, inventoryLen),
                      db->blob_to_str(2, creature, creatureLen),
                      sig->m_characNo);
    }

    r = db->exec(true);
    if (r != 1)
        return 0;

    db->set_query("inSert into member_avatar_coin (m_id , avatar_coin ) values (%s , %u) on duplicate key upDate avatar_coin = %u ",
                  NumberToString(sig->m_accId, 0), sig->m_avatarCoin, sig->m_avatarCoin);
    r = db->exec(true);
    if (r != 1)
        return 0;
    return 1;
}