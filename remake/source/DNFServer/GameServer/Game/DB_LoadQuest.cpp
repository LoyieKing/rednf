// ============================================================================
// df_game_r 还原 —— DB_LoadQuest.cpp
// 目标：清零 /tmp/only_orig/DB_LoadQuest.txt 全部 ORIG 符号。
// 每函数按 docs/class_func_reports/DB_LoadQuest/<fn>.md（汇编为准）转写。
// SIG_LOAD_QUEST 权威布局见 SigTypes_C.h（pack(1)，size 0x7608）：
//   +0x000 m_characNo | +0x004 m_field04（错误标志）| +0x00c m_quest[20]
//   | +0x05c m_trigger[20] | +0x0ac m_urgentQuest | +0x0b0 m_cleared
//   | +0x75e0 m_authen
// ============================================================================

#include <string.h>

#include "DB_LoadQuest.h"

#include "MySQL.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "Stream.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "GameTypes.h"          // cMyTrace
#include "LogManager.h"         // logFormat
#include "AdvanceAltarZip.h"    // uncompress_zip

// ============================================================================
// DB_LoadQuest::DB_LoadQuest @0x0844d930（C1/C2 同址）
// ORIG：call DBDispatcher::DBDispatcher(this) 后设 vptr；基类 ctor/dtor 为空体，
// 对象尺寸 4。析构链（D0 0x0844d97c deleting）由类定义自然产出同形符号。
// ============================================================================
DB_LoadQuest::DB_LoadQuest()
{
}

// ============================================================================
// ~DB_LoadQuest @0x0844d97c（D1/D2/D0）
// ============================================================================
DB_LoadQuest::~DB_LoadQuest()
{
}

// ============================================================================
// DB_LoadQuest::dispatch @0x0840e236
// 基类 dispatch 通过 → BigStreamPool::Acquire("DBThread.cpp",0x29f9) →
// guard<<userIdx<<slot → GetInBuffer<SIG_LOAD_QUEST>（memset 0x7608、
// +0x4 清零）→ Stream::GetOutBuffer 首字（charac_no）拷入 →
// GetUserQuest 失败置 +0x4=1 → MsgQueueMgr::put(QUEUE_IDX 1)。返 1/0。
// ============================================================================
bool DB_LoadQuest::dispatch(int userIdx, int slot, Stream* stream)
{
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
    {
        return 0;
    }

    CStreamGuard guard(GlobalData::s_big_stream_pool->Acquire("DBThread.cpp",
                                                              0x29f9), true);
    **guard << userIdx;
    **guard << slot;
    SIG_LOAD_QUEST* sig = guard->GetInBuffer<SIG_LOAD_QUEST>();
    memset(sig, 0, sizeof(SIG_LOAD_QUEST));
    sig->m_field04 = 0;
    sig->m_characNo = stream->GetOutBuffer<SIG_LOAD_QUEST>()->m_characNo;
    if (!GetUserQuest(sig))
    {
        sig->m_field04 = 1;
    }
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
    return 1;
}

