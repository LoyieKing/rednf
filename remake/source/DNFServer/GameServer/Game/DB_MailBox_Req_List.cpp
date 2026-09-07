// ============================================================================
// df_game_r 还原 —— DB_MailBox_Req_List（OrigMailList19 批，独占 TU，未入 CMake，
// 待 Main 登记）
// 覆盖 3 个 ORIG T 符号（地址见 DB_MailBox_Req_List.h 头注释）：
//   1. DB_MailBox_Req_List::SelectLetter    0x0841d52c
//   2. DB_MailBox_Req_List::setCreatureData 0x0841dc84
//   3. DB_MailBox_Req_List::dispatch        0x0841de4a
// 报告均已全文阅读后转写（docs/class_func_reports/DB_MailBox_Req_List/
//   {SelectLetter,setCreatureData,dispatch}.md）。
// SIG 布局：SIG_MAILBOX_LIST（SigTypes.h m_data[0x338a]）跨字段区按 ORIG 偏移
//   访问（pack(1) 视图 + 偏移宏，与 DB_MailBox_Dispatch.cpp 同款先例）；
//   SIG_LETTER_INFO（CMailBoxHelper.h，已按 ORIG 实证修正为 0x12b）。
// 依赖：DBDispatcher::dispatch 定义归 DB_Login.cpp；ArrangeMailAndLetterIndex /
//   DoChangeMailList / IsContainedLetter 定义归 W8LT_DBTask 域其它批次（仅声明）；
//   stAvatarEmblemInfo_t::init 归 GameBasicsScripts.cpp；RandomOption::reset 语义
//   为清零 0xe 字节（ORIG 0x080cb2d8 逐子对象清零实证），本 TU 以 memset 等价实现。
// ============================================================================

#include "DB_MailBox_Req_List.h"

#include "SigTypes.h"        // SIG_MAILBOX_LIST
#include "GlobalData.h"      // s_db_mgr / s_stream_pool / s_msgq_mgr
#include "CStreamGuard.h"
#include "MySQL.h"
#include "DBMgr.h"
#include "MsgQueueMgr.h"
#include "LogManager.h"
#include "GamePODTypes.h"    // stAvatarEmblemInfo_t
#include "CMailBoxHelper.h"  // SIG_LETTER_INFO（0x12b）
#include "DB_MailBox_Dispatch.h"  // getList2inQuery（ORIG 0x083ff968，已实现）

#include <map>
#include <vector>
#include <cstring>
#include <cstdio>
#include <algorithm>

// SIG_MAILBOX_LIST 响应区字段偏移（dispatch/SelectLetter/setCreatureData 报告）
#define SIG_I32(p, off)  (*(int*)((char*)(p) + (off)))
#define SIG_U32(p, off)  (*(unsigned int*)((char*)(p) + (off)))
#define SIG_PTR(p, off)  ((char*)(p) + (off))

enum
{
    SIG_OFF_POSTAL_COUNT   = 0x794,   // int 已填 postal 条数
    SIG_OFF_REMAIN_COUNT   = 0x798,   // int 剩余可拉条数
    SIG_OFF_MAX_POSTAL_ID  = 0x79c,   // uint 最大 postal_id / 增量游标
    SIG_OFF_USER_ITEM_CNT  = 0x7a0,   // int user_items 条数
    SIG_OFF_CREATURE_CNT   = 0xd30,   // int creature_items 条数
    SIG_OFF_LETTER_COUNT   = 0x1018,  // int 信件总条数
    SIG_OFF_LETTER_USED    = 0x3326,  // int 已用信件上限（= 请求 +0x08）
    SIG_OFF_LETTER_NEW     = 0x332a,  // int 本次新增信件条数（SelectLetter 写）
    SIG_OFF_LETTER_ADD     = 0x332e,  // int 增量计数（dispatch 第二循环）
    SIG_OFF_CHARAC_NO      = 0x3332,  // int 角色号（回包）
    SIG_OFF_FIELD3336      = 0x3336,  // int 请求 +0x10 透传
    SIG_OFF_EXTRA_IDS      = 0x333a,  // uint[20] map 键回写区
};

