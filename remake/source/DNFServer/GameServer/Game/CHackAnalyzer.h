#ifndef GAME_CHACKANALYZER_H_
#define GAME_CHACKANALYZER_H_

// ============================================================================
// df_game_r 还原 —— WongWork::CHackAnalyzer / Secu_AccountHacking 权威声明点
// （2026-08-27 自 CUser.h 原地整体迁出，Main 方案 B 放行；字段偏移原样搬运，
//   未增删任何布局字节。）
//
// 布局依据（ORIG nm -S --defined-only df_game_r）：
//   WongWork::CHackAnalyzer ctor 0x086950d2 / resetHackInfo 0x080f81a6
//     总尺寸 0x7a3c；CUser 子对象 @+0x71bac。
//   Secu_AccountHacking C1/D1 0x08695aa4/aaa（空体）；CUser 子对象 @+0x795e8，
//     下一子对象 CodeHackCheckStorage @+0x795fc → sizeof 0x14。
// ============================================================================

#include <map>
#include <vector>

class CUser;
class PacketBuf;
struct MSG_CHECK_MAIL_RECEIVER;

namespace WongWork
{

// ---- ENUM_HACKTYPE（自 CUser.h 迁入；拼写/mangling 由 NS_13ENUM_HACKTYPEE 锁定）----
enum ENUM_HACKTYPE { ENUM_HACKTYPE_0 = 0 };
typedef ENUM_HACKTYPE ENUM_HACK_TYPE;

}  // namespace WongWork

// ============================================================================
// ENUM_REPORT_4_HACK_FLAG —— analyzeHackReport(0x080f7a22) 位测试证据：
//   flags & 0x01 speed / & 0x02 difficulty / & 0x04 equipment-upgrade /
//   & 0x08 memory / & 0x10 (byte payload) / & 0x20 / & 0x40 incurrect /
//   & 0x80 monster-die。位值为 ORIG 事实；成员名为重建推断。
// ORIG mangled：E23ENUM_REPORT_4_HACK_FLAG（printf|wc -c 实测 23）。
// ============================================================================
enum ENUM_REPORT_4_HACK_FLAG
{
    REPORT_4_HACK_FLAG_NONE              = 0x000,
    REPORT_4_HACK_FLAG_SPEED             = 0x001,
    REPORT_4_HACK_FLAG_DIFFICULTY        = 0x002,
    REPORT_4_HACK_FLAG_EQUIPMENT_UPGRADE = 0x004,
    REPORT_4_HACK_FLAG_MEMORY            = 0x008,
    REPORT_4_HACK_FLAG_PAYLOAD_BYTE      = 0x010,  // 推断：analyzeHackReport +0x10 位
    REPORT_4_HACK_FLAG_MISC              = 0x020,  // 推断：+0x20 位
    REPORT_4_HACK_FLAG_INCURRECT         = 0x040,
    REPORT_4_HACK_FLAG_MONSTER_DIE       = 0x080
};

// ============================================================================
// AccountHackingType::T —— ORIG mangled N18AccountHackingType1TE
// （"AccountHackingType"=18 实测）。值域 0..5 为事实
// （getMatchingHacktype 0x0827912a 线性比较链 / setPatten byte 索引范围；
//   0..3 金币检测、4..5 avatar 检测，isGoldPatten/isAvatarPatten 分支证据），
// 成员名为重建推断。
// ============================================================================
struct AccountHackingType
{
    enum T
    {
        GOLD_0 = 0,
        GOLD_1 = 1,
        GOLD_2 = 2,
        GOLD_3 = 3,
        AVATAR_COUNT = 4,
        AVATAR_TIME = 5
    };
};

// ============================================================================
// Secu_AccountHacking —— CUser 子对象 +0x795e8（sizeof 0x14）
// 布局（resetInfo 0x08278ab4 / init 0x08278a98 / setLoginTime 0x08279020 /
// sendAvatar 0x08278f16 反汇编实证）：
//   +0x00..0x05 六个检测标志字节（resetInfo memset 0x6；setPatten 按
//     AccountHackingType 索引写 1；isDetectionGoldPatten 读 [0..3]）
//   +0x08 avatar 发送计数（sendAvatar 自增，<8 阈值判定）
//   +0x0c 登录秒（setLoginTime = GlobalData::s_systemTime_.getCurSec()）
//   +0x10 绑定用户指针（init 写入）
// ============================================================================
class Secu_AccountHacking
{
public:
    Secu_AccountHacking();                 // ORIG W 0x08695aa4（空体）
    ~Secu_AccountHacking();                // ORIG W 0x08695aaa（空体）

