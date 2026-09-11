// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ARAD_Database/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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
                                              char const* dbName)
{
    int rc = db->select("seLect delete_flag from %s.charac_info where charac_name='%s'",
                        dbName, sig->m_name);
    if (rc != 0)
    {
        sig->m_resultCode = 2;                                                                                // +0x5344 直写（helper 外联消除）
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x1f, "%s", "select error");
        return -1;
    }
    int deleteFlag = 0;
    if (db->get_n_rows() != 0)
    {
        sig->m_resultCode = 2;                                                                            // +0x5344 直写（helper 外联消除）
        if (!db->get_int(0, deleteFlag))
        {
            LogManager::logFormat(
                1, "localjapan/Arad_DB_Query.cpp",
                __PRETTY_FUNCTION__,
                0x28, "%s", "Parsing Error of Selected Field");
            return -1;
        }
        if (deleteFlag != 0)
        {
            sig->m_resultCode = 3;                                                                        // +0x5344 直写（helper 外联消除）；已删除角色名占用（可复用场景由上层定夺）
        }
        else
        {
            sig->m_resultCode = 1;                                                                        // +0x5344 直写（helper 外联消除）；在用角色名占用
        }
        return -1;
    }
    return 0;                                   // 名字不存在，可直接创建
}

// ORIG 0x0818ad8b：取 @@identity 作为新 charac_no 写 +0x5348。
#include "ARAD_Database/arad_sp_get_characer_index.inc"
{
    // ORIG 0x0818ad91：函数头 `movl $0x0,-0xc(%ebp)` 死槽写（原码遗留未用局部，
    //   P197③/ P171 照抄）；加死局部复现。
    int index = 0;
    (void)index;
    if (db->select("seLect @@identity") != 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x47, "%s", "select @@identity error");
        return -1;
    }
    if (!db->get_uint(0, sig->m_characNo))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x4c, "%s", "Parsing Error of Selected Field");
        return -1;
    }
    if (sig->m_characNo == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x51, "%s", "charac_no error");
        return -1;
    }
    return 0;
}

// ORIG 0x0818aea2：count(delete_flag=0 角色)>=0x24 → 槽位上限错误码2。
#include "ARAD_Database/arad_sp_create_character_check.inc"
{
    int rc = db->select("seLect count(*) from charac_info where m_id=%d and delete_flag=0",
                        sig->m_accId);
    if (rc != 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x62, "%s", "select charac_info error");
        return -1;
    }

    int count = 0;
    if (!db->get_int(0, count) || count >= 0x24)
    {
        sig->m_resultCode = 2;                                                                                // +0x5344 直写（helper 外联消除）
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x69, "%s", "max charac slot limit");
        return -1;
    }
    return 0;
}

