// ============================================================================
// df_game_r 还原 —— Taiwan Query/Garena/Sig 族实现（Wave5 仅ORIG清零批）。
// 函数级依据见 TaiwanQueries.h 头注释（docs/class_func_reports/Taiwan__*
// 报告反编译 C 与 df_game_r objdump 逐指令核对）；SQL 格式串一律原样保留。
// ============================================================================

#include "TaiwanQueries.h"

#include <string.h>
#include <time.h>

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "LogManager.h"
#include "MsgQueueMgr.h"
#include "MySQL.h"
#include "TaiwanCash.h"        // BillTransaction 权威布局
#include "DNFFunctionLib.h"    // StringToNumber

// ORIG 的 GarenaAuthData getter 直接调用全局 C 的 ntohl/ntohs（@plt）。
// 经 <netinet/in.h> 获取正确 C-linkage 声明（extern "C" 由系统头给出），
// 再 #undef 其字节序宏（glibc 在 little-endian 下把 ntohl 宏为恒等/swap，
// 会吞掉函数调用）；这样 getter 内 `ntohl(m_uid)` 编译为对 `ntohl@plt` 的
// C 调用，而不绑定到 namespace Taiwan 内的 C++ 修饰成员。
#include <netinet/in.h>
#undef ntohl
#undef ntohs

// base64_decode：ORIG 为全局 C++ 链接 T 0x0807ebf4（_Z13base64_decodeRKSsRSt6vectorIhSaIhEE），
// 权威定义在 PacketTypes.cpp（TU 归属证据：_GLOBAL__I__Z13base64_encodePKhjRSs @0x0807ef30
// 与 base64_encode/base64_decode 地址紧邻域）。本 TU 仅声明引用，调用点未限定名解析到
// 全局版本（namespace Taiwan 内不再有同名声明）。
void base64_decode(std::string const& encoded_string,
                   std::vector<unsigned char>& out);

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
// COpensslRSA —— OpenSSL RSA 公钥包装（全局命名空间，ORIG mangled _ZN11COpensslRSA*）
// ---------------------------------------------------------------------------

// ORIG W 0x0816c238
COpensslRSA::COpensslRSA() : m_rsa(0) {}

// ---------------------------------------------------------------------------
// GetAppFolder —— ORIG W 0x0816c715（_Z12GetAppFolderv，COpensslRSA TU 内全局
// 函数，由 sret 返回空串 "\"\"" 字面量 @0x08b81360）。重建以空串最小实现满足
// Init 拼路径语义；真实路径来源（注册表/配置）缺省时返回空。
// ---------------------------------------------------------------------------
std::string GetAppFolder()
{
    return std::string("");
}

// ORIG W 0x0816c246：key 非空 RSA_free 后置空。
// 第三方 libcrypto（OpenSSL）豁免（§4.5 例外，不转不违例）：C 符号需 extern "C" 链接。
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
bool COpensslRSA::Init() throw()
{
    return Init("publickey.pem");
}

