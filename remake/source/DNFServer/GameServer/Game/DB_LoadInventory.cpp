// ============================================================================
// df_game_r 还原 —— DB_LoadInventory.cpp
// 目标：清零 /tmp/only_orig/DB_LoadInventory.txt 全部 14 个 ORIG 符号。
// 每函数按 docs/class_func_reports/DB_LoadInventory/<fn>.md（汇编为准）转写。
// SIG_LOAD_INVENTORY 权威布局见 SigTypes_C.h（pack(1)，size 0x1544c）：
//   +0x004 m_characNo | +0x008 m_field8(tag charac_no) | +0x00c m_tagInven
//   | +0x4030 m_result | +0x4034 m_money | +0x4044 m_invenType2（avatar 主区）
//   | +0x5949 m_invenType1 | +0xa3a1 m_equipMirror | +0xa8e0 m_cargoCapacity
//   | +0xa8e4 m_cargo | +0xcd1c m_avatarCount | +0xcd20 m_avatarDetail
//   | +0xeed1 m_invenType3（creature 槽位区，步长 0x3d）| +0x1287c m_creatureCount
//   | +0x12880 m_creature（0x38 步长详情条目）| +0x14780 m_avatarCoin
//   | +0x14784 m_expandData2（GetItemLock 输出区）| +0x1533c m_inventoryCapacity
//   | +0x15448 m_maxEquipLevel
// ============================================================================

#include <string.h>
#include <stdio.h>
#include <vector>

#include "DB_LoadInventory.h"

#include "MySQL.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "GameTypes.h"          // cMyTrace
#include "LogManager.h"         // logFormat
#include "AdvanceAltarZip.h"    // uncompress_zip 权威声明
#include "Inven_Item.h"
#include "GamePODTypes.h"      // SIG_CREATURE_ITEM_DETAIL_INFO/AvatarDetailInfo 权威
                              // 轻量叶子头（ORIG 本 TU 无 ios_base static-init）
#include "CItemAmplifier.h"     // stAmplifyOption_t::SetLock/GetLock
#include "W8LT_DBTask.h"        // DB_SwapCreatureItem 权威声明（makeRequest）

// ---- 本 TU 转接声明（UNLINKED_CALLEE，源码暂无 owner；照常调用，保持
// ORIG 全局 mangled 名以便链接器命中真实 T 符号）----
// ORIG T _Z24get_compressed_blob_dataP5MySQLiPcii @0x083fbc79
// （定义在 DB_Login.cpp:1967）。
bool get_compressed_blob_data(MySQL* db, int col, char* out, int outCap,
                              int recordSize);

extern char* NumberToString(unsigned int value, int radix);  // C++ linkage

// ============================================================================
// DB_LoadInventory::DB_LoadInventory @0x0840a604（C1/C2 同址）
// ORIG：call DBDispatcher::DBDispatcher(this) 后设 vptr；基类 ctor/dtor 为空体，
// 对象尺寸 4。析构链（D1 0x0840a620 设 vptr→基类空 dtor→恒假 delete 分支；
// D0 0x0840a650 再 operator delete）由类定义自然产出同形符号。
// ============================================================================
DB_LoadInventory::DB_LoadInventory()
{
}

// ============================================================================
// ~DB_LoadInventory @0x0840a620（D1/D2）/ deleting dtor @0x0840a650（D0）
// ============================================================================
DB_LoadInventory::~DB_LoadInventory()
{
}

// ============================================================================
// DB_LoadInventory::dispatch @0x0840a66e
// 基类 dispatch 通过 → GetOutBuffer<SIG_LOAD_INVENTORY> → GetUserInven/
// GetUserAvatar/creature 槽 reset/GetUserCreature/_getCharacInvenExpand/
// GetItemLock/_GetEquipMaxItemLevel，任一步失败 throw int（1/2/3/4/6/7），
// catch 后置 sig->m_result=1 继续 SendResult。错误行号 0x2364..0x23a4。
// b28d-solo-DB_LoadInventory：P227 全表达式临时链（6 处 trace 的 ebx 保活）
// + 尾 SendResult 具名 bool 早返已归位；残差 = 4.4 EH 帧/槽布局（sub 0x94 vs
// 0x84、sig -0x1c vs -0x14、catch 段 __cxa_get_exception_ptr + _Unwind_Resume）
// ⇒ N104/跨版本 EH 封顶；p167 差集 MISSING=0（语义完整）。
// ============================================================================
bool DB_LoadInventory::dispatch(int userIdx, int slot, Stream* stream)
{
    // P212/P309 试验：全头部声明 + 声明序按 ORIG 槽深序（sig -0x14 / err -0x10
    // / i -0xc），复现 4.1「先声明深」的标量区布局。
    SIG_LOAD_INVENTORY* sig;
    int err;
    int i;
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
    {
        return false;
    }

    // Stream::GetOutBuffer<SIG_LOAD_INVENTORY>(param_3)（模板实例本 TU 隐式
    // 产出，ORIG 对应 W 实例符号）。
    sig = stream->GetOutBuffer<SIG_LOAD_INVENTORY>();
    if (sig == 0)
    {
        return false;
    }

    try
    {
        if (!GetUserInven(sig))
        {
            // P227 全表达式临时链：实参在 ctor 前求值并入 %ebx 跨 ctor 保活
            // （ORIG `mov 0x4(%eax),%ebx` + `mov %ebx,0x8(%esp)`）。
            cMyTrace(__PRETTY_FUNCTION__, 0x2364, 5)
                ("DB_LoadInventory::dispatch, GetUserInven ERROR charac_no=%u",
                 sig->m_characNo);
            throw 1;
        }
        if (!GetUserAvatar(sig))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2369, 5)
                ("DB_LoadInventory::dispatch, GetUserAvatar ERROR charac_no=%u",
                 sig->m_characNo);
            throw 2;
        }
        // 140 个 creature 槽 + 1 个尾槽 reset（Inven_Item::reset W 0x080cb7d8）。
        for (i = 0; i < 0x8c; ++i)
        {
            sig->m_invenType3[i].reset();
        }
        sig->m_invenType3[0xee].reset();   // 尾槽 0x12787 = 0xeed1+0xee×0x3d

        if (sig->m_creatureCount != 0)
        {
            if (!GetUserCreature(sig))
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x2379, 5)
                    ("DB_LoadInventory::dispatch, GetUserCreature ERROR"
                     " charac_no=%u", sig->m_characNo);
                throw 3;
            }
        }
        err = _getCharacInvenExpand(sig);
        if (err != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2381, 5)
                ("DB_LoadInventory::dispatch, _getCharacInvenExpand ERROR"
                 " charac_no=%u, line(%d)", sig->m_characNo, err);
            throw 4;
        }
        err = GetItemLock(sig->m_characNo,
                          &sig->m_locks[0]);
        if (err != 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2395, 5)
                ("DB_LoadInventory::dispatch, GetItemLock ERROR charac_no=%u,"
                 " line(%d)", sig->m_characNo, err);
            throw 6;
        }
        if (!_GetEquipMaxItemLevel(sig))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x23a4, 5)
                ("DB_LoadInventory::dispatch, _GetEquipMaxItemLevel ERROR"
                 " charac_no=%u", sig->m_characNo);
            throw 7;
        }
    }
    catch (int)
    {
        sig->m_result = 1;      // ORIG catch handler @0x84aa6a：+0x4030 置 1
    }
    // ORIG 尾形：SendResult 结果落具名 bool 槽 -0x15 → `xor $1; test; je` 分支，
    // 成功 `mov $1,%eax`、失败 `mov $0,%eax`（直接 return 调用被 4.4 tail 化）。
    bool sent = SendResult(userIdx, slot, sig);
    if (!sent)
    {
        return false;
    }
    return true;
}

