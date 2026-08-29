// ============================================================================
// df_game_r 还原 —— Taiwan Query/Garena/Sig 族实现（Wave5 仅ORIG清零批）。
// 函数级依据见 TaiwanQueries.h 头注释（docs/class_func_reports/Taiwan__*
// 报告反编译 C 与 df_game_r objdump 逐指令核对）；SQL 格式串一律原样保留。
// ============================================================================

#include "TaiwanQueries.h"

#include <ctype.h>
#include <string.h>
#include <time.h>

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "LogManager.h"
#include "MsgQueueMgr.h"
#include "MySQL.h"
#include "TaiwanCash.h"        // BillTransaction 权威布局
#include "DNFFunctionLib.h"    // StringToNumber

// 说明：ORIG GarenaAuthPCRoom 只取 CUser::GetUID()/get_acc_id() 两个 const
// 访问器。因上游 CUser.h→CDataManager.h→CItemGenerator.h 链正被其它批改造
// （2026-08-27 波动期），此处采用【最小只读类视图】生成对真实 ORIG 符号
// _ZNK5CUser6GetUIDEv/_ZNK5CUser10get_acc_idEv 的常规成员调用（ABI 即
// const thiscall、this-only，指针传递不变）；调用语义与签名以 CUser.h 权威
// 声明为准，Wave5 收尾可直接换回 #include "CUser.h"。
class CUser
{
public:
    unsigned int GetUID() const;
    unsigned int get_acc_id() const;
};

// s_msgq_mgr / s_stream_pool 全局实例见 GlobalData.h（namespace GlobalData）。

// ---------------------------------------------------------------------------
// GetAppFolder —— ORIG W 0x0816c715（_Z12GetAppFolderv，COpensslRSA TU 内全局
// 函数，由 sret 返回空串 "\"\"" 字面量 @0x08b81360）。重建以空串最小实现满足
// Init 拼路径语义；真实路径来源（注册表/配置）缺省时返回空。
// ---------------------------------------------------------------------------
std::string GetAppFolder()
{
    return std::string("");
}

// ---------------------------------------------------------------------------
// COpensslRSA —— OpenSSL RSA 公钥包装（全局命名空间，ORIG mangled _ZN11COpensslRSA*）
// ---------------------------------------------------------------------------

// ORIG W 0x0816c238
COpensslRSA::COpensslRSA() : m_rsa(0) {}

// ORIG W 0x0816c246：key 非空 RSA_free 后置空。
extern "C" void RSA_free(void* rsa);

COpensslRSA::~COpensslRSA()
{
    if (m_rsa != 0)
    {
        RSA_free(m_rsa);
        m_rsa = 0;
    }
}

// ORIG T 0x0816c26e：Init("publickey.pem")。
bool COpensslRSA::Init()
{
    return Init("publickey.pem");
}

// ORIG T 0x0816c28a：path 空 → false；BIO_new(BIO_s_file()) →
// BIO_ctrl(bio,0x6c,3,(GetAppFolder()+path).c_str())（BIO_C_SET_FILENAME）→
// PEM_read_bio_RSA_PUBKEY(bio,0,0,0) 存 m_rsa → BIO_free_all → key 非空真。
bool COpensslRSA::Init(const char* path)
{
    if (path == 0)
        return false;
    void* bio = BIO_new(BIO_s_file());
    std::string full = GetAppFolder();
    std::string rel(full + path);
    BIO_ctrl(bio, 0x6c, 3, const_cast<char*>(rel.c_str()));
    m_rsa = PEM_read_bio_RSA_PUBKEY(bio, 0, 0, 0);
    BIO_free_all(bio);
    if (m_rsa == 0)
        return false;
    return true;
}