// ---- Mail helper 域 ORIG 自由函数（本 TU 独占实现；报告
//      docs/class_func_reports/_lt_global_gt_/{IsContainedLetter,DoChangeMailList,
//      ArrangeMailAndLetterIndex,FindLastLetterIndexMail,RemoveMailByLatestLetter,
//      CompareLetterIndex}.md 逐指令转写） ----
// _Z17IsContainedLetterP16SIG_MAILBOX_LISTi          0x0841ccb6
// _Z16DoChangeMailListP16SIG_MAILBOX_LISTPji         0x0841d470
// _Z25ArrangeMailAndLetterIndexP16SIG_MAILBOX_LISTRSt6vectorI15SIG_LETTER_INFOSaIS2_EEiS5_ 0x0841d16d
// _Z23FindLastLetterIndexMailP16SIG_MAILBOX_LISTi    0x0841d080
// _Z24RemoveMailByLatestLetterP16SIG_MAILBOX_LISTi   0x0841cd03
// _Z18CompareLetterIndexRK15SIG_LETTER_INFOS1_       0x0841d12f
bool CompareLetterIndex(const SIG_LETTER_INFO& a, const SIG_LETTER_INFO& b);
int  FindLastLetterIndexMail(SIG_MAILBOX_LIST* list, int letterId);
bool RemoveMailByLatestLetter(SIG_MAILBOX_LIST* list, int mailIdx);

// ============================================================================
// IsContainedLetter —— ORIG 0x0841ccb6：postal 区（stride 0x61）扫 +0x5c
//   letter_id 是否含目标。
// ============================================================================
bool IsContainedLetter(SIG_MAILBOX_LIST* list, int letterId)
{
    // 视图（P82/P95）：ORIG count 读为单 disp（mov 0x794(%eax)）、条目为
    // m_postals[i].m_letterId（+0x50 基址 +0xc disp 拆段）。
    #pragma pack(push, 1)
    struct PostalEntry
    {
        unsigned char m_pad[0xc];
        int m_letterId;
        unsigned char m_tail[0x61 - 0x10];
    };
    struct ListView
    {
        unsigned char m_pad[0x50];
        PostalEntry m_postals[0x12];
    };
    struct CountView
    {
        unsigned char m_pad[0x794];
        int m_count;
    };
    #pragma pack(pop)
    int i;
    for (i = 0; i < ((CountView*)list)->m_count; ++i)
    {
        if (((ListView*)list)->m_postals[i].m_letterId == letterId)
            return 1;
    }
    return 0;
}

// ============================================================================
// DoChangeMailList —— ORIG 0x0841d470（void）：清 mailIdx 表头；postal 数为 0 时
//   +0x79c 直接置 param_3；否则 +0x79c=0 并逐项取 max(postal_id) 回写 +0x79c，
//   mailIdx 表收 postal 的 +0x2e add_info。
// ============================================================================
void DoChangeMailList(SIG_MAILBOX_LIST* list, unsigned int* mailIdx, int count)
{
    mailIdx[0] = 0;
    if (SIG_I32(list, SIG_OFF_POSTAL_COUNT) == 0)
    {
        SIG_U32(list, SIG_OFF_MAX_POSTAL_ID) = (unsigned int)count;
    }
    else
    {
        SIG_U32(list, SIG_OFF_MAX_POSTAL_ID) = 0;
        int i;
        for (i = 0; i < SIG_I32(list, SIG_OFF_POSTAL_COUNT); ++i)
        {
            unsigned int id = (unsigned int)SIG_I32(list, i * 0x61);
            SIG_U32(list, SIG_OFF_MAX_POSTAL_ID) =
                std::max(SIG_U32(list, SIG_OFF_MAX_POSTAL_ID), id);
            mailIdx[0] = mailIdx[0] + 1;
            mailIdx[mailIdx[0]] = SIG_U32(list, i * 0x61 + 0x2e);
        }
    }
}

// ============================================================================
// CompareLetterIndex —— ORIG 0x0841d12f：比较首字段（letter_id）严格小于。
// ============================================================================
bool CompareLetterIndex(const SIG_LETTER_INFO& a, const SIG_LETTER_INFO& b)
{
    // ORIG 0x0841d12f：`return uint_a < uint_b;`（GCC4.1 生成 a<b→1; a==b→setb; a>b→0
    // 三分支）。`if(x)return 1; return 0` 隔成两分支（缺失 a==b 的 setb 路径）。
    if (*(const unsigned int*)&a < *(const unsigned int*)&b)
        return 1;
    if (*(const unsigned int*)&a == *(const unsigned int*)&b)
        return *(const unsigned int*)&a < *(const unsigned int*)&b;
    return 0;
}

// ============================================================================
// FindLastLetterIndexMail —— ORIG 0x0841d080：postal 区扫 +0x5c，找大于
//   letterId 的最大 letter_id 所在 postal 下标；无则 -1。
// ============================================================================
int FindLastLetterIndexMail(SIG_MAILBOX_LIST* list, int letterId)
{
    int maxIdx = 0;
    int i;
    for (i = 0; i < SIG_I32(list, SIG_OFF_POSTAL_COUNT); ++i)
    {
        int v = SIG_I32(list, i * 0x61 + 0x5c);
        if (letterId < v && maxIdx < v)
            maxIdx = v;
    }
    if (maxIdx != 0)
    {
        for (i = 0; i < SIG_I32(list, SIG_OFF_POSTAL_COUNT); ++i)
        {
            if (SIG_I32(list, i * 0x61 + 0x5c) == maxIdx)
                return i;
        }
    }
    return -1;
}