// ORIG 0x0818af9a：inSert into charac_info ……（新角色主记录，含两张 blob）。
// 列名/占位与字段映射完全照抄 ORIG 格式串与实参顺序。
// b28d-solo-ARAD_Database verdict=DIFF(193/195) N 封顶：P167 调用点集 8/8 CLEAR；
//   残余=槽组整体平移（ORIG escName -0x5c / spill -0xac..-0x7c vs 4.4 -0x98/-0x58..，
//   N82/N95 槽分配策略）+ maxHP/maxMP 无槽（ORIG 用 %esi/%edi）+ insert 返回值尾部
//   双 return 块序（ORIG Bfalse-ret0 在前，4.4 恒 Btrue-ret1 在前，N93附）。语义闭合。
// [S5-2026-09-11 复攻] gap −2→**0**（195→193，ndiff 154→144）：删 maxHP/maxMP 两个
//   具名局部（ORIG 0x0818b168 `mov 0x26(%eax),%esi` / 0x0818b223 `mov %esi,0x18(%esp)`
//   实证无槽、寄存器直传），实参位直接展开 SIG_ATTR ⇒ 4.4 亦不再建槽往返（P0 正例）。
//   残余 `je`(ORIG)/`jne`(OURS) 1 条 = N93附 尾块序；槽组平移 = N82/N95（编译器分配
//   策略差：ORIG push %esi/%edi 保活、4.4 用栈槽）。语义闭合。
#include "ARAD_Database/arad_sp_create_character.inc"
{
    if (arad_sp_create_character_check(db, sig) != 0)
    {
        return -1;
    }

    // ORIG：库名 = G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)2].m_db(+0x42)，
    // 具名直访 public m_dbConnInfo（+0x68）内联进 name_check 第三实参（ORIG 无
    // connMap/dbName 具名槽；operator[] 的 key 临时槽 -0x20 由右值实参自然产生）。
    if (arad_sp_charater_name_check(
            db, sig,
            G_CEnvironment()->m_dbConnInfo[
                static_cast<ENUM_DB_HANDLE_IDX>(2)].m_db) != 0)
    {
        return -1;
    }

    char escName[60] = {0};                          // ORIG 0x818b015 rep stos 60B
    db->escape_string(escName, sig->m_name);

    // 【ABI 镜像】charac 主属性打包区（+0x26..+0x79 内散列整型，按报告字节宽读取）。
    // ORIG 0x818b4a9 起为 insert 实参右→左求值 spill：具名局部按 ORIG 槽深→浅
    // （-0xac..-0x6c）顺序声明，4.4 全头部声明同向分配即逐槽对齐。
    int expertJob        = static_cast<int>(static_cast<char>(sig->m_pad25)); // -0xac
    int maxFatigue       = DEFAULT_MAX_FATIGUE;                              // -0xa8
    unsigned int chaWt   = SIG_ATTR(sig, m_chaWt);                           // -0xa4
    int jump             = static_cast<short>(SIG_ATTR(sig, m_jump));        // -0xa0
    int hitRecov         = static_cast<short>(SIG_ATTR(sig, m_hitRecov));    // -0x9c
    unsigned int castSpd = SIG_ATTR(sig, m_castSpd);                         // -0x98
    unsigned int atkSpd  = SIG_ATTR(sig, m_atkSpd);                          // -0x94
    unsigned int moveSpd = SIG_ATTR(sig, m_moveSpd);                         // -0x90
    int mpRegen          = static_cast<short>(SIG_ATTR(sig, m_mpRegen));     // -0x8c
    int hpRegen          = static_cast<short>(SIG_ATTR(sig, m_hpRegen));     // -0x88
    unsigned int invenWt = SIG_ATTR(sig, m_invenWt);                         // -0x84
    // blob 序列化：element_resist 取 +0x3e 起 0x22 字节、spec_property 取 +0x36 起 8 字节
    char* elementResist  = db->blob_to_str(1, reinterpret_cast<char*>(sig) + 0x3e, 0x22); // -0x80
    char* specProperty   = db->blob_to_str(0, reinterpret_cast<char*>(sig) + 0x36, 8);    // -0x7c
    unsigned int magDef  = SIG_ATTR(sig, m_magDef);                          // -0x78
    unsigned int magAtk  = SIG_ATTR(sig, m_magAtk);                          // -0x74
    unsigned int phyDef  = SIG_ATTR(sig, m_phyDef);                          // -0x70
    unsigned int phyAtk  = SIG_ATTR(sig, m_phyAtk);                          // -0x6c
    int ret = db->insert(
            "inSert into charac_info "
            "( m_id,charac_name,job,sex,maxHP,maxMP,phy_attack,phy_defense,mag_attack,"
            "mag_defense,element_resist,spec_property,inven_weight,hp_regen,mp_regen,"
            "move_speed,attack_speed,cast_speed,hit_recovery,jump,charac_weight,"
            "max_fatigue,create_time,expert_job) "
            "values(%u,'%s',%d,%d,%d,%u,%d,%d,%d,%d,'%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,"
            "%d,%d,NOW(),%d)",
            sig->m_accId,
            escName,
            static_cast<int>(static_cast<char>(sig->m_slot)),            // +0x22
            static_cast<int>(static_cast<char>(sig->m_ssnCheckFail)),    // +0x534c
            SIG_ATTR(sig, m_maxHP), SIG_ATTR(sig, m_maxMP), phyAtk, phyDef, magAtk, magDef,
            elementResist, specProperty,
            invenWt, hpRegen, mpRegen, moveSpd, atkSpd, castSpd, hitRecov, jump,
            chaWt, maxFatigue, expertJob);
    // [round5 C05-F1 三形复验，判 N93附 维持封顶] ORIG 0x0818b25d 尾块序 =
    //   `cmpl $0,-0x1c(%ebp); je <成功块>; mov $0xffffffff,%eax; jmp <尾>; <成功>:
    //   mov $0x0,%eax`（失败直落、成功块落函数尾）。三形实测：
    //   ① 平铺 `if (ret==0){return 0;} return -1;` ⇒ 4.4 反序 `jne <失败块>; mov $0; jmp`；
    //   ② `goto ok_ret;` 尾标签形 ⇒ **极性/块序翻正**（`je <成功块>; mov $-1; jmp`）但多发
    //     1 条对齐 nop（指令数 +1 = 退化）⇒ 按 C05③/A28 边界回退；
    //   ③ P47 三目 `return (ret==0)?0:-1;` ⇒ 同 ①（反序）。
    //   TU 四数三形恒 193/0/6/13（无净收益）⇒ 取最小改动形（①），N93附 封顶。
    if (ret == 0)
    {
        return 0;
    }
    return -1;
}
// ORIG 0x0818b26d：inSert into charac_stat …… 失败时错误码置 2。
#include "ARAD_Database/arad_sp_create_charac_stat.inc"
{
    if (db->insert(
            "inSert into charac_stat  (charac_no,HP, forbidden_to_play, "
            "forbidden_due_to)  values(%d,100,0,0) ",
            sig->m_characNo) != 0)
    {
        sig->m_resultCode = 2;                                                                                // +0x5344 直写（helper 外联消除）
        return -1;
    }
    return 0;
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
// [S5-2026-09-11 复核] 维持 N：ORIG 的 `mov $imm,%edx; mov %edx,0x8(%esp)` 常量经寄存器
//   中转是 4.1 风格（4.4 恒直出 `movl $imm,0x8(%esp)`），源码侧无旋钮（字面量/&arr[0]/
//   sizeof 三形已试）；gap +1 = ORIG 独有段，判 N 不硬啃。SQL 串逐字节命中 ORIG。
#include "ARAD_Database/arad_sp_create_inven.inc"
{
    // [符号名轴 2026-09-11 W13D 探针实测] 原 `static const unsigned int kInvenSrcLen =
    //   sourceInvenSize;` 会在本 .o 发射 ORIG 侧不存在的
    //   `_ZZN4ARAD8DATABASE12CREATE_QUERY20arad_sp_create_inven…E12kInvenSrcLen`（4B r）
    //   ⇒ 发明符号。改用命名空间常量 sourceInvenSize（ORIG 侧有同名
    //   `_ZN4ARAD8DATABASEL15sourceInvenSizeE`；编译期常量 ⇒ 仍出立即数）。
    //   overlay 实测：OUR-only 2→0、四数 63/130/0/6 零变化。
    char zipBuf[0x4152];
    memset(zipBuf, 0, sizeof(zipBuf));
    unsigned int zipLen = 0x4152;
    char* invenSrc = reinterpret_cast<char*>(sig) + 0x3a9;

    if (sig->m_giftFindOk != 0)
    {
        // ORIG 判定与 equip 相反：compress 返回非 0 即放弃（逐指令转写，勿"修正"）
        if (compress_zip(zipBuf, zipLen, invenSrc, sourceInvenSize))
        {
            return -1;
        }
        if (db->insert("inSert into inventory (charac_no, inventory,coin) "
                       "values(%u, '%s',%d)",
                       sig->m_characNo,
                       db->blob_to_str(0, zipBuf, static_cast<int>(zipLen)),
                       db->blob_to_str(0, invenSrc, sourceInvenSize),
                       WongWork::CEventMgr::GetCoinCharacCreate()) != 0)
        {
            return -1;
        }
    }
    else
    {
        db->insert("inSert into inventory  (charac_no, coin, money)  values(%u, %d) ",
                   sig->m_characNo);
        if (WongWork::CEventMgr::GetCoinCharacCreate() != 0)
        {
            return -1;
        }
    }
    return 0;
}

// ORIG 0x0818b406：inSert into charac_inven_expand 空记录。
#include "ARAD_Database/arad_sp_create_expand_inven.inc"
{
    if (db->insert(" inSert into charac_inven_expand  "
                   "(charac_no,cargo,cargo_capacity, jewel)  values(%u,'',0,'')",
                   sig->m_characNo) != 0)
    {
        return -1;
    }
    return 0;
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
{
    // [符号名轴 2026-09-11 W13D] 同 inven：原函数内 static const 会在 .o 发射 ORIG 侧
    //   不存在的 `…arad_sp_create_equip…E12kEquipSrcLen` ⇒ 改用命名空间常量
    //   sourceEquipSize（ORIG 有 `_ZN4ARAD8DATABASEL15sourceEquipSizeE`）。
    char zipBuf[0x333] = {0};
    // ORIG 槽布局：zipLen@-0x10（深）、equipSrc@-0xc（浅）；存储序 zipLen=0x333
    //   先于 equipSrc 计算；调用实参序则 equipSrc 读取先于第 4 参常量压栈
    //   （0x818b4bb）。4.4 恒「后声明深」，故 equipSrc 先声明（浅 −0xc）但延后赋值，
    //   zipLen 后声明（深 −0x10）带初值 ⇒ 同时满足槽位与存储序；equipSrc 以
    //   volatile 变量钉住读取点，复现「先读第 3 参」（create_skill 同法已 AE）。
    char* volatile equipSrc;                                              // ORIG -0xc 具名局部
    unsigned int zipLen = 0x333;                                          // ORIG -0x10 具名局部
    equipSrc = reinterpret_cast<char*>(&sig->m_giftItem);
    if (!compress_zip(zipBuf, zipLen, equipSrc,
                      sourceEquipSize))   // +0x4e01 初始装备区
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x155, "%s", "equip compress zip fail");
        return -1;
    }

    if (db->update(" upDate inventory set equipslot='%s' where charac_no = %u ",
                   db->blob_to_str(0, zipBuf, static_cast<int>(zipLen)),
                   sig->m_characNo) != 0)
    {
        return -1;
    }
    return 0;
}

