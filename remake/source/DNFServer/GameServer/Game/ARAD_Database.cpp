// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ARAD_Database/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// ARAD::DATABASE::CREATE_QUERY / LOGIN_QUERY 与 ARAD::Arad_DB_Charac_Dispatcher
// 真实实现 —— 逐函数按 docs/class_func_reports 汇编+反编译 C 转写（W6_ARAD.DbQueries）。
// ORIG 文件映射：
//   CREATE_QUERY/LOGIN_QUERY → localjapan/Arad_DB_Query.cpp
//   Arad_DB_Charac_Dispatcher → localjapan/Arad_Character.cpp
// 附带：WongWork::CEventMgr::GetCoinCharacCreate（ORIG T 0x0848f002，声明 CEventMgr.h
//       经 Main 收口）、CCoinEventOnCharCreate 全类（0x0810a914..aa9c objdump 转写）。
// 【ABI 镜像】SIG_CREATE_CHARAC/SIG_LOGIN_DATA 未建模偏移按报告实证字节序直访并逐处标注；
//   待 W6TB_DbCreateCharac 将 +0x5344/+0x5348 具名化后可切 sig->m_resultCode/m_characNo。
// ============================================================================
#include "ARAD_Database.h"

#include "MySQL.h"
#include "Stream.h"
#include "SigTypes.h"
#include "GlobalData.h"          // GlobalData::s_db_mgr / s_systemTime_
#include "DBMgr.h"               // DBMgr::GetDBHandle / addUserDBInfo / stUserDBInfo_t
#include "CEnvironment.h"        // G_CEnvironment / STDBConnInfo
#include "CEventMgr.h"           // WongWork::CEventMgr（声明经 Main 收口）
#include "AdvanceAltarZip.h"     // compress_zip
#include "GameTypes.h"           // cMyTrace
#include "CSystemTime.h"
#include "GameWorld.h"           // G_GameWorld / GameWorld::GetChannelType
#include "LogManager.h"

extern int DEFAULT_MAX_FATIGUE;  // CUserCharacInfo.h 声明的全局（ORIG B）

namespace OS_API
{
#include "ARAD_Database/snprintf.inc"
}

namespace
{

// 【ABI 镜像】SIG_CREATE_CHARAC 尾部两个 ORIG 实证字段（错误码/charac_no），
// W6TB_DbCreateCharac 正在 SigTypes.h 具名化为 m_resultCode(+0x5344)/m_characNo(+0x5348)。
inline unsigned int& create_result_code(SIG_CREATE_CHARAC* sig)
{
    return sig->m_resultCode;    // +0x5344（W6TB_DbCreateCharac 已具名化）
}
// 【ABI 镜像】POD 区未建模单值字段直访（SIG_LOGIN_DATA 在 SigTypes.h 仅 char m_pad[0x4358]；
// SIG_CREATE_CHARAC 内散列属性同样适用）。
// 宏而非模板函数：ORIG(4.1) 对该访问无外联 call 且无形参槽往返
//（mov 0x3914(%eax),%eax 单条实证 0x0818c6c4），函数形（含 always_inline）
// 在 4.4-O0 下均发射 call 或 base/off 槽。指针加法生 add+mov 两步，须用
// 类型化数组下标折叠单条 disp。
#define LOGIN_FIELD(T, base, off) \
    (reinterpret_cast<T*>(base)[(off) / sizeof(T)])

// 【ABI 镜像】SIG_CREATE_CHARAC 主属性区具名视图（ORIG 0x0818af9a 各
//   `mov off(%eax)` disp 单条实证；pack(1) 使 +0x26 等非对齐 int 折叠 disp，
//   替代 LOGIN_FIELD 指针加法在 4.4 下多出的 add+mov 两步形）。POD 无符号发射。
#pragma pack(push, 1)
struct SigCreateCharacAttrView
{
    char m_pad00[0x26];
    unsigned int   m_maxHP;      // +0x26
    unsigned int   m_maxMP;      // +0x2a
    unsigned short m_phyAtk;     // +0x2e
    unsigned short m_phyDef;     // +0x30
    unsigned short m_magAtk;     // +0x32
    unsigned short m_magDef;     // +0x34
    char m_pad36[0x60 - 0x36];   // +0x36..+0x5f（spec_property 8B / element 0x22B 区）
    unsigned int   m_invenWt;    // +0x60
    unsigned short m_hpRegen;    // +0x64
    unsigned short m_mpRegen;    // +0x66
    unsigned int   m_moveSpd;    // +0x68
    unsigned short m_atkSpd;     // +0x6c
    unsigned short m_castSpd;    // +0x6e
    unsigned short m_hitRecov;   // +0x70
    unsigned short m_jump;       // +0x72
    unsigned int   m_chaWt;      // +0x74
};
#pragma pack(pop)

#define SIG_ATTR(sig, field) \
    (reinterpret_cast<const SigCreateCharacAttrView*>(sig)->field)

} // namespace