// ============================================================================
// RemoveMailByLatestLetter —— ORIG 0x0841cd03：postal 区自 idx 起左移一项
//   （0x61 字节），count-1、remain+1，恒返 1。
// ============================================================================
bool RemoveMailByLatestLetter(SIG_MAILBOX_LIST* list, int mailIdx)
{
    if (SIG_I32(list, SIG_OFF_POSTAL_COUNT) - 1 != mailIdx)
    {
        int i;
        for (i = mailIdx; i < SIG_I32(list, SIG_OFF_POSTAL_COUNT) - 1; ++i)
        {
            memmove(SIG_PTR(list, i * 0x61), SIG_PTR(list, (i + 1) * 0x61), 0x61);
        }
    }
    SIG_I32(list, SIG_OFF_POSTAL_COUNT) -= 1;
    SIG_I32(list, SIG_OFF_REMAIN_COUNT) += 1;
    return 1;
}

// ============================================================================
// ArrangeMailAndLetterIndex —— ORIG 0x0841d16d：vecNew 按 letter_id 排序后，从
//   size-count 处起逐项：找 postal 区中 letter_id 更大的最新邮件（FindLast
//   LetterIndexMail）并删除（RemoveMailByLatestLetter），同时从 vecOld 删同
//   letter_id 项。恒返 1。
// ============================================================================
bool ArrangeMailAndLetterIndex(SIG_MAILBOX_LIST* list,
                               std::vector<SIG_LETTER_INFO>& vecNew,
                               int count,
                               std::vector<SIG_LETTER_INFO>& vecOld)
{
    std::sort(vecNew.begin(), vecNew.end(), CompareLetterIndex);
    int start = (int)vecNew.size() - count;
    while ((int)vecNew.size() > start)
    {
        int mailIdx = -1;
        int letterId = *(int*)&vecNew[(size_t)start];
        mailIdx = FindLastLetterIndexMail(list, letterId);
        if (mailIdx != -1)
        {
            int letterIdx = SIG_I32(list, mailIdx * 0x61 + 0x5c);
            if (RemoveMailByLatestLetter(list, mailIdx) && letterIdx != 0)
            {
                std::vector<SIG_LETTER_INFO>::iterator it;
                for (it = vecOld.begin(); it != vecOld.end(); ++it)
                {
                    if (*(int*)&*it == letterIdx)
                    {
                        vecOld.erase(it);
                        break;
                    }
                }
            }
        }
        ++start;
    }
    return 1;
}


// ---- SIG_MAILBOX_LIST 请求头 / 明细区视图（pack(1)，偏移对照报告逐指令） ----
#pragma pack(push, 1)
struct MailBoxListReqView
{
    unsigned int m_characNo;          // +0x00
    unsigned int m_maxPostalId;       // +0x04（上次最大 postal_id，增量拉取）
    unsigned int m_maxLetter;         // +0x08（本次最大信件条数）
    unsigned int m_letterIdx;         // +0x0c（增量 letter_id）
    unsigned int m_field10;           // +0x10
    unsigned int m_postalExtra[20];   // +0x14（非 0 项 → extraMap / DoChangeMailList）
};
#pragma pack(pop)

// postal 明细项（stride 0x61=97）
#pragma pack(push, 1)
struct MailPostalItem
{
    int m_postalId;                  // +0x00
    int m_sendCharacNo;              // +0x04
    char m_sendName[0x1d];           // +0x08
    int m_itemId;                    // +0x25
    char m_sealFlag;                 // +0x29（unlimit_flag==1）
    unsigned int m_occTime;          // +0x2a（unix_timestamp(occ_time)，movl 4 字节）
    unsigned int m_addInfo;          // +0x2e
    char m_endurance;                // +0x32
    short m_upgrade;                 // +0x33
    unsigned int m_itemGuid;         // +0x35（movl 4 字节）
    unsigned int m_ampOpt;           // +0x39（amplify_option）
    unsigned int m_ampVal;           // +0x3d（amplify_value）
    unsigned short m_rndOpt;         // +0x41（random_option）
    char m_field43;                  // +0x43（get_byte 16）
    short m_field44;                 // +0x44（get_short 17）
    unsigned int m_ext0;             // +0x46（extend_info 14 字节）
    unsigned int m_ext1;             // +0x4a
    unsigned int m_ext2;             // +0x4e
    unsigned short m_ext3;           // +0x52
    char m_field54;                  // +0x54（get_byte 19）
    int m_field55;                   // +0x55（send_charac_no + 0x13c680）
    char m_avataFlag;                // +0x59（avata_flag==1）
    char m_creatureFlag;             // +0x5a（creature_flag==1）
    char m_unlimitFlag;              // +0x5b（unlimit_flag==1）
    unsigned int m_letterId;         // +0x5c
    char m_field60;                  // +0x60（get_byte 20）
};
#pragma pack(pop)