// ============================================================================
// DB_LoadQuest::GetUserQuest @0x0840e39a
// 列序：charac_no(0)/clear_quest(1)/play_1..play_20(2..39 交替 quest/trigger)/
// urgentQuestIndex(40)/quest_notify(41)。clear_quest blob 解压进 +0xb0；
// 40 个 int 依序 get_int 写 m_quest[i]/m_trigger[i]；+0xac urgent、
// +0x75e0 notify(0x28)。任一步失败 throw int（DBThread.cpp 行号），
// catch 后 logFormat 0x2a81 并返 0；成功返 1。
// ============================================================================
bool DB_LoadQuest::GetUserQuest(SIG_LOAD_QUEST* sig)
{
    char buf[0x8cac];
    unsigned int outLen;
    unsigned int col0;
    MySQL* db;
    int idx;
    unsigned int binLen;

    db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)2,
                                           (ENUM_SERVER_GROUP)0);
    db->set_query("seLect charac_no, clear_quest,play_1, play_1_trigger, play_2,"
                  " play_2_trigger, play_3, play_3_trigger, play_4,"
                  " play_4_trigger, play_5, play_5_trigger,play_6,"
                  " play_6_trigger, play_7, play_7_trigger, play_8,"
                  " play_8_trigger, play_9, play_9_trigger, play_10,"
                  " play_10_trigger,play_11, play_11_trigger, play_12,"
                  " play_12_trigger, play_13, play_13_trigger, play_14,"
                  " play_14_trigger, play_15, play_15_trigger,play_16,"
                  " play_16_trigger, play_17, play_17_trigger, play_18,"
                  " play_18_trigger, play_19, play_19_trigger, play_20,"
                  " play_20_trigger,urgentQuestIndex,quest_notify from"
                  " new_charac_quest  where charac_no=%u", sig->m_characNo);
    memset(buf, 0, 0x8cac);

    outLen = 0x8cac;
    idx = 0;

    try
    {
        if (!db->exec(true))
        {
            throw 0x2a30;
        }
        if (db->get_n_rows() == 0)
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x2a33,
                     5)("QUEST_EXPEND_LOG : NO QUEST DATA EXIST!! :"
                        " CHARAC_NO(%u)", sig->m_characNo);
            throw 0x2a34;
        }
        if (!db->fetch())
        {
            throw 0x2a36;
        }
        col0 = 0;
        if (!db->get_int(idx++, col0))
        {
            throw 0x2a39;
        }
        unsigned int binLen = db->get_binary_length(idx);
        if (!db->get_binary(idx++, buf, outLen))
        {
            throw 0x2a3c;
        }
        if (!uncompress_zip((char*)sig->m_cleared, outLen, buf, binLen))
        {
            throw 0x2a3d;
        }
        if (!db->get_int(idx++, sig->m_quest[0]))
        {
            throw 0x2a40;
        }
        if (!db->get_int(idx++, sig->m_trigger[0]))
        {
            throw 0x2a41;
        }
        if (!db->get_int(idx++, sig->m_quest[1]))
        {
            throw 0x2a42;
        }
        if (!db->get_int(idx++, sig->m_trigger[1]))
        {
            throw 0x2a43;
        }
        if (!db->get_int(idx++, sig->m_quest[2]))
        {
            throw 0x2a44;
        }
        if (!db->get_int(idx++, sig->m_trigger[2]))
        {
            throw 0x2a45;
        }
        if (!db->get_int(idx++, sig->m_quest[3]))
        {
            throw 0x2a46;
        }
        if (!db->get_int(idx++, sig->m_trigger[3]))
        {
            throw 0x2a47;
        }
        if (!db->get_int(idx++, sig->m_quest[4]))
        {
            throw 0x2a48;
        }
        if (!db->get_int(idx++, sig->m_trigger[4]))
        {
            throw 0x2a49;
        }
        if (!db->get_int(idx++, sig->m_quest[5]))
        {
            throw 0x2a4a;
        }
        if (!db->get_int(idx++, sig->m_trigger[5]))
        {
            throw 0x2a4b;
        }
        if (!db->get_int(idx++, sig->m_quest[6]))
        {
            throw 0x2a4c;
        }
        if (!db->get_int(idx++, sig->m_trigger[6]))
        {
            throw 0x2a4d;
        }
        if (!db->get_int(idx++, sig->m_quest[7]))
        {
            throw 0x2a4e;
        }
        if (!db->get_int(idx++, sig->m_trigger[7]))
        {
            throw 0x2a4f;
        }
        if (!db->get_int(idx++, sig->m_quest[8]))
        {
            throw 0x2a50;
        }
        if (!db->get_int(idx++, sig->m_trigger[8]))
        {
            throw 0x2a51;
        }
        if (!db->get_int(idx++, sig->m_quest[9]))
        {
            throw 0x2a52;
        }
        if (!db->get_int(idx++, sig->m_trigger[9]))
        {
            throw 0x2a53;
        }
        if (!db->get_int(idx++, sig->m_quest[10]))
        {
            throw 0x2a55;
        }
        if (!db->get_int(idx++, sig->m_trigger[10]))
        {
            throw 0x2a56;
        }
        if (!db->get_int(idx++, sig->m_quest[11]))
        {
            throw 0x2a57;
        }
        if (!db->get_int(idx++, sig->m_trigger[11]))
        {
            throw 0x2a58;
        }
        if (!db->get_int(idx++, sig->m_quest[12]))
        {
            throw 0x2a59;
        }
        if (!db->get_int(idx++, sig->m_trigger[12]))
        {
            throw 0x2a5a;
        }
        if (!db->get_int(idx++, sig->m_quest[13]))
        {
            throw 0x2a5b;
        }
        if (!db->get_int(idx++, sig->m_trigger[13]))
        {
            throw 0x2a5c;
        }
        if (!db->get_int(idx++, sig->m_quest[14]))
        {
            throw 0x2a5d;
        }
        if (!db->get_int(idx++, sig->m_trigger[14]))
        {
            throw 0x2a5e;
        }
        if (!db->get_int(idx++, sig->m_quest[15]))
        {
            throw 0x2a5f;
        }
        if (!db->get_int(idx++, sig->m_trigger[15]))
        {
            throw 0x2a60;
        }
        if (!db->get_int(idx++, sig->m_quest[16]))
        {
            throw 0x2a61;
        }
        if (!db->get_int(idx++, sig->m_trigger[16]))
        {
            throw 0x2a62;
        }
        if (!db->get_int(idx++, sig->m_quest[17]))
        {
            throw 0x2a63;
        }
        if (!db->get_int(idx++, sig->m_trigger[17]))
        {
            throw 0x2a64;
        }
        if (!db->get_int(idx++, sig->m_quest[18]))
        {
            throw 0x2a65;
        }
        if (!db->get_int(idx++, sig->m_trigger[18]))
        {
            throw 0x2a66;
        }
        if (!db->get_int(idx++, sig->m_quest[19]))
        {
            throw 0x2a67;
        }
        if (!db->get_int(idx++, sig->m_trigger[19]))
        {
            throw 0x2a68;
        }
        if (!db->get_int(idx++, sig->m_urgentQuest))
        {
            throw 0x2a6f;
        }
        if (!db->get_binary(idx++, sig->m_authen, 0x28))
        {
            throw 0x2a7c;
        }
        return 1;
    }
    catch (int err)
    {
        LogManager::logFormat(1, "DBThread.cpp",
                              __PRETTY_FUNCTION__,
                              0x2a81,
                              "3RD_EXPEND : DB_LoadQuest::GetUserQuest line(%d)"
                              " charac_no(%u)", err, sig->m_characNo);
        return 0;
    }
}
