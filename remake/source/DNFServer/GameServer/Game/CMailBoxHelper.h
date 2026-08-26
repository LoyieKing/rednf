#ifndef GAME_CMAILBOXHELPER_H_
#define GAME_CMAILBOXHELPER_H_

// ============================================================================
// WongWork::CMailBox / WongWork::CMailBoxHelper 权威叶子头
//   （自 CGameManager.h 原样提取，2026-08-23；唯一声明点，CGameManager.h 经
//    #include 本头保持对所有既有 TU 的可见性。）
//
// 目的：CAchievement.cpp / CBoosterGage.cpp 等仅需邮箱助手 static 方法的 TU，
//   不再为拿到声明而引入完整 CGameManager.h（其局部最小镜像与权威类存在
//   ODR/布局漂移风险）。依赖仅限：
//     - Inven_Item 完整类型（CMail 内嵌成员，+0x2c，sizeof 0x3d）
//     - ENUM_SERVER_GROUP 完整枚举（ReqDBSendNewSystemMail/MultiMail 按值传参）
//     - StaticPool<T,N> / CUser / PacketGuard / SIG_* / MSG_* 仅前向声明
// ============================================================================

#include <set>
#include <utility>
#include <list>
#include <map>
#include <vector>

#include "Inven_Item.h"     // CMailBox::CMail 内嵌 Inven_Item（完整类型）
#include "CEnvironment.h"   // ENUM_SERVER_GROUP（按值参数）
#include "StaticPool.h"      // 完整模板（g_MailBoxPool 静态成员定义所需）

// 声明所需不完整类型
class CUser;
class PacketGuard;

class Stream;

// ---- ORIG 全局信件明细载荷（SetMailBoxInfo 栈帧推断，约 300 字节；原
//      CGameManager.cpp 局部定义上移至此，letterInfo map 值类型所需） ----
struct SIG_LETTER_INFO
{
    char m_pad[0x12c];
};
struct MSG_CHECK_MAIL_RECEIVER;               // 定义见 CGameManager.h（0x3f88）
struct SIG_MAILBOX_LIST;                      // 定义见 SigTypes.h
struct SIG_MAILBOX_SEND_NEW_MAIL;             // 定义见 SigTypes.h

// 全局占位类型（ORIG 全局符号签名所需；布局待细化）
#ifndef DNF_GAME_ENUM_ERROR_DEFINED
#define DNF_GAME_ENUM_ERROR_DEFINED
enum ENUM_ERROR { ENUM_ERROR_NONE = 0 };
#endif

class MSG_MAILBOX_SEND;  // PacketTypes.h 实定义

namespace WongWork
{
class CMailBox
{
public:
    struct stAddNewMailInput { char m_pad[0x100]; };

    CMailBox();
    ~CMailBox();

    class CMail
    {
    public:
        CMail() {}
        ~CMail() {}

        // ORIG 访问器（nm 权威符号；字段偏移由 CMailBox.cpp 反汇编核对）
        unsigned int GetLetterId();                       // 0x085574d4 (W)
        long GetPeriod() const;                           // 0x085574f2 (W)
        bool IsTimeOver(long nowSec) const;               // 0x085574e0 (W)
        bool isUnlimit() const;                           // 0x08557590 (W)
        const char* GetSenderName() const;                // 0x085574fe (W)
        unsigned int GetPackageMoney() const;             // 0x08557516 (W)
        Inven_Item GetPackageItem() const;                // 0x08557522 (W, sret 按值拷贝 0x3d 字节)
        unsigned int GetSenderCharacNo() const;           // 0x0855750a (W)
        bool isAvatarPackage() const;                     // 0x0855759c (W)
        bool isCreaturePackage() const;                   // 0x085575ae (W)
        void ResetPackage();                              // 0x085519fe (T)
        void MakePacket(PacketGuard* packet, long nowSec,
                        CUser const* user) const;         // 0x08551a36 (T)

    // ORIG 布局（ResetPackage 0x85519fe / MakePacket 0x8551a36 / 访问器反汇编）
    public:
        unsigned int m_mailIndex;       // +0x00（Init 清零；AddNewMail ← input+0x00）
        unsigned int m_senderCharacNo;  // +0x04
        char m_senderName[0x20];        // +0x08（strncpy 0x1d）
        unsigned int m_packageMoney;    // +0x28
        Inven_Item m_item;              // +0x2c..+0x68（sizeof 0x3d）
        char m_pad69[3];
        long m_period;                  // +0x6c（到期秒；GetPeriod/IsTimeOver）
        int m_packageType;              // +0x70（1=avatar / 2=creature / 0=无）
        unsigned char m_unlimit;        // +0x74（isUnlimit）
        unsigned int m_letterId;        // +0x78（GetLetterId；AddNewMail ← input+0x0c）
        unsigned int m_remainSec;       // +0x7c（AddNewMail ← input+0x10）
    };                                  // sizeof 0x80

