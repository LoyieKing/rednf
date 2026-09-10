// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 DB_MailBox_Req_List/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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
// [b28d-lane32 视图 cast 批量] 22 条 `(MailPostalItem*)SIG_PTR(sig, rowIdx*0x61)`
//   写访问 → 权威头 SIG_MAILBOX_POSTAL 具名成员直访问（SigTypes_D.h §8.3 字段化，
//   R2 已广播）：行首 m_postalId / m_sendCharacNo(+0x04) / m_sendName[0x1d](+0x08)；
//   m_tail 顺序具名 itemId/sealFlag/occTime/addInfo/endurance/upgrade/itemGuid/
//   m_amp/m_field43/m_field44/m_ext0/m_ext1/m_ext2Lo；m_occ（TAIL 直接成员，
//   保 4.4 折叠锚点 abs 0x50）m_field55/m_avataFlag/m_creatureFlag/m_unlimit/
//   m_letterId/m_field60/m_ext2Hi/m_ext3/m_field54。TU 局部视图类型
//   （MailPostalItem/MailAmpView/MailExtView）已删除；amp/ext 源改类型化局部
//   （SIG_MAILBOX_POSTAL_AMP ampBuf / union MailExtSource{m_ro,m_ext}）以复现
//   ORIG 栈直读。嵌套 if 展平为 continue 链（复现 ORIG 0x0841e8a0..0x0841e8e2
//   的 23 个「失败 → 循环尾」跳转岛）。
//   读数：dispatch orig 1169 / our 1224 / gap −55 / ndiff 856（改前 cast 版 1084）；
//   分支目标逐位比对 equal 块内 0 mismatch（116 vs 118）；p167 88=88 CLEAR
//   （仅 C1/C2、D1/D2 同址别名对）。TU 四数 53/131/1/4 不变。
//   协同：CMailBoxHelper 141/308/2/6、DB_MailBox_MailHelper 11/40/0/2 均零回退。
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
#include "DB_MailBox_Req_List/CompareLetterIndex.inc"
#include "DB_MailBox_Req_List/FindLastLetterIndexMail.inc"
#include "DB_MailBox_Req_List/RemoveMailByLatestLetter.inc"
#include "DB_MailBox_Req_List/IsContainedLetter.inc"
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
#include "DB_MailBox_Req_List/DoChangeMailList.inc"
}

// ============================================================================
// CompareLetterIndex —— ORIG 0x0841d12f：比较首字段（letter_id）严格小于。
// ============================================================================
#include "DB_MailBox_Req_List/CompareLetterIndex_1.inc"
}

// ============================================================================
// FindLastLetterIndexMail —— ORIG 0x0841d080：postal 区扫 +0x5c，找大于
//   letterId 的最大 letter_id 所在 postal 下标；无则 -1。
// ============================================================================
#include "DB_MailBox_Req_List/FindLastLetterIndexMail_1.inc"
}

// ============================================================================
// RemoveMailByLatestLetter —— ORIG 0x0841cd03：postal 区自 idx 起左移一项
//   （0x61 字节），count-1、remain+1，恒返 1。
// ============================================================================
#include "DB_MailBox_Req_List/RemoveMailByLatestLetter_1.inc"
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
// [round5-A29 · C05④ 四步流程复核，2026-09-10 定稿] 按 Main 最新四步判定顺序逐关走过：
//   ① **无消费者死比较？** 否 —— ORIG 0x0841d1fc 的 `setne %al; test %al,%al; je +0x159`
//      有真实消费者（je 跳过 if 体）⇒ 配方㉓ 空 switch 不适用。
//   ② **槽值/表达式物化？** 是 —— ORIG 正是「先落槽再比较」（`mov %eax,-0x10(%ebp)` →
//      `cmpl $0xffffffff,-0x10(%ebp); setne …`，无 movzbl/movsbl 归一）。按 D04 反证
//      （`DB_LoadInventory::dispatch` 259=259 AE）应整块换 **P225 原形**；**本件已实测**
//      `while (mailIdx != -1) { …; break; }`（见下方 b28e-L5 记录：clean 差异 24→24、
//      jcc 相对偏移反 +7 字节）——与 D04 的差别在 D04 的 body 以 `throw` 终止（无回边），
//      本件 body 尾部 `++start` 需真实 break ⇒ 得回边 + jmp 残留，故 P225 原形在此不成立；
//      本件亦无嵌套 if 可拆（推广形不适用）。
//   ③ **callee 返回类型与 ORIG 不一致？** 否 —— `FindLastLetterIndexMail` 两侧同为
//      `int`（返回下标、与 -1 有符号比较，`jle/jge` 有符号序与 ORIG 一致）⇒ 归因 1 不成立。
//   ④ ⇒ 按 **N-88 / P197④ 封顶收工**（不动控制流、不动 callee）。letterId 一项已由本轮
//      P235 具名视图修复（见函数体注释）。
#include "DB_MailBox_Req_List/ArrangeMailAndLetterIndex.inc"
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

// ext 源 union（ORIG 0x0841e7fb..0x0841e82c：源为 RandomOption 局部 -0x102 起的**直读**
// `mov -0x102(%ebp),%edx`；目标 ext 行同上）。同址双视角：写侧 m_ro（reset/get_binary），
// 读侧 m_ext（权威头行类型 SIG_MAILBOX_POSTAL_EXT，保 ORIG 的直读形）。
union MailExtSource
{
    RandomOption m_ro;
    SIG_MAILBOX_POSTAL_EXT m_ext;
};

// postal 明细项视图（stride 0x61）已**整体迁入权威头** SigTypes_D.h：
//   SIG_MAILBOX_POSTAL/SIG_MAILBOX_POSTAL_TAIL/SIG_MAILBOX_POSTAL_OCC/
//   SIG_MAILBOX_POSTAL_AMP/SIG_MAILBOX_POSTAL_EXT（§8.3 字段化，R2 已广播）。
// 旧 TU 局部 cast 视图（MailPostalItem/MailAmpView/MailExtView）已删除——
// cast 视图形恒「基址先载」，与 ORIG 4.1 的「下标先、基址后」装载序相反（P235），
// 且属「视图 cast 反模式」；现全部经参数类型自身成员具名访问。

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
//   [F2d-C05② 甄别] 本轮实测（show_func_diff fresh .o）orig=511 our=544 gap=-33 ndiff=357
//   （旧读数 -41 作废）；全 ORIG mem 直比点（`cmp -0x38/-0x40(%ebp),%eax`）我方同为
//   mem 直比（同形）⇒ 无 C05② 命中形；多出 = OURS-only{mov×11,lea×11,add×4,movzwl×3,…}
//   （具名局部缓存 + SIG 宏裸偏移 add/lea）；另含符号扩展点（ORIG cwtl vs 我方 movzwl×3，
//   建议后续按 P196 复核）。无 F2 落点。
// 帧-槽号族 + SIG 宏 offset 形（本轮已修 exec 校验为 ORIG 的
// `movzbl; mov slot; cmpl; jne` 形）。语义完成、byte 层 N 封顶。
#include "DB_MailBox_Req_List/SelectLetter.inc"
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
#include "DB_MailBox_Req_List/setCreatureData.inc"
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
#include "DB_MailBox_Req_List/dispatch.inc"
}