// ORIG T 0x0816c532：rsaSize=RSA_size(key)；pos=0 起按块
// min(len-pos, rsaSize) RSA_public_decrypt(...,RSA_PKCS1_PADDING=1)，块失败
// 即整体失败；成功块 insert 到 out 尾部，直至 pos>=len。
bool COpensslRSA::Decrypt(const char* in, int len, std::vector<unsigned char>& out)
{
    int rsa_size = RSA_size(m_rsa);
    int pos = 0;
    while (true)
    {
        int chunk = len - pos;
        if (chunk > rsa_size)
            chunk = rsa_size;
        if (!(chunk > 0))
            break;
        std::vector<unsigned char> block;
        block.resize(static_cast<size_t>(rsa_size), 0);
        int written = RSA_public_decrypt(chunk,
                                         reinterpret_cast<const unsigned char*>(in) + pos,
                                         &block[0], m_rsa, 1);
        if (written <= 0)
        {
            return false;
        }
        out.insert(out.end(), block.begin(), block.begin() + written);
        pos += chunk;
    }
    return true;
}

// ORIG T 0x0816c3a2：rsaSize=RSA_size(key)；maxChunk=rsaSize-0xc；pos=0 起按块
// min(len-pos, maxChunk) RSA_private_encrypt(...,RSA_PKCS1_PADDING=1)（块长上限
// rsaSize-12 预留 PKCS#1 填充），块失败即整体失败；成功块 insert 到 out 尾部，
// 剩余不足一块时返回 true。
bool COpensslRSA::Encrypt(const char* in, int len, std::vector<unsigned char>& out)
{
    int rsa_size = RSA_size(m_rsa);
    int max_chunk = rsa_size - 0xc;
    int pos = 0;
    while (true)
    {
        int chunk = len - pos;
        if (chunk > max_chunk)
            chunk = max_chunk;
        if (chunk < 1)
            return true;
        std::vector<unsigned char> block;
        block.resize(static_cast<size_t>(rsa_size), 0);
        int written = RSA_private_encrypt(chunk,
                                          reinterpret_cast<const unsigned char*>(in) + pos,
                                          &block[0], m_rsa, 1);
        if (written < 1)
        {
            return false;
        }
        out.insert(out.end(), block.begin(), block.begin() + written);
        pos += chunk;
    }
    return true;
}

