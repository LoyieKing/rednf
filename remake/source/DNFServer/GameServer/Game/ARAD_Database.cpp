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
int snprintf(char* buf, unsigned int n, const char* fmt, ...);   // CInventory.h 声明
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
int CREATE_QUERY::arad_sp_charater_name_check(MySQL* db, SIG_CREATE_CHARAC* sig,
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
int CREATE_QUERY::arad_sp_get_characer_index(MySQL* db, SIG_CREATE_CHARAC* sig)
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
int CREATE_QUERY::arad_sp_create_character_check(MySQL* db, SIG_CREATE_CHARAC* sig)
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
int CREATE_QUERY::arad_sp_create_character(MySQL* db, SIG_CREATE_CHARAC* sig)
{
    if (arad_sp_create_character_check(db, sig) != 0)
    {
        return -1;
    }

    // ORIG：库名 = G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)2].m_db(+0x42)，
    // 该 map 为 private 且无访问器，此处在类外按 ABI 镜像定位 +0x68 后调用 operator[]。
    // ORIG：库名 = G_CEnvironment()->m_dbConnInfo[(ENUM_DB_HANDLE_IDX)2].m_db(+0x42)，
    // 经 get_db_conn_info accessor 具名访问（+0x68）。
    typedef std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo> DbConnInfoMap;
    DbConnInfoMap& connMap = G_CEnvironment()->get_db_conn_info();
    char const* dbName = connMap[static_cast<ENUM_DB_HANDLE_IDX>(2)].m_db;

    if (arad_sp_charater_name_check(db, sig, dbName) != 0)
    {
        return -1;
    }

    char escName[60];                                // ORIG 60 字节清零缓冲
    memset(escName, 0, sizeof(escName));
    db->escape_string(escName, sig->m_name);

    // blob 序列化：spec_property 取 +0x36 起 8 字节、element_resist 取 +0x3e 起 0x22 字节
    //（【ABI 镜像】同落 SigTypes.h 权威头 m_jobData 区间内，具名化待脚本域收口）。
    char* specProperty =
        db->blob_to_str(0, reinterpret_cast<char*>(sig) + 0x36, 8);
    char* elementResist =
        db->blob_to_str(1, reinterpret_cast<char*>(sig) + 0x3e, 0x22);

    // 【ABI 镜像】charac 主属性打包区（+0x26..+0x79 内散列整型，按报告字节宽读取）
    unsigned int maxHP   = LOGIN_FIELD(unsigned int, sig, 0x26);      // %d
    unsigned int maxMP   = LOGIN_FIELD(unsigned int, sig, 0x2a);      // %u
    unsigned int phyAtk  = LOGIN_FIELD(unsigned short, sig, 0x2e);    // %d(zero-extend)
    unsigned int phyDef  = LOGIN_FIELD(unsigned short, sig, 0x30);
    unsigned int magAtk  = LOGIN_FIELD(unsigned short, sig, 0x32);
    unsigned int magDef  = LOGIN_FIELD(unsigned short, sig, 0x34);
    unsigned int invenWt = LOGIN_FIELD(unsigned int, sig, 0x60);
    int hpRegen          = static_cast<short>(LOGIN_FIELD(unsigned short, sig, 0x64));
    int mpRegen          = static_cast<short>(LOGIN_FIELD(unsigned short, sig, 0x66));
    unsigned int moveSpd = LOGIN_FIELD(unsigned int, sig, 0x68);
    unsigned int atkSpd  = LOGIN_FIELD(unsigned short, sig, 0x6c);
    unsigned int castSpd = LOGIN_FIELD(unsigned short, sig, 0x6e);
    int hitRecov         = static_cast<short>(LOGIN_FIELD(unsigned short, sig, 0x70));
    int jump             = static_cast<short>(LOGIN_FIELD(unsigned short, sig, 0x72));
    unsigned int chaWt   = LOGIN_FIELD(unsigned int, sig, 0x74);

    if (db->insert(
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
            maxHP, maxMP, phyAtk, phyDef, magAtk, magDef,
            elementResist, specProperty,
            invenWt, hpRegen, mpRegen, moveSpd, atkSpd, castSpd, hitRecov, jump,
            chaWt, DEFAULT_MAX_FATIGUE,
            static_cast<int>(static_cast<char>(sig->m_pad25))) != 0)
    {
        return -1;
    }
    return 0;
}
// ORIG 0x0818b26d：inSert into charac_stat …… 失败时错误码置 2。
int CREATE_QUERY::arad_sp_create_charac_stat(MySQL* db, SIG_CREATE_CHARAC* sig)
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
int CREATE_QUERY::arad_sp_create_inven(MySQL* db, SIG_CREATE_CHARAC* sig)
{
    if (sig->m_giftFindOk == 0)
    {
        if (db->insert(
                "inSert into inventory  (charac_no, coin, money)  values(%u, %d) ",
                sig->m_characNo) != 0)
        {
            return -1;
        }
        if (WongWork::CEventMgr::GetCoinCharacCreate() != 0)
        {
            return -1;
        }
        return 0;
    }

    static const unsigned int kInvenSrcLen = sourceInvenSize;   // ORIG 背包快照长度
    char zipBuf[0x4152];
    memset(zipBuf, 0, sizeof(zipBuf));
    unsigned int zipLen = 0x4152;
    // ORIG 判定与 equip 相反：compress 返回非 0 即放弃（逐指令转写，勿"修正"）
    if (compress_zip(zipBuf, zipLen,
                     reinterpret_cast<char*>(sig) + 0x3a9, kInvenSrcLen))
    {
        return -1;
    }

    int coin = WongWork::CEventMgr::GetCoinCharacCreate();
    char* invenRaw = db->blob_to_str(0, reinterpret_cast<char*>(sig) + 0x3a9,
                                     kInvenSrcLen);
    char* invenZip = db->blob_to_str(0, zipBuf, static_cast<int>(zipLen));
    if (db->insert("inSert into inventory (charac_no, inventory,coin) "
                   "values(%u, '%s',%d)",
                   sig->m_characNo, invenZip, invenRaw, coin) != 0)
    {
        return -1;
    }
    return 0;
}

// ORIG 0x0818b406：inSert into charac_inven_expand 空记录。
int CREATE_QUERY::arad_sp_create_expand_inven(MySQL* db, SIG_CREATE_CHARAC* sig)
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
int CREATE_QUERY::arad_sp_create_equip(MySQL* db, SIG_CREATE_CHARAC* sig)
{
    static const unsigned int kEquipSrcLen = sourceEquipSize;    // ORIG 初始装备区长度
    char zipBuf[0x333];
    memset(zipBuf, 0, sizeof(zipBuf));
    unsigned int zipLen = 0x333;
    bool compressed = compress_zip(zipBuf, zipLen,
                                   reinterpret_cast<char*>(&sig->m_giftItem),
                                   kEquipSrcLen);   // +0x4e01 初始装备区
    if (!compressed)
    {
        LogManager::logFormat(
            1, "localjapan/Arad_DB_Query.cpp",
            __PRETTY_FUNCTION__,
            0x155, "%s", "equip compress zip fail");
        return -1;
    }

    char* equipZip = db->blob_to_str(0, zipBuf, static_cast<int>(zipLen));
    if (db->update(" upDate inventory set equipslot='%s' where charac_no = %u ",
                   equipZip, sig->m_characNo) != 0)
    {
        return -1;
    }
    return 0;
}

// ORIG 0x0818b588：inSert into skill（+0x79 起 0x198 字节技能树 blob）。
int CREATE_QUERY::arad_sp_create_skill(MySQL* db, SIG_CREATE_CHARAC* sig)
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
int CREATE_QUERY::arad_sp_create_pvp(MySQL* db, SIG_CREATE_CHARAC* sig)
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
int CREATE_QUERY::arad_sp_create_dungeon_point(MySQL*, SIG_CREATE_CHARAC*)
{
    return -1;
}

// ORIG 0x0818b642：inSert into charac_dungeon 空记录。
int CREATE_QUERY::arad_sp_create_charac_dungeon(MySQL* db, SIG_CREATE_CHARAC* sig)
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
int CREATE_QUERY::arad_sp_create_charac_quest(MySQL* db, SIG_CREATE_CHARAC* sig)
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
int CREATE_QUERY::arad_sp_create_npc_relationship(MySQL* db, SIG_CREATE_CHARAC* sig)
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
int LOGIN_QUERY::arad_sp_select_account_info(SIG_LOGIN_DATA* data, char* param_2)
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
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x1f5, 5);
        trace("query get_n_rows() ERROR m_id=%u",
              data->m_accId);
        return 0;
    }

    unsigned int fssn = 0;                    // first_ssn
    unsigned int sssn = 0;                    // second_ssn
    unsigned int regMid = 0;                  // mid(from_unixtime(reg_date))
    unsigned int hangameFlag = 0;
    int col = 1;
    if (!db->get_uint(col, data->m_accId))       // col1
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x201, 5);
        trace("user m_id ERROR m_id=%u",
              data->m_accId);
        return 0;
    }
    if (!db->get_str(++col, param_2, 0x84))                              // col2
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x207, 5);
        trace("user password ERROR m_id=%u",
              data->m_accId);
        return 0;
    }
    if (!db->get_uint(col++, fssn))
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x20d, 5);
        trace("user fssn ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_uint(col++, sssn))
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x213, 5);
        trace("user sssn ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_uint(col++, regMid))
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x219, 5);
        trace("user regdate ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_str(col++, data->m_userDetailIpPort, 0x80))  // user_name（+0x2184）
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x21f, 5);
        trace("user user_name ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_str(col++, data->m_userDetailSsn, 0x80))  // email（+0x2204）
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x225, 5);
        trace("user email ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_str(col++, reinterpret_cast<char*>(data), 0x1e))           // user_id
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x22b, 5);
        trace("user id ERROR m_id=%u", data->m_accId);
        return 0;
    }
    if (!db->get_uint(col++, hangameFlag))                                  // col9
    {
        cMyTrace trace(
            __PRETTY_FUNCTION__,
            0x231, 5);
        trace("user HanGameUser Check ERROR m_id=%u",
              data->m_accId);
        return 0;
    }

    data->m_regDate = regMid;
    data->m_hangameUser =
        static_cast<unsigned char>(hangameFlag == 1 ? 1 : 0);
    OS_API::snprintf(data->m_birth, 0x10, "%d%d",   // +0x2284
                     static_cast<int>(fssn), static_cast<int>(sssn));
    // ORIG 0x0818bcf1：先 fssn/=10000、sssn/=1000000（mul/shr 常量除法），
    // 再 sssn > 2 → year=fssn+2000(0x7d0)，否则 year=fssn+1900(0x76c)。
    fssn = fssn / 10000;
    sssn = sssn / 1000000;
    unsigned int year;
    if (sssn > 2)
    {
        year = fssn + 2000;
    }
    else
    {
        year = fssn + 1900;
    }
    data->m_birthYear = year;
    return 0;
}

