// ============================================================================
// df_game_r 还原 —— DB_MailBox_Req_List（OrigMailList19 批，独占 TU，未入 CMake，
// 待 Main 登记）
// 覆盖 3 个 ORIG T 符号（地址见 DB_MailBox_Req_List.h 头注释）：
//   1. DB_MailBox_Req_List::SelectLetter    0x0841d52c
//   2. DB_MailBox_Req_List::setCreatureData 0x0841dc84
//   3. DB_MailBox_Req_List::dispatch        0x0841de4a
// 报告均已全文阅读后转写（docs/class_func_reports/DB_MailBox_Req_List/
//   {SelectLetter,setCreatureData,dispatch}.md）。
// SIG 布局：SIG_MAILBOX_LIST（SigTypes_D.h，sizeof 0x338a）——b28d-solo 在本
//   权威头加匿名 union 具名视图（m_box postal 区 / m_user user_items /
//   m_creature creature_items，§8.3 字段化 + R2 广播），本 TU 按视图成员访问；
//   SIG_LETTER_INFO（CMailBoxHelper.h，已按 ORIG 实证修正为 0x12b）。
// 依赖：DBDispatcher::dispatch 定义归 DB_Login.cpp；stAvatarEmblemInfo_t::init
//   归 GameBasicsScripts.cpp；RandomOption::reset 定义归 Inven_Item.cpp（本 TU
//   按 ORIG 0x0841e8d7 直调真函数，p167 MISSING 归零）。
// b28d-solo 判词见各函数头 [b28d-solo-DB_MailBox_Req_List]；结果 TSV
//   /tmp/b28d_solo_DB_MailBox_Req_List_result.tsv。
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

// ---- postal 区扫描视图（pack(1)，ORIG stride 0x61） ----
// [b28d-solo-DB_MailBox_Req_List] 视图必须落**文件作用域**：4.4.7 对函数内局部
// 结构体先载基址（`mov 0x8(%ebp),%edx; mov -0x4(%ebp),%eax`），文件作用域才复现
// ORIG 4.1 的「下标先、基址后」装载序（探针 /tmp/p1/q.cpp f1 vs r.cpp g1）。
#pragma pack(push, 1)
struct LetterPostalEntry
{
    unsigned char m_pad[0xc];
    int m_letterId;
    unsigned char m_tail[0x61 - 0x10];
};
struct LetterListView
{
    unsigned char m_pad[0x50];
    LetterPostalEntry m_postals[0x12];
};
struct LetterCountView
{
    unsigned char m_pad[0x794];
    int m_count;
};
#pragma pack(pop)

// ============================================================================
// IsContainedLetter —— ORIG 0x0841ccb6：postal 区（stride 0x61）扫 +0x5c
//   letter_id 是否含目标。
bool IsContainedLetter(SIG_MAILBOX_LIST* list, int letterId)
// [b28d-solo-DB_MailBox_Req_List] verdict=DIFF（26 vs 25），N 封顶两项：
// ①@0x0841ccd4 ORIG 两段物化 `mov 0xc(%eax),%edx; mov 0xc(%ebp),%eax;
//   cmp %eax,%edx` vs 4.4 单段 `cmp 0xc(%ebp),%eax` ⇒ N18b（成员 vs 形参比较
//   恒折，volatile 形可出两段但多 sete/test 三段，探针 /tmp/p2/vm.cpp p2）；
// ②循环体首装载序 ORIG「下标先、基址后」vs 4.4「基址先」——视图已落**文件作用域**
//   （局部 struct 亦为 base 先，探针 /tmp/p2/r.cpp g1），但实参类型 SIG_MAILBOX_LIST*
//   到视图类型的 cast 仍触发 base 先（w.cpp h1 vs h2 对照；索引先仅在本型视图/局部
//   基址指针形出现，后者多 2 条）⇒ P30/P235 结构性伪影。
// 遍历区间与命中语义与 ORIG 全等；p167 不适用（无 call）。
{
    for (int i = 0; i < ((LetterCountView*)list)->m_count; ++i)
    {
        // b28c-l23：ORIG @0x0841ccd4 为 `mov 0xc(%eax),%edx; mov 0xc(%ebp),%eax;
        // cmp %eax,%edx` ⇒ **letterId 是左操作数**（条目值先物化 %edx，形参后载 %eax），
        // 我方把条目写在左侧使 4.4 折成 `cmp MEM,reg` 单段（N18b 镜像，少 1 条）。
        // 循环体首两条 ORIG 为 `mov -0x4(i),%eax; mov 0x8(list),%edx`（下标先于基址）。
        if (letterId == ((LetterListView*)list)->m_postals[i].m_letterId)
            return 1;
    }
    return 0;
}