// ORIG 0x0818b588：inSert into skill（+0x79 起 0x198 字节技能树 blob）。
#include "ARAD_Database/arad_sp_create_skill.inc"
{
    // b28d-50 AE：ORIG 0x818b588。ORIG 0x818b58e：0x198 先落 -0xc 槽并被
    // blob_to_str 第三参读用（P0 具名局部）；blob 返回值 eax 寄存器直传 insert
    // 第三参（无 skillBlob 槽）。实参装载序：ORIG 先读 len（-0xc→%eax）再算
    // 指针（`mov 0xc(%ebp),%edx; add $0x79,%edx`）；4.4 对普通 int 局部反向
    // （先 lea 指针、后读 len，N-G/P75 跨版本序）。加 volatile 限定使该读取
    // 成为不可延迟的求值点 ⇒ 4.4 复现 ORIG 的「先读 len 后 add 指针」序。
    volatile int len = 0x198;
    if (db->insert("inSert into skill (charac_no, skill_slot) values(%u, '%s')",
                   sig->m_characNo,
                   db->blob_to_str(0, (void*)((char*)sig + 0x79), len)) != 0)
    {
        return -1;
    }
    return 0;
}

// ORIG 0x0818b5f4：inSert into pvp_result。
#include "ARAD_Database/arad_sp_create_pvp.inc"
{
    // ORIG 0x0818b5fa：函数头 `movl $0x198,-0xc(%ebp)` 死槽写（原码遗留未用局部，
    //   P171 照抄），加死局部复现。
    int queryId = 0x198;
    (void)queryId;
    if (db->insert("inSert into pvp_result (charac_no) values(%u)",
                   sig->m_characNo) != 0)
    {
        return -1;
    }
    return 0;
}

// ORIG 0x0818b638：ORIG 本体即恒返 -1（纯 ret #0xffffffff，保留语义勿"修复"）。
#include "ARAD_Database/arad_sp_create_dungeon_point.inc"
{
    return -1;
}

// ORIG 0x0818b642：inSert into charac_dungeon 空记录。
#include "ARAD_Database/arad_sp_create_charac_dungeon.inc"
{
    // ORIG 0x0818b648：先 `movl $0x0,-0xc`（双槽置 0/P197③）再用具名局部 characNo
    // 经槽传递（mov 0x5348→-0xc 再重读入参），非直读成员。
    int characNo = 0;
    characNo = sig->m_characNo;
    if (db->insert("inSert into charac_dungeon(charac_no, dungeon) values(%d, '')",
                   characNo) != 0)
    {
        return -1;
    }
    return 0;
}

// ORIG 0x0818b68c：inSert into charac_quest 全空任务行。
#include "ARAD_Database/arad_sp_create_charac_quest.inc"
{
    if (db->insert(
            "inSert into charac_quest(charac_no,quest_10,quest_15,quest_20,quest_30,"
            "quest_40,quest_50, quest_60,quest_70,quest_etc,play_1,play_1_trigger,"
            "play_2,play_2_trigger,play_3,play_3_trigger,play_4,play_4_trigger,"
            "play_5,play_5_trigger,play_6,play_6_trigger,play_7,play_7_trigger,"
            "play_8,play_8_trigger,play_9,play_9_trigger,play_10,play_10_trigger) "
            "values(%d,'','','','','','','','','',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0)",
            sig->m_characNo) != 0)
    {
        return -1;
    }
    return 0;
}