// ORIG 0x0818bd67：member_punish_info 行集 → +0x3a54 起 stride 0x18 ×6 字段。
// b21 verdict=DIFF（N-JMP 残余）：调用点集与 ORIG 全等（GetDBHandle/select/logFormat/
//   get_int×3/get_uint×3/get_n_rows）；仅余 4.1 对每 get_* 失败独立 `mov -1; jmp exit`
//   块 vs 4.4 jump-threading 折成共享 return 块（155/150，gap=5）。禁 pun 引用局部
//   （P0：ORIG 右值内联，引局部占槽产生 EXTRA mov）。语义完成，byte 层 N 封顶。
int LOGIN_QUERY::arad_sp_select_punish_user_info(SIG_LOGIN_DATA* data)
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
int LOGIN_QUERY::arad_sp_select_punish_second_user(SIG_LOGIN_DATA* data)
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
int LOGIN_QUERY::arad_sp_convert_pass_pad_info(SIG_LOGIN_DATA* data)
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
int LOGIN_QUERY::arad_sp_delete_member_mouspass(SIG_LOGIN_DATA* data)
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
int LOGIN_QUERY::arad_sp_update_fail_record(SIG_LOGIN_DATA* data)
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
//   （P214③/P253）；残余尾部 `test %eax,%eax; sete %al; test %al,%al; je` 为
//   4.1 的 setcc 物化，4.4 对 `x==0` 恒折直跳或落槽（探针 /tmp/arad_probe/z1/z2
//   六形全败）⇒ setcc-fold N 封顶，语义等价（86/84 条，仅差该 2 条）。
int LOGIN_QUERY::arad_sp_update_history(SIG_LOGIN_DATA* data, int modifyType)
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
    return db->getAffectedRowCount() == 0 ? -1 : 0;
}