// ============================================================================
// DB_LoadInventory::SendResult @0x0840da6e：直接转发 SendResult_forchina。
// ============================================================================
bool DB_LoadInventory::SendResult(int userIdx, int slot, SIG_LOAD_INVENTORY* sig)
{
    char frame[0x10];      // ORIG 0840xxxx sub $0x28（4.1 转发帧留白）；4.4 叶折 0x18
    (void)frame;
    return SendResult_forchina(userIdx, slot, sig);
}

// ============================================================================
// DB_LoadInventory::SendResult_forchina @0x0840d98c
// BigStreamPool::Acquire("DBThread.cpp",0x2939) → guard<<userIdx<<slot →
// put_binary(sig,0x1544c) → MsgQueueMgr::put(s_msgq_mgr,(QUEUE_IDX)1,guard)。
// ============================================================================
bool DB_LoadInventory::SendResult_forchina(int userIdx, int slot,
                                           SIG_LOAD_INVENTORY* sig)
{
    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("DBThread.cpp", 0x2939),
                       true);
    guard.operator*()->operator<<(userIdx);          // op*(guard)→op<<(int)
    guard.operator*()->operator<<(slot);             // op*(guard)→op<<(int)
    guard.operator->()->put_binary(sig, 0x1544c);    // op->(guard)→put_binary
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
    return true;                                     // ORIG ebx=1 恒定
}

// ============================================================================
// DB_LoadInventory::GetUserInven @0x0840b0d8
// 列序 money(0)/coin(1)/inventory(2)/equipslot(3)/pay_coin(4)/event_coin(5)/
// creature(6)/creature_flag(7)/inventory_capacity(8)。三 blob 分别解压到
// +0x5949 / +0xa603 / +0x1102d，容量 0x473e/0x37a/0x1d36；尺寸校验
// equip 0x53e / inven 0x4a58 / creature 0x4a58 对 0x3d 对齐。随后
// member_avatar_coin 聚合查询入 +0x14780；tag charac（+0x8）非零走
// _GetTagCharacInven。blob 溢出/uncompress 失败 throw unsigned int
// （0x255a/0x255c/0x2563/0x2565/0x2567/0x256e），catch 后 logFormat
// 0x259c 并返 0。错误行号 0x24cb..0x25b2。
// b28d-solo-DB_LoadInventory：ok 变量（14 处共用槽）+ P227 临时链 +
// len/binLen 副本（P171，声明序对齐 ORIG 槽）已归位；残差 = 4.4 标量分区
// （ok -0x25 vs -0x31、binLen 副本整体深 4）⇒ N82 封顶；p167 MISSING=0。
// 本轮复核修复（真语义 bug）：creature blob 解压目标。ORIG 0840b8ef
// `add $0x1102d,%eax`（= 0xeed1+0x8c×0x3d = m_invenType3[0x8c] 首槽，
// 0x8c 正是 dispatch reset 循环 0..0x8b 之后第一个 creature 槽）；旧译
// `&m_invenType3[72].m_reserved` 编译成 +0x1002d，与 ORIG 差 0x1000 字节
// ⇒ blob 落到错误槽区。已改为 `&m_invenType3[0x8c]`，.o 立即数 0x1102d
// 与 ORIG 逐字节一致（四数不变、p167 81/81 CLEAR）。
// ============================================================================
bool DB_LoadInventory::GetUserInven(SIG_LOAD_INVENTORY* sig)
{
    static const int handleIdx = 3;
    // b28d-214③/N78：ORIG 对三缓冲区发 rep stos 内联清零（无 memset call）
    // ⇒ 用数组零初始化器复现。
    char equipBuf[0x37a] = {0};
    char invenBuf[0x473e] = {0};
    char creatureBuf[0x1d36] = {0};
    // ORIG 标量区（4.1 先声明浅）：creatureLen -0x148 / equipLen -0x14c /
    // invenLen -0x150；binLen 副本 -0x30/-0x2c/-0x28 成对初始化（P171）。
    unsigned int creatureLen = 0x1d36;  // ORIG -0x148
    unsigned int equipLen = 0x37a;      // ORIG -0x14c（解压 in/out 容量）
    unsigned int invenLen = 0x473e;     // ORIG -0x150
    int invenBinLen = invenLen;         // ORIG -0x30
    int equipBinLen = equipLen;         // ORIG -0x2c
    int creatureBinLen = creatureLen;   // ORIG -0x28
    bool ok;                            // ORIG -0x31（所有 db 检查共用槽）

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect money,coin,inventory,equipslot,pay_coin,event_coin,"
                  "creature,creature_flag,inventory_capacity from inventory"
                  " where charac_no=%u", sig->m_characNo);
    ok = db->exec(true);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24cb, 5)
            ("DB_LoadInventory::GetUserInven, exec ERROR charac_no=%u",
             sig->m_characNo);
        return 0;
    }
    ok = db->fetch();
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24d2, 5)
            ("DB_LoadInventory::GetUserInven, fetch ERROR charac_no=%u",
             sig->m_characNo);
        return 0;
    }
    ok = db->get_uint(0, *(unsigned int*)&sig->m_money);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24d9, 5)
            ("DB_LoadInventory::GetUserInven, get_uint(0, pContext->money)"
             " ERROR charac_no=%u", sig->m_characNo);
        return 0;
    }
    ok = db->get_uint(1, sig->m_coin);             // +0x4038（unsigned int 直引）
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24e0, 5)
            ("DB_LoadInventory::GetUserInven, get_uint(1, pContext->coin)"
             " ERROR charac_no=%u", sig->m_characNo);
        return 0;
    }
    invenBinLen = db->get_binary_length(2);
    ok = db->get_binary(2, invenBuf, invenLen);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24e8, 5)
            ("DB_LoadInventory::GetUserInven, get_binary(2) ERROR"
             " charac_no=%u", sig->m_characNo);
        return 0;
    }
    equipBinLen = db->get_binary_length(3);
    ok = db->get_binary(3, equipBuf, equipLen);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24f0, 5)
            ("DB_LoadInventory::GetUserInven, get_binary(3) ERROR"
             " charac_no=%u", sig->m_characNo);
        return 0;
    }
    ok = db->get_uint(4, sig->m_payCoin);          // +0x403c
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24f7, 5)
            ("DB_LoadInventory::GetUserInven, get_uint(4) ERROR charac_no=%u",
             sig->m_characNo);
        return 0;
    }
    ok = db->get_uint(5, sig->m_eventCoin);        // +0x4040
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24ff, 5)
            ("DB_LoadInventory::GetUserInven, get_uint(5) ERROR charac_no=%u",
             sig->m_characNo);
        return 0;
    }
    // 注：ORIG 此处用 col 变量（-0x20）+ col++ 形；4.4 引入该变量会把整个
    // 标量区推深 4（len -0x14c/-0x150/-0x154），净劣 ⇒ 保持字面量（N82 封顶）。
    creatureBinLen = db->get_binary_length(6);
    ok = db->get_binary(6, creatureBuf, creatureLen);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x251b, 5)
            ("DB_LoadCreature, get_binary(8) ERROR charac_no=%u",
             sig->m_characNo);
        return 0;
    }
    ok = db->get_int(7, sig->m_creatureCount);     // +0x1287c（int 直引）
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x2522, 5)
            ("DB_LoadInventory::GetUserInven, get_uint(9) ERROR charac_no=%u",
             sig->m_characNo);
        return 0;
    }
    ok = db->get_uint(8, *(unsigned int*)&sig->m_inventoryCapacity);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x253a, 5)
            ("DB_LoadInventory::GetUserInven, get_uint(13) ERROR charac_no=%u",
             sig->m_characNo);
        return 0;
    }

    try
    {
        if (equipLen < static_cast<unsigned int>(equipBinLen))
        {
            throw static_cast<unsigned int>(0x255a);
        }
        if (invenLen < static_cast<unsigned int>(invenBinLen))
        {
            throw static_cast<unsigned int>(0x255c);
        }
        if (creatureLen < static_cast<unsigned int>(creatureBinLen))
        {
            throw static_cast<unsigned int>(0x2563);
        }
        // equipslot 解压目标 = equip 镜像区第 10 槽起（GetUserInven.md ORIG
        // `add $0xa603` = 0xa3a1+10×0x3d）；inventory blob → m_invenType1 首
        // （ORIG +0x5949）；creature blob → +0x1102d = 0xeed1+0x8c×0x3d =
        // m_invenType3[0x8c] 首槽（ORIG 0840b8ef `add $0x1102d,%eax` 实证；
        // 0x8c 正是 dispatch reset 循环 0..0x8b 之后的第一个 creature 槽）。
        // 旧译写作 `m_invenType3[72].m_reserved`（= +0x1002d），与 ORIG 差
        // 0x1000 字节 ⇒ 真语义 bug（creature blob 落到错误槽区）。
        if (!uncompress_zip(reinterpret_cast<char*>(&sig->m_equipMirror[10]),
                            equipLen, equipBuf,
                            static_cast<unsigned int>(equipBinLen)))
        {
            throw static_cast<unsigned int>(0x2565);
        }
        if (!uncompress_zip(reinterpret_cast<char*>(sig->m_invenType1), invenLen,
                            invenBuf,
                            static_cast<unsigned int>(invenBinLen)))
        {
            throw static_cast<unsigned int>(0x2567);
        }
        if (!uncompress_zip(
                reinterpret_cast<char*>(&sig->m_invenType3[0x8c]),
                creatureLen, creatureBuf,
                            static_cast<unsigned int>(creatureBinLen)))
        {
            throw static_cast<unsigned int>(0x256e);
        }
    }
    catch (unsigned int err)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x259c, "Uncompress failed. charac_no(%d)(%d)",
                              sig->m_characNo, err);
        return 0;
    }

    if ((equipLen % 0x3d) == 0 || equipLen == 0)
    {
        if ((invenLen % 0x3d) == 0 || invenLen == 0)
        {
            if ((creatureLen % 0x3d) == 0 || creatureLen == 0)
            {
                char* m = NumberToString(sig->m_accId, 0);
                db->set_query("seLect ifnull(min(avatar_coin),0) as avatar_coin"
                              " from member_avatar_coin where m_id = %s", m);
                // ORIG 尾部为 guard 形（fail 块直落、je 跳主流程）⇒ 逐个早返。
                ok = db->exec(true);
                if (!ok)
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x25a6, 5)
                        ("DB_LoadInventory::GetUserInven, avatar_coin exec"
                         " ERROR m_id=%s", NumberToString(sig->m_accId, 0));
                    return 0;
                }
                ok = db->fetch();
                if (!ok)
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x25ac, 5)
                        ("DB_LoadInventory::GetUserInven, avatar_coin fetch"
                         " ERROR m_id=%s", NumberToString(sig->m_accId, 0));
                    return 0;
                }
                ok = db->get_uint(0, sig->m_avatarCoin);   // +0x14780
                if (!ok)
                {
                    cMyTrace(__PRETTY_FUNCTION__, 0x25b2, 5)
                        ("DB_LoadInventory::GetUserInven, get_uint(%u,"
                         " pContext->avatar_coin) ERROR m_id=%s",
                         NumberToString(sig->m_accId, 0));
                    return 0;
                }
                if (sig->m_field8 != 0)
                {
                    _GetTagCharacInven(sig->m_field8, &sig->m_tagInven);
                }
                return 1;
            }
            cMyTrace(__PRETTY_FUNCTION__, 0x2588, 5)
                ("incorrect inventory size charac_no(%d), creature,"
                 " original_size(%d), db_size(%d)\n", sig->m_characNo, 0x4a58,
                 creatureLen);
            return 0;
        }
        cMyTrace(__PRETTY_FUNCTION__, 0x2579, 5)
            ("incorrect inventory size charac_no(%d), inven,"
             " original_size(%d), db_size(%d)\n", sig->m_characNo, 0x4a58,
             invenLen);
        return 0;
    }
    cMyTrace(__PRETTY_FUNCTION__, 0x2573, 5)
        ("incorrect inventory size charac_no(%d), equip,"
         " original_size(%d), db_size(%d)\n", sig->m_characNo, 0x53e, equipLen);
    return 0;
}