// ORIG 0x0818b6c9：inSert into charac_npc 空关系行。
#include "ARAD_Database/arad_sp_create_npc_relationship.inc"
{
    if (db->insert("inSert into charac_npc(charac_no,npc_cnt,npc_data) values(%d,0,'')",
                   sig->m_characNo) != 0)
    {
        return -1;
    }
    return 0;
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
// [S5-2026-09-11 复攻] gap −3→**0**（356→353，ndiff 75→66）：①删 snprintf 两处
//   `static_cast<int>`（直接传 unsigned，变参提升等价）⇒ 消 1 条 mov；②出生年两臂
//   `(int)fssn + 2000/1900`（int 域加法）⇒ 4.4 由 `add $imm,%eax; mov %eax,%edx`
//   翻成 ORIG `lea imm(%eax),%edx`，消 2 条 mov（重定基址）。负结论：`++col` 拆成
//   `++col; get_uint(col,…)` 实测零变化（4.4 仍先算 arg3 再读 col）⇒ 已回滚；
//   余下 3 处 lea↔add 为纯 N107（4.1 lea/4.4 add 风格差），源码不可择。
#include "ARAD_Database/arad_sp_select_account_info.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x1e8, "%s", "Database null!!");
        return -1;
    }

    if (db->select(
            "seLect m_id, passwd,first_ssn, second_ssn, "
            "mid(from_unixtime(reg_date)+0,1,8),user_name,email, user_id, hangame_flag "
            "from member_info where m_id=%d",
            data->m_accId) != 0)
    {
        return -1;
    }
    if (db->get_n_rows() != 1)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1f5, 5)("query get_n_rows() ERROR m_id=%u", data->m_accId);
        return 0;
    }

    unsigned int fssn = 0;                    // first_ssn
    unsigned int sssn = 0;                    // second_ssn
    unsigned int regMid = 0;                  // mid(from_unixtime(reg_date))
    unsigned int hangameFlag = 0;
    // ORIG 0x818b7f0 起 col 槽 -0xc 为 0，各调用实参为 `addl $1,槽; mov 槽,%eax`
    //   ⇒ 源形统一 `++col`（先自增后读）。
    int col = 0;
    if (!db->get_uint(++col, data->m_accId))       // col1
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x201, 5)("user m_id ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_str(++col, param_2, 0x84))                              // col2
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x207, 5)("user password ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_uint(++col, fssn))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x20d, 5)("user fssn ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_uint(++col, sssn))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x213, 5)("user sssn ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_uint(++col, regMid))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x219, 5)("user regdate ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_str(++col, data->m_userDetailIpPort, 0x80))  // user_name（+0x2184）
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x21f, 5)("user user_name ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_str(++col, data->m_userDetailSsn, 0x80))  // email（+0x2204）
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x225, 5)("user email ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_str(++col, reinterpret_cast<char*>(data), 0x1e))           // user_id
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x22b, 5)("user id ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_uint(++col, hangameFlag))                                  // col9
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x231, 5)("user HanGameUser Check ERROR m_id=%u", data->m_accId);
        return 0;
    }

    data->m_regDate = regMid;
    data->m_hangameUser =
        static_cast<unsigned char>(hangameFlag == 1 ? 1 : 0);
    OS_API::snprintf(data->m_birth, 0x10, "%d%d",   // +0x2284
                     fssn, sssn);
    // ORIG 0x0818bcf1：先 fssn/=10000、sssn/=1000000（mul/shr 常量除法），
    // 再 sssn > 2 → year=fssn+2000(0x7d0)，否则 year=fssn+1900(0x76c)。
    fssn = fssn / 10000;
    sssn = sssn / 1000000;
    if (sssn > 2)
    {
        data->m_birthYear = static_cast<int>(fssn) + 2000;
    }
    else
    {
        data->m_birthYear = static_cast<int>(fssn) + 1900;
    }
    return 0;
}

// ORIG 0x0818bd67：member_punish_info 行集 → +0x3a54 起 stride 0x18 ×6 字段。
// b21 verdict=DIFF（N-JMP 残余）：调用点集与 ORIG 全等（GetDBHandle/select/logFormat/
//   get_int×3/get_uint×3/get_n_rows）；仅余 4.1 对每 get_* 失败独立 `mov -1; jmp exit`
//   块 vs 4.4 jump-threading 折成共享 return 块（155/150，gap=5）。禁 pun 引用局部
//   （P0：ORIG 右值内联，引局部占槽产生 EXTRA mov）。语义完成，byte 层 N 封顶。
#include "ARAD_Database/arad_sp_select_punish_user_info.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_1,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x255, "%s", "Database null!!");
        return -1;
    }

    if (db->select(
            "seLect punish_type, unix_timestamp(occ_time),punish_value, apply_flag, "
            "unix_timestamp(start_time),unix_timestamp(end_time) from "
            "member_punish_info where m_id=%d and apply_flag=1 LIMIT %d",
            data->m_accId, 0x32) != 0)
    {
        return -1;
    }

    for (unsigned int row = 0; row < static_cast<unsigned int>(db->get_n_rows());
         ++row)
    {
        if (!db->get_int(0, data->m_punishInfos[row].m_type))
            return -1;
        if (!db->get_uint(1, reinterpret_cast<unsigned int&>(
                                 data->m_punishInfos[row].m_field04)))
            return -1;
        if (!db->get_int(2, data->m_punishInfos[row].m_stdDropRate))
            return -1;
        if (!db->get_int(3, data->m_punishInfos[row].m_field0c))
            return -1;
        if (!db->get_uint(4, reinterpret_cast<unsigned int&>(
                                 data->m_punishInfos[row].m_field10)))
            return -1;
        if (!db->get_uint(5, reinterpret_cast<unsigned int&>(
                                 data->m_punishInfos[row].m_endTime)))
        {
            return -1;
        }
    }
    return 0;
}

// ORIG 0x0818bfb0：auto_punish_second_user 单行；punish_flag 字节为 0 即失败。
#include "ARAD_Database/arad_sp_select_punish_second_user.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x293, "%s", "Database null!!");
        return -1;
    }

    if (db->select(
            "seLect trade_cnt,trade_gold,punish_flag,unix_timestamp(occ_time) from "
            "auto_punish_second_user where m_id=%d",
            data->m_accId) != 0)
    {
        return -1;
    }

    int occTime = 0;                           // ORIG select 前双预清槽（-0x10/-0x11）
    char punishFlag = 0;                       // ORIG 不回写 sig，仅做判定
    db->get_uint(0, data->m_totalHackUserTradeCnt);           // trade_cnt
    // trade_gold：ORIG 按 8 字节 long 槽读取（覆盖 m_totalHackUserTradeGold 与高位槽）
    db->get_ulonglong(
        1, *reinterpret_cast<unsigned long long*>(&data->m_totalHackUserTradeGold));
    db->get_byte(2, punishFlag);
    db->get_int(3, occTime);
    if (punishFlag != 0)
        return 0;
    return -1;
}