// ORIG 0x0818c4f3：按时间窗查 member_mousepass_history 决定 Goblin 奖励标志。
int LOGIN_QUERY::arad_sp_check_goblin_reward(SIG_LOGIN_DATA* data, char* sinceTime)
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
int LOGIN_QUERY::arad_sp_load_event_entry(
    SIG_LOGIN_DATA* data, std::vector<DB_Login::tagDNFEventEntry>& out)
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

    // P0：Ghidra C 直接读 env+0x378（=m_serverEnvir.m_serverGroup），勿经 get_server_group helper
    int serverGroup = G_CEnvironment()->m_serverEnvir.m_serverGroup;
    if (!db->set_query(
            "seLect event_id,unix_timestamp(occ_date),server_id,charac_no,"
            "unix_timestamp(obtain_date) from dnf_event_entry where m_id=%d "
            "and server_id in(0,%d)",
            data->m_accId, serverGroup))
    {
        return -1;
    }

    // P0：Ghidra C 中 entry（local_2c）为函数级局部，循环外声明一次复用（免每轮 dtor）
    DB_Login::tagDNFEventEntry entry;
    for (unsigned int row = 0; row < static_cast<unsigned int>(db->get_n_rows());
         ++row)
    {
        // P0：Ghidra C 无 occDate/serverId/characNo/obtainDate 中间局部 ——
        // 5 个 get_int 直接写 tagDNFEventEntry 各字段（+0/+8/+0xc/+0x10/+0x14）
        if (db->get_int(0, entry.m_eventId) &&
            db->get_int(1, entry.m_occDate) &&
            db->get_int(2, entry.m_serverId) &&
            db->get_int(3, entry.m_characNo) &&
            db->get_int(4, entry.m_obtainDate))
        {
            out.push_back(entry);
        }
    }
    return 0;
}