    CMail* AddNewMail(const stAddNewMailInput&);   // ORIG 0x8551xxx 返回新 CMail*
    void ClearLetterKeepCount();
    void DecLoadedLetterCount();
    int DeleteLetterKeepCount(unsigned int);
    int FindPackageLoadLack(unsigned int);
    int GetLastLoadIdx();
    int GetLastLoadLetterIdx();
    int GetLetterKeepCount();
    int GetLoadedLetterCount();
    CMail* GetMail(unsigned int) const;   // ORIG _ZNK 0x085522c4
    int GetNotLoadedMailCount();
    void GetPackageLoadLack(unsigned int*, unsigned int);   // ORIG 0x08557608
    int GetRecvSize();
    int GetRemainSize();
    void IncNotLoadedMailCount();
    void Init();
    void InsertLetterKeepCount(unsigned int);
    bool IsLoaded();
    int RemoveMail(unsigned int);
    void SetLastLoadLetterIdx(unsigned int);
    void SetLoadState(bool, long);
    void SetLoadedLetterCount(int);
    void SetNotLoadedMailCount(int);
    void SetPackageLoadLack(const unsigned int*, int, std::set<unsigned int>&);
    int getMailLoadCount();
    CMail* getNextMail();
    void incMailLoadCount();
    void reset();
    void setMailIterator();

private:
    // ---- ORIG 真实布局（ctor 0x085579b2 / dtor 0x08557aac / Init 0x08551d98
    //      / getNextMail 0x0855236a 偏移证据；2026-08-26 自 m_pad[0x100] 提升）----
    bool m_bInit;                   // +0x00（Init 判 1 清理后置 0）
    unsigned int m_lastLoadIdx;     // +0x04（GetLastLoadIdx/SetLoadState 写时间）
    int m_notLoadedMailCount;       // +0x08（Get/SetNotLoadedMailCount）
    char m_pad0c[4];                // +0x0c
    StaticPool<CMail, 20> m_mailPool;               // +0x10（sizeof 0x30）
    std::map<unsigned int, CMail*> m_mailMap;       // +0x40
    std::list<Stream*> m_streamList;                // +0x58
    std::map<unsigned int, CMail*>::const_iterator m_mailIterator;  // +0x60
    std::map<int, bool> m_letterKeepCount;          // +0x64（sizeof 0x18）
    unsigned int m_lastLoadLetterIdx;               // +0x7c（Get/SetLastLoadLetterIdx）
    int m_loadedLetterCount;                        // +0x80（Get/SetLoadedLetterCount）
    std::vector<unsigned int> m_loadLackList;       // +0x84
    std::map<unsigned int, SIG_LETTER_INFO> m_letterInfo;   // +0x90
    char m_padTail[0x100 - 0xa8];   // 尾垫至 ORIG 总尺寸 0x100
};

// ---- 布局守 ABI 断言（成员自然偏移依赖下列尺寸；漂移即编译失败） ----
static_assert(sizeof(SIG_LETTER_INFO) == 0x12c, "SIG_LETTER_INFO ABI drift");
static_assert(sizeof(WongWork::CMailBox) == 0x100, "CMailBox ABI drift (ORIG 0x100)");

// SIG_MAILBOX_* 完整定义见 SigTypes.h（全局作用域）
class CMailBoxHelper
{
public:
    static StaticPool<CMailBox, 600> g_MailBoxPool;    // ORIG B 0x0943e000
    // ---- 生命周期 / 池 ----
    static CMailBox* AssignMailBox();                  // ORIG 0x08697838 (W)
    static void      FreeMailBox(CMailBox* box);       // ORIG 0x0869786a (W)
    static bool      IsCeraPointItem(unsigned long);   // ORIG 0x08553912

    // ---- 用户侧取件 ----
    static void GetPackege(CUser* user, unsigned int mailIndex);
    static void ReqDBLoadMail(CUser* user, unsigned int mailIndex);
    static void ReqDBRemoveMail(CUser const* user, unsigned int* mailIdx, bool a);
    static void SetPeriodCheckTimer(CUser* user);
    static unsigned int CheckMailTimeout(CUser* user, PacketGuard* packet); // ORIG 返回删除数
    static void MakeMailBoxListPacket(CUser* user, PacketGuard* packet);
    static void SetMailBoxInfo(CUser* user, SIG_MAILBOX_LIST const* info);