// ORIG 0x0818c0e0：member_mousepass（GoblinPad）加载。
#include "ARAD_Database/arad_sp_convert_pass_pad_info.inc"
{
    // b28d-50 AE：ORIG 0x0818c0e0。槽位：ORIG db=-0xc、cMyTrace=-0x1c..-0xd、
    // enableFlag=-0x1d（4.1 按首次使用序分槽）；4.4 先给函数作用域标量分槽
    // （db -0xc、enableFlag -0xd），再给块内聚合 cMyTrace 分槽 ⇒ 对象被挤到
    // -0x20。修法：enableFlag 改为**后置块内局部**（声明在 cMyTrace 块之后的
    // 复合块内），使其分槽晚于该块展开 ⇒ cMyTrace 落 -0x1c、enableFlag 落
    // -0x1d，与 ORIG 逐条全等（探针 /tmp/arad_probe/s5.cpp f7 实证）。
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x2b0, "%s", "Database null!!");
        return -1;
    }

    if (!db->set_query(
            "seLect mousepass, fail_cnt, cancel_cnt, version_info, validity_time, "
            "enable_flag from member_mousepass where m_id=%d",
            data->m_accId))
    {
        return -1;
    }
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            700, "%s", "arad_sp_convert_pass_pad_info fail");
        return -1;
    }

    if (db->get_n_rows() == 0)
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x2c2, 0);
        trace("Goblin Pad Exist");
        data->m_passPadVersion = 0;      // version_info 置无
        return 0;
    }

    if (!db->fetch())
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x2c9, "%s", "arad_sp_convert_pass_pad_info fetch fail");
        return -1;
    }

    if (!db->get_str(0, data->m_passPadPassword, 0x24))   // +0x38e1
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x2cf, "%s", "Mouse Pass Encrypted Parsing error");
        return -1;
    }
    // [b21 N·P198-②/P197-④] 以下 get_str/get_int/get_byte/get_uint 五连判定——
    // ORIG(4.1) 对 call 结果 `xor $0x1,%eax; test; je` 寄存器直测（无槽）;4.4-O0 对
    // 该类 call 结果 bool 判定恒 materialize 到栈槽 `mov %al,-off; movzbl; xor; test`
    // （P198-②/P197-④：源形不可复现，内联也出同样栈槽，禁改比较=双调用真 bug）。
    // 逐 getter 列/目标偏移/错误日志行号已与 ORIG 逐一核对，语义闭合，封 N。
    if (!db->get_int(1, LOGIN_FIELD(int, data, 0x3914)))     // fail_cnt
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x2d5, "%s", "Mouse PassPad Fail Count Parsing error");
        return -1;
    }
    if (!db->get_int(2, LOGIN_FIELD(int, data, 0x3918)))     // cancel_cnt
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x2db, "%s", "PassPad Cancel Count Parsing error");
        return -1;
    }
    if (!db->get_byte(3, *reinterpret_cast<char*>(&data->m_passPadVersion)))   // version_info（+0x390c）
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x2e0, "%s", "VersionInfo Parsing error");
        return -1;
    }
    if (!db->get_uint(4, LOGIN_FIELD(unsigned int, data, 0x3910))) // validity_time
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x2e6, "%s", "Goblin Validity Time Parsing error");
        return -1;
    }
    {
        char enableFlag;  // b28d-50：后置块内局部，见函数头判词
        enableFlag = 0;   // ORIG movb $0x0,-0x1d 预清（P198-①两句式）
        if (!db->get_byte(5, enableFlag))                   // enable_flag
        {
            LogManager::logFormat(
                1, "localjapan/Arad_DB_Query.cpp",
                __PRETTY_FUNCTION__,
                0x2ed, "%s", "Goblin enable flage Parsing error");
            return -1;
        }
        if (enableFlag == 1)
        {
            // ORIG：movb 直写 0x390e——即 SIG_LOGIN_DATA::m_passPadCertified
            // （bool，具名化替代 login_field 外联 call）。
            data->m_passPadCertified = true;
        }
    }
    return 0;
}

// ORIG 0x0818c609：deLete member_mousepass；affected==0 视为失败。
#include "ARAD_Database/arad_sp_delete_member_mouspass.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x345, "%s", "Database null!!");
        return -1;
    }
    if (db->select("deLete from member_mousepass where m_id=%d",
                   data->m_accId) != 0)
    {
        return -1;
    }
    // ORIG 0x0818c6ab：64 位行数 or %edx,%eax 判零后走 sete 物化
    // （sete %al; test %al,%al; je），源用 if(x==0) return -1 而非三目（三目 O0 直出 jne）。
    if (db->getAffectedRowCount() == 0)
        return -1;
    return 0;
}

// ORIG 0x0818c6c4：upDate member_mousepass.fail_cnt。
#include "ARAD_Database/arad_sp_update_fail_record.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x365, "%s", "Database null!!");
        return -1;
    }
    if (db->select("upDate member_mousepass set fail_cnt=%d where m_id=%d",
                   data->m_passPadFailCnt,
                   data->m_accId) != 0)
    {
        return -1;
    }
    return 0;
}

// ORIG 0x0818c76f：先取旧 mousepass 再写入 history 表。
// b28d-50：memset→`char[36]={0}` 已复现 ORIG rep stos 段与 push edi/ebx 帧
//   （P214③/P253）。
// b28d-solo-ARAD_Database：尾部 `test %eax,%eax; sete %al; test %al,%al; je`
//   由 `if (getAffectedRowCount() == 0) return -1; return 0;` 复现（P197④ 寄存器
//   直比物化；三元 `?:` 形反产 jne 直跳）。探针 /tmp/arad_p2.cpp f1 逐条全等。
#include "ARAD_Database/arad_sp_update_history.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x37c, "%s", "Database null!!");
        return -1;
    }

    if (db->select("seLect mousepass from member_mousepass  where m_id=%d",
                   data->m_accId) != 0)
    {
        return -1;
    }

    // b28d-50：ORIG 0x818c7f5 段 `lea -0x30(%ebp),%ebx; mov $0x9,%edx; rep stos`
    // （9 dword = 36B 内联清零）；4.4 对 memset 恒发 call（N78）⇒ 改显式零初始化
    // 复现 rep stos 与 push edi/ebx 帧形（P214③/P253，探针 /tmp/arad_probe/z1.cpp f2）。
    char oldMousePass[36] = {0};
    if (!db->get_str(0, oldMousePass, 0x24))
    {
        return -1;
    }
    if (db->select("inSert into member_mousepass_history"
                   "(occ_time, m_id, pre_mousepass, modify_type)  "
                   "values (now(),%d,'%s',%d)",
                   data->m_accId, oldMousePass,
                   modifyType) != 0)
    {
        return -1;
    }
    if (db->getAffectedRowCount() == 0)
    {
        return -1;
    }
    return 0;
}

// ORIG 0x0818c4f3：按时间窗查 member_mousepass_history 决定 Goblin 奖励标志。
#include "ARAD_Database/arad_sp_check_goblin_reward.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x30d, "%s", "Database null!!");
        return -1;
    }

    // ORIG 未检查 set_query 返回值，直接 exec
    db->set_query("seLect m_id from member_mousepass_history where modify_type = %d "
                  "and m_id=%d and occ_time >= '%s'",
                  0x10, data->m_accId, sinceTime);
    if (!db->exec(true))
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            799, "%s", "arad_sp_check_goblin_reward fail");
        return -1;
    }
    // ORIG 0x0818c5e7-0x0818c5f1：`call get_n_rows; test %eax,%eax; mov...` ——
    // 单次调用后紧跟无效 test（Z 标志随后被 mov 覆盖，无分支消费），即原码空体
    // if(get_n_rows()){} 的 O0 残留（探针实证 if(x){} 恒出 call+test）。存储无条件。
    if (db->get_n_rows()) {}
    // ORIG 0x0818c4f3+0x??：`movb $0x1,0x38e0(%data)` 直写——0x38e0 即
    // SIG_LOGIN_DATA::m_passPadEnable（GoblinPad 启用标志，真名具名化；
    // login_field helper 在此 GCC 不内联反产外联 call，改用具名字段直写）。
    data->m_passPadEnable = true;
    data->m_passPadVersion = 2;
    return 0;
}