// ORIG 0x0818ca69：member_broadcast 单行广播信息；到期未过→-1；
// channelType==7 时附 addUserDBInfo。
int LOGIN_QUERY::arad_sp_select_member_brodcast(
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

    unsigned int startTime = 0;
    unsigned int characNo = 0;
    unsigned int serverId = 0;
    DBMgr::stUserDBInfo_t userInfo;
    // P0：Ghidra C 无 memset —— get_uint(0) 直接写 stUserDBInfo_t 首成员
    if (!db->get_uint(0, reinterpret_cast<unsigned int&>(userInfo.m_userDBIdx)) ||
        !db->get_uint(1, characNo) ||
        !db->get_uint(2, serverId) ||
        !db->get_uint(3, startTime))
    {
        return -1;
    }
    if (!db->get_str(4, reinterpret_cast<char*>(data) + 0x39b0, 0x1e))
    {
        return -1;
    }

    if (GlobalData::s_systemTime_.getCurSec() < static_cast<int>(startTime))
    {
        return -1;                                           // 广播尚未生效
    }
    if (G_GameWorld()->GetChannelType() == 7)
    {
        GlobalData::s_db_mgr->addUserDBInfo(userIdx, userInfo);
    }
    // P0：Ghidra C 直接偏移写 —— 0x39ac/0x39a8 对应具名字段（login_field 外联 call 需免）
    data->m_tournamentAccountFlag = characNo;
    data->m_tournamentServerGroup = serverId;
    return 0;
}