    void init(CUser* user);                // ORIG T 0x08278a98
    void resetInfo();                      // ORIG T 0x08278ab4
    void setLoginTime();                   // ORIG T 0x08279020
    bool isAvatar(unsigned int itemIdx);   // ORIG T 0x08278aec
    void sendGold(char const* name, unsigned int a, unsigned int money,
                  unsigned int b, unsigned char trackType, char* ip);
                                           // ORIG T 0x08278cce
    void sendMail(char const* name, MSG_CHECK_MAIL_RECEIVER* msg);
                                           // ORIG T 0x08278b48
    bool isSameAccountCharacter(char const* name);  // ORIG T 0x08279182
    WongWork::ENUM_HACKTYPE getMatchingHacktype(AccountHackingType::T t);
                                           // ORIG W 0x0827912a
    void setPatten(AccountHackingType::T t, char const* name, unsigned int a,
                   unsigned int b);        // ORIG T 0x0827903c
    bool isGoldPatten(AccountHackingType::T t);      // ORIG W 0x08279258
    bool isAvatarPatten(AccountHackingType::T t);    // ORIG W 0x08279282
    bool isDetectionGoldPatten();                    // ORIG W 0x082792a0
    void sendAvatar(char const* name, unsigned int a);  // ORIG T 0x08278f16
    static bool checkAccountHackingHackType(WongWork::ENUM_HACKTYPE type);
                                           // ORIG T 0x08278a48

    unsigned char m_flag[6];               // +0x00（六种检测标志，resetInfo 清 6 字节）
    unsigned int m_sendCnt;                // +0x08（sendAvatar 自增计数；ORIG 无符号比较 jbe）
    int m_loginTime;                       // +0x0c（setLoginTime 写当前秒）
    CUser* m_userP;                        // +0x10（init 写入）
};

// typedef char[sizeof...] 断言放在文件尾（sizeof 需完整类型，此处已完整）

namespace WongWork
{

// stHackInfo_t —— 全局（ORIG mangled _ZN8WongWork12stHackInfo_tE；D1/D2 W
// @0x080fb5c8，空析构）。reportHackInfo/_addHackInfo 读取视图：
// {accId, curCharacNo, hackType(u16), value(int)}，sizeof 0x10。
// 权威声明点（2026-08-30 自 WongWorkEvents_A.h 迁入）；CHackAnalyzer 成员
// m_hackInfoList 直接使用。原类内嵌套视图已删除——ORIG ctor 构造的是全局
// vector<WongWork::stHackInfo_t>（SUB 差异根因：嵌套 mangling 不匹配）。
// ============================================================================
struct stHackInfo_t
{
    unsigned int   m_uid;        // +0x00（acc_id）
    unsigned int   m_characNo;   // +0x04（curCharacNo）
    unsigned short m_hackType;   // +0x08（hack type）
    unsigned short m_pad0a;
    int            m_value;      // +0x0c（value）

    ~stHackInfo_t();             // inline 定义见下，产生 W D1/D2 弱符号
};

inline stHackInfo_t::~stHackInfo_t() {}

// ============================================================================
// WongWork::CHackAnalyzer —— CUser 子对象 +0x71bac（总尺寸 0x7a3c）
class CHackAnalyzer
{
public:
    CHackAnalyzer();
    ~CHackAnalyzer();

    void addServerHackCnt(CUser* user, ENUM_HACKTYPE type, unsigned int count,
                          unsigned int a, unsigned int b);   // ORIG 0x080f8c7e
    void resetHackInfo();                    // ORIG 0x080f81a6
    void resetServerHackAccumulatedCnt(ENUM_HACKTYPE type);  // ORIG W 0x086950b6
    int getServerHackAccumulatedCnt(ENUM_HACKTYPE type) const;  // ORIG W 0x0822eef6（读 B 表）
    int getServerHackCnt(ENUM_HACKTYPE type);          // ORIG W 0x08695086
    void resetServerHackCnt(ENUM_HACKTYPE type);       // ORIG W 0x0869509c
    void setServerHackAccumulatedCnt(ENUM_HACKTYPE type, unsigned int cnt);
                                                       // ORIG W 0x084eb57c
    void reportHackInfo();                   // ORIG 0x080f8080
    void beginCollectHackInfo(CUser* user);  // ORIG 0x080f79e8
    void setLastMonsterDeadTime(long t);     // ORIG W 0x085df98c