// user_items 明细项（stride 0x47=71，基址 +0x7a0）
#pragma pack(push, 1)
struct MailUserItemView
{
    int m_uiId;                      // +0x04
    unsigned int m_regDate;          // +0x08（unix_timestamp(reg_date)）
    unsigned int m_expireDate;       // +0x0c（unix_timestamp(expire_date)）
    char m_ipgAgency[0x18];          // +0x10
    unsigned char m_jewel[0x1e];     // +0x29（jewel_socket，stAvatarEmblemInfo_t 30 字节）
};
#pragma pack(pop)

// creature_items 明细项（stride 0x25=37，基址 +0xd30）
#pragma pack(push, 1)
struct MailCreatureItemView
{
    int m_uiId;                      // +0x04
    int m_expireDate;                // +0x08（unix_timestamp(expire_date)）
    int m_regDate;                   // +0x0c（unix_timestamp(reg_date)）
};
#pragma pack(pop)

// ============================================================================
// SelectLetter —— ORIG 0x0841d52c（普通成员，this 调用）：

//   清 SIG_MAILBOX_LIST 信件区（+0x101c 起 0x230a）→ letterIdx==0 时先删过期
//   stat=2/1 邮件 → 按 (charac_no, stat=3) or (charac_no, reg_date 15 天内)
//   [letterIdx>0 时加 letter_id>idx] 查 letter 表 → 逐行按 stat 分流到三个 vector
//   （stat==3 单独；stat 1/2 按 IsContainedLetter 分流）→ ArrangeMailAndLetterIndex
//   整理 → 限额拷回 +0x101c 区（stride 0x12b）→ 写 +0x332a/+0x798/+0x1018。
// ============================================================================
bool DB_MailBox_Req_List::SelectLetter(MySQL* db, int slot, int characNo,
                                       unsigned int letterIdx,
                                       SIG_MAILBOX_LIST* out)
{
    (void)slot;
    memset(SIG_PTR(out, 0x101c), 0, 0x230a);

    if (letterIdx == 0)
    {
        db->set_query(
            "deLete from letter where charac_no=%d and reg_date<subdate(now(),interval %d day) and stat=%d",
            characNo, 0xf, 2);
        if (!(unsigned char)db->exec(true))
            return 0;

        db->set_query(
            "deLete from letter where charac_no=%d and reg_date<subdate(now(),interval %d day) and stat=%d",
            characNo, 0xf, 1);
        if (!(unsigned char)db->exec(true))
            return 0;

        db->set_query(
            "seLect letter_id,send_charac_no,send_charac_name,letter_text,unix_timestamp(reg_date),stat from letter where (charac_no=%d and stat = 3) or (charac_no=%d and reg_date>subdate(now(),interval %d day)) order by letter_id",
            characNo, characNo, 0xf);
    }
    else
    {
        db->set_query(
            "seLect letter_id,send_charac_no,send_charac_name,letter_text,unix_timestamp(reg_date),stat from letter where (charac_no=%d and stat = 3) or (charac_no=%d and letter_id > %d and reg_date>subdate(now(),interval %d day)) order by letter_id",
            characNo, characNo, letterIdx, 0xf);
    }

    if (!(unsigned char)db->exec(true))
        return 0;

    int oldCount = SIG_I32(out, SIG_OFF_POSTAL_COUNT);   // +0x794
    int oldLetter = SIG_I32(out, SIG_OFF_LETTER_COUNT);  // +0x1018
    SIG_I32(out, SIG_OFF_LETTER_COUNT) = 0;

    int letterCount = db->get_n_rows();
    SIG_I32(out, SIG_OFF_LETTER_COUNT) = letterCount;

    // 三个分流 vector：vec[0] 已含（IsContainedLetter）、vec[1] 新（stat 1/2）、
    // vec[2] stat==3；局部 SIG_LETTER_INFO 缓冲按 ORIG 0x12b。
    std::vector<SIG_LETTER_INFO> vec[3];
    int groupSize[3] = { 0, 0, 0 };

    int i;
    for (i = 0; i < letterCount; ++i)
    {
        if (letterIdx != 0 && oldCount >= oldLetter &&
            SIG_I32(out, SIG_OFF_REMAIN_COUNT) != 0 &&
            (int)vec[0].size() >= oldLetter)
            break;

        if (!db->fetch())
            return 0;


        SIG_LETTER_INFO infoRaw;
        unsigned int letterId = 0;
        int sendCharacNo = 0;
        int regDate = 0;
        short stat = 0;
        memset(&infoRaw, 0, 0x12b);

        if (!db->get_uint(0, letterId)) return 0;
        if (!db->get_int(1, sendCharacNo)) return 0;
        if (!db->get_str(2, SIG_PTR(&infoRaw, 0x08), 0x1d)) return 0;
        if (!db->get_str(3, SIG_PTR(&infoRaw, 0x25), 0x100)) return 0;
        if (!db->get_int(4, regDate)) return 0;
        if (!db->get_short(5, stat)) return 0;


        SIG_U32(&infoRaw, 0x00) = letterId;
        SIG_I32(&infoRaw, 0x04) = sendCharacNo;
        SIG_I32(&infoRaw, 0x125) = regDate;
        *(short*)SIG_PTR(&infoRaw, 0x129) = stat;

        if (stat > 0)
        {
            if (stat < 3)
            {
                if (!IsContainedLetter(out, (int)letterId))
                    vec[1].push_back(infoRaw);
                else
                    vec[0].push_back(infoRaw);
            }
            else if (stat == 3)
            {
                vec[2].push_back(infoRaw);
            }
        }
    }

    int n = (int)vec[1].size() + (int)vec[0].size() - SIG_I32(out, SIG_OFF_LETTER_USED);

    if (n > 0)
    {
        ArrangeMailAndLetterIndex(out, vec[1], n, vec[0]);
        if (SIG_I32(out, SIG_OFF_POSTAL_COUNT) != 0)
        {
            SIG_U32(out, SIG_OFF_MAX_POSTAL_ID) =
                SIG_U32(out, (SIG_I32(out, SIG_OFF_POSTAL_COUNT) - 1) * 0x61);
        }
    }

    int adjust = 0;
    if (SIG_I32(out, SIG_OFF_LETTER_COUNT) != 0)
    {
        int k;
        for (k = 0; k < 3; ++k)
            groupSize[k] = (int)vec[k].size();

        if (SIG_I32(out, SIG_OFF_LETTER_USED) < groupSize[0])
            groupSize[0] = SIG_I32(out, SIG_OFF_LETTER_USED);
        if (SIG_I32(out, SIG_OFF_LETTER_USED) <
            SIG_I32(out, SIG_OFF_POSTAL_COUNT) + groupSize[1])
        {
            adjust = (SIG_I32(out, SIG_OFF_POSTAL_COUNT) + groupSize[1]) -
                     SIG_I32(out, SIG_OFF_LETTER_USED);
            groupSize[1] -= adjust;
        }
        if (groupSize[2] > 10)
            groupSize[2] = 10;
    }

    int outIdx = 0;
    int k;
    for (k = 0; k < 3; ++k)
    {
        unsigned int j;
        for (j = 0; (int)j < groupSize[k]; ++j)
        {
            *(SIG_LETTER_INFO*)SIG_PTR(out, 0x101c + outIdx * 0x12b) = vec[k].at(j);
            ++outIdx;
        }
    }

    SIG_I32(out, SIG_OFF_LETTER_NEW) = groupSize[1];
    int remain = SIG_I32(out, SIG_OFF_REMAIN_COUNT) +
                 std::max(adjust - SIG_I32(out, SIG_OFF_LETTER_ADD), 0);
    SIG_I32(out, SIG_OFF_REMAIN_COUNT) = remain;
    SIG_I32(out, SIG_OFF_LETTER_COUNT) = outIdx;
    return 1;
}