// ORIG 0x0818cc74：member_premium 首条生效记录 → SIG premium 表格区。
// b21 verdict=DIFF（N 残余）：202/190。调用点集全等（GetDBHandle/select/get_n_rows×n/fetch/
//   get_int×3/cMyTraceC1+cMyTracecl×3/vector::push_back/LogManager::logFormat）。
//   premium 写位与 restrictedGoods/Count 偏移已逐位核（m_premiums@+0x2298 使字段落
//   +0x2298/+0x229c/+0x22a0/+0x22a4=ORIG +0x2290+{8,c,10,14}）；cMyTrace trace 引用局部
//   占槽 → N82 槽号错位；残余仅帧差(0x78 vs push%ebx+0x74)、槽号、循环退出块
//   (jmp;nop;jmp vs 单 exit)、寄存器分配。语义完成，byte 层 N 封顶。
int LOGIN_QUERY::arad_sp_select_member_premium(
    SIG_LOGIN_DATA* data, DB_Login::tagDNFEventEntry& entry,
    std::vector<int>& removeList, std::vector<int> removeScratch)
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
    for (unsigned int row = 0;
         row < static_cast<unsigned int>(db->get_n_rows()) && db->fetch();
         ++row)
    {
        int preType = 0;
        int startSec = 0;
        int endSec = 0;
        if (!db->get_int(0, preType))
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x432, 5);
            trace("DB_Login::GetPremiumInfo, get(1) ERROR m_id=%u",
                  data->m_accId);
            return -1;
        }
        if (!db->get_int(1, startSec))
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x438, 5);
            trace("DB_Login::GetPremiumInfo, get(2) ERROR m_id=%u",
                  data->m_accId);
            return -1;
        }
        if (!db->get_int(2, endSec))
        {
            cMyTrace trace(
                __PRETTY_FUNCTION__,
                0x43e, 5);
            trace("DB_Login::GetPremiumInfo, get(3) ERROR m_id=%u",
                  data->m_accId);
            return -1;
        }
        if (slot > 0x69)
        {
            break;
        }

        // premium 元素基址 = sig + (slot+0x229)*0x10；字段位 [+8 pre_type]
        // [+0xc start][+0x10 end][+0x14 event id]【ABI 镜像】
        // premium 元素（+0x2290 起 stride 0x10 的窗口，字段对应 m_premiums[slot]）
        SIG_LOGIN_PREMIUM_ENTRY& prem = data->m_premiums[slot];
        prem.m_type = static_cast<unsigned int>(preType);
        prem.m_startTime = static_cast<unsigned int>(startSec);
        prem.m_endTime = static_cast<unsigned int>(endSec);
        prem.m_flag = entry.m_eventId;

        // 计数 ushort@0x293a（m_restrictedGoodsCount）与 m_id 数组（+0x293c 起 int 表）
        unsigned short cnt = data->m_restrictedGoodsCount;
        data->m_restrictedGoods[cnt] = entry.m_eventId;
        data->m_restrictedGoodsCount = static_cast<unsigned short>(cnt + 1);
        ++slot;
    }
    return 0;
}
} // namespace DATABASE