namespace Taiwan {

// ---------------------------------------------------------------------------
// base64（std::string/std::vector 语义版）
// ORIG：_Z13base64_decodeRKSsRSt6vectorIhSaIhEE T 0x0807ebf4 /
//       _Z13base64_encodePKhjRSs T 0x0807ea2e（静态链 libcrypto TU 内实现，
//       表 _ZL12base64_chars B 0x093f56f4 由静态构造填充）。按 ORIG 反汇编
//       逐分支还原；解密哈希类按用户规则以算法正确为准。
// ---------------------------------------------------------------------------
static const char k_base64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static bool is_base64(unsigned char c)
{
    return isalnum(c) || (c == '+') || (c == '/');
}

void base64_decode(std::string const& encoded_string,
                   std::vector<unsigned char>& out)
{
    // ORIG 循环：每收满 4 字符查表组 3 字节 push_back（i<in_len 判界，
    // 不足处以 0 补齐第 4 路，与 0x807ed8d 循环一致）。
    int in_len = static_cast<int>(encoded_string.size());
    int i = 0;
    int in_ = 0;
    unsigned char char_array_4[4];
    unsigned char char_array_3[3];

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
    {
        char_array_4[i++] = encoded_string[in_];
        in_++;
        if (i == 4)
        {
            for (i = 0; i < 4; i++)
                char_array_4[i] = static_cast<unsigned char>(
                    static_cast<const char*>(strchr(k_base64_chars, char_array_4[i]))
                        ? strchr(k_base64_chars, char_array_4[i]) - k_base64_chars
                        : 0);
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            for (i = 0; (i < 3); i++)
                out.push_back(char_array_3[i]);
            i = 0;
        }
    }
}

// ---------------------------------------------------------------------------
// GarenaAuthData getter/setter（读侧网络序转换与 ORIG ntohl/ntohs 调用一致）
// ---------------------------------------------------------------------------

// ORIG T 0x08172fc8
unsigned int GarenaAuthData::getUid()
{
    extern unsigned long ntohl(unsigned long);
    return static_cast<unsigned int>(ntohl(m_uid));
}

// ORIG T 0x08172fde
char* GarenaAuthData::getUname()
{
    return m_uname;
}

// ORIG T 0x08173008
unsigned short GarenaAuthData::getGcaType()
{
    extern unsigned short ntohs(unsigned short);
    return ntohs(m_gcaType);
}

// ORIG T 0x08173022
bool GarenaAuthData::getMobileAuth()
{
    return m_mobileAuth == 1;
}

// ORIG T 0x0817304e
unsigned char GarenaAuthData::getGoldMemberType()
{
    return m_goldMemberType;
}

// ORIG T 0x08172fea
unsigned int GarenaAuthData::getTimeStamp()
{
    extern unsigned long ntohl(unsigned long);
    return static_cast<unsigned int>(ntohl(m_timeStamp)) + m_patchTime;
}

// ORIG T 0x08173042
unsigned int GarenaAuthData::getPatchTime()
{
    return m_patchTime;
}

// ORIG T 0x08173034
void GarenaAuthData::setPatchTime(unsigned int t)
{
    m_patchTime = t;
}

// ---------------------------------------------------------------------------
// AuthGarenaToken
// ---------------------------------------------------------------------------

// ORIG B 0x09404754（静态对象；本体由 __static_initialization 构造）
COpensslRSA AuthGarenaToken::openSSLRSA_;

// ORIG T 0x081730c2
void AuthGarenaToken::init()
{
    openSSLRSA_.Init();
}

// ORIG T 0x081730d6：param==""（operator== 成立）走默认公钥，否则指定路径；
// 返回对应 Init 的布尔结果。
bool AuthGarenaToken::init(const char* path)
{
    std::string empty("");
    std::string arg(path == 0 ? "" : path);
    if (arg == empty)
        return openSSLRSA_.Init();
    return openSSLRSA_.Init(path);
}

// ORIG T 0x081731f2
int AuthGarenaToken::decrypt(const char* token, int len, GarenaAuthData& out)
{
    if (token == 0)
    {
        cMyTrace trace(__FUNCTION__, 0x7a, 5);
        trace("[Taiwan, Auth] Token is null!");
        return 0x7b;
    }

    std::vector<unsigned char> encrypted;
    base64_decode(std::string(token), encrypted);
    if (encrypted.size() == 0)
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanAuth.cpp", __FUNCTION__, 0x83,
            "[Taiwan, Auth] Invalid token format!");
        return 0x84;
    }
    std::vector<unsigned char> decrypted;
    bool ok = openSSLRSA_.Decrypt(
        reinterpret_cast<const char*>(encrypted.size() ? &encrypted[0] : 0),
        static_cast<int>(encrypted.size()), decrypted);
    if (!ok)
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanAuth.cpp", __FUNCTION__, 0x8b,
            "[Taiwan, Auth] Decrypt token failed!");
        return 0x8c;
    }

    if (decrypted.size() >= 0x69)
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanAuth.cpp", __FUNCTION__, 0x92,
            "[Taiwan, Auth] Invalid decrypted token length! "
            "(buffer_size:%u, token_size:%u)",
            0x68, static_cast<unsigned int>(decrypted.size()));
        return 0x93;
    }

    memset(&out, 0, 0x68);
    memcpy(&out, decrypted.size() ? &decrypted[0] : 0,
           decrypted.size() - 1);

    bool mobile = out.getMobileAuth();
    unsigned int gold = out.getGoldMemberType();
    unsigned short gca = out.getGcaType();
    unsigned int stamp = out.getTimeStamp();
    char* uname = out.getUname();
    unsigned int uid = out.getUid();

    cMyTrace trace(__FUNCTION__, 0x99, 0);
    trace("[Taiwan, Auth] Success!! Uid:%u, UserName:%s, Timestamp:%u, "
          "GcaType:%u, GoldMemberType:%u, Mobile:%u",
          uid, uname, stamp, gca & 0xffff, gold & 0xff,
          static_cast<unsigned int>(mobile));
    return 0;
}

// ---------------------------------------------------------------------------
// usp Query 族
// ---------------------------------------------------------------------------

// ---- EntranceQuery --------------------------------------------------------

// ORIG T 0x08173ffa（C2 同址别名）
EntranceQuery::EntranceQuery() : m_field0(0), m_resultCode(0) {}