// ORIG T 0x0816c28a：path 空 → false；BIO_new(BIO_s_file()) →
// BIO_ctrl(bio,0x6c,3,(GetAppFolder()+path).c_str())（BIO_C_SET_FILENAME）→
// PEM_read_bio_RSA_PUBKEY(bio,0,0,0) 存 m_rsa → BIO_free_all → key 非空真。
// b28d-36 判词（N104+P78，ORIG 0x0816c28a）：块内容逐条对齐，仅两处跨版本差：
// ①EH 尾块相对序——ORIG 正常路径尾块居最末、unwind 块居中（0x816c308..0x816c345），
// 4.4 把同一段 epilogue 就地内联、unwind 压最末，difflib 放大成 ~24 行 rotate；
// ②返回汇聚寄存器——ORIG 各 return 直接 `mov $0/1,%eax`（0x816c298/0x816c38c/0x816c393），
// 4.4 因 return 前需析构 full/rel 两个 string 而用 %ebx 汇聚、尾 `mov %ebx,%eax`。
// 语义等价（三 return 与 ORIG 同控制流），封顶。
bool COpensslRSA::Init(const char* path) throw()
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
    int chunk = len - pos;
    if (chunk > rsa_size)
        chunk = rsa_size;
    while (chunk > 0)
    {
        std::vector<unsigned char> block;
        block.resize(static_cast<size_t>(rsa_size), 0);
        int written = RSA_public_decrypt(chunk,
                                         reinterpret_cast<const unsigned char*>(in) + pos,
                                         &block[0], m_rsa, 1);
        if (written <= 0)
        {
            return false;
        }
        // [L10-2] P167 实证：ORIG 0x0816c5ff..0x0816c644 为
        // `operator[](0)`×2（first/last 各一次）+ `insert<unsigned char*>`（指针版）
        // ⇒ 源形 `&block[0], &block[0] + written`；`.begin()+n` 会发 `begin()`×2 +
        // `normal_iterator::operator+` + 迭代器版 insert（p167 EXTRA/MISSING 差集）。
        out.insert(out.end(), &block[0], &block[0] + written);
        pos += chunk;
        chunk = len - pos;
        if (chunk > rsa_size)
            chunk = rsa_size;
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
    int chunk = len - pos;
    if (chunk > max_chunk)
        chunk = max_chunk;
    while (chunk > 0)
    {
        std::vector<unsigned char> block;
        block.resize(static_cast<size_t>(rsa_size), 0);
        int written = RSA_private_encrypt(chunk,
                                          reinterpret_cast<const unsigned char*>(in) + pos,
                                          &block[0], m_rsa, 1);
        if (written < 1)
        {
            return false;
        }
        // [L10-2] 同 Decrypt（ORIG 0x0816c47x 段同形：指针版 insert + 2×operator[]）。
        out.insert(out.end(), &block[0], &block[0] + written);
        pos += chunk;
        chunk = len - pos;
        if (chunk > max_chunk)
            chunk = max_chunk;
    }
    return true;
}