// ============================================================================
// DB_LoadInventory::GetUserAvatar @0x0840bd3a
// 主 charac（+4）与 tag charac（+8，非零才做）各调 _GetUserAvatar：
//   主：sig+0xcd20 / sig+0x4044 / sig+0xa3a1 / sig+0xcd1c
//   tag：sig+0x1e53 / sig+0x54a / sig+0xc / sig+0x1e4f
// 错误行号 0x25cb / 0x25d7。
// b28d-solo-DB_LoadInventory：AE（87=87 仅重定位差）。修法 = P59 guard 形
// （if(!ok) 早返，ORIG `movzbl; xor $1; test; je` fail 直落）+ P227 全表达式
// 临时链（实参入 %ebx 跨 ctor 保活）。
// ============================================================================
bool DB_LoadInventory::GetUserAvatar(SIG_LOAD_INVENTORY* sig)
{
    bool ok = _GetUserAvatar(sig->m_characNo,
                             reinterpret_cast<AvatarDetailInfo*>(sig->m_avatarDetail),
                             sig->m_invenType2,
                             sig->m_equipMirror,
                             sig->m_avatarCount);
    // P59 guard 形：ORIG `movzbl -0x9; xor $1; test; je <主流程>`（fail 块直落）
    // ⇒ 必须写 if(!ok) 早返，写 if(ok){...}else{...} 会被 4.4 折 cmpb 直比。
    // P227 全表达式临时链：实参在 ctor 前求值并以 %ebx 跨 ctor 保活（ORIG 形）。
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x25cb, 5)
            ("DB_LoadInventory::_GetUserAvatar ERROR::charac_no(%d)",
             sig->m_characNo);
        return 0;
    }
    if (sig->m_field8 != 0)
    {
        ok = _GetUserAvatar(sig->m_field8,
                            reinterpret_cast<AvatarDetailInfo*>(
                                sig->m_tagInven.m_avatarDetail),      // +0x1e53
                            sig->m_tagInven.m_items2,                 // +0x54a
                            sig->m_tagInven.m_items,                  // +0xc
                            sig->m_tagInven.m_avatarCount);           // +0x1e4f
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x25d7, 5)
                ("DB_LoadInventory::_GetUserAvatar ERROR::tag_charac_no(%d)",
                 sig->m_field8);
            return 0;
        }
    }
    return 1;
}