bool EntranceQuery::setQuery(MySQL&) { return true; }     // ORIG 0x08174012
bool EntranceQuery::execQuery(MySQL&) { return true; }    // ORIG 0x0817401c
int EntranceQuery::getResult(MySQL&) { return 1; }        // ORIG 0x08174026

// ---- BalanceQuery ---------------------------------------------------------

// ORIG T 0x08174030（C2 同址别名）
BalanceQuery::BalanceQuery(const char* account)
    : m_account(account), m_balance(0), m_resultCode(0) {}

// ORIG T 0x08174052
bool BalanceQuery::setQuery(MySQL& db)
{
    return db.set_query("call usp_balance('%s', @out_balance, @out_result)",
                        m_account);
}

// ORIG T 0x08174076
bool BalanceQuery::execQuery(MySQL& db)
{
    return db.exec(true);
}

// ORIG T 0x08174092
int BalanceQuery::getResult(MySQL& db)
{
    try
    {
        if (!db.set_query("select @out_balance, @out_result"))
            throw static_cast<int>(0x116);
        if (!db.exec(true))
            throw static_cast<int>(0x119);
        if (!db.fetch())
            throw static_cast<int>(0x11c);
        if (!db.get_int(0, m_balance))
            throw static_cast<int>(0x120);
        if (!db.get_int(1, m_resultCode))
            throw static_cast<int>(0x123);
        return 1;
    }
    catch (const int& e)
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanCash.cpp", __FUNCTION__, 0x129,
            "[Taiwan, Billing] BalanceQuery Error Line:%u\n",
            static_cast<unsigned int>(e));
        return 0;
    }
}

// ---- BalancePointQuery ----------------------------------------------------

// ORIG T 0x08174290（C2 同址别名）
BalancePointQuery::BalancePointQuery(const char* account)
    : m_account(account), m_balance(0), m_point(0), m_resultCode(0) {}

// ORIG T 0x081742bc
bool BalancePointQuery::setQuery(MySQL& db)
{
    return db.set_query(
        "call usp_balance_point('%s', @out_balance, @out_point, @out_result)",
        m_account);
}

bool BalancePointQuery::execQuery(MySQL& db)
{
    return db.exec(true);
}

// ORIG T 0x081742fc
int BalancePointQuery::getResult(MySQL& db)
{
    try
    {
        if (!db.set_query("select @out_balance, @out_point, @out_result"))
            throw static_cast<int>(0x14b);
        if (!db.exec(true))
            throw static_cast<int>(0x14e);
        if (!db.fetch())
            throw static_cast<int>(0x151);
        if (!db.get_int(0, m_balance))
            throw static_cast<int>(0x155);
        if (!db.get_int(1, m_point))
            throw static_cast<int>(0x158);
        if (!db.get_int(2, m_resultCode))
            throw static_cast<int>(0x15b);
        return 1;
    }
    catch (const int& e)
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanCash.cpp", __FUNCTION__, 0x161,
            "[Taiwan, Billing] BalanceQuery Error Line:%u\n",
            static_cast<unsigned int>(e));
        return 0;
    }
}

// ---- GiftQuery ------------------------------------------------------------

// ORIG T 0x081747fe（C2 同址别名）
GiftQuery::GiftQuery(const char* account, const char* sendCharac,
                     const char* recvCharac, unsigned int price,
                     unsigned int pay, long long uid)
    : m_account(account), m_sendCharac(sendCharac), m_recvCharac(recvCharac),
      m_uid(uid), m_price(price), m_pay(pay), m_resultCode(0)
{
    m_trans.init();
}

// ORIG T 0x08174866
bool GiftQuery::setQuery(MySQL& db)
{
    return db.set_query(
        "call usp_gift('%s', '%s', '%s', %u, %u, @out_tran, @out_code)",
        m_account, m_sendCharac, m_recvCharac, m_price, m_pay);
}

bool GiftQuery::execQuery(MySQL& db)
{
    return db.exec(true);
}