    void setUserP(CUser* user);              // ORIG T 0x080f79c0
    CUser* getUserP();                       // ORIG T 0x080f79ce
    bool checkCollectedHack(ENUM_REPORT_4_HACK_FLAG flag);  // ORIG W 0x080f9876
    void _addHackInfo(ENUM_REPORT_4_HACK_FLAG flag, ENUM_HACKTYPE type, int v);
                                             // ORIG T 0x080f7fd2
    int analyzeHackReport(CUser* user, PacketBuf* pkt);     // ORIG T 0x080f7a22
    void _analyzeSpeedHack(unsigned int ms);           // ORIG T 0x080f7cca
    void _analyzeDifficultyHack(char level);           // ORIG T 0x080f7d82
    void _analyzeMemoryHack(unsigned int v);           // ORIG T 0x080f7dce
    void _analyzeEquipmentUpgradeHack(unsigned short const* upgradeTbl);
                                             // ORIG T 0x080f7e18
    void _analyzeMonsterDieHack();                     // ORIG T 0x080f7eb6
    void analyzeMonsterDieHack();                      // ORIG W 0x080f7cb6
    void _reportIncurrectReport();                     // ORIG W 0x080f8032
    void ValidateFixMobDiePos(CUser* user, unsigned short mobId,
                              unsigned short x, unsigned short y);
    void onUpdateHackCount(CUser* user, ENUM_HACKTYPE type, int a,
                           unsigned int b, unsigned int c);   // ORIG T 0x080f8c4e（Eijj：第3参 int）

    ENUM_HACKTYPE getBaseHackType();         // ORIG W 0x080f9868
    void setBaseHackType(ENUM_HACKTYPE t);   // ORIG W 0x080f9856
    bool CheckExceptionCnt(ENUM_HACKTYPE type, int cnt);          // 0x080f81fc
    bool CheckHackRateByPartyCount(ENUM_HACKTYPE type, int cnt);  // 0x080f8320
    bool CheckHackRateByDifficult(ENUM_HACKTYPE type, int lvDiff,
                                  int difficult);                 // 0x080f842e
    static void ReqDBSaveHackInfo(unsigned int uid, unsigned int type,
                                  unsigned short cnt, int a, int b);  // 0x080f8f62
    static void ReqDBSaveHackUserPunish(unsigned int accId, unsigned int punish);
                                             // ORIG T 0x080f906a
    void AddCheckRateHackTypeList();         // ORIG 0x080f8550

    // ---- ORIG 布局（ctor 0x86950d2 / resetHackInfo 反汇编）总 0x7a3c ----
    int m_baseHackType;                                   // +0x00（槽复用：userP）
    unsigned char m_flag04;                               // +0x04（collect 标志位图）
    char m_pad05[3];                                      // +0x05
    std::vector<stHackInfo_t> m_hackInfoList;             // +0x08
    std::vector<int> m_checkTypes;                        // +0x14（resetHackInfo clear）
    int m_field20;                                        // +0x20（收集起始秒）
    int m_field24;                                        // +0x24（采集结束秒）
    char m_cntRegion[0x7a08 - 0x28];                      // +0x28 计数区（ctor memset 段）
    int m_field7a08;                                      // +0x7a08
    std::map<int, int> m_exceptionCntMap;                 // +0x7a0c
    std::vector<int> m_checkRateHackTypes;                // +0x7a24（AddCheckRateHackTypeList 填充）
    std::vector<int> m_vec7a30;                           // +0x7a30
};

}  // namespace WongWork

// stGetHackTypeOperator<unsigned short,2>::getHackType(PacketBuf*,unsigned short&)
// ORIG W 0x080f9b82（模板实例弱符号真实存在）；仓库唯一声明点。
template <typename T, int N>
struct stGetHackTypeOperator
{
    static bool getHackType(PacketBuf* pkt, T& out);
};

typedef char _assert_secu_accounthacking_size[
    (sizeof(Secu_AccountHacking) == 0x14) ? 1 : -1];
typedef char _assert_chackanalyzer_size[(sizeof(WongWork::CHackAnalyzer) == 0x7a3c) ? 1 : -1];

#endif  // GAME_CHACKANALYZER_H_