namespace ARAD
{

namespace DATABASE
{

// ===========================================================================
// CREATE_QUERY（Arad_DB_Query.cpp）
// ===========================================================================

// ORIG 0x0818ac54：查 charac_info.delete_flag 判定角色名可用性；
//   select 失败→错误码2；命中行 delete_flag==0→错误码1（名字全新）、非0→3（已删角色名）。
#include "ARAD_Database/arad_sp_charater_name_check.inc"
}

// ORIG 0x0818ad8b：取 @@identity 作为新 charac_no 写 +0x5348。
#include "ARAD_Database/arad_sp_get_characer_index.inc"
}

// ORIG 0x0818aea2：count(delete_flag=0 角色)>=0x24 → 槽位上限错误码2。
#include "ARAD_Database/arad_sp_create_character_check.inc"
}

// ORIG 0x0818af9a：inSert into charac_info ……（新角色主记录，含两张 blob）。
// 列名/占位与字段映射完全照抄 ORIG 格式串与实参顺序。
// b28d-solo-ARAD_Database verdict=DIFF(193/195) N 封顶：P167 调用点集 8/8 CLEAR；
//   残余=槽组整体平移（ORIG escName -0x5c / spill -0xac..-0x7c vs 4.4 -0x98/-0x58..，
//   N82/N95 槽分配策略）+ maxHP/maxMP 无槽（ORIG 用 %esi/%edi）+ insert 返回值尾部
//   双 return 块序（ORIG Bfalse-ret0 在前，4.4 恒 Btrue-ret1 在前，N93附）。语义闭合。
#include "ARAD_Database/arad_sp_create_character.inc"
}
// ORIG 0x0818b26d：inSert into charac_stat …… 失败时错误码置 2。
#include "ARAD_Database/arad_sp_create_charac_stat.inc"
}

// ORIG 0x0818b2b7：inSert into inventory ……
//   m_giftFindOk(+0x533f)==0 → 无背包快照，仅插空库存（coin 由送币事件定）；
//   否则把 +0x3a9 起 0x3b55 字节 zip 压缩为 blob 连同原串一并入库。
// b28d-solo-ARAD_Database：ORIG 0x818b2b7 全头部声明（P212）——memset(zipBuf)/
//   zipLen/invenSrc 三条在 giftFindOk 判断之前；giftFindOk==0 分支的 insert 返回值
//   按 ORIG 不检查（P171 照抄不写）；主流程 insert 四可变实参右→左求值（coin 先入
//   %esi、两个 blob_to_str 结果 %ebx/%eax 寄存器直传，无具名局部槽）。
// b28d-solo-ARAD_Database verdict=DIFF(85/84)：P167 8/8 CLEAR；残余=memset 首参
//   edx 中转（待解条目，字面量/&arr[0]/sizeof 均试过无效）+ compress_zip 实参
//   装载序（同 create_equip）。语义闭合。
// [round5 复验] P68 形 `memset((char*)zipBuf, 0, sizeof(zipBuf))`（dest 先入 %eax）
//   实测 TU 四数零变化（63/130/0/6），dest 装载序未翻 ⇒ 维持 N 判词 | ORIG 0x0818b299
//   `lea -0x4162(%ebp),%eax; mov $0x4152,%edx; mov %edx,0x8(%esp); movl $0x0,0x4(%esp);
//   mov %eax,(%esp); call memset@plt`。
#include "ARAD_Database/arad_sp_create_inven.inc"
}

// ORIG 0x0818b406：inSert into charac_inven_expand 空记录。
#include "ARAD_Database/arad_sp_create_expand_inven.inc"
}

// ORIG 0x0818b443：初始 equipslot 快照（+0x4e01 起 0x2dc 字节）zip 后 upDate 入库。
// b28d-solo-ARAD_Database：ORIG 0x818b44e 起为内联 memset 对齐剥离慢路（P214③/
//   P253：`char zipBuf[0x333]={0}` 复现 rep stos 慢路与 push edi/ebx 帧）；
//   compress_zip 结果无槽往返（`xor $0x1,%eax; test %al,%al`，P0 右值内联）；
//   update 实参右→左求值：charac_no 先入 %ebx 跨 blob_to_str 保活，blob 返回值
//   %eax 直传（无 equipZip 槽）。
// b28d-solo-ARAD_Database verdict=DIFF(91/91)：P167 4/4 CLEAR；残余=compress_zip
//   第3参局部读与第4参立即数压栈先后（ORIG 先读局部，4.4 先压常量，P75/N-G 跨版本
//   调度）。语义闭合。
#include "ARAD_Database/arad_sp_create_equip.inc"
}