// ORIG 0x0818c8b8：dnf_event_entry 全量装载到 vector<tagDNFEventEntry>。
// [S5-2026-09-11] §6.5 prett：ORIG 串为 `…arad_sp_load_event_entry(SIG_LOGIN_DATA*,
//   DB_Login::vecDNFEventEntry_t&)`（typedef 拼写）⇒ 第2参由展开型改 typedef 型，
//   串逐字节命中；余 `static ` 前缀同一结构性报告（见 premium 处）。
#include "ARAD_Database/arad_sp_load_event_entry.inc"
    SIG_LOGIN_DATA* data, DB_Login::vecDNFEventEntry_t& out)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x3a5, "%s", "Database null!!");
        return -1;
    }

    // P0：Ghidra C 直接读 env+0x378（=m_serverEnvir.m_serverGroup）内联为实参，
    //   ORIG 无 serverGroup 局部槽（值在 %edx 直传）。
    // P171 照抄原码：ORIG 0x0818c951 为 `test %al,%al; je 主流程`（无 xor）⇒ 原码把
    //   set_query 返回 true 判为失败（与同 TU arad_sp_convert_pass_pad_info
    //   0x0818c16a 的 `xor $1; test; je` 相反）。可观察行为必须与 ORIG 一致 ⇒ 照抄。
    if (db->set_query(
            "seLect event_id,unix_timestamp(occ_date),server_id,charac_no,"
            "unix_timestamp(obtain_date) from dnf_event_entry where m_id=%d "
            "and server_id in(0,%d)",
            data->m_accId, G_CEnvironment()->m_serverEnvir.m_serverGroup))
    {
        return -1;
    }

    // P0：Ghidra C 中 entry（local_2c）为函数级局部，循环外声明一次复用（免每轮 dtor）
    // b28d-solo-ARAD_Database：ORIG 0x818c96b 起五守卫各带独立 nop 中继岛（P223
    //   多岛形：`if(!c) goto Lk` 标签序=守卫序 + 各岛尾 goto 循环尾），且每守卫为
    //   `call; xor $1,%eax; test %al,%al; jne <岛>`（get_int 返回 bool 裸形，P292）。
    // verdict=AE 目标：P167 11/11 CLEAR；set_query 判定按 ORIG 反向形照抄后
    //   111/111（消 xor 1 条）。
    DB_Login::tagDNFEventEntry entry;
    for (unsigned int row = 0; row < static_cast<unsigned int>(db->get_n_rows());
         ++row)
    {
        // P0：Ghidra C 无 occDate/serverId/characNo/obtainDate 中间局部 ——
        // 5 个 get_int 直接写 tagDNFEventEntry 各字段（+0/+8/+0xc/+0x10/+0x14）
        if (!db->get_int(0, entry.m_eventId)) goto skip1;
        if (!db->get_int(1, entry.m_occDate)) goto skip2;
        if (!db->get_int(2, entry.m_serverId)) goto skip3;
        if (!db->get_int(3, entry.m_characNo)) goto skip4;
        if (!db->get_int(4, entry.m_obtainDate)) goto skip5;
        out.push_back(entry);
        goto skipend;
    skip1: goto skipend;
    skip2: goto skipend;
    skip3: goto skipend;
    skip4: goto skipend;
    skip5:;
    skipend:;
    }
    return 0;
}