// ============================================================================
// setCreatureData —— ORIG 0x0841dc84（普通成员）：getList2inQuery(list) 拼串 →
//   查 creature_items（stat=1）→ +0xd30 条数 → 逐行回填 +0xd30+i*0x25 区。
// ============================================================================
bool DB_MailBox_Req_List::setCreatureData(MySQL* db, unsigned int characNo,
                                          unsigned int* list,
                                          SIG_MAILBOX_LIST* out)
{
    (void)characNo;
    char buf[0x1000] = { 0 };

    getList2inQuery(list, buf);
    db->set_query(
        "seLect ui_id,unix_timestamp(expire_date),unix_timestamp(reg_date) from creature_items where ui_id %s and stat = 1",
        buf);
    db->exec(true);

    SIG_I32(out, SIG_OFF_CREATURE_CNT) = db->get_n_rows();

    unsigned int i;
    for (i = 0; i < (unsigned int)db->get_n_rows(); ++i)
    {
        if (!db->fetch())
            return 0;

        MailCreatureItemView* item =
            (MailCreatureItemView*)SIG_PTR(out, SIG_OFF_CREATURE_CNT + i * 0x25);
        if (!db->get_int(0, item->m_uiId))
            return 0;
        if (!db->get_int(1, item->m_expireDate))
            return 0;
        if (!db->get_int(2, item->m_regDate))
            return 0;
    }
    return 1;
}