// ORIG 0x0818b588：inSert into skill（+0x79 起 0x198 字节技能树 blob）。
#include "ARAD_Database/arad_sp_create_skill.inc"
}

// ORIG 0x0818b5f4：inSert into pvp_result。
#include "ARAD_Database/arad_sp_create_pvp.inc"
}

// ORIG 0x0818b638：ORIG 本体即恒返 -1（纯 ret #0xffffffff，保留语义勿"修复"）。
#include "ARAD_Database/arad_sp_create_dungeon_point.inc"
}

// ORIG 0x0818b642：inSert into charac_dungeon 空记录。
#include "ARAD_Database/arad_sp_create_charac_dungeon.inc"
}

// ORIG 0x0818b68c：inSert into charac_quest 全空任务行。
#include "ARAD_Database/arad_sp_create_charac_quest.inc"
}

// ORIG 0x0818b6c9：inSert into charac_npc 空关系行。
#include "ARAD_Database/arad_sp_create_npc_relationship.inc"
}
// ===========================================================================
// LOGIN_QUERY（Arad_DB_Query.cpp）。ORIG 各查询经 DBMgr::GetDBHandle 取句柄。
// ===========================================================================

// ORIG 0x0818b706：member_info 主账号信息。逐列解析失败仅 trace 并返 0（ORIG 如此）；
//   成功时写 passwd 数值(+0xc0)、regdate(+0xcc)、hangame 标志(+0x38dd)、
   // ssn 拼接(+0x2284)与出生年(+0xc8)。【ABI 镜像】字段见 login_field 标注偏移。
// verdict=DIFF 353/356（gap=-3）：P167 调用点集 34/34 CLEAR。已修：
//   ①删 `unsigned int year` 具名槽（ORIG 两臂各自直写 data->m_birthYear，P0）；
//   ②10 处具名 cMyTrace 对象改**全表达式临时链** `cMyTrace(f,l,v)(fmt,…)`（P227，
//     Main/L4 实证配方）⇒ ORIG prologue `push %ebx; sub $0xf4` 逐条复现
//     （旧具名形 4.4 无 push %ebx、帧 0xf8），difflib 差异段 72→27。
// 残余（语义等价）：①col 清零与 -0xc0 槽的先后（ORIG -0xc 先）；②成员取址
//   `lea 0xc0(%eax),%edx` vs 4.4 `add $0xc0,%eax`（N107 六形探针全败）；③实参装载序
//   （P75，ORIG 先读 col 再压常量/参数）⇒ 净 3 条。语义闭合。
#include "ARAD_Database/arad_sp_select_account_info.inc"
}

// ORIG 0x0818bd67：member_punish_info 行集 → +0x3a54 起 stride 0x18 ×6 字段。
// b21 verdict=DIFF（N-JMP 残余）：调用点集与 ORIG 全等（GetDBHandle/select/logFormat/
//   get_int×3/get_uint×3/get_n_rows）；仅余 4.1 对每 get_* 失败独立 `mov -1; jmp exit`
//   块 vs 4.4 jump-threading 折成共享 return 块（155/150，gap=5）。禁 pun 引用局部
//   （P0：ORIG 右值内联，引局部占槽产生 EXTRA mov）。语义完成，byte 层 N 封顶。
#include "ARAD_Database/arad_sp_select_punish_user_info.inc"
}

// ORIG 0x0818bfb0：auto_punish_second_user 单行；punish_flag 字节为 0 即失败。
#include "ARAD_Database/arad_sp_select_punish_second_user.inc"
}

// ORIG 0x0818c0e0：member_mousepass（GoblinPad）加载。
#include "ARAD_Database/arad_sp_convert_pass_pad_info.inc"
}

// ORIG 0x0818c609：deLete member_mousepass；affected==0 视为失败。
#include "ARAD_Database/arad_sp_delete_member_mouspass.inc"
}

// ORIG 0x0818c6c4：upDate member_mousepass.fail_cnt。
#include "ARAD_Database/arad_sp_update_fail_record.inc"
}

// ORIG 0x0818c76f：先取旧 mousepass 再写入 history 表。
// b28d-50：memset→`char[36]={0}` 已复现 ORIG rep stos 段与 push edi/ebx 帧
//   （P214③/P253）。
// b28d-solo-ARAD_Database：尾部 `test %eax,%eax; sete %al; test %al,%al; je`
//   由 `if (getAffectedRowCount() == 0) return -1; return 0;` 复现（P197④ 寄存器
//   直比物化；三元 `?:` 形反产 jne 直跳）。探针 /tmp/arad_p2.cpp f1 逐条全等。
#include "ARAD_Database/arad_sp_update_history.inc"
}