// ORIG 0x0818ca69：member_broadcast 单行广播信息；到期未过→-1；
// channelType==7 时附 addUserDBInfo。
#include "ARAD_Database/arad_sp_select_member_brodcast.inc"
    SIG_LOGIN_DATA* data, DB_Login::tagDNFEventEntry& entry, int userIdx)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x3d6, "%s", "Database null!!");
        return -1;
    }

    if (db->select(
            "seLect m_id,charac_no,server_id,unix_timestamp(start_time),charac_name "
            "from member_broadcast where event_id=%d and m_id=%d and end_time>now() limit 1",
            data->m_accId,
            entry.m_eventId) != 0)
    {
        return -1;
    }

    // b28d-solo-ARAD_Database：ORIG 0x818cb10 起逐条独立守卫（每条
    //   `call; xor $1,%eax; test %al,%al; je <成功继续>`，P168a bool callee 裸形）；
    //   仅 startTime 有零初始化槽（-0x1c）。ORIG 把 col0/1/2 直接写 userInfo 三个
    //   连续成员（-0x18/-0x14/-0x10），无独立 characNo/serverId 槽。
    // verdict=NEAR 132/132（gap 2→0）：P167 12/12 CLEAR。已修两处：①DBMgr.h
    //   stUserDBInfo_t 三成员 int→unsigned int（同宽/偏移/mangled 不变；ORIG
    //   0x0818cc51/0x0818cc5d 两处 store 无 eax→edx 中转 ⇒ P211 档③同型直存；
    //   探针实证 unsigned→unsigned 单段）消 2 条中转；②比较改
    //   `(int)startTime > getCurSec()`（P243/P23 书写序=求值序）⇒ 复现 ORIG
    //   `mov -N,%eax; mov %eax,%ebx; …call…; cmp %eax,%ebx; setg` 的 %ebx 保活。
    //   残余仅槽号（ORIG startTime -0x1c/userInfo -0x18 vs 4.4 -0x10/-0x1c，
    //   4.4 按首次使用序，声明序调整无效）与数据地址。语义闭合。
    // 判据⑩（b28d 槽序律）：4.4 恒「后声明者占深槽」；ORIG 布局 startTime@-0x1c（深）/
    //   userInfo@-0x18..-0x10（浅）⇒ 源码须 userInfo 先声明、startTime 后声明
    //   （create_equip 同法已 AE）。
    DBMgr::stUserDBInfo_t userInfo;             // ORIG -0x18：col0/1/2 复用 +0/+4/+8
    unsigned int startTime = 0;                 // ORIG -0x1c
    if (!db->get_uint(0, reinterpret_cast<unsigned int&>(userInfo.m_userDBIdx)))
    {
        return -1;
    }
    if (!db->get_uint(1, reinterpret_cast<unsigned int&>(userInfo.m_userDBType)))
    {
        return -1;
    }
    if (!db->get_uint(2, reinterpret_cast<unsigned int&>(userInfo.m_serverGroup)))
    {
        return -1;
    }
    if (!db->get_uint(3, startTime))
    {
        return -1;
    }
    if (!db->get_str(4, reinterpret_cast<char*>(data) + 0x39b0, 0x1e))
    {
        return -1;
    }

    // P243/P23：ORIG 0x0818cbfe 先把 startTime 载入 %ebx 跨 getCurSec call 保活
    //   （`mov -0x1c(%ebp),%eax; mov %eax,%ebx; …call…; cmp %eax,%ebx; setg`）
    //   ⇒ 左操作数写 startTime、右操作数写 call 表达式（求值序=书写序）。
    if (static_cast<int>(startTime) > GlobalData::s_systemTime_.getCurSec())
    {
        return -1;                                           // 广播尚未生效
    }
    if (G_GameWorld()->GetChannelType() == 7)
    {
        GlobalData::s_db_mgr->addUserDBInfo(userIdx, userInfo);
    }
    // P0：Ghidra C 直接偏移写 —— 0x39ac/0x39a8 对应具名字段（login_field 外联 call 需免）
    // P211 档③：ORIG 两处 store 无 eax→edx 中转 ⇒ 两侧同型；stUserDBInfo_t 成员是 int，
    //   显式 static_cast<unsigned int> 使 RHS 与字段同型（同宽零指令），消 2 条中转。
    data->m_tournamentAccountFlag =
        static_cast<unsigned int>(userInfo.m_userDBType);
    data->m_tournamentServerGroup =
        static_cast<unsigned int>(userInfo.m_serverGroup);
    return 0;
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
// [S5-2026-09-11 复攻] §6.5 串：SQL 串逐字节命中 ORIG（`seLect pre_type,…`）；
//   prett 第4参拼写还原为 `vecPremiumRemoveInfo_t`（ORIG 串实证，见 .rodata 比对）；
//   余 `static ` 前缀 + 第3参 `IntVector&` 需改 .h（class→namespace；全局 typedef
//   IntVector 本仓缺失）⇒ 上游报告，不在本 TU 写域。gap −1 / ndiff 17 维持（N 族：
//   ORIG `seta;test;jne` vs 4.4 `jbe` 循环条件物化侧）。
#include "ARAD_Database/arad_sp_select_member_premium.inc"
    SIG_LOGIN_DATA* data, DB_Login::tagDNFEventEntry& entry,
    IntVector& removeList, vecPremiumRemoveInfo_t removeScratch)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_6,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x412, "%s", "Database null!!");
        return -1;
    }

    if (db->select(
            "seLect pre_type,unix_timestamp(service_start),unix_timestamp(service_end) "
            "from member_premium where m_id=%d and event_id=%d and service_end>now() "
            "and server_id in(0,%d) order by service_end desc limit 1",
            data->m_accId, entry.m_eventId,
            entry.m_serverId) != 0)
    {
        return -1;
    }

    if (db->get_n_rows() == 0)
    {
        // 无记录 → 把事件 id 计入第4参 scratch 表（ORIG 仅 push param_4）
        (void)removeList;
        removeScratch.push_back(entry.m_eventId);
        return 0;
    }

    int slot = 0;
    // [round5 Main-B轴 实测负例·P196 红线] 曾试 `int row` + `(int)get_n_rows()`：改后该比较
    //   变有符号 `jle`，而 ORIG 是 `call get_n_rows; cmp -0xc(%ebp),%eax; seta %al; test %al,%al;
    //   jne 818cd68`（**seta = 无符号**）⇒ 违反 P196（禁止把无符号序改有符号），已回退。
    //   真正的残差是「ORIG 物化 `seta;test;jne` vs 我方直接 `jbe`」，非符号性；row 保持 unsigned。
    for (unsigned int row = 0;
         row < static_cast<unsigned int>(db->get_n_rows()) && db->fetch();
         ++row)
    {
        int preType = 0;
        int startSec = 0;
        int endSec = 0;
        if (!db->get_int(0, preType))
        {
            // P221/P227：ORIG 槽序 -0x44/-0x34/-0x24 与 4.4 具名序反向，且
            //   `data->m_accId` 在 ctor 前物化 %ebx 跨调用保活 ⇒ 全表达式临时链。
            cMyTrace(
                __PRETTY_FUNCTION__,
                0x432, 5)("DB_Login::GetPremiumInfo, get(1) ERROR m_id=%u",
                          data->m_accId);
            return -1;
        }
        if (!db->get_int(1, startSec))
        {
            cMyTrace(
                __PRETTY_FUNCTION__,
                0x438, 5)("DB_Login::GetPremiumInfo, get(2) ERROR m_id=%u",
                          data->m_accId);
            return -1;
        }
        if (!db->get_int(2, endSec))
        {
            cMyTrace(
                __PRETTY_FUNCTION__,
                0x43e, 5)("DB_Login::GetPremiumInfo, get(3) ERROR m_id=%u",
                          data->m_accId);
            return -1;
        }
        if (slot > 0x69)
        {
            break;
        }

        // premium 元素基址 = sig + (slot+0x229)*0x10；字段位 [+8 pre_type]
        // [+0xc start][+0x10 end][+0x14 event id]【ABI 镜像】
        // P183：ORIG 每个字段写都重发完整 (slot+0x229)*0x10 地址链（9/8 条/处），
        //   缓存 `prem` 引用会塌成 4 条/处（gap 12 根因）⇒ 逐字段全表达式展开；
        //   写入顺序照 ORIG：m_flag(+0x14) → m_type(+8) → start(+0xc) → end(+0x10)。
        (&data->m_premiums[slot])->m_flag = entry.m_eventId;
        (&data->m_premiums[slot])->m_type = static_cast<unsigned int>(preType);
        (&data->m_premiums[slot])->m_startTime = static_cast<unsigned int>(startSec);
        (&data->m_premiums[slot])->m_endTime = static_cast<unsigned int>(endSec);
        ++slot;

        // 计数 ushort@0x293a（m_restrictedGoodsCount）与 m_id 数组（+0x293c 起 int 表）：
        // ORIG 读 count 一次（%eax）复用索引与 +1 写回（P0 无具名槽）⇒ 后置 ++。
        data->m_restrictedGoods[data->m_restrictedGoodsCount++] = entry.m_eventId;
    }
    return 0;
}
} // namespace DATABASE

// ===========================================================================
// Arad_DB_Charac_Dispatcher（Arad_Character.cpp）
// ===========================================================================

// 各 create_* 共通形态：GetDBHandle(idx) → CREATE_QUERY::arad_sp_xxx。
// ORIG 行号：0x4e/0x67/0x7b/0x95/0xb0/0xbf/0xcd/0xdb/0xe9。