// ORIG T 0x081748d6
int GiftQuery::getResult(MySQL& db)
{
    try
    {
        if (!db.set_query("select @out_tran, @out_code"))
            throw static_cast<int>(0x1bf);
        if (!db.exec(true))
            throw static_cast<int>(0x1c2);
        if (!db.fetch())
            throw static_cast<int>(0x1c5);
        if (!db.get_ulonglong(0, m_trans.m_tranNo))
            throw static_cast<int>(0x1c8);
        if (!db.get_int(1, m_resultCode))
            throw static_cast<int>(0x1cb);
        return 1;
    }
    catch (const int& e)
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanCash.cpp", __FUNCTION__, 0x1d1,
            "[Taiwan, Billing] GiftQuery Error Line:%u\n",
            static_cast<unsigned int>(e));
        return 0;
    }
}

// ---- ConfirmQuery ---------------------------------------------------------
// ORIG T 0x08174ac6（C2 同址别名）：trans 前 12 字节逐 dword 拷贝进 m_trans。
ConfirmQuery::ConfirmQuery(const BillTransaction& trans)
    : m_resultCode(0)
{
    memcpy(&m_trans, &trans, sizeof(BillTransaction));
}

// ORIG T 0x08174aec
bool ConfirmQuery::setQuery(MySQL& db)
{
    return db.set_query("call usp_confirm(%s, @out_code)",
                        NumberToString(m_trans.m_tranNo, 0));
}

bool ConfirmQuery::execQuery(MySQL& db)
{
    return db.exec(true);
}

// ORIG T 0x08174b44
int ConfirmQuery::getResult(MySQL& db)
{
    try
    {
        if (!db.set_query("select @out_code"))
            throw static_cast<int>(0x1ee);
        if (!db.exec(true))
            throw static_cast<int>(0x1f1);
        if (!db.fetch())
            throw static_cast<int>(0x1f4);
        if (!db.get_int(0, m_resultCode))
            throw static_cast<int>(0x1f7);
        return 1;
    }
    catch (const int& e)
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanCash.cpp", __FUNCTION__, 0x1fd,
            "[Taiwan, Billing] ConfirmQuery Error Line:%u\n",
            static_cast<unsigned int>(e));
        return 0;
    }
}

// ---- CancelQuery ----------------------------------------------------------

// ORIG T 0x08174ce2（C2 同址别名）
CancelQuery::CancelQuery(const BillTransaction& trans)
    : m_resultCode(0)
{
    memcpy(&m_trans, &trans, sizeof(BillTransaction));
}

// ORIG T 0x08174d08
bool CancelQuery::setQuery(MySQL& db)
{
    return db.set_query("call usp_cancel(%s, @out_code)",
                        NumberToString(m_trans.m_tranNo, 0));
}

bool CancelQuery::execQuery(MySQL& db)
{
    return db.exec(true);
}

// ORIG T 0x08174d60
int CancelQuery::getResult(MySQL& db)
{
    try
    {
        if (!db.set_query("select @out_code"))
            throw static_cast<int>(0x21a);
        if (!db.exec(true))
            throw static_cast<int>(0x21d);
        if (!db.fetch())
            throw static_cast<int>(0x220);
        if (!db.get_int(0, m_resultCode))
            throw static_cast<int>(0x223);
        return 1;
    }
    catch (const int& e)
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanCash.cpp", __FUNCTION__, 0x229,
            "[Taiwan, Billing] CancelQuery Error Line:%u\n",
            static_cast<unsigned int>(e));
        return 0;
    }
}

// ---- ChargeQuery ----------------------------------------------------------

// ORIG T 0x08174efe（C2 同址别名）
ChargeQuery::ChargeQuery(const char* account, const char* p2, const char* p3,
                         unsigned int price, char chargeType)
    : m_account(account), m_p2(p2), m_p3(p3), m_price(price),
      m_chargeType(chargeType), m_resultCode(0)
{
    m_trans.init();
}

// ORIG T 0x08174f52：%d 打印有符号 chargeType。
bool ChargeQuery::setQuery(MySQL& db)
{
    return db.set_query(
        "call usp_recharge('%s', '%s', '%s', %u, %d, @out_tran, @out_code)",
        m_account, m_p2, m_p3, m_price, static_cast<int>(m_chargeType));
}