// ============================================================================
// DoChangeMailList —— ORIG 0x0841d470（void）：清 mailIdx 表头；postal 数为 0 时
//   +0x79c 直接置 param_3；否则 +0x79c=0 并逐项取 max(postal_id) 回写 +0x79c，
//   mailIdx 表收 postal 的 +0x2e add_info。
// ============================================================================
// [b28d-solo-DB_MailBox_Req_List] verdict=AE（check_tu_routed，56=56）。修法：
// ①SIG_MAILBOX_LIST 权威头新增 m_box 具名视图（SigTypes_D.h，R2 广播）——单 disp
//   count/0x79c + `mov (%edx,%eax,1)` postal_id + `add $0x20; mov 0xe` add_info；
// ②if/else 极性反写复现 `test; je <count==0 块>` 块序；③max 目标地址不再被 4.4
//   跨 call 缓存（cast 视图形恒缓存于 %ebx，本型视图形不缓存，探针 /tmp/p2/w.cpp
//   w1 vs /tmp/p2/rm2.cpp）。
void DoChangeMailList(SIG_MAILBOX_LIST* list, unsigned int* mailIdx, int count)
{
    mailIdx[0] = 0;
    // [b28d-solo-DB_MailBox_Req_List] 已按 ORIG 布局改写：①SIG_MAILBOX_LIST 权威头
    // 新增 m_box 具名视图（SigTypes_D.h，R2 已广播）后成员访问复现单 disp
    // （count/0x79c）与 `add $0x20; mov 0xe` 两级尾段、且装载序为 ORIG 的
    // 「下标先、基址后」（cast 视图形恒 base 先，P235）；②if/else 极性反写，复现
    // ORIG `test; je <count==0 块>`（then 块落尾）的块序。
    // 残余：`std::max` 赋值目标地址被 4.4 跨 call 缓存于 %ebx（ORIG 4.1 在 call 后
    // `mov 0x8(%ebp),%eax` 重算），四形探针（视图/宏/混合/别名视图）均恒缓存 ⇒
    // 跨版本代码生成差，记 N（+push/pop %ebx）。
    if (list->m_box.m_count != 0)
    {
        list->m_box.m_maxPostalId = 0;
        for (int i = 0; i < list->m_box.m_count; ++i)
        {
            unsigned int id = (unsigned int)list->m_box.m_postals[i].m_postalId;
            list->m_box.m_maxPostalId = std::max(list->m_box.m_maxPostalId, id);
            mailIdx[0] = mailIdx[0] + 1;
            mailIdx[mailIdx[0]] = list->m_box.m_postals[i].m_tail.m_addInfo;
        }
    }
    else
    {
        list->m_box.m_maxPostalId = (unsigned int)count;
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
    // [b28d-solo-DB_MailBox_Req_List] verdict=NEAR（check_tu_routed，56=56，仅剩
    // 「下标先/基址先」装载序差，P30/P235 结构性伪影；助记符序列全等）。
    // 修法：ORIG 0x0841d080 逐条复现：两循环各自
    // **for 内声明** i/v（ORIG 槽 -0x10/-0xc 与 -0x8/-0x4 两套独立槽，同一声明
    // 复用会并槽）、命中写 ret 后 break（ORIG 尾 `mov ret,%eax; leave; ret` 单出口）。
    int maxIdx = 0;
    for (int i = 0; i < ((LetterCountView*)list)->m_count; ++i)
    {
        int v = ((LetterListView*)list)->m_postals[i].m_letterId;
        // ORIG 0x0841d0ae `mov v,%eax; cmp letterId,%eax; jle` ⇒ 写法取
        // `v > letterId`（4.4 对 `letterId < v` 恒折 `mov letterId,%eax;
        // cmp v,%eax; jge`，jcc 方向相反，探针 /tmp/p2/cmp.cpp c1/c2）。
        if (v > letterId && maxIdx < v)
            maxIdx = v;
    }
    int ret = -1;
    if (maxIdx != 0)
    {
        for (int i = 0; i < ((LetterCountView*)list)->m_count; ++i)
        {
            int v = ((LetterListView*)list)->m_postals[i].m_letterId;
            if (v == maxIdx)
            {
                ret = i;
                break;
            }
        }
    }
    return ret;
}

// ============================================================================
// RemoveMailByLatestLetter —— ORIG 0x0841cd03：postal 区自 idx 起左移一项
//   （0x61 字节），count-1、remain+1，恒返 1。
// ============================================================================
bool RemoveMailByLatestLetter(SIG_MAILBOX_LIST* list, int mailIdx)
{
    // [b28d-solo-DB_MailBox_Req_List] verdict=AE（check_tu_routed，87=87）。
    // 原 N78 判词（memmove 恒 call）已被 P312/N84
    // 路线推翻：ORIG 0x0841cd03 的 0x61 字节搬移 = **结构体整体赋值**的对齐剥离
    // rep movsl 展开（探针 /tmp/p2/c.cpp c1 与 ORIG 0x0841cd44..0x0841cdae 逐条同形）。
    // 改用 SIG_MAILBOX_LIST::m_box 整体赋值后该段与 count/remain 段全部复现
    // （探针 /tmp/p2/rm2.cpp 与 ORIG 逐条同形）；memmove 会退化为 call。
    if (list->m_box.m_count - 1 != mailIdx)
    {
        int i;
        for (i = mailIdx; i < list->m_box.m_count - 1; ++i)
        {
            list->m_box.m_postals[i] = list->m_box.m_postals[i + 1];
        }
    }
    list->m_box.m_count -= 1;
    list->m_box.m_remain += 1;
    return 1;
}

// ============================================================================
// ArrangeMailAndLetterIndex —— ORIG 0x0841d16d：vecNew 按 letter_id 排序后，从
//   size-count 处起逐项：找 postal 区中 letter_id 更大的最新邮件（FindLast
//   LetterIndexMail）并删除（RemoveMailByLatestLetter），同时从 vecOld 删同
//   letter_id 项。恒返 1。
// ============================================================================
// [b28d-solo-DB_MailBox_Req_List] verdict=DIFF（111 vs 109），N 封顶两项：
// ①ORIG 0x0841d1fc `cmpl $0xffffffff,-0x10; setne %al; test %al,%al; je` 为
//   4.1.2 槽比 setcc 物化，4.4 对槽比恒直跳 `je`（P197④；`bool z=(x!=-1)` 会多槽）
//   ⇒ 不可复现；②letterId 取用 cast 视图（保留 ORIG `add $0x50; mov 0xc(%eax)`
//   两段形）的「基址先」装载序（P30/P235，同 IsContainedLetter②）。
// 已修：start 改 unsigned（复现 `mov count,%edx; sub %edx,%eax` + seta）、
// operator[] 结果内联作实参（去 letterId 具名槽）、拆两条独立 if（消 && 物化岛）、
// begin/end 迭代器循环头缓存（P13）。语义等价。
bool ArrangeMailAndLetterIndex(SIG_MAILBOX_LIST* list,
                               std::vector<SIG_LETTER_INFO>& vecNew,
                               int count,
                               std::vector<SIG_LETTER_INFO>& vecOld)
{
    std::sort(vecNew.begin(), vecNew.end(), CompareLetterIndex);
    // ORIG 0x0841d1c2 `mov count,%edx; sub %edx,%eax` + 尾测 `cmp start,%eax;
    // seta` ⇒ start 为 unsigned（int 形恒折 `sub mem,%eax` + setg，探针
    // /tmp/p2/sub.cpp a1/a2 对照）。
    unsigned int start = vecNew.size() - count;
    while (vecNew.size() > start)
    {
        int mailIdx = -1;
        // [b28d-solo-DB_MailBox_Req_List] ORIG 把 operator[] 结果直接作实参
        // （0x0841d1e3 `call operator[]; mov (%eax),%eax; mov %eax,0x4(%esp)`），
        // 无 letterId 具名局部；具名会多 1 槽 + 2 条 mov。
        mailIdx = FindLastLetterIndexMail(list, *(int*)&vecNew[start]);
        if (mailIdx != -1)
        {
            int letterIdx = ((LetterListView*)list)->m_postals[mailIdx].m_letterId;
            // 拆两条独立 if：ORIG 0x0841d234/0x0841d23e 两处直跳统一失败出口；
            // 合并 && 在 4.4 生 mov$1/jmp/mov$0/test 物化岛（P225 推广反向应用）。
            if (RemoveMailByLatestLetter(list, mailIdx))
            {
                if (letterIdx != 0)
                {
                    // ORIG 在循环头缓存 begin/end 两枚迭代器槽（-0x24/-0x28，
                    // 0x0841d251/0x0841d266 两 call 实证），非每轮重调 end()（P13）。
                    std::vector<SIG_LETTER_INFO>::iterator it = vecOld.begin();
                    std::vector<SIG_LETTER_INFO>::iterator e = vecOld.end();
                    for (; it != e; ++it)
                    {
                        if (*(int*)&*it == letterIdx)
                        {
                            vecOld.erase(it);
                            break;
                        }
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

// ============================================================================
// SelectLetter —— ORIG 0841d52c（普通成员，this 调用）：

//   清 SIG_MAILBOX_LIST 信件区（+0x101c 起 0x230a）→ letterIdx==0 时先删过期
//   stat=2/1 邮件 → 按 (charac_no, stat=3) or (charac_no, reg_date 15 天内)
//   [letterIdx>0 时加 letter_id>idx] 查 letter 表 → 逐行按 stat 分流到三个 vector
//   （stat==3 单独；stat 1/2 按 IsContainedLetter 分流）→ ArrangeMailAndLetterIndex
//   整理 → 限额拷回 +0x101c 区（stride 0x12b）→ 写 +0x332a/+0x798/+0x1018。
// ============================================================================
// [b28d-solo-DB_MailBox_Req_List] verdict=DIFF（ORIG 513 vs 554），§6.2 超大函数
// （orig_insns>500）三件套：①p167 调用点差集 **CLEAR 34=34**；②gap = -41（我方大）；
// ③帧 ORIG sub $0x1cc vs 我方 sub $0x1ec（+0x20 槽组平移）。残余为 N82/N95
// 帧-槽号族 + SIG 宏 offset 形（本轮已修 exec 校验为 ORIG 的
// `movzbl; mov slot; cmpl; jne` 形）。语义完成、byte 层 N 封顶。
bool DB_MailBox_Req_List::SelectLetter(MySQL* db, int slot, int characNo,
                                       unsigned int letterIdx,
                                       SIG_MAILBOX_LIST* out)
{
    (void)slot;
    memset(SIG_PTR(out, 0x101c), 0, 0x230a);
    // [b28d-solo-DB_MailBox_Req_List] ORIG 0x0841d5a8 三处 exec 校验为
    // `call exec; movzbl %al,%eax; mov %eax,-0x40(%ebp); cmpl $0x0,-0x40(%ebp);
    // jne` ⇒ 源形是「int 局部接收 + `== 0` 早返」（不是 `!(unsigned char)exec()`
    // 的 xor 形，探针 /tmp/p2/ex.cpp g1/g2）。
    int bExec;   // ORIG 无前置清零（首次写入在 exec 调用后）

    if (letterIdx == 0)
    {
        db->set_query(
            "deLete from letter where charac_no=%d and reg_date<subdate(now(),interval %d day) and stat=%d",
            characNo, 0xf, 2);
        bExec = db->exec(true);
        if (bExec == 0)
            return 0;

        db->set_query(
            "deLete from letter where charac_no=%d and reg_date<subdate(now(),interval %d day) and stat=%d",
            characNo, 0xf, 1);
        bExec = db->exec(true);
        if (bExec == 0)
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

    bExec = db->exec(true);
    if (bExec == 0)
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
// [b28d-solo-DB_MailBox_Req_List] verdict=AE（check_tu_routed，125=125）。
// 修法：①SIG_MAILBOX_LIST::m_creature 具名视图（stride 0x25、基址 +0xd30、
// item[0] 首 4 字节条数）复现 `mov (%edx,%eax,1)` 与 `lea; add $0xd30; mov disp`
// 形；②三个具名局部（声明序按 ORIG 槽 -0x10/-0x14/-0x18）承接 get_int 再回写；
// ③m_count 用 unsigned 触发 `mov %eax,%edx` 中转（P211/P217）。
bool DB_MailBox_Req_List::setCreatureData(MySQL* db, unsigned int characNo,
                                          unsigned int* list,
                                          SIG_MAILBOX_LIST* out)
{
    (void)characNo;
    char buf[0x1000] = { 0 };

    // [b28d-solo-DB_MailBox_Req_List] ORIG 0x0841dc84 逐条复现：三个**具名局部**
    // （ORIG -0x10/-0x14/-0x18 三槽，0x0841dca5..0x0841dcb3 三条 movl $0 实证）
    // 逐个 get_int 接收后再写入明细（ORIG 不把 &item->field 直接传给 get_int）；
    // 明细经 SIG_MAILBOX_LIST::m_creature 视图（stride 0x25、基址 +0xd30、
    // item[0] 首 4 字节为条数）。
    // 声明序按 ORIG 槽位排（4.4「先声明占浅槽」：-0x10/-0x14/-0x18）。
    int expireDate = 0;
    int regDate = 0;
    int uiId = 0;

    getList2inQuery(list, buf);
    db->set_query(
        "seLect ui_id,unix_timestamp(expire_date),unix_timestamp(reg_date) from creature_items where ui_id %s and stat = 1",
        buf);
    db->exec(true);

    out->m_creature.m_items[0].m_count = db->get_n_rows();

    unsigned int i;
    for (i = 0; i < (unsigned int)db->get_n_rows(); ++i)
    {
        if (!db->fetch())
            return 0;

        if (!db->get_int(0, uiId))
            return 0;
        if (!db->get_int(1, expireDate))
            return 0;
        if (!db->get_int(2, regDate))
            return 0;

        out->m_creature.m_items[i].m_uiId = uiId;
        out->m_creature.m_items[i].m_expireDate = expireDate;
        out->m_creature.m_items[i].m_regDate = regDate;
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
// [b28d-solo-DB_MailBox_Req_List] verdict=DIFF（ORIG 1193 vs 1160），§6.2 超大函数
// （orig_insns>500）三件套：①p167 调用点差集 **CLEAR 88=88**（本轮归零 4 项：
// MISSING RandomOption::reset（memset 等价物改真调用）、EXTRA memcpy（jewel 改
// stAvatarEmblemInfo_t 结构体整体赋值）、EXTRA _Rb_tree_iterator::C1Ev（写回循环
// 改 begin() 直接初始化）、EXTRA ptEv（`it->second += 1` 单次 operator->）；
// ②gap = +33（我方小）；③帧 ORIG sub $0x463c vs 我方 sub $0x464c（+0x10）。
// 本轮同时修真 bug：user_items 视图缺前导 4 字节 pad（ui_id/reg_date/agency/jewel
// 全部低移 4 字节）→ 权威头 SIG_MAILBOX_USER_ITEM（+0x04/+0x08/+0x0c/+0x10/+0x29）。
// 残余为 N82/N95 帧-槽号族；语义完成、byte 层 N 封顶。
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
        // [b28d-solo-DB_MailBox_Req_List] ORIG 0x0841e8d7 `lea -0x106(%ebp),%eax;
        // call RandomOption::reset` ⇒ 原码是 RandomOption 局部 + reset()（不是
        // memset 等价物）；p167 MISSING `_ZN12RandomOption5resetEv` 由此归零。
        RandomOption extOpt;
        extOpt.reset();

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
                                if (db->get_binary(idx, &extOpt, 0xe)) // extend_info 区（-0x102）
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
                                            // ORIG 0x0841f0xx 段：`lea opt; mov (%eax),%edx`
                                            // / `lea opt; add $4; mov (%eax),%edx`（+0/+4/+8
                                            // 逐段 add 形，P82 尾部视图）。
                                            item->m_ext0 = *(unsigned int*)&extOpt;
                                            item->m_ext1 = *(unsigned int*)((char*)&extOpt + 4);
                                            item->m_ext2 = *(unsigned int*)((char*)&extOpt + 8);
                                            item->m_ext3 = *(unsigned short*)((char*)&extOpt + 12);
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
                // ORIG 0x0841e9e5 `call ptEv; mov 0x4(%eax),%edx; add $1,%edx;
                // mov %edx,0x4(%eax)` ⇒ `+= 1`（`= it->second + 1` 恒发两次 ptEv，
                // 探针 /tmp/p2/mp.cpp a1/a2）。
                it->second += 1;
            SIG_I32(sig, SIG_OFF_LETTER_ADD) += 1;
        }
    }

    // map 键回写 +0x333a（stride 4，最多 0x14 项）
    if (!extraMap.empty())
    {
        int idx = 0;
        // ORIG 0x0841ea9e：begin() 直构进 it 槽（-0x124）+ 循环内每轮重调 end()
        // （0x0841eab8 call end）⇒ 直接初始化、非「默认构造 + 赋值」（P314/P50，
        // 旧形多发 _Rb_tree_iterator::C1Ev，p167 EXTRA）。
        std::map<unsigned int, int>::iterator it = extraMap.begin();
        for (; it != extraMap.end(); ++it)
        {
            // ORIG 0x0841ea63 `test %eax,%eax; jle` ⇒ 判据是 `> 0`（`>= 1` 产 cmp $1）。
            if (it->second > 0 && idx <= 0x13)
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
        sig->m_user.m_items[0].m_count = db->get_n_rows();

        int ui;
        for (ui = 0; ui < db->get_n_rows(); ++ui)
        {
            if (!db->fetch())
                return 0;
            // [b28d-solo-DB_MailBox_Req_List] 真 bug 修正：ORIG 明细基址
            // sig+0x7a0+ui*0x47 的字段在 **+0x04 ui_id / +0x08 expire_date /
            // +0x0c reg_date / +0x10 agency / +0x29 jewel**（0x0841ee1a/ee35/ee50/
            // ee61 实证）；旧 MailUserItemView 缺前导 4 字节 pad ⇒ ui_id/reg_date/
            // agency/jewel 全部低移 4 字节。改用权威头 SIG_MAILBOX_USER_ITEM 视图。
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

            sig->m_user.m_items[ui].m_expireDate = expireDate;
            sig->m_user.m_items[ui].m_uiId = uiId;
            sig->m_user.m_items[ui].m_regDate = regDate;
            strncpy(sig->m_user.m_items[ui].m_ipgAgency, agency, 0x18);
            // ORIG 0x0841ee95..0x0841eed5 段为 7×dword + 1×word 结构体整体拷贝
            // （非 memcpy 调用；P167 EXTRA memcpy 由此归零）。
            *(stAvatarEmblemInfo_t*)sig->m_user.m_items[ui].m_jewel = emblem;
        }
    }

    sig->m_creature.m_items[0].m_count = 0;   // +0xd30
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