// ============================================================================
// DB_LoadInventory::_GetUserAvatar @0x0840be8a（static bool）
// §6.2 收口注记（b28d 复攻，gap 76→1）：语义/列序/行号与 ORIG 全对齐。
// 本轮修复（逐段对照 ORIG 汇编 + LCS 段级归一化多重集）：
//   ①【真语义 bug】slot<10 与 slot>=10 的目标数组写反：ORIG 0840c971 的
//     slot<10 分支基址是 `add 0x14(%ebp),%eax`（第 4 形参 = 调用点 sig+0xa3a1），
//     0840c895 的 idx 分支是 `add 0x10(%ebp),%eax`（第 3 形参 = sig+0x4044）；
//     旧码 slot<10 写 inven（第 3 参）、slot>=10 写 mirror（第 4 参）——反了。
//   ② P183 去缓存：删 `AvatarDetailInfo& e = out[row]` 与 `Inven_Item* item`，
//     每写点重发 `mov row,%eax; imul $0x4b; add 0xc(%ebp),%eax`（14 写点 ×3 条）。
//   ③ 列索引具名局部 `int col = 7;` + `get_*(col++)`（8 处 ×2 条）+ col10/col11
//     错误分支 P227 全表达式临时链用 `col - 1`。
//   ④ P312：ipgAgencyNo 声明 0x19 字节 ⇒ 落非 4 对齐槽 -0x5d，4.4 发与 ORIG
//     同形的「对齐剥离 + rep stosl」慢路（0840bf59..0840bfbd）。
//   ⑤ 声明序照 ORIG 清零序 uiId→clearAvatarId→count→abilityNo；slot/itId 去 `=0`。
//   ⑥ color1/color2 写点前移到 get_byte(14) 之前（ORIG 0840c6fe/712 < 0840c726）。
//   ⑦ P223 三失败出口 goto 共享尾标签 ⇒ 复现 0840ca41/44/47 三个 `nop;jmp` 岛。
//   ⑧ trade_restrict 位：常量位域赋值 `m_bits.m_flag = 1/0` 复现 ORIG 单次基址
//     `movzbl 0x33(%eax); or $0x20/and $0xdf; mov %dl,0x33(%eax)`（m_field 复合
//     赋值 4.4 恒两次基址 +3 条/处；探针 /tmp/p_sep.cpp f1 vs f4）。
// 剩余真缺口 = 0；唯一净差 1 条 = count 赋值 `mov %eax,%edx; mov 0x18(%ebp),%eax;
//   mov %edx,(%eax)`（4.1 RHS-first+中转 vs 4.4 LHS-first 直存）⇒ N76/P211 封顶：
//   异型化旋钮在共享头 MySQL::get_n_rows 返回类型（改它牵动全仓符号性比较），
//   按 S2 不在本泳道改；unsigned 视图/cast/字节视图三形实测均劣
//   （698/710/712 vs 695，且视图形占用 %ebx/%edx 破坏 callee-saved 布局）。
// 残差余项 = 槽号分区（db -0x1c vs -0x18、trace -0x7c vs -0x12c）⇒ N82/N95 豁免。
// p167 调用点差集 = C1/C2 同址别名伪差（59/59，P93 豁免）。
// user_items 15 列（limit 0x73）。AvatarDetailInfo 条目步长 0x4b：
//   +0 ui_id / +4 expire / +8 reg / +0xc ipgAgencyNo(0x18) / +0x25 clearAvatarId
//   / +0x29 jewelSocket(0x1e) / +0x47 color1 / +0x49 color2。
// slot<10 → 第 4 形参（mirror 名，调用点 sig+0xa3a1）；10<=slot-10<0x69 →
// 第 3 形参（inven 名，调用点 sig+0x4044）。Inven_Item 写：m_field1=8、
// m_fieldb=ability_no、m_addInfo=it_id、m_addInfo2=ui_id、m_amp lock、
// m_fielddLo=hidden、m_avatarEndur=endurance、+0x33 trade_restrict 位 0x20。
// 错误行号 0x26dc..0x277d（col12 失败仅记日志）。
// b28d-solo-DB_LoadInventory：17 处 trace 改 __PRETTY_FUNCTION__（ORIG 有
// _ZZ...E19__PRETTY_FUNCTION__ 符号）+ 前 6 处 db 检查改共用 bool ok（ORIG
// `mov %al,-0x19`）。
// ============================================================================
bool DB_LoadInventory::_GetUserAvatar(unsigned int characNo,
                                      AvatarDetailInfo* out,
                                      Inven_Item* inven, Inven_Item* mirror,
                                      int& count)
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect slot,it_id,unix_timestamp(expire_date),ability_no,"
                  "ipg_agency_no,unix_timestamp(reg_date),ui_id,"
                  "clear_avatar_id,jewel_socket,item_lock_key, hidden_option,"
                  " emblem_endurance, color1, color2, trade_restrict"
                  " from user_items where charac_no=%u and expire_date>now()"
                  " and stat=0 limit %d", characNo, 0x73);
    // ORIG：exec/fetch/get_int(0)/get_int(1)/get_uint(2)/get_int(3) 六处共用
    // 函数级 bool 槽 -0x19（`mov %al,-0x19; movzbl`），其余 get_* 内联 xor 判。
    bool ok = db->exec(true);
    if (!ok)
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x26dc, 5);
        t("DB_LoadInventory::GetUserAvatar, exec() ERROR charac_no=%u", characNo);
        return 0;
    }

    unsigned int expireDate = 0;
    int regDate = 0;
    // b28d-214③/N78：ORIG rep stos 内联清零（无 memset call）⇒ 数组零初始化器。
    // P312：ORIG 数组落在 -0x155（非 4 对齐）且 rep stos 展开的 len 立即数为
    // 0x19（0840bf5f `mov $0x19,%edx`，dword 计数前的字节数）⇒ 原码数组 25 字节。
    // 声明 0x19 字节使 4.4 把起始槽顶到非 4 对齐（-0x5d），复现「对齐剥离 +
    // rep stosl」慢路（0840bf59..0840bfbd 共 34 条）；get_str 仍只写 0x18。
    char ipgAgencyNo[0x19] = {0};
    // ORIG 清零/赋值序（0840bfbd..0840bfe3）：uiId(-0x15c) → clearAvatarId
    // (-0x160) → get_n_rows/count → abilityNo(-0x164) ⇒ 声明序照抄。
    int uiId = 0;
    int clearAvatarId = 0;
    // ORIG 0840bfdc 为 `mov %eax,%edx; mov 0x18(%ebp),%eax; mov %edx,(%eax)`
    // （RHS-first + 中转）；4.4 对同型 int& 恒 LHS-first，视图/cast 形实测均劣化
    // （710 vs 707）⇒ 记 N76/P211 封顶（改 get_n_rows 返回类型属共享头 S2 范围）。
    count = db->get_n_rows();
    int abilityNo = 0;
    int row = 0;
    while (row < db->get_n_rows())
    {
        ok = db->fetch();
        if (!ok)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x26f3, 5);
            t("DB_LoadInventory::GetUserAvatar, fetch() ERROR charac_no=%u,%d",
              characNo, row);
            break;
        }
        // ORIG 0840c061：get_int 目标槽直接取址（`lea -0x130(%ebp),%eax`），
        // 无预清零 ⇒ 不得写 `int slot = 0;`（多一条 movl $0x0）。
        int slot;
        ok = db->get_int(0, slot);
        if (!ok)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x26fa, 5);
            t("DB_LoadInventory::GetUserAvatar, get_int(0) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        int itId;
        ok = db->get_int(1, itId);
        if (!ok)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2701, 5);
            t("DB_LoadInventory::GetUserAvatar, get_int(1) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        ok = db->get_uint(2, expireDate);
        if (!ok)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2708, 5);
            t("DB_LoadInventory::GetUserAvatar, get_uint(2) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        ok = db->get_int(3, abilityNo);
        if (!ok)
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x270f, 5);  // ORIG 0840c1fc
            t("DB_LoadInventory::GetUserAvatar, get_int(3) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        if (!db->get_str(4, ipgAgencyNo, 0x18))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2727, 5);
            t("DB_LoadInventory::GetUserAvatar, get_int(4) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        if (!db->get_int(5, regDate))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x272e, 5);
            t("DB_LoadInventory::GetUserAvatar, get_int(5) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        if (!db->get_int(6, uiId))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2734, 5);
            t("DB_LoadInventory::GetUserAvatar, get_int(6) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        // ORIG 0840c39c：列索引具名局部 `col` 从 7 起后自增（每处
        // `mov -0x10(%ebp),%eax; addl $0x1,-0x10(%ebp)`），错误分支日志用 col-1。
        int col = 7;
        if (!db->get_int(col++, clearAvatarId))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x273b, 5);
            t("DB_LoadInventory::GetUserAvatar, get_int(6) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        stAvatarEmblemInfo_t jewel;
        jewel.init();
        if (!db->get_binary(col++, &jewel, 0x1e))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2742, 5);
            t("DB_LoadInventory::GetUserAvatar, get_int(6) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        unsigned char lockKey = 0;
        if (!db->get_ubyte(col++, lockKey))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2749, 5);
            t("DB_LoadInventory::GetUserAvatar, get_ubyte(8) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        unsigned int hiddenOption = 0;     // ORIG local_190[1]（col10）
        if (!db->get_uint(col++, hiddenOption))
        {
            // ORIG 0840c547 在 ctor 前 `mov -0x10,%eax; lea -0x1(%eax),%ebx`
            // 跨 ctor 保活 ⇒ P227 全表达式临时链。
            cMyTrace(__PRETTY_FUNCTION__, 0x275c, 5)
                ("DB_LoadInventory::GetUserAvatar, get_int(%d) ERROR"
                 " charac_no=%u", col - 1, characNo);
            return 0;
        }
        unsigned int emblemEndurance = 0;  // ORIG local_190[0]（col11）
        if (!db->get_uint(col++, emblemEndurance))
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2762, 5)
                ("DB_LoadInventory::GetUserAvatar, get_int(%d) ERROR"
                 " charac_no=%u", col - 1, characNo);
            return 0;
        }
        short color1 = 0;
        if (!db->get_short(col++, color1))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x276a, 5);
            t("DB_LoadInventory::GetUserAvatar, get_short(10) ERROR charac_no=%u",
              characNo);
            // ORIG：本列失败仅记录日志，继续执行
        }
        short color2 = 0;
        if (!db->get_short(col++, color2))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2771, 5);
            t("DB_LoadInventory::GetUserAvatar, get_short(10) ERROR charac_no=%u",
              characNo);
            return 0;
        }
        // ORIG 0840c6fe/0840c712：两个 color 写点在 tradeRestrict 初始化
        // （0840c726）与 get_byte(14) 之前 —— 原码写序如此，勿合并到尾部。
        out[row].m_expansionInfo.m_color[0] = color1;  // +0x47（movw）
        out[row].m_expansionInfo.m_color[1] = color2;  // +0x49（movw）
        char tradeRestrict = 0;
        if (!db->get_byte(col++, tradeRestrict))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x277d, 5);
            t("DB_LoadInventory::GetUserAvatar, get_byte(11) ERROR charac_no=%u",
              characNo);
            return 0;
        }

        // ---- AvatarDetailInfo 条目（步长 0x4b，ORIG _GetUserAvatar.md 写点：
        // +0x47/+0x49 movw short、+0x04/+0x00/+0x08 movl、+0x0c strncpy 0x18、
        // +0x25 movl、+0x29 起 7×movl+1×movw 共 0x1e 字节）----
        // P183：ORIG 每个写点重发 `mov -0x14(%ebp),%eax; imul $0x4b,%eax,%eax;
        // add 0xc(%ebp),%eax`（0840c6fe/712/71b/…/805），禁缓存 `T& e` 引用。
        out[row].m_expireDate = expireDate;            // +0x04（movl）
        out[row].m_key = uiId;                         // +0x00（movl）
        out[row].m_registTime = regDate;               // +0x08（movl）
        strncpy(out[row].m_ipgAgencyNo, ipgAgencyNo, 0x18);  // +0x0c
        out[row].m_clearAvatarId = clearAvatarId;      // +0x25（movl）
        // b28d-83/84：ORIG +0x29 为 7×movl+1×movw 结构整体赋值（非 memcpy call）
        // ⇒ 用 stAvatarEmblemInfo_t 结构赋值（同 CInventory.cpp:5554 镜像法）。
        out[row].m_jewelSocket = jewel;                // +0x29（ORIG 逐字段 8 写）

        // ---- Inven_Item 槽位（avatar 主区 / equip 镜像区）----
        // ORIG 0840c85b：`cmp $0x72; jg 尾` → `cmp $9; jle 主区`（主区物理在后）
        // ⇒ 镜像分支 fall-through、主区分支后置。主区入口 0840c956 还有一次
        // 冗余 `cmp $9; jg 尾`（原码嵌套 if 遗留，4.1 -O0 照发）。
        // P223：ORIG 三个失败出口各自在函数尾成 `nop; jmp join` 岛
        // （0840ca42/0840ca45/0840ca47 前的 jmp），4.4 复现形 = 负条件 goto
        // 到共享尾空标签。
        if (slot >= 0x73) goto L_avatarJoin;
        if (slot >= 10)
        {
            int idx = slot - 10;
            if (idx >= 0x69) goto L_avatarJoin;
            {
                    // ORIG 0840c88f..0840ca3f：idx 分支基址 `add 0x10(%ebp),%eax`
                    // = 第 3 形参（调用点 sig+0x4044）。P183：每写点重发
                    // `mov -0xc(%ebp),%eax; imul $0x3d,%eax,%eax; add 0x10(%ebp),%eax`。
                    inven[idx].m_field1 = Inven_Item::ITEM_TYPE_AVATAR;
                    inven[idx].m_fieldb = static_cast<unsigned short>(abilityNo);
                    inven[idx].m_addInfo = itId;
                    inven[idx].m_addInfo2 = uiId;
                    inven[idx].m_amp.SetLock(lockKey);
                    inven[idx].m_fieldSplit.m_fielddLo =
                        static_cast<short>(hiddenOption);
                    inven[idx].m_fieldSplit.m_avatarEndur =
                        static_cast<unsigned short>(emblemEndurance);
                    // ORIG 0840c92f/0840c947：单次基址 `movzbl 0x33(%eax),%edx;
                    // or $0x20,%edx; mov %dl,0x33(%eax)`（else 为 and $0xdf）。
                    // = 常量位域赋值 if/else 形（探针 /tmp/p_sep.cpp f4 实证 4.4
                    // 单次基址 6 条/分支；m_field 复合赋值 f1 恒两次基址 9 条/分支，
                    // 三目位域形反产读-改-写位域全展开）。
                    if (tradeRestrict != 0)
                    {
                        inven[idx].m_upgradeSep.m_bits.m_flag = 1;
                    }
                    else
                    {
                        inven[idx].m_upgradeSep.m_bits.m_flag = 0;
                    }
            }
        }
        else
        {
            if (slot >= 10) goto L_avatarJoin;
            {
                    // ORIG 0840c956..0840ca3f：slot<10 分支基址 `add 0x14(%ebp),%eax`
                    // = 第 4 形参（调用点 sig+0xa3a1）——与镜像分支相反，勿再对调。
                    mirror[slot].m_field1 = Inven_Item::ITEM_TYPE_AVATAR;
                    mirror[slot].m_fieldb = static_cast<unsigned short>(abilityNo);
                    mirror[slot].m_addInfo = itId;
                    mirror[slot].m_addInfo2 = uiId;
                    mirror[slot].m_amp.SetLock(lockKey);
                    mirror[slot].m_fieldSplit.m_fielddLo =
                        static_cast<short>(hiddenOption);
                    mirror[slot].m_fieldSplit.m_avatarEndur =
                        static_cast<unsigned short>(emblemEndurance);
                    if (tradeRestrict != 0)
                    {
                        mirror[slot].m_upgradeSep.m_bits.m_flag = 1;
                    }
                    else
                    {
                        mirror[slot].m_upgradeSep.m_bits.m_flag = 0;
                    }
            }
        }
        L_avatarJoin:;
        ++row;
    }
    return 1;
}