bool ChargeQuery::execQuery(MySQL& db)
{
    return db.exec(true);
}

// ORIG T 0x08174fc6
int ChargeQuery::getResult(MySQL& db)
{
    try
    {
        if (!db.set_query("select @out_tran, @out_code"))
            throw static_cast<int>(0x24c);
        if (!db.exec(true))
            throw static_cast<int>(0x24f);
        if (!db.fetch())
            throw static_cast<int>(0x252);
        if (!db.get_ulonglong(0, m_trans.m_tranNo))
            throw static_cast<int>(0x255);
        if (!db.get_int(1, m_resultCode))
            throw 600;   // ORIG movl $0x258 Ghidra 十进制化显示（同 0x258）
        return 1;
    }
    catch (const int& e)
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanCash.cpp", __FUNCTION__, 0x25e,
            "[Taiwan, Billing] ChargeQuery Error Line:%u\n",
            static_cast<unsigned int>(e));
        return 0;
    }
}

// ---- CeraPointQuery -------------------------------------------------------

// ORIG T 0x081751b6（C2 同址别名）
CeraPointQuery::CeraPointQuery(const char* account, const char* p2,
                               char* uidStr, char* p4, unsigned int price,
                               char c6, char c7, char c8)
    : m_account(account), m_p2(p2), m_uidStr(uidStr), m_p4(p4),
      m_price(price), m_c6(c6), m_c7(c7), m_c8(c8), m_resultCode(0)
{
}

// ORIG T 0x08175224：%d 参数序 price/c6/StringToNumber(uidStr)/c8 与 ORIG 一致。
bool CeraPointQuery::setQuery(MySQL& db)
{
    return db.set_query(
        "call usp_point_process('%s', '%s', '%c', %u, %d, %d , %d, @out_code)",
        m_account, m_p2, static_cast<char>(m_c8), m_price,
        static_cast<int>(m_c6),
        static_cast<int>(StringToNumber(m_uidStr)),
        static_cast<int>(m_c7));
}

bool CeraPointQuery::execQuery(MySQL& db)
{
    return db.exec(true);
}

// ORIG T 0x081752cc
int CeraPointQuery::getResult(MySQL& db)
{
    try
    {
        if (!db.set_query("select @out_code"))
            throw static_cast<int>(0x282);
        if (!db.exec(true))
            throw static_cast<int>(0x285);
        if (!db.fetch())
            throw static_cast<int>(0x288);
        if (!db.get_int(0, m_resultCode))
            throw static_cast<int>(0x28b);
        return 1;
    }
    catch (const int& e)
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanCash.cpp", __FUNCTION__, 0x291,
            "[Taiwan, Billing] ChargePointQuery Error Line:%u\n",
            static_cast<unsigned int>(e));
        return 0;
    }
}

// ---------------------------------------------------------------------------
// Sig 族
// ---------------------------------------------------------------------------

// ORIG W 0x0815df38
void SigUpdateExitTime::init()
{
    m_timeA = 0;
    m_timeB = 0;
}

// ORIG W 0x0815df50
void SigUpdateExitTime::set(unsigned int a, unsigned int b)
{
    m_timeA = a;
    m_timeB = b;
}

// ORIG W 0x0815de9c
void SigLoadGeolocation::init()
{
    m_count = 0;
    memset(&m_countries, 0, sizeof(m_countries));
}

// ORIG W 0x0815decc：dst = this+4+(count<<4)，strncpy 15 字节后 ++count。
void SigLoadGeolocation::set(char* country)
{
    strncpy(m_countries[m_count], country, 15);
    ++m_count;
}

// ORIG W 0x0815df06
void SigLoadGeolocation::setData(SigLoadGeolocation& other)
{
    m_count = other.m_count;
    memcpy(&m_countries, &other.m_countries, sizeof(m_countries));
}

// ORIG W 0x0815dda8
void SigLoadRestrictPolicy::init()
{
    m_restrictValue = 0;
    m_count = 0;
    memset(m_data, 0, sizeof(m_data));
    memset(m_extra, 0, sizeof(m_extra));
}