// ORIG 0x0818c4f3：按时间窗查 member_mousepass_history 决定 Goblin 奖励标志。
#include "ARAD_Database/arad_sp_check_goblin_reward.inc"
}

// ORIG 0x0818c8b8：dnf_event_entry 全量装载到 vector<tagDNFEventEntry>。
#include "ARAD_Database/arad_sp_load_event_entry.inc"
}

// ORIG 0x0818ca69：member_broadcast 单行广播信息；到期未过→-1；
// channelType==7 时附 addUserDBInfo。
#include "ARAD_Database/arad_sp_select_member_brodcast.inc"
}

// ORIG 0x0818cc74：member_premium 首条生效记录 → SIG premium 表格区。
// verdict=DIFF 202/197（gap 12→5）。P167 调用点集 16/16 CLEAR。已修三处：
//   ①`prem` 引用缓存去 P183 ⇒ 四字段各自全表达式地址链（ORIG 9/8 条/处）；
//   ②cnt 具名槽去 P0 ⇒ `m_restrictedGoods[m_restrictedGoodsCount++]`（ORIG 读一次复用）；
//   ③三处 cMyTrace 具名对象改全表达式临时链 P221/P227 ⇒ 槽落 ORIG 的
//     -0x44/-0x34/-0x24 且 `data->m_accId` 在 ctor 前物化 %ebx 保活。
// 残余 N（5 条）：4 处 ORIG `add %ecx,%edx; add $K,%edx; mov %eax,(%edx)` vs 4.4
//   `mov %edx,K(%ecx,%eax,1)`（N4/N107 寻址折叠 + 值/基址寄存器互换）+ 循环条件
//   `&&` 短路物化侧（ORIG 物化第一条件 seta vs 4.4 物化第二条件，P197④/N 族；
//   德摩根形实测不变）。语义等价。
#include "ARAD_Database/arad_sp_select_member_premium.inc"
}
} // namespace DATABASE

// ===========================================================================
// Arad_DB_Charac_Dispatcher（Arad_Character.cpp）
// ===========================================================================

// 各 create_* 共通形态：GetDBHandle(idx) → CREATE_QUERY::arad_sp_xxx。
// ORIG 行号：0x4e/0x67/0x7b/0x95/0xb0/0xbf/0xcd/0xdb/0xe9。

#include "ARAD_Database/dispatch.inc"
}

#include "ARAD_Database/create_charac.inc"
}

#include "ARAD_Database/create_charac_stat.inc"
}

#include "ARAD_Database/create_inventory.inc"
}

#include "ARAD_Database/create_equip.inc"
}

#include "ARAD_Database/create_skill.inc"
}

#include "ARAD_Database/create_pvp.inc"
}

#include "ARAD_Database/create_charac_dungeon.inc"
}

#include "ARAD_Database/create_charac_quest.inc"
}

#include "ARAD_Database/create_charac_npc_relationship.inc"
}

} // namespace ARAD

// ===========================================================================
// WongWork::CEventMgr::GetCoinCharacCreate —— ORIG T 0x0848f002 objdump 转写。
// GetRepeatEvent(5) 且事件 IsEventing → CCoinEventOnCoinCharCreate::GetCoinNoOnCreateCharac；
// 否则默认 5。
// b28d-solo-ARAD_Database N7 封顶：ORIG 0x0848f045 `mov (%eax),%edx; …; call *%edx`（目标 %edx、this
//   %eax），4.4.7 恒目标 %eax、this %edx（间接 call 族，P230补：4.4.6 后端才出
//   %edx 形）。本 TU 未路由 446（改路由表属 Main 权限，禁自改）；源形不可择 ⇒ N7
//   封顶，语义等价（虚槽 +0x34 = IsEventing 已核）。
// ===========================================================================
namespace WongWork
{

#include "ARAD_Database/GetCoinCharacCreate.inc"
}

} // namespace WongWork

// ===========================================================================
// CCoinEventOnCharCreate —— ORIG 0x0810a914..aa9c objdump 全量转写。
// ===========================================================================

#include "ARAD_Database/CCoinEventOnCharCreate.inc"
}

#include "ARAD_Database/dtor_CCoinEventOnCharCreate.inc"
}                                                    // ORIG 仅恢复 vptr + 调基类析构

#include "ARAD_Database/StartEvent.inc"
}

#include "ARAD_Database/StartEvent_1.inc"
}

#include "ARAD_Database/EndEvent.inc"
}

#include "ARAD_Database/GetCoinNoOnCreateCharac.inc"
}