// ============================================================================
// DB_LoadInventory::GetUserCreature @0x0840ca74
// creature_items 12 列；行数入 +0x1287c（>=0x8e 拒绝）。slot==0xee 映射到
// 0x8c 槽；槽位占用标记 used[0x8d]；重复槽 → Inven_Item 进 overflow vector，
// 遍历后填首个空槽并调 DB_SwapCreatureItem::makeRequest(ui_id, slot)。
// 主槽位（m_invenType3，步长 0x3d）：m_field1=5、m_fieldb=endurance、
// m_addInfo(+0x02) 单条 movl 4B ← it_id（Ghidra 的 8B ulong 表达为伪影，
// GetUserCreature.md 实证 +0x02/+0x07 两条独立 movl）、m_addInfo2=ui_id、
// m_amp lock。详情条目
// （+0x12880，步长 0x38）按 SIG_CREATURE_ITEM_DETAIL_INFO 写点填充。
// 错误行号 0x27f0..0x28e9。
// b28d-solo-DB_LoadInventory：exec/fetch/get_int(0) 共用 bool ok（ORIG -0x1d）
// + 13 处 trace 全表达式临时链（ebx 保活）已归位；残差 = 4.4 标量/临时槽分区
// （帧 0x280 vs 0x2c0、db -0x34 vs -0x1c）+ 循环体局部块（ORIG 单次 memset
// 0x74 合并清零 vs 4.4 逐条初始化）⇒ N82 封顶；p167 差集 MISSING=0。
// ============================================================================
bool DB_LoadInventory::GetUserCreature(SIG_LOAD_INVENTORY* sig)
{
    static const int handleIdx = 3;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect slot,it_id,name,stomach,exp,endurance,creature_type,"
                  "ui_id,no_charge,stat,item_lock_key,"
                  "unix_timestamp(expire_date) from creature_items"
                  " where charac_no=%u and stat = 0 and expire_date>now()",
                  sig->m_characNo);
    // ORIG：exec/fetch/get_int(0) 共用函数级 bool 槽 -0x1d（`mov %al,-0x1d;
    // movzbl -0x1d`），其余 get_* 为内联 xor 判（无槽）⇒ 源码用同一 ok 变量。
    bool ok = db->exec(true);
    if (!ok)
    {
        // P227 全表达式临时链：实参在 ctor 前求值入 %ebx 跨 ctor 保活。
        cMyTrace(__PRETTY_FUNCTION__, 0x27f0, 5)
            ("DB_LoadInventory::GetUserCreature, exec() ERROR charac_no=%u",
             sig->m_characNo);
        return 0;
    }
    sig->m_creatureCount = db->get_n_rows();
    int row = 0;
    if (sig->m_creatureCount >= 0x8e)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x27fb, 5)
            ("DB_LoadInventory::GetUserCreature, creature_item_count overflow"
             " ERROR charac_no=%u, item_count=%d", sig->m_characNo,
             sig->m_creatureCount);
        return 0;
    }

    char used[0x8d];
    memset(used, 0, sizeof(used));
    std::vector<Inven_Item> overflow;
    bool dupSlot = false;
    for (row = 0; row < sig->m_creatureCount; ++row)
    {
        ok = db->fetch();
        if (!ok)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x280e, 5)
                ("DB_LoadInventory::GetUserCreature, fetch() ERROR"
                 " charac_no=%u,%d", sig->m_characNo, row);
            break;
        }
        // ORIG 以 memset(local_220,0,0x74) 合并清零下列局部变量，语义等价。
        char name[16];
        unsigned long itId = 0;
        unsigned short endurance = 0;
        int slot = 0, uiId = 0, stomach = 0, exp = 0;
        int creatureType = 0, noCharge = 0, stat = 0, expire = 0;
        unsigned char lockKey = 0;
        memset(name, 0, sizeof(name));

        ok = db->get_int(0, slot);
        // dup 检测在 get_int 失败检查之前（ORIG 0x282c 出口顺序）。
        int checkSlot = (slot == 0xee) ? 0x8c : slot;
        if (checkSlot < 0x8d)
        {
            if (used[checkSlot] == 1)
            {
                dupSlot = true;
            }
            else
            {
                used[checkSlot] = 1;
            }
            if (!ok)
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x282c, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(0) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_ulong(1, itId))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2833, 5)
                    ("DB_LoadInventory::GetUserCreature, get_ushort(1) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_str(2, name, 0xd))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2846, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(2) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_int(3, stomach))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x284e, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(3) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_int(4, exp))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2855, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(4) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_ushort(5, endurance))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x285b, 5)
                    ("DB_LoadInventory::GetUserCreature, get_ushort(5) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_int(6, creatureType))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2862, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(6) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_int(7, uiId))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2869, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(7) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_int(8, noCharge))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2870, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(8) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_int(9, stat))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2878, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(9) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_ubyte(10, lockKey))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x2880, 5)
                    ("DB_LoadInventory::GetUserCreature, get_ubyte(10) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (!db->get_int(11, expire))
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x28a6, 5)
                    ("DB_LoadInventory::GetUserCreature, get_int(12) ERROR"
                     " charac_no=%u", sig->m_characNo);
                return 0;
            }
            if (0xf1 < slot)
            {
                cMyTrace("bool DB_LoadInventory::GetUserCreature("
                         "SIG_LOAD_INVENTORY*)", 0x28ad, 5)
                    ("DB_LoadInventory::GetUserCreature, slot_no overflow ERROR"
                     " charac_no=%u, slot_no=%d", sig->m_characNo, slot);
                return 0;
            }
            // detailWord 字节布局（SIG_CREATURE_ITEM_DETAIL_INFO +0x24）：
            // [0]=creature_type [1]=0 [2]=no_charge [3]=stat。
            int detailWord = static_cast<unsigned char>(creatureType);
            detailWord |= static_cast<unsigned char>(noCharge) << 16;
            detailWord |= static_cast<unsigned char>(stat) << 24;

            if (!dupSlot)
            {
                // 主槽位（m_invenType3，步长 0x3d）。GetUserCreature.md 实证
                // +0x02/+0x07 为两条独立 movl（4B），Ghidra 的 *(ulong*) 8B
                // 表达为伪影 —— 原 (unsigned long long*) 写为宽度错误，修正。
                Inven_Item& item = sig->m_invenType3[slot];
                item.m_field1 = Inven_Item::ITEM_TYPE_CREATURE_EQUIP;  // +0x01 movb 5
                item.m_fieldb = endurance;                              // +0x0b movw
                item.m_addInfo = static_cast<int>(itId);                // +0x02 movl
                item.m_addInfo2 = uiId;                                 // +0x07 movl
                item.m_amp.SetLock(lockKey);                            // +0x11 r/w
            }
            else
            {
                Inven_Item item;
                memset(&item, 0, sizeof(item));
                item.m_field1 = Inven_Item::ITEM_TYPE_CREATURE_EQUIP;
                item.m_fieldb = endurance;
                item.m_addInfo = static_cast<int>(itId);   // +0x02 movl 4B（ORIG 栈副本同构）
                item.m_addInfo2 = uiId;
                item.m_amp.SetLock(lockKey);
                dupSlot = false;
                overflow.push_back(item);
            }
            // 详情条目（基址 +0x12880 = m_creature，步长 0x38；ORIG 全 14 条
            // movl 升序写：+0x08..0x18 为 name 16B 四 dword，其中后 4B 落
            // m_pad14；+0x1c/+0x2c/+0x30/+0x34 为清 0 字段）。
            SIG_CREATURE_ITEM_DETAIL_INFO* d =
                reinterpret_cast<SIG_CREATURE_ITEM_DETAIL_INFO*>(sig->m_creature)
                + row;
            d->m_slotNo = slot;                              // +0x00
            d->m_uid = uiId;                                 // +0x04
            *reinterpret_cast<int*>(&d->m_name[0])  = *reinterpret_cast<int*>(&name[0]);
            *reinterpret_cast<int*>(&d->m_name[4])  = *reinterpret_cast<int*>(&name[4]);
            *reinterpret_cast<int*>(&d->m_name[8])  = *reinterpret_cast<int*>(&name[8]);
            *reinterpret_cast<int*>(&d->m_pad14[0]) = *reinterpret_cast<int*>(&name[12]);
            d->m_stomachValue = stomach;                     // +0x18
            *reinterpret_cast<int*>(&d->m_pad1c[0]) = 0;     // +0x1c
            d->m_exp = exp;                                  // +0x20
            *reinterpret_cast<int*>(&d->m_creatureItemType) =
                detailWord;                                  // +0x24..0x27 四字节
            d->m_expireTime = expire;                        // +0x28
            *reinterpret_cast<int*>(&d->m_pad2c[0]) = 0;     // +0x2c
            *reinterpret_cast<int*>(&d->m_growExpFlag) = 0;  // +0x30
            *reinterpret_cast<int*>(&d->m_pad32[2]) = 0;     // +0x34
        }
        else
        {
            cMyTrace("bool DB_LoadInventory::GetUserCreature("
                     "SIG_LOAD_INVENTORY*)", 0x281d, 5)
                ("DB_LoadInventory::GetUserCreature, charac_no=%u, slot=%d"
                 " overflow", sig->m_characNo, checkSlot);
        }
    }

    // ---- overflow vector：填首个空槽并触发 SwapCreatureItem ----
    // b28d-167/28：ORIG 0x0840ca74 的 p167 差集 = MISSING operator*(deEv)×4 +
    // operator++(int)(ppEi)×1，EXTRA operator->(ptEv)×4 + operator++(ppEv)×1
    // ⇒ 源码须写 `(*it).field` 与后置 `it++`（P167/P28）。
    int emptySlot = 0;
    for (std::vector<Inven_Item>::iterator it = overflow.begin();
         it != overflow.end(); it++)
    {
        for (; emptySlot < 0x8d; ++emptySlot)
        {
            if (used[emptySlot] != 1)
            {
                used[emptySlot] = 1;
                if (emptySlot == 0x8c)
                {
                    emptySlot = 0xee;
                }
                break;
            }
        }
        int cur = emptySlot;
        if (cur != 0xee && cur > 0x8b)
        {
            cMyTrace("bool DB_LoadInventory::GetUserCreature("
                     "SIG_LOAD_INVENTORY*)", 0x28e9, 5)
                ("DB_LoadInventory::GetUserCreature, charac_no=%u,"
                 " slot overflow", sig->m_characNo);
            break;
        }
        Inven_Item& dst = sig->m_invenType3[cur];
        dst.m_field1 = Inven_Item::ITEM_TYPE_CREATURE_EQUIP;
        dst.m_fieldb = (*it).m_fieldb;                     // +0x0b movw
        dst.m_addInfo = (*it).m_addInfo;                   // +0x02 movl（4B，ORIG）
        dst.m_addInfo2 = (*it).m_addInfo2;                 // +0x07 movl
        dst.m_amp.SetLock((*it).m_amp.GetLock());          // +0x11
        DB_SwapCreatureItem::makeRequest(dst.m_addInfo2, cur);
    }
    return 1;
}