namespace Taiwan {

// ---------------------------------------------------------------------------
// GarenaAuthData getter/setter（读侧网络序转换与 ORIG ntohl/ntohs 调用一致）
// ---------------------------------------------------------------------------

// ORIG T 0x08172fc8
unsigned int GarenaAuthData::getUid()
{
    return ntohl(m_uid);
}

// ORIG T 0x08172fde
char* GarenaAuthData::getUname()
{
    return m_uname;
}

// ORIG T 0x08173008
unsigned short GarenaAuthData::getGcaType()
{
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
    return ntohl(m_timeStamp) + m_patchTime;
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
    // ORIG 81730d6：arg==empty（0x8b83f20）走带参 Init(path)，
    // 否则无参 Init()（= Init("publickey.pem"@0x8b81361)）。
    std::string empty("");
    std::string arg(path == 0 ? "" : path);
    if (arg == empty)
        return openSSLRSA_.Init(path);
    return openSSLRSA_.Init();
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

bool EntranceQuery::setQuery(MySQL&) throw() { return true; }   // ORIG 0x08174012
bool EntranceQuery::execQuery(MySQL&) throw() { return true; }  // ORIG 0x0817401c
bool EntranceQuery::getResult(MySQL&) throw() { return 1; }     // ORIG 0x08174026

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
bool BalanceQuery::getResult(MySQL& db)
{
    try
    {
        if (!db.set_query("select @out_balance, @out_result"))
            throw static_cast<int>(0x116);
        if (!db.exec(true))
            throw static_cast<int>(0x119);
        if (!db.fetch())
            throw static_cast<int>(0x11c);
        // [b28BM13] ORIG 0x08174092 -0x10(%ebp) 为共享 idx 槽，init 落 fetch
        // 检查之后：addl $1 后置自增、值经 eax 直传首参（P0：有槽⇒具名局部）。
        unsigned int idx = 0;
        if (!db.get_int(idx++, m_balance))
            throw static_cast<int>(0x120);
        if (!db.get_int(idx++, m_resultCode))
            throw static_cast<int>(0x123);
        return 1;
    }
    catch (int e)
    {
        // [b28BM13] ORIG 0x0817422d = `begin_catch; mov (%eax),%eax; mov %eax,-0xc`
        // （值先物化入槽，P35/P0）⇒ 原码为按值 catch；引用形反产指针落槽+回读
        // 解引用两段（b28BM13 实测）。throw(int) 语义等价（POD）。
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
bool BalancePointQuery::getResult(MySQL& db)
{
    try
    {
        if (!db.set_query("select @out_balance, @out_point, @out_result"))
            throw static_cast<int>(0x14b);
        if (!db.exec(true))
            throw static_cast<int>(0x14e);
        if (!db.fetch())
            throw static_cast<int>(0x151);
        // [b28BM13] 同 BalanceQuery::getResult：ORIG 共享 idx 槽（-0x10）后置自增。
        unsigned int idx = 0;
        if (!db.get_int(idx++, m_balance))
            throw static_cast<int>(0x155);
        if (!db.get_int(idx++, m_point))
            throw static_cast<int>(0x158);
        if (!db.get_int(idx++, m_resultCode))
            throw static_cast<int>(0x15b);
        return 1;
    }
    catch (int e)   // [b28BM13] ORIG 值先物化入槽 ⇒ 按值 catch（同 BalanceQuery）
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
// N76 封顶：uid 写入块 4.4 恒 LHS(this/ecx) 先物化、ORIG 4.1 恒 RHS(槽回读)先，
// 3 行时序差不可翻；函数体赋值序（price,pay,uid）已复原，帧 0x28 一致。
GiftQuery::GiftQuery(const char* account, const char* sendCharac,
                     const char* recvCharac, unsigned int price,
                     unsigned int pay, long long uid)
{
    m_account = account;
    m_sendCharac = sendCharac;
    m_recvCharac = recvCharac;
    m_price = price;
    m_pay = pay;
    m_uid = uid;
    m_resultCode = 0;
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
bool GiftQuery::getResult(MySQL& db)
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
    catch (int e)   // ORIG: 按值 catch —— begin_catch 后先 deref 再存槽
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanCash.cpp", __FUNCTION__, 0x1d1,
            "[Taiwan, Billing] GiftQuery Error Line:%u\n",
            static_cast<unsigned int>(e));
        return 0;
    }
}

// ---- ConfirmQuery ---------------------------------------------------------
// ORIG T 0x08174ac6（C2 同址别名）：trans 前 12 字节逐 dword 拷贝进 m_trans；
// ORIG 尾部 movl $0x0,(%eax) 清零 m_resultCode（+0x00）。
ConfirmQuery::ConfirmQuery(const BillTransaction& trans)
{
    // [b27L8] ORIG 0x08174d08：trans 整体赋值在前、m_resultCode=0 在后
    // （同 CancelQuery 0x08174ce2 形）。
    m_trans = trans;
    m_resultCode = 0;
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
bool ConfirmQuery::getResult(MySQL& db)
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
    catch (int e)   // ORIG: 按值 catch —— begin_catch 后先 deref 再存槽
    {
        LogManager::logFormat(
            1, "localtaiwan/System/TaiwanCash.cpp", __FUNCTION__, 0x1fd,
            "[Taiwan, Billing] ConfirmQuery Error Line:%u\n",
            static_cast<unsigned int>(e));
        return 0;
    }
}

// ---- CancelQuery ----------------------------------------------------------

// ORIG T 0x08174ce2（C2 同址别名）；ORIG 尾部同样清零 m_resultCode（+0x00）。
CancelQuery::CancelQuery(const BillTransaction& trans)
{
    // [b27L8] ORIG 0x08174ce2：trans 12B 整体赋值（dest 缓存三连 mov）在前、
    // m_resultCode=0 在后；memcpy 形出 call 且 m_resultCode(0) 列表形提前。
    m_trans = trans;
    m_resultCode = 0;
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
bool CancelQuery::getResult(MySQL& db)
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
    catch (int e)   // ORIG: 按值 catch —— begin_catch 后先 deref 再存槽
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
      m_chargeType(chargeType)
{
    m_trans.init();
    // [b27L8] ORIG 0x08174efe：m_resultCode=0（movl $0x0,0x14）在 m_trans.init
    // call 之后发射；初始化列表形会提前到 call 前。
    m_resultCode = 0;
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
bool ChargeQuery::getResult(MySQL& db)
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
    catch (int e)   // ORIG: 按值 catch —— begin_catch 后先 deref 再存槽
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
// [b28BM13 N7] 封顶：指令/槽/助记符全等，仅 5 个 callee-saved 分配整体错位
// （4.1 首位 edx vs 4.4 首位 edi）。register 绑定按 P0 全域禁用（旧 asm("edx")
// 尝试注记作废，实测亦劣化），穷举后封顶。
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
bool CeraPointQuery::getResult(MySQL& db)
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
    catch (int e)   // ORIG: 按值 catch —— begin_catch 后先 deref 再存槽
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
    // [b27L8] ORIG 0x0815de02 物化序 index→this→flag，4.4 恒 index→flag→this
    // （N18a 族寄存器物化序差；指针算术形丢 scaled-index 更劣，探针已证）。
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
    if ((cafeFlag & 1) != 0)
        return true;
    return false;
}

// ORIG T 0x08173080
bool GarenaCheckTimeStamp(unsigned int nowA, unsigned int nowB,
                          unsigned int serverC, unsigned int serverD)
{
    // [b28BM13 P222] ORIG 中层 = `cmp; jne <mov$0 岛>`（正向 == 包裹、else 归
    // return-false 岛，探针 bm13_p6 g2 实证；!=/!(==) 形 4.4 恒产 je 反极性）。
    if (!(nowA + 300 > nowB))       // ja：无符号 >
    {
        if (nowA == serverC)
        {
            if (!(serverD + 300 < nowB))  // jb：无符号 <
                return true;
            return false;
        }
        else
            return false;
    }
    return true;
}

// ORIG T 0x08471c74
void GarenaAuthPCRoom(CUser* user)
{
    if (user == 0)
        return;

    // b28d-36（修法 P104/P76）：改用全局权威类（0x1f，PacketHeader 派生）——
    // ORIG 0x08471c8c 先 call 全局 ctor `_ZN22Packet_Pcroom_ResponseC1Ev`，
    // 再字段直写 +0x12/-0x1a/-0x1b/-0x16（movl/movb/movb/movl），put_binary
    // 长度取 +0x02 packetSize（movzwl -0x25）。本地 F 视图版多 4 条 lea 基址
    // 且 ctor 落在 Taiwan 命名空间（p167 MISSING）。
    Packet_Pcroom_Response resp;
    resp.m_field12 = 1;
    resp.m_field1a = 1;
    resp.m_field1b = 0xb;
    resp.m_expireTime = static_cast<int>(time(0)) + 0x93a80;

    // b28d-36（P0/P92）：ORIG 0x08471cc2/0x08471cc8 直写 resp 字段
    // （-0x1d=resp+0xa m_uid、-0x19=resp+0xe m_accId），非具名 uid/accId 局部
    // ——具名版把 resp 基址顶深 8（-0x2f vs -0x27）且 guard 槽 -0x38 vs -0x30。
    resp.m_uid = static_cast<int>(user->GetUID());
    resp.m_accId = static_cast<int>(user->get_acc_id());   // ORIG 写入后不再读

    // [b23S3C] P48/P0：Acquire 右值直落 ctor 实参（无 stream 槽，ORIG +0x081）；
    // 写流两跳 `(*guard)->operator<<`（deEv 080c8c46 + lsEi 080c8c56 各一次，
    // +0x090/+0x0ae）；put_binary 走 operator->（ptEv +0x0d2）。
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("DF_PCRoomServerProxy.cpp", 0x1ab),
        true);
    (*guard)->operator<<(0x17e);
    (*guard)->operator<<(resp.m_uid);
    guard.operator->()->put_binary(
        &resp, resp.packetSize);

    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
}   // guard 析构回收 stream（ORIG D1 调用一致）

}  // namespace Taiwan