// ===========================================================================
// Arad_DB_Charac_Dispatcher（Arad_Character.cpp）
// ===========================================================================

// 各 create_* 共通形态：GetDBHandle(idx) → CREATE_QUERY::arad_sp_xxx。
// ORIG 行号：0x4e/0x67/0x7b/0x95/0xb0/0xbf/0xcd/0xdb/0xe9。

int ARAD::Arad_DB_Charac_Dispatcher::dispatch(int param1, int param2, Stream* stream)
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

int ARAD::Arad_DB_Charac_Dispatcher::create_charac(SIG_CREATE_CHARAC* sig)
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

int ARAD::Arad_DB_Charac_Dispatcher::create_charac_stat(SIG_CREATE_CHARAC* sig)
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

int ARAD::Arad_DB_Charac_Dispatcher::create_inventory(SIG_CREATE_CHARAC* sig)
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

int ARAD::Arad_DB_Charac_Dispatcher::create_equip(SIG_CREATE_CHARAC* sig)
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

int ARAD::Arad_DB_Charac_Dispatcher::create_skill(SIG_CREATE_CHARAC* sig)
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

int ARAD::Arad_DB_Charac_Dispatcher::create_pvp(SIG_CREATE_CHARAC* sig)
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

int ARAD::Arad_DB_Charac_Dispatcher::create_charac_dungeon(SIG_CREATE_CHARAC* sig)
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

int ARAD::Arad_DB_Charac_Dispatcher::create_charac_quest(SIG_CREATE_CHARAC* sig)
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

int ARAD::Arad_DB_Charac_Dispatcher::create_charac_npc_relationship(
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
// b28d-50 N7：ORIG 0x0848f045 `mov (%eax),%edx; …; call *%edx`（目标 %edx、this
//   %eax），4.4.7 恒目标 %eax、this %edx（间接 call 族，P230补：4.4.6 后端才出
//   %edx 形）。本 TU 未路由 446（改路由表属 Main 权限，禁自改）；源形不可择 ⇒ N7
//   封顶，语义等价（虚槽 +0x34 = IsEventing 已核）。
// ===========================================================================
namespace WongWork
{

int CEventMgr::GetCoinCharacCreate()
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

CCoinEventOnCharCreate::CCoinEventOnCharCreate()     // C1/C2 T 0x0810a914
{
    m_coin = 5;                                      // this+0x0c（CEventBase ctor 之后）
}

CCoinEventOnCharCreate::~CCoinEventOnCharCreate()    // D1/D2 T 0x0810a93a
{
}                                                    // ORIG 仅恢复 vptr + 调基类析构

void CCoinEventOnCharCreate::StartEvent()            // T 0x0810a988
{
    SetEventFlag(true);
    m_coin = 5;
    cMyTrace trace(__FUNCTION__, 0x15, 0);           // __FUNCTION__@0x8b4ac44
    trace("Coin Event On Char Create Start!");       // @0x8b4a4a0
}

void CCoinEventOnCharCreate::StartEvent(Word_Param param)   // T 0x0810a9e4
{
    SetEventFlag(true);
    m_coin = static_cast<int>(param.m_duration);     // 低 ushort 直接写入 +0xc
    cMyTrace trace(__FUNCTION__, 0x1c, 0);           // __FUNCTION__@0x8b4ac39
    trace("Coin Event On Char Create Start!");
}

void CCoinEventOnCharCreate::EndEvent()              // T 0x0810aa42
{
    SetEventFlag(false);
    m_coin = 5;
    cMyTrace trace(__FUNCTION__, 0x24, 0);             // __FUNCTION__@0x8b4ac30
    trace("Coin Event On Char Create End!");         // @0x8b4a4c4
}

int CCoinEventOnCharCreate::GetCoinNoOnCreateCharac()  // W 0x0848f200
{
    return m_coin;
}