// ============================================================================
// DB_LoadInventory::_getCharacInvenExpand @0x0840ad2a（const）
// charac_inven_expand 4 列：cargo_capacity(0)→+0xa8e0，cargo(1)→+0xa8e4，
// jewel(2)→+0x949e，expand_equipslot(3)→+0x14aa4。容量 0x2b82/0x120f/0xa5c；
// 解压后 cargo/jewel 按 0x3d 对齐校验，expand_equipslot 按 (size-4)%0x3d。
// 返回 LineFunc 风格错误行号 0x243b..0x2463，成功 0。
// b28d-solo-DB_LoadInventory：len/binLen 副本（P171）+ col 变量 + 清零三形
// （memset call / 对齐剥离慢路 / 简单 rep stos）已归位（277 vs 278，残差 9 行）
// ＝N107（`&sig->m_cargoCapacity` 4.4 恒 add $0xa8e0 两步 vs ORIG lea；字段须
// unsigned 不可改）+ P75（3 处 uncompress_zip arg0/arg4 装载序跨版本）；
// p167 差集 MISSING=0。
// ============================================================================
int DB_LoadInventory::_getCharacInvenExpand(SIG_LOAD_INVENTORY* sig) const
{
    // ORIG 本函数无 handleIdx 静态符号（其余 4 函数有 `_ZZ...E9handleIdx`）
    // ⇒ 直接字面量 3，避免仅NEW 符号。
    // ORIG 清零三形（memset call / 对齐剥离慢路 / 简单 rep stos）由数组尺寸与
    // 槽对齐自然产生；长度变量与其 binLen 副本成对（ORIG `mov -0x20,%eax;
    // mov %eax,-0x1c`），照抄 P171。
    char cargoBuf[0x2b82] = {0};
    unsigned int cargoLen = 0x2b82;     // ORIG -0x20
    int cargoBinLen = cargoLen;         // ORIG -0x1c（初始化副本，后被覆盖）
    char jewelBuf[0x120f] = {0};
    unsigned int jewelLen = 0x120f;     // ORIG -0x24
    int jewelBinLen = jewelLen;         // ORIG -0x18
    char equipBuf[0xa5c] = {0};
    unsigned int equipLen = 0xa5c;      // ORIG -0x28
    int equipBinLen = equipLen;         // ORIG -0x14

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect cargo_capacity,cargo,jewel,expand_equipslot from"
                  " charac_inven_expand where charac_no=%u", sig->m_characNo);
    if (!db->exec(true) || !db->fetch())
    {
        return 0x243b;
    }
    // 声明序=槽深序（4.4 先声明深）：db 声明在前占 -0x10，col 声明在后占
    // -0xc，与 ORIG 一致；col 的初始化位置也落在 exec/fetch 检查之后。
    int col = 0;                        // ORIG -0xc（get_* 的 col++ 实参形）
    if (!db->get_int(col++, reinterpret_cast<int&>(sig->m_cargoCapacity)))
    {
        return 0x243f;
    }
    cargoBinLen = db->get_binary_length(col);
    if (!db->get_binary(col++, cargoBuf, cargoLen))
    {
        return 0x2443;
    }
    jewelBinLen = db->get_binary_length(col);
    if (!db->get_binary(col++, jewelBuf, jewelLen))
    {
        return 0x2448;
    }
    equipBinLen = db->get_binary_length(col);
    if (!db->get_binary(col++, equipBuf, equipLen))
    {
        return 0x2453;
    }
    if (!uncompress_zip(reinterpret_cast<char*>(sig->m_cargo), cargoLen, cargoBuf,
                        static_cast<unsigned int>(cargoBinLen)))
    {
        return 0x2457;
    }
    if ((cargoLen % 0x3d) != 0 && cargoLen != 0)
    {
        return 0x2458;
    }
    // jewel 区 ORIG `add $0x949e` = 0x5949+249×0x3d（与 invenType1 重叠序列化区）。
    if (!uncompress_zip(reinterpret_cast<char*>(&sig->m_invenType1[249]),
                        jewelLen, jewelBuf,
                        static_cast<unsigned int>(jewelBinLen)))
    {
        return 0x245c;
    }
    if ((jewelLen % 0x3d) != 0 && jewelLen != 0)
    {
        return 0x245d;
    }
    if (!uncompress_zip(reinterpret_cast<char*>(&sig->m_expandEquip), equipLen,
                        equipBuf,
                        static_cast<unsigned int>(equipBinLen)))
    {
        return 0x2462;
    }
    if (((equipLen - 4) % 0x3d) != 0 && equipLen != 0)
    {
        return 0x2463;
    }
    return 0;
}