// ORIG W 0x0815de02：restrictValue 写 +0；flag 写 m_extra[index]（0x119c 段）；
// 计数自增。index 无界（ORIG 同样直接下标）。
void SigLoadRestrictPolicy::set(int restrictValue, int index, char flag)
{
    m_restrictValue = restrictValue;
    m_extra[index] = flag;
    ++m_count;
}

// ORIG W 0x0815de38：整结构拷贝（两段 memcpy 同型展开）。
void SigLoadRestrictPolicy::set(SigLoadRestrictPolicy& other)
{
    m_restrictValue = other.m_restrictValue;
    m_count = other.m_count;
    memcpy(m_data, other.m_data, sizeof(m_data));
    memcpy(m_extra, other.m_extra, sizeof(m_extra));
}

// ORIG W 0x08163a04
void SigLoadHeroMissionData::init()
{
    m_field0 = 0;
    memset(m_data, 0, sizeof(m_data));
}

// ORIG W 0x081639f0（C2 同址别名）
SigLoadHeroMissionData::SigLoadHeroMissionData()
{
    init();
}

// ORIG W 0x081639c0
void SigSaveHeroMissionData::init()
{
    m_field0 = 0;
    memset(m_data, 0, sizeof(m_data));
}

// ORIG W 0x081639ac（C2 同址别名）
SigSaveHeroMissionData::SigSaveHeroMissionData()
{
    init();
}

// ---------------------------------------------------------------------------
// Garena 自由函数
// ---------------------------------------------------------------------------

// ORIG T 0x0817305a
bool GarenaCyberCafe(unsigned short cafeFlag)
{
    return (cafeFlag & 1) != 0;
}

// ORIG T 0x08173080
bool GarenaCheckTimeStamp(unsigned int nowA, unsigned int nowB,
                          unsigned int serverC, unsigned int serverD)
{
    if (nowA + 300 > nowB)          // ja：无符号 >
        return true;
    if (nowA != serverC)
        return false;
    return !(serverD + 300 < nowB); // jb：无符号 <
}

// ORIG T 0x08471c74
void GarenaAuthPCRoom(CUser* user)
{
    if (user == 0)
        return;

    Packet_Pcroom_Response resp;
    resp.setResult(1);
    resp.setMobileAuth(1);
    resp.setGcaType(0xb);
    resp.setPatchTime(static_cast<unsigned int>(time(0)) + 0x93a80);

    int uid = static_cast<int>(user->GetUID());
    int accId = static_cast<int>(user->get_acc_id());   // ORIG 读取未再使用

    Stream* st = GlobalData::s_stream_pool->Acquire("DF_PCRoomServerProxy.cpp", 0x1ab);
    CStreamGuard guard(st, true);
    guard << 0x17e;   // ORIG: operator<<(int) 后经 operator*() 再 <<（无副作用）
    guard << uid;
    guard.put_binary(&resp, static_cast<int>(resp.getLen()));

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    (void)accId;
}   // guard 析构回收 stream（ORIG D1 调用一致）

// ---------------------------------------------------------------------------
// Packet_Pcroom_Response
// ---------------------------------------------------------------------------

// ORIG W 0x0822be14：PacketHeader(0xfaf,0x1f) 展开（type=0xfaf/len=0x1f/
// 清零区 +4..+9）+ +0xa/+0xe dword 清零 + +0x1e 字节清零。
unsigned short Packet_Pcroom_Response::rd16(int off) const
{
    unsigned short v;
    memcpy(&v, m_buf + off, sizeof(v));
    return v;
}
void Packet_Pcroom_Response::wr16(int off, unsigned short v)
{
    memcpy(m_buf + off, &v, sizeof(v));
}
void Packet_Pcroom_Response::wr32(int off, unsigned int v)
{
    memcpy(m_buf + off, &v, sizeof(v));
}

Packet_Pcroom_Response::Packet_Pcroom_Response() : m_buf()
{
    setType(0xfaf);
    setLen(0x1f);
    clearHeaderTail();   // +4..+9 清零区、+0xa/+0xe dword、+0x1e byte
}

}  // namespace Taiwan