#include "ARAD_Database/dispatch.inc"
{
    (void)param1;
    (void)param2;
    SIG_CREATE_CHARAC* sig = stream->GetOutBuffer<SIG_CREATE_CHARAC>();
    if (sig == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Character.cpp",
            __PRETTY_FUNCTION__,
            0x18, "%s", "Context Null");
        return -1;
    }

    if (create_charac(sig) != 0)
    {
        return -1;
    }
    if (create_charac_stat(sig) != 0)
    {
        return -1;
    }
    if (create_inventory(sig) != 0)
    {
        return -1;
    }
    if (create_equip(sig) != 0)
    {
        return -1;
    }
    if (create_skill(sig) != 0)
    {
        return -1;
    }
    if (create_pvp(sig) != 0)
    {
        return -1;
    }
    if (create_charac_dungeon(sig) != 0)
    {
        return -1;
    }
    if (create_charac_quest(sig) != 0)
    {
        return -1;
    }
    return 0;
}

#include "ARAD_Database/create_charac.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Character.cpp",
            __PRETTY_FUNCTION__,
            0x4e, "%s", "Database null!!");
        return -1;
    }
    if (DATABASE::CREATE_QUERY::arad_sp_create_character(db, sig) != 0)
    {
        return -1;
    }
    if (DATABASE::CREATE_QUERY::arad_sp_get_characer_index(db, sig) != 0)
    {
        return 0x59;                     // ORIG 唯一非 -1 错误码
    }
    return 0;
}

#include "ARAD_Database/create_charac_stat.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Character.cpp",
            __PRETTY_FUNCTION__,
            0x67, "%s", "Database null!!");
        return -1;
    }
    if (DATABASE::CREATE_QUERY::arad_sp_create_charac_stat(db, sig) != 0)
    {
        return -1;
    }
    return 0;
}

#include "ARAD_Database/create_inventory.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Character.cpp",
            __PRETTY_FUNCTION__,
            0x7b, "%s", "Database null!!");
        return -1;
    }
    if (DATABASE::CREATE_QUERY::arad_sp_create_inven(db, sig) != 0)
    {
        return -1;
    }
    if (DATABASE::CREATE_QUERY::arad_sp_create_expand_inven(db, sig) != 0)
    {
        return -1;
    }
    return 0;
}

#include "ARAD_Database/create_equip.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Character.cpp",
            __PRETTY_FUNCTION__,
            0x95, "%s", "Database null!!");
        return -1;
    }
    // ORIG 荒诞哨兵比较照抄：(char*)sig + 0x4e01 == 0（即 sig==0xffffb1ff 时跳过写库）
    if ((char*)sig + 0x4e01 == 0)
    {
        return 0;
    }
    if (DATABASE::CREATE_QUERY::arad_sp_create_equip(db, sig) != 0)
    {
        return -1;
    }
    return 0;
}

#include "ARAD_Database/create_skill.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_3,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Character.cpp",
            __PRETTY_FUNCTION__,
            0xb0, "%s", "Database null!!");
        return -1;
    }
    if (DATABASE::CREATE_QUERY::arad_sp_create_skill(db, sig) != 0)
    {
        return -1;
    }
    return 0;
}

#include "ARAD_Database/create_pvp.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Character.cpp",
            __PRETTY_FUNCTION__,
            0xbf, "%s", "Database null!!");
        return -1;
    }
    if (DATABASE::CREATE_QUERY::arad_sp_create_pvp(db, sig) != 0)
    {
        return -1;
    }
    return 0;
}

#include "ARAD_Database/create_charac_dungeon.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Character.cpp",
            __PRETTY_FUNCTION__,
            0xcd, "%s", "Database null!!");
        return -1;
    }
    if (DATABASE::CREATE_QUERY::arad_sp_create_charac_dungeon(db, sig) != 0)
    {
        return -1;
    }
    return 0;
}

#include "ARAD_Database/create_charac_quest.inc"
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Character.cpp",
            __PRETTY_FUNCTION__,
            0xdb, "%s", "Database null!!");
        return -1;
    }
    if (DATABASE::CREATE_QUERY::arad_sp_create_charac_quest(db, sig) != 0)
    {
        return -1;
    }
    return 0;
}

#include "ARAD_Database/create_charac_npc_relationship.inc"
    SIG_CREATE_CHARAC* sig)
{
    MySQL* db = GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_2,
                                                  static_cast<ENUM_SERVER_GROUP>(0));
    if (db == 0)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_Character.cpp",
            __PRETTY_FUNCTION__,
            0xe9, "%s", "Database null!!");
        return -1;
    }
    if (DATABASE::CREATE_QUERY::arad_sp_create_npc_relationship(db, sig) != 0)
    {
        return -1;
    }
    return 0;
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
{
    int coin = 5;
    CEventBase* event =
        GlobalData::s_event_manager->GetRepeatEvent(5);
    if (event != 0 && event->IsEventing(static_cast<CUser*>(0)))
    {
        coin = reinterpret_cast<CCoinEventOnCharCreate*>(
                   event)->GetCoinNoOnCreateCharac();
    }
    return coin;
}

} // namespace WongWork

// ===========================================================================
// CCoinEventOnCharCreate —— ORIG 0x0810a914..aa9c objdump 全量转写。
// ===========================================================================

#include "ARAD_Database/CCoinEventOnCharCreate.inc"
{
    m_coin = 5;                                      // this+0x0c（CEventBase ctor 之后）
}

#include "ARAD_Database/dtor_CCoinEventOnCharCreate.inc"
{
}                                                    // ORIG 仅恢复 vptr + 调基类析构

#include "ARAD_Database/StartEvent.inc"
{
    SetEventFlag(true);
    m_coin = 5;
    cMyTrace trace(__FUNCTION__, 0x15, 0);           // __FUNCTION__@0x8b4ac44
    trace("Coin Event On Char Create Start!");       // @0x8b4a4a0
}

#include "ARAD_Database/StartEvent_1.inc"
{
    SetEventFlag(true);
    m_coin = static_cast<int>(param.m_duration);     // 低 ushort 直接写入 +0xc
    cMyTrace trace(__FUNCTION__, 0x1c, 0);           // __FUNCTION__@0x8b4ac39
    trace("Coin Event On Char Create Start!");
}

#include "ARAD_Database/EndEvent.inc"
{
    SetEventFlag(false);
    m_coin = 5;
    cMyTrace trace(__FUNCTION__, 0x24, 0);             // __FUNCTION__@0x8b4ac30
    trace("Coin Event On Char Create End!");         // @0x8b4a4c4
}

#include "ARAD_Database/GetCoinNoOnCreateCharac.inc"
{
    return m_coin;
}