// ============================================================================
// DB_LoadInventory::_GetTagCharacInven @0x0840aba2（static bool）
// inventory 表 equipslot 列 → get_compressed_blob_data 解压进
// TAG_CHARAC_INVEN_DATA+0x262（cap 0x2dc，记录 0x3d）。错误行号 0x23e2/0x23e8/
// 0x23f1。
// ============================================================================
    // N(新候选)封顶：三个兄弟 if 块内 cMyTrace 槽深分配方向跨版本相反
    // （4.1 先声明者深：ORIG tt1 -0x44/tt2 -0x34/tt3 -0x24；4.4 反向：
    // 我方 tt1 -0x24/tt3 -0x44）。探针 /tmp/l22p/scope_probe.cpp 两形不可翻。
bool DB_LoadInventory::_GetTagCharacInven(unsigned int characNo,
                                          TAG_CHARAC_INVEN_DATA* out)
{
    // 声明序=槽深序（4.4 先声明→更深，实测）：db -0x14 / col -0x10 / ok -0xc
    // （ORIG 08409xxx 区实证 mov%eax,-0x14 / movl$0,-0x10 / mov%al,-0x9）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)3,
                                                  (ENUM_SERVER_GROUP)0);
    int col;
    bool ok;
    db->set_query("seLect equipslot from inventory where charac_no = %d",
                  characNo);
    // ORIG 08409xxx：exec/fetch 直 xor 判（无 -0x9 保存）
    if (!db->exec(true))
    {
        // P221：具名 cMyTrace 在兄弟 if 块的槽深方向跨版本相反；全表达式
        // 临时链复现 ORIG（tt1 -0x44 / tt2 -0x34 / tt3 -0x24）。
        cMyTrace(__PRETTY_FUNCTION__, 0x23e2, 5)
            ("DB_LoadInventory::_GetTagCharacInven, exec ERROR charac_no=%u",
             characNo);
        return 0;
    }
    if (!db->fetch())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x23e8, 5)
            ("DB_LoadInventory::_GetTagCharacInven, fetch ERROR charac_no=%u",
             characNo);
        return 0;
    }
    // TAG_CHARAC_INVEN_DATA equipslot 区：ORIG `add $0x262` = 第 10 槽起。
    // ORIG 8409：col 入 -0x10 槽后传值（非字面量、无自增）。
    col = 0;
    ok = get_compressed_blob_data(db, col,
                                  reinterpret_cast<char*>(&out->m_items[10]),
                                  0x2dc, 0x3d);
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x23f1, 5)
            ("DB_LoadInventory::GetUserInven, get_compressed_blob_data ERROR"
             " charac_no=%u", characNo);
        return 0;
    }
    return 1;
}