    // ---- 取件内部实现 ----
    static bool _DoGetPackage(CUser* user, CMailBox::CMail const* mail,
                              std::vector<std::pair<int, int> >& out,
                              bool bReset, ENUM_ERROR& err);
    static bool _DoGetPackageCoin(CUser* user, CMailBox::CMail const* mail,
                                  std::vector<std::pair<int, int> >& out,
                                  bool bReset, ENUM_ERROR& err);
    static bool _DoGetPackageItem(CUser* user, CMailBox::CMail const* mail,
                                  std::vector<std::pair<int, int> >& out,
                                  bool bReset, ENUM_ERROR& err);
    static bool _DoGetPackageAvatar(CUser* user, CMailBox::CMail const* mail,
                                    std::vector<std::pair<int, int> >& out,
                                    bool bReset, ENUM_ERROR& err);
    static bool _DoGetPackageCreature(CUser* user, CMailBox::CMail const* mail,
                                      std::vector<std::pair<int, int> >& out,
                                      bool bReset, ENUM_ERROR& err);

    // ---- 收件人校验 ----
    static int ReqDBCheckReceiver(CUser* user, const char* name,
                                  MSG_CHECK_MAIL_RECEIVER* msg);
    static int ReqDBCheckReceiver_forchina(CUser* user, const char* name,
                                           MSG_CHECK_MAIL_RECEIVER* msg);
    static int ReqChangeLetterStat(CUser* user, int a, int b); // ORIG 0x85568a6 返回错误码

    // ---- 发信 ----
    static int ReqDBSendNewMail(CUser* user, Inven_Item const& item,
                                unsigned int nCharacNo, unsigned int nAmount,
                                bool bA, const char* pTitle, int nTitleLen);
    static int ReqDBSendNewMail(CUser* user, MSG_MAILBOX_SEND* msg,
                                unsigned int nCharacNo, bool bA,
                                unsigned int nB, int nC);
    static int ReqDBSendNewMailCashShop(CUser* user, Inven_Item const& item,
                                        unsigned int nCharacNo,
                                        unsigned int nAmount, bool bA,
                                        const char* pTitle, int nTitleLen);
    static int ReqDBSendStoredMail(CUser* user);
    static int _DoSendNewMail(CUser* user, Inven_Item const& item,
                               unsigned int nCharacNo, unsigned int nAmount,
                               bool bA, const char* pTitle, int nTitleLen,
                               int nType);
    static void _makeSendMailData(SIG_MAILBOX_SEND_NEW_MAIL* msg,
                                  const char* pSender, int nSenderLen,
                                  Inven_Item const& item, unsigned int nCharacNo,
                                  unsigned int nAmount, const char* pTitle,
                                  int nTitleLen, unsigned int nB, int nType);
    static int MakeSystemMultiMailPostal(
        std::vector<std::pair<int, int> >& items, Inven_Item* itemBase,
        unsigned int nCount);   // ORIG 0x8556a14 返回成功标志

    // ---- 系统邮件 ----
    static void ReqDBSendNewAvatarMail(CUser* user, int nCharacNo, int nItemIdx,
                                       int nExpireDate, char cA, char cB,
                                       const char* pAgency, const char* pTitle,
                                       int nTitleLen);
    static void ReqDBSendNewAvatarMailCashShop(CUser* user, int nCharacNo,
                                               int nItemIdx, int nExpireDate,
                                               char cA, char cB,
                                               const char* pAgency,
                                               const char* pTitle, int nTitleLen);
    static void ReqDBSendNewCreatureMail(CUser* user, int nCharacNo,
                                         Inven_Item const& item,
                                         const char* pTitle, int nTitleLen);
    static void ReqDBSendNewCreatureMailCashShop(CUser* user, int nCharacNo,
                                                 Inven_Item const& item,
                                                 const char* pTitle,
                                                 int nTitleLen);
    static int ReqDBSendNewSystemMail(const char* pSender, Inven_Item const& item,
                                       unsigned int nA, unsigned int nCharacNo,
                                       const char* pTitle, int nTitleLen,
                                       unsigned int nB,
                                       ENUM_SERVER_GROUP nServerGroup,
                                       bool bA, bool bB);
    static int ReqDBSendNewSystemMultiMail(const char* pSender,
                                           Inven_Item const* itemBase,
                                           unsigned int nCount,
                                           unsigned int nA,
                                           unsigned int nCharacNo,
                                           const char* pTitle, int nTitleLen,
                                           unsigned int nB,
                                           ENUM_SERVER_GROUP nServerGroup,
                                           bool bA);
};
}  // namespace WongWork

#endif  // GAME_CMAILBOXHELPER_H_