// ============================================================================
// dispatch —— ORIG 0x0841de4a（虚覆盖）：基类校验 → 读请求 → 清理过期 postal →
//   查 postal 主表（20 条上限，增量游标）→ 逐行解析回填 postal 区（stride 0x61）
//   并收集 avata/creature ui_id 列表 → 剩余行计 map 频次 → map 键回写 +0x333a
//   → SelectLetter 信件 → DoChangeMailList 整理 → user_items 回填（stride 0x47）
//   → setCreatureData 回填（stride 0x25）→ 0x7f 包投递 DISPATCH_Q。
// ============================================================================
bool DB_MailBox_Req_List::dispatch(int userIdx, int slot, Stream* stream)
{
    static const int handleIdx = 3;
    if (!DBDispatcher::dispatch(userIdx, slot, stream))
        return 0;

    MailBoxListReqView* req = (MailBoxListReqView*)stream->GetOutBuffer<SIG_MAILBOX_LIST>();
    SIG_MAILBOX_LIST localSig;
    memset(&localSig, 0, sizeof(localSig));
    SIG_MAILBOX_LIST* sig = &localSig;

    MySQL* db = GlobalData::s_db_mgr->GetDBHandle((ENUM_DB_HANDLE_IDX)handleIdx, (ENUM_SERVER_GROUP)0);

    // 首段：无增量游标时清理过期未限时 postal
    if (req->m_maxPostalId == 0)
    {
        db->set_query(
            "upDate postal set delete_flag=%d where receive_charac_no=%d and unlimit_flag=0 and occ_time<subdate(now(),interval %d day) and delete_flag=0",
            2, req->m_characNo, 0xf);
        if (!db->exec(true))
            return 0;
    }

    // 增量游标后缀（char sql[0x80] rep stos 0x20 dword 实证）
    char sql[0x80] = { 0 };
    if (req->m_maxPostalId != 0)
        sprintf(sql, " and postal_id > %d", req->m_maxPostalId);

    db->set_query(
        "seLect postal_id,unix_timestamp(occ_time),send_charac_no,seal_flag,item_id,add_info,endurance,upgrade,gold,send_charac_name,unlimit_flag,avata_flag,creature_flag,letter_id,extend_info,item_guid,amplify_option,amplify_value,random_option,seperate_upgrade,type from postal where receive_charac_no=%d and delete_flag=0 %s and (unlimit_flag=1 or occ_time>subdate(now(),interval 15 day)) order by postal_id limit %d",
        req->m_characNo, sql, 0x14);
    if (!db->exec(true))
        return 0;

    // 请求附加 postal_id 列表 → map（make_pair<unsigned int&, int> 实证）
    std::map<unsigned int, int> extraMap;
    int mi;
    for (mi = 0; mi < 0x14; ++mi)
    {
        if (req->m_postalExtra[mi] != 0)
        {
            extraMap.insert(std::make_pair((unsigned int&)req->m_postalExtra[mi],
                                           (int)req->m_postalExtra[mi]));
        }
    }

    SIG_I32(sig, SIG_OFF_LETTER_USED) = (int)req->m_maxLetter;   // +0x3326
    SIG_U32(sig, SIG_OFF_MAX_POSTAL_ID) = 0;                     // +0x79c
    int nRows = db->get_n_rows();
    SIG_I32(sig, SIG_OFF_POSTAL_COUNT) = std::min((int)req->m_maxLetter, nRows);
    SIG_I32(sig, SIG_OFF_REMAIN_COUNT) = 0;
    if ((int)req->m_maxLetter <= SIG_I32(sig, SIG_OFF_POSTAL_COUNT))
    {
        SIG_I32(sig, SIG_OFF_REMAIN_COUNT) =
            db->get_n_rows() - SIG_I32(sig, SIG_OFF_POSTAL_COUNT);
    }

    // avata / creature ui_id 收集表（首元素为条数；rep stos 0x15 dword 实证）
    unsigned int avataList[0x15] = { 0 };
    unsigned int creatureList[0x15] = { 0 };

    int rowIdx = 0;    // +0x794 最终值
    int loopIdx = 0;
    while (loopIdx < SIG_I32(sig, SIG_OFF_POSTAL_COUNT) && loopIdx < 0x14)
    {
        if (!db->fetch())
            return 0;

        int p0 = 0, p1 = 0, p2 = 0;
        short s3 = 0;
        unsigned long u4 = 0;
        int i5 = 0;
        short s6 = 0;
        char b7 = 0;
                int i8 = 0;
        char name9[0x1d] = { 0 };
        short s10 = 0, s11 = 0, s12 = 0;
        unsigned char extBuf[0xe];
        memset(extBuf, 0, sizeof(extBuf));   // RandomOption::reset 等价（清零 0xe）

        if (!db->get_int(0, p0)) return 0;

        if (!db->get_int(1, p1)) return 0;
        if (!db->get_int(2, p2)) return 0;
        if (!db->get_short(3, s3)) return 0;
        if (!db->get_ulong(4, u4)) return 0;
        if (!db->get_int(5, i5)) return 0;
        if (!db->get_short(6, s6)) return 0;
        if (!db->get_byte(7, b7)) return 0;
        if (!db->get_int(8, i8)) return 0;
        if (!db->get_str(9, name9, 0x1d)) return 0;
        if (!db->get_short(10, s10)) return 0;
        if (!db->get_short(11, s11)) return 0;
        if (!db->get_short(12, s12)) return 0;

        // avata==1 && creature==1 的行跳过（ORIG 跳过条件）
        if (s11 != 1 || s12 != 1)
        {
                        int idx = 0xe;
            unsigned int u13 = 0;
            unsigned long u14 = 0;
            int b16 = 0;
            int s17 = 0;
            unsigned char ampBuf[10];
            char b19 = 0, b20 = 0;

            if (db->get_uint(13, u13) && (u4 != 0 || i8 != 0))
            {
                if (db->get_ulong(idx, u14))
                {
                    ++idx;
                    if (db->get_binary(idx, ampBuf, 10))              // amplify 区（-0x116）
                    {
                        ++idx;
                        if (db->get_byte(idx, b16))
                        {
                            ++idx;
                            if (db->get_short(idx, s17))
                            {
                                ++idx;
                                if (db->get_binary(idx, extBuf, 0xe)) // extend_info 区（-0x102）
                                {
                                    ++idx;
                                    if (db->get_byte(idx, b19))
                                    {
                                        ++idx;
                                        if (db->get_byte(idx, b20))
                                        {
                                            if (u13 != 0)
                                            {
                                                extraMap.insert(
                                                    std::make_pair((unsigned int&)u13,
                                                                   (int)u13));
                                            }

                                            MailPostalItem* item =
                                                (MailPostalItem*)SIG_PTR(sig, rowIdx * 0x61);
                                            item->m_postalId = p0;
                                            item->m_field55 = p1 + 0x13c680;
                                            item->m_sendCharacNo = p2;
                                            item->m_sealFlag = (char)(s3 == 1);
                                            item->m_occTime = (unsigned int)u4;
                                            item->m_addInfo = (unsigned int)i5;
                                            item->m_upgrade = s6;
                                            item->m_endurance = b7;
                                            item->m_itemGuid = (unsigned int)u14;
                                            item->m_ampOpt = *(unsigned int*)&ampBuf[0];
                                            item->m_ampVal = *(unsigned int*)&ampBuf[4];
                                            item->m_rndOpt = *(unsigned short*)&ampBuf[8];
                                            item->m_itemId = i8;
                                            item->m_unlimitFlag = (char)(s10 == 1);
                                            if (s11 == 1)
                                            {
                                                item->m_avataFlag = 1;
                                                avataList[0] = avataList[0] + 1;
                                                avataList[avataList[0]] = (unsigned int)i5;
                                            }
                                            else
                                            {
                                                item->m_avataFlag = 0;
                                            }
                                            if (s12 == 1)
                                            {
                                                item->m_creatureFlag = 1;
                                                creatureList[0] = creatureList[0] + 1;
                                                creatureList[creatureList[0]] = (unsigned int)i5;
                                            }
                                            else
                                            {
                                                item->m_creatureFlag = 0;
                                            }
                                            item->m_letterId = u13;
                                            item->m_field43 = (char)b16;
                                            item->m_field44 = (short)s17;
                                            item->m_field60 = b20;
                                            item->m_ext0 = *(unsigned int*)&extBuf[0];
                                            item->m_ext1 = *(unsigned int*)&extBuf[4];
                                            item->m_ext2 = *(unsigned int*)&extBuf[8];
                                            item->m_ext3 = *(unsigned short*)&extBuf[12];
                                            item->m_field54 = b19;
                                            strncpy(item->m_sendName, name9, 0x1d);


                                            SIG_U32(sig, SIG_OFF_MAX_POSTAL_ID) =
                                                std::max(SIG_U32(sig, SIG_OFF_MAX_POSTAL_ID),
                                                         (unsigned int)p0);
                                            ++rowIdx;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        ++loopIdx;
    }
    SIG_I32(sig, SIG_OFF_POSTAL_COUNT) = rowIdx;
    SIG_I32(sig, SIG_OFF_LETTER_ADD) = 0;

    // 第二循环：剩余行只读 letter_id，统计 map 频次与 +0x332e 增量
    int k;
    for (k = 0; k < SIG_I32(sig, SIG_OFF_REMAIN_COUNT); ++k)
    {
        if (!db->fetch())
            return 0;
        unsigned int u13b = 0;
        if (db->get_uint(13, u13b) && u13b != 0)
        {
            std::map<unsigned int, int>::iterator it = extraMap.find(u13b);
            if (it != extraMap.end())
                it->second = it->second + 1;
            SIG_I32(sig, SIG_OFF_LETTER_ADD) += 1;
        }
    }

    // map 键回写 +0x333a（stride 4，最多 0x14 项）
    if (!extraMap.empty())
    {
        int idx = 0;
        std::map<unsigned int, int>::iterator it;
        for (it = extraMap.begin(); it != extraMap.end(); ++it)
        {
            if (it->second >= 1 && idx <= 0x13)
            {
                SIG_U32(sig, SIG_OFF_EXTRA_IDS + idx * 4) = it->first;
                ++idx;
            }
        }
    }

    int oldCount = SIG_I32(sig, SIG_OFF_POSTAL_COUNT);
    SIG_I32(sig, SIG_OFF_LETTER_COUNT) = (int)req->m_maxLetter;   // +0x1018
    if (!SelectLetter(db, slot, (int)req->m_characNo, req->m_letterIdx, sig))
    {
        LogManager::logFormat(1, "DBThread.cpp", __PRETTY_FUNCTION__, 0x5438,
                              "SelectLetter() error");
        SIG_I32(sig, SIG_OFF_LETTER_COUNT) = 0;
    }
    if (SIG_I32(sig, SIG_OFF_POSTAL_COUNT) == oldCount)
    {
        if (SIG_I32(sig, SIG_OFF_POSTAL_COUNT) == 0)
            SIG_U32(sig, SIG_OFF_MAX_POSTAL_ID) = req->m_maxPostalId;
    }
    else
    {
        DoChangeMailList(sig, avataList, (int)req->m_maxPostalId);
    }

    SIG_I32(sig, SIG_OFF_USER_ITEM_CNT) = 0;   // +0x7a0
    if (avataList[0] != 0)
    {
        char sql2[0x1000] = { 0 };
        getList2inQuery(avataList, sql2);
        db->set_query(
            "seLect ui_id,unix_timestamp(reg_date),unix_timestamp(expire_date),ipg_agency_no,jewel_socket from user_items where charac_no=%d and ui_id %s",
            req->m_characNo, sql2);
        db->exec(true);
        SIG_I32(sig, SIG_OFF_USER_ITEM_CNT) = db->get_n_rows();

        int ui;
        for (ui = 0; ui < db->get_n_rows(); ++ui)
        {
            if (!db->fetch())
                return 0;
            MailUserItemView* item =
                (MailUserItemView*)SIG_PTR(sig, SIG_OFF_USER_ITEM_CNT + ui * 0x47);
            int uiId = 0;
            if (!db->get_int(0, uiId))
                return 0;
            unsigned int regDate = 0, expireDate = 0;
            if (!db->get_uint(1, regDate))
                return 0;
            if (!db->get_uint(2, expireDate))
                return 0;
            char agency[0x18] = { 0 };
            if (!db->get_str(3, agency, 0x18))
                return 0;
            stAvatarEmblemInfo_t emblem;
            emblem.init();
            if (!db->get_binary(4, &emblem, 0x1e))
                return 0;

            item->m_expireDate = expireDate;
            item->m_uiId = uiId;
            item->m_regDate = regDate;
            strncpy(item->m_ipgAgency, agency, 0x18);
            memcpy(item->m_jewel, &emblem, 0x1e);
        }
    }

    SIG_I32(sig, SIG_OFF_CREATURE_CNT) = 0;   // +0xd30
    bool bCreatureOk;
    if (creatureList[0] == 0)
        bCreatureOk = true;
    else
        bCreatureOk = setCreatureData(db, req->m_characNo, creatureList, sig);

    if (bCreatureOk)
    {
        SIG_U32(sig, SIG_OFF_CHARAC_NO) = req->m_characNo;
        SIG_U32(sig, SIG_OFF_FIELD3336) = req->m_field10;

        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("DBThread.cpp", 0x5492), true);
        guard.operator*()->operator<<(0x7f);
        guard.operator*()->operator<<(slot);
        guard->put_binary(sig, 0x338a);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        return 1;
    }
    return 0;
}