// ============================================================================
// DB_LoadInventory::_GetEquipMaxItemLevel @0x0840aac8
// charac_manage_info.max_equip_level → sig+0x15448（m_maxEquipLevel）。
// exec 失败/fetch 失败/get_int 失败 → 0；无行（getAffectedRowCount==0）→ 1。
// ============================================================================
bool DB_LoadInventory::_GetEquipMaxItemLevel(SIG_LOAD_INVENTORY* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect max_equip_level from charac_manage_info"
                  " where charac_no = %d", sig->m_characNo);
    if (!db->exec(true))
    {
        return 0;
    }
    if (db->getAffectedRowCount() == 0)
    {
        return 1;
    }
    if (!db->fetch())
    {
        return 0;
    }
    // ORIG 0840ab62..：col 命名槽（-0xc 浅，后声明→浅）+ col++ 实参形；
    // get_int 失败（==0）返回 0，否则返回 1（勿直接返回 get_int）。
    int col = 0;
    if (!db->get_int(col++, *reinterpret_cast<int*>(&sig->m_maxEquipLevel)))
    {
        return 0;
    }
    return 1;
}

// ============================================================================
// DB_LoadInventory::GetItemLock @0x0840d766（static unsigned int）
// charac_item_lock_info.item_lock_info blob → get_binary(0, 0x3cc) →
// uncompress_zip 进 out（stItemLockInfo）；解压后尺寸 mod 8 != 0 且非 0
// → 0x2931。返回 0=OK，其余为 DBThread.cpp 行号（0x291a/0x2924/0x292b/
// 0x292f/0x2931）。
// b28d-solo-DB_LoadInventory：AE（137=137 仅重定位差）。修法 = 独立
// `char buf[0x3cc]={0}`（rep stos 0xf3 精确）+ 独立 outLen（取址折叠
// lea -0x48，消旧结构体形的 N78 计数差与 add 两步）+ P221 全表达式临时链
// （3 处 trace 槽 -0x44/-0x34/-0x24）+ get_binary 判空内联（ORIG 无 ok 槽）。
// ============================================================================
unsigned int DB_LoadInventory::GetItemLock(unsigned int characNo,
                                           stItemLockInfo* out)
{
    static const int handleIdx = 3;
    // ORIG 槽距：buf@-0x414（rep stos 0xf3）、outLen@-0x48（独立标量 ⇒ 取址
    // 单条 lea）、ok=-0x11（exec/fetch 保存读回位）。
    char buf[0x3cc] = {0};
    unsigned int outLen = 0x3cc;
    bool ok;
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx,
                                                  (ENUM_SERVER_GROUP)0);
    db->set_query("seLect item_lock_info  from charac_item_lock_info"
                  " where charac_no=%u", characNo);
    ok = db->exec(true);
    if (!ok)
    {
        // P221：互斥 if 块内具名 cMyTrace 的槽深方向跨版本相反（ORIG 先出现者
        // 最深 -0x44/-0x34/-0x24；4.4 具名恒反向）⇒ 全表达式临时链复现。
        cMyTrace(__PRETTY_FUNCTION__, 0x2919, 5)
            ("DB_LoadInventory::GetItemLock, exec() ERROR charac_no=%u", characNo);
        return 0x291a;
    }
    if (db->get_n_rows() == 0)
    {
        return 0;
    }
    ok = db->fetch();
    if (!ok)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x2923, 5)
            ("DB_LoadInventory::GetItemLock, fetch() ERROR charac_no=%u", characNo);
        return 0x2924;
    }
    unsigned int binLen = db->get_binary_length(0);
    // ORIG 第三实参 `mov -0x48,%eax` = outLen 变量（非字面量）。
    // ORIG 此处直接 `xor $0x1,%eax`（无 ok 槽保存）⇒ 判空内联，勿经具名 ok。
    if (!db->get_binary(0, buf, outLen))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x292a, 5)
            ("DB_LoadInventory::GetItemLock, get_binary(2) ERROR charac_no=%u",
             characNo);
        return 0x292b;
    }
    if (!uncompress_zip(reinterpret_cast<char*>(out), outLen, buf,
                        binLen))
    {
        return 0x292f;
    }
    if ((outLen & 7) != 0 && outLen != 0)
    {
        return 0x2931;
    }
    return 0;
}