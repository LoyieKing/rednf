#ifndef DNF_GAME_TAIWAN_QUERIES_H_
#define DNF_GAME_TAIWAN_QUERIES_H_

// ============================================================================
// df_game_r 还原 —— Taiwan Query/Garena/Sig 族（Wave5 仅ORIG清零批）。
// 权威定义点：TaiwanQueries.h / TaiwanQueries.cpp（OnlyOrig_TaiwanLog.TwQuery 批）。
//
// ORIG 依据（docs/class_func_reports/Taiwan__* + df_game_r objdump 实测）：
//   BalanceQuery      ctor T 0x08174030 | setQuery 0x08174052 | execQuery 0x08174076 | getResult 0x08174092
//   BalancePointQuery ctor T 0x08174290 | setQuery 0x081742bc | execQuery —       | getResult 0x081742fc
//     （execQuery 与 BalanceQuery 同型：MySQL::exec(true)，ORIG 亦有该 W 符号）
//   GiftQuery         ctor T 0x081747fe | setQuery 0x08174866 | getResult 0x081748d6
//   ConfirmQuery      ctor T 0x08174ac6 | setQuery 0x08174aec | getResult 0x08174b44
//   CancelQuery       ctor T 0x08174ce2 | setQuery 0x08174d08 | getResult 0x08174d60
//   ChargeQuery       ctor T 0x08174efe | setQuery 0x08174f52 | getResult 0x08174fc6
//   CeraPointQuery    ctor T 0x081751b6 | setQuery 0x08175224 | getResult 0x081752cc
//   AuthGarenaToken   init() T 0x081730c2 | init(char const*) T 0x081730d6 |
//                     decrypt(char const*,int,GarenaAuthData&) T 0x081731f2
//                     openSSLRSA_（静态对象 B 0x09404754）
//   SigLoadRestrictPolicy init W 0x0815dda8 | set(int,int,char) W 0x0815de02 |
//                         set(SigLoadRestrictPolicy&) W 0x0815de38
//   SigLoadGeolocation    init W 0x0815de9c | set(char*) W 0x0815decc |
//                         setData W 0x0815df06
//   SigUpdateExitTime     init W 0x0815df38 | set(uint,uint) W 0x0815df50
//   SigLoadHeroMissionData ctor W 0x081639f0 | init W 0x08163a04
//   SigSaveHeroMissionData ctor W 0x081639ac | init W 0x081639c0
//
// 布局要点（ORIG）：全部字段偏移按 ctor/getResult 写入实测；类均为 POD，
// 无虚表。各 Query::getResult 失败路径 throw int（源码行号常量），外层
// try/catch 记 LogManager::logFormat 后 return 0 —— 忠实转写。
// ============================================================================

#include <string>
#include <vector>

#include "GameTypes.h"  // cMyTrace 声明（真实定义属其它 TU）

class MySQL;
#include "TaiwanCash.h"    // BillTransaction 权威布局（唯一归口；无环）
class CUser;

// ---------------------------------------------------------------------------
// COpensslRSA —— OpenSSL RSA 公钥包装（全局命名空间；ORIG mangled
// _ZN11COpensslRSA*，AuthGarenaToken 静态成员所需完整类型）。
// ORIG：ctor/dtor/Init()/Init(char const*)/Encrypt/Decrypt 六函数全实现；
// Encrypt（T 0x0816c3a2）RSA_private_encrypt 分块，Decrypt（T 0x0816c532）
// RSA_public_decrypt 分块，均 RSA_PKCS1_PADDING=1。m_rsa 以 void* 持 opaque
// RSA*（免 openssl 头依赖）；openssl C 符号由 LibCryptoStatic.cpp 收口实现。
// ---------------------------------------------------------------------------
extern "C" {   // 第三方 libcrypto（OpenSSL）豁免（§4.5 例外，不转不违例）：C 符号需 extern "C" 链接
void* BIO_s_file();                                   // libcrypto（运行期由系统库提供）
void* BIO_new(void* type);
int BIO_ctrl(void* bio, int cmd, long arg, void* p);
void BIO_free_all(void* bio);
void* PEM_read_bio_RSA_PUBKEY(void* bp, void** x, void* cb, void* u);
int RSA_size(const void* rsa);
int RSA_private_encrypt(int flen, const unsigned char* from,
                        unsigned char* to, void* rsa, int padding);
int RSA_public_decrypt(int flen, const unsigned char* from,
                       unsigned char* to, void* rsa, int padding);
}

// ORIG W 0x0816c715：GetAppFolder（_Z12GetAppFolderv，COpensslRSA TU 内全局函数，
// 由 sret 返回）。重建以空串最小实现满足 Init 拼路径语义；真实路径来源（注册表/
// 配置）缺省时返回空。
std::string GetAppFolder();

class COpensslRSA
{
public:
    COpensslRSA();                       // ORIG W 0x0816c238：m_rsa = 0
    ~COpensslRSA();                      // ORIG W 0x0816c246：RSA_free + 置空

    // ORIG T 0x0816c26e：Init("publickey.pem")。
    bool Init();
    // ORIG T 0x0816c28a：BIO_new(BIO_s_file()) → GetAppFolder()+path →
    // BIO_ctrl(bio,0x6c/*BIO_C_SET_FILENAME*/,3,path) → PEM_read_bio_RSA_PUBKEY
    // → key 非空返回 true。
    bool Init(const char* path);

    // ORIG T 0x0816c532：按 RSA_size 分块 RSA_public_decrypt(RSA_PKCS1_PADDING=1)
    // 后 insert 到 out 尾部；任一块失败返回 false。
    bool Decrypt(const char* in, int len, std::vector<unsigned char>& out);

    // ORIG T 0x0816c3a2：maxChunk=RSA_size-12，按块 RSA_private_encrypt(
    // RSA_PKCS1_PADDING=1) 后 insert 到 out 尾部；任一块失败返回 false，
    // 全部块处理完返回 true。
    bool Encrypt(const char* in, int len, std::vector<unsigned char>& out);

private:
    void* m_rsa;                          // +0x00（opaque RSA*）
};

namespace Taiwan {

// ---------------------------------------------------------------------------
// GarenaAuthData —— 解密令牌视图（Garena 对端下发，网络字节序原样存放）。
// ORIG 死常量（头文件 static，逐 TU 一份；值按 .rodata 实测）：
static const unsigned int MAX_GARENA_ID_LENTH = 0x20;                 // _ZN6TaiwanL19MAX_GARENA_ID_LENTHE r 0x4 (=32)
static const unsigned short C_ARAD_PROD_TYPE = 0x2d;                  // _ZN6TaiwanL16C_ARAD_PROD_TYPEE r 0x2 (=45)
static const unsigned short C_ARAD_PROD_TYPE_POINT_AUCTION_PRICE = 0x3c;      // _ZN6TaiwanL36C_ARAD_PROD_TYPE_POINT_AUCTION_PRICEE r 0x2 (=60)
static const unsigned short C_ARAD_PROD_TYPE_POINT_AUCTION_COMMISSION = 0x3e; // _ZN6TaiwanL41C_ARAD_PROD_TYPE_POINT_AUCTION_COMMISSIONE r 0x2 (=62)
// sizeof = 0x68（decrypt 按 size-1 memcpy 校验上限 0x68 字节）。
// ---------------------------------------------------------------------------
#pragma pack(push, 1)
class GarenaAuthData
{
public:
    unsigned int getUid();                // ORIG T 0x08172fc8：ntohl(m_uid)
    char* getUname();                     // ORIG T 0x08172fde：return m_uname
    unsigned short getGcaType();          // ORIG T 0x08173008：ntohs(@+0x28)
    bool getMobileAuth();                 // ORIG T 0x08173022：m_mobileAuth == 1
    unsigned char getGoldMemberType();    // ORIG T 0x0817304e：byte @+0x2a
    unsigned int getTimeStamp();          // ORIG T 0x08172fea：ntohl(+0x24)+m_patchTime
    unsigned int getPatchTime();          // ORIG T 0x08173042
    void setPatchTime(unsigned int t);    // ORIG T 0x08173034

private:
    unsigned int m_uid;                             // +0x00（ntohl 读取）
    char m_uname[MAX_GARENA_ID_LENTH];               // +0x04..+0x23
    unsigned int m_timeStamp;                       // +0x24（ntohl 读取）
    unsigned short m_gcaType;                       // +0x28（ntohs 读取）
    unsigned char m_goldMemberType;                 // +0x2a
    unsigned char m_pad2b;                          // +0x2b（ORIG 空洞；getMobileAuth 读 +0x2c）
    unsigned char m_mobileAuth;                     // +0x2c（==1 视为已认证）
    unsigned char m_pad2d[0x64 - 0x2d];             // +0x2d..+0x63
    unsigned int m_patchTime;                       // +0x64（本地附加补偿秒数）
};
#pragma pack(pop)

// 布局断言：packed 后 sizeof 必须仍为 0x68（ORIG decrypt memcpy 上限），
// 且 CUser 成员 m_garenaAuthData 恢复非对齐偏移 +0x6ef92（Main 批准 2026-08-29）。
typedef char _assert_garena_size[(sizeof(GarenaAuthData) == 0x68) ? 1 : -1];

// ---------------------------------------------------------------------------
// AuthGarenaToken —— Garena 登录令牌静态接口。
// ---------------------------------------------------------------------------
class AuthGarenaToken
{
public:
    static void init();                                     // 默认公钥 publickey.pem
    static bool init(const char* path);                     // path 为空串→Init()
    static int decrypt(const char* token, int len,         // 返回 0 成功；错误码见 cpp
                       GarenaAuthData& out);

    static COpensslRSA openSSLRSA_;        // ORIG B 0x09404754
};

// ---------------------------------------------------------------------------
// usp 存储过程查询族（供 TaiwanCash::processBilling<T> 使用）。
// 统一接口：bool setQuery(MySQL&); bool execQuery(MySQL&); int getResult(MySQL&)
// （getResult 失败内部 catch 后返回 0；格式错误抛 int 行号码）。
// ---------------------------------------------------------------------------

// ---- EntranceQuery（0x08173ffa；set/exec/getResult 三方法 ORIG 均为 mov eax,1 直返）----
class EntranceQuery
{
public:
    EntranceQuery();

    // [b28BM13 P216] throw()：ORIG processBilling<EntranceQuery> 0x0817555c 无
    // 任何 EH landing pad（其余实例的 getResult 才带 try/throw）；三方法声明
    // nothrow 使 4.4 判 call 无抛、cleanup 链与 ORIG 同形。
    bool setQuery(MySQL& db) throw();     // ORIG T 0x08174012：恒 true
    bool execQuery(MySQL& db) throw();    // ORIG T 0x0817401c：恒 true
    bool getResult(MySQL& db) throw();    // ORIG T 0x08174026：恒 1


    friend class TaiwanCash;  // ORIG 同 TU 内联直读字段（TwCash 批）

private:
    int m_field0;                         // +0x00（ctor 清零；语义未建模【推断】）
    int m_resultCode;                     // +0x04（@out_code 回传位；ORIG 未实际写入）
};

// ---- BalanceQuery（usp_balance('%s', @out_balance, @out_result)）----
class BalanceQuery
{
public:
    explicit BalanceQuery(const char* account);

    bool setQuery(MySQL& db);             // ORIG T 0x08174052
    bool execQuery(MySQL& db);            // ORIG T 0x08174076：db.exec(true)
    bool getResult(MySQL& db);            // ORIG T 0x08174092


    friend class TaiwanCash;  // ORIG 同 TU 内联直读字段（TwCash 批）

private:
    const char* m_account;                // +0x00
    unsigned int m_balance;               // +0x04（@out_balance；ORIG get_int(EiRj)）
    int m_resultCode;                     // +0x08（@out_result；ORIG get_int(EiRi)）
};

// ---- BalancePointQuery（usp_balance_point('%s', @out_balance, @out_point, @out_result)）----
class BalancePointQuery
{
public:
    explicit BalancePointQuery(const char* account);

    bool setQuery(MySQL& db);             // ORIG T 0x081742bc
    bool execQuery(MySQL& db);            // 与其它 Query 同型 db.exec(true)
    bool getResult(MySQL& db);            // ORIG T 0x081742fc


    friend class TaiwanCash;  // ORIG 同 TU 内联直读字段（TwCash 批）

private:
    const char* m_account;                // +0x00
    unsigned int m_balance;               // +0x04（@out_balance；ORIG get_int(EiRj)）
    unsigned int m_point;                 // +0x08（@out_point；ORIG get_int(EiRj)）
    int m_resultCode;                     // +0x0c（@out_result；ORIG get_int(EiRi)）
};

// ---- GiftQuery（usp_gift('%s','%s','%s',%u,%u,@out_tran,@out_code)）----
class GiftQuery
{
public:
    GiftQuery(const char* account, const char* sendCharac, const char* recvCharac,
              unsigned int price, unsigned int pay, long long uid);

    bool setQuery(MySQL& db);             // ORIG T 0x08174866
    bool execQuery(MySQL& db);
    bool getResult(MySQL& db);            // ORIG T 0x081748d6


    friend class TaiwanCash;  // ORIG 同 TU 内联直读字段（TwCash 批）

private:
    const char* m_account;                // +0x00
    const char* m_sendCharac;             // +0x04
    const char* m_recvCharac;             // +0x08
    unsigned long long m_uid;             // +0x0c（b28BM13 P211 符号性异型化：
                                          // 翻 ORIG 0x081747bc ctor 的 RHS-first
                                          // store 序；宽度/偏移不变）
    unsigned int m_price;                 // +0x14
    unsigned int m_pay;                   // +0x18
    int m_resultCode;                     // +0x1c（@out_code）
    BillTransaction m_trans;              // +0x20（@out_tran；init 清零）
};

// ---- ConfirmQuery（usp_confirm(%s, @out_code)；事务号由 BillTransaction 拷贝）----
class ConfirmQuery
{
public:
    explicit ConfirmQuery(const BillTransaction& trans);

    bool setQuery(MySQL& db);             // ORIG T 0x08174aec
    bool execQuery(MySQL& db);
    bool getResult(MySQL& db);            // ORIG T 0x08174b44


    friend class TaiwanCash;  // ORIG 同 TU 内联直读字段（TwCash 批）

private:
    int m_resultCode;                     // +0x00（@out_code）
    BillTransaction m_trans;              // +0x04（trans 拷贝；tranNo 在 +0x08 读）
};

// ---- CancelQuery（usp_cancel(%s, @out_code)；与 ConfirmQuery 同布局）----
class CancelQuery
{
public:
    explicit CancelQuery(const BillTransaction& trans);

    bool setQuery(MySQL& db);             // ORIG T 0x08174d08
    bool execQuery(MySQL& db);
    bool getResult(MySQL& db);            // ORIG T 0x08174d60


    friend class TaiwanCash;  // ORIG 同 TU 内联直读字段（TwCash 批）

private:
    int m_resultCode;                     // +0x00
    BillTransaction m_trans;              // +0x04
};

// ---- ChargeQuery（usp_recharge('%s','%s','%s',%u,%d,@out_tran,@out_code)）----
class ChargeQuery
{
public:
    ChargeQuery(const char* account, const char* p2, const char* p3,
                unsigned int price, char chargeType);

    bool setQuery(MySQL& db);             // ORIG T 0x08174f52
    bool execQuery(MySQL& db);
    bool getResult(MySQL& db);            // ORIG T 0x08174fc6


    friend class TaiwanCash;  // ORIG 同 TU 内联直读字段（TwCash 批）

private:
    const char* m_account;                // +0x00
    const char* m_p2;                     // +0x04
    const char* m_p3;                     // +0x08
    unsigned int m_price;                 // +0x0c
    char m_chargeType;                    // +0x10（%d 有符号打印）
    int m_resultCode;                     // +0x14（@out_code）
    BillTransaction m_trans;              // +0x18（@out_tran）
};

// ---- CeraPointQuery（usp_point_process('%s','%s','%c',%u,%d,%d,%d,@out_code)）----
class CeraPointQuery
{
public:
    CeraPointQuery(const char* account, const char* p2, char* uidStr, char* p4,
                   unsigned int price, char c6, char c7, char c8);

    bool setQuery(MySQL& db);             // ORIG T 0x08175224
    bool execQuery(MySQL& db);
    bool getResult(MySQL& db);            // ORIG T 0x081752cc


    friend class TaiwanCash;  // ORIG 同 TU 内联直读字段（TwCash 批）

private:
    const char* m_account;                // +0x00
    const char* m_p2;                     // +0x04
    char* m_uidStr;                       // +0x08（StringToNumber 转数值）
    char* m_p4;                           // +0x0c
    unsigned int m_price;                 // +0x10
    char m_c6;                            // +0x14
    char m_c7;                            // +0x15
    char m_c8;                            // +0x16（%c 字符位）
    int m_resultCode;                     // +0x18
};

// ---------------------------------------------------------------------------
// Sig 族 —— 与客户端走 StreamGuard 收发的载荷结构（GetInBuffer<T> 目标类型）。
// ---------------------------------------------------------------------------

// SigUpdateExitTime（sizeof 0x8）
class SigUpdateExitTime
{
public:
    void init();                          // ORIG W 0x0815df38
    void set(unsigned int a, unsigned int b);  // ORIG W 0x0815df50

private:
    unsigned int m_timeA;                 // +0x00（字段语义未建模【推断】）
    unsigned int m_timeB;                 // +0x04
};

// SigLoadGeolocation（sizeof 0x2d4：计数 + 45 条 ×16 字节国别串）
class SigLoadGeolocation
{
public:
    void init();                          // ORIG W 0x0815de9c
    void set(char* country);              // ORIG W 0x0815decc：strncpy(第 count 条, s, 15)
    void setData(SigLoadGeolocation& other);            // ORIG W 0x0815de06

    // b28c-L26：LoadGeolocation::dispatch 0x0815b17x 直读 +0x00 并 `cmp $0x2b;
    // jbe`（无符号）⇒ 字段 unsigned；调用点无 accessor（ORIG 直 `mov -0x2f8(%ebp),%eax`）
    // ⇒ §8.3 字段化，公开以消 reinterpret_cast 裸偏移。
    unsigned int m_count;                             // +0x00 已写入条目数

private:
    char m_countries[(0x2d4 - 4) / 0x10][0x10];         // +0x04（720=45×16）
};

class SigLoadRestrictPolicy
{
public:
    void init();                              // ORIG W 0x0815dda8
    void set(int restrictValue, int index, char flag);  // ORIG W 0x0815de02
    void set(SigLoadRestrictPolicy& other);             // ORIG W 0x0815de38 全量拷贝

    // ORIG SecurityRestrictPolicy::getRestrictValue（0x0816dd22）直接写四字段，
    // 无 public API 可拼 → 字段公开（Main 2026-08-28 批准；不新增符号）。
    // m_data 真实语义：100 条 × 0x2d 字节的类别名缓冲（0x1194 = 100*0x2d；
    // getRestrictValue strncpy(p.m_data[i], src, 0x2d) 实证）。
    int m_restrictValue;                                  // +0x000（getRestrictValue 读）
    int m_count;                                          // +0x004
    char m_data[100][0x2d];                               // +0x008
    unsigned char m_extra[0x64];                          // +0x119c（flag 写入区。
                                        // b28BM13：bool 字段实测触发 setne 归一
                                        // (+2)，保持 uchar；装载序 index→this→
                                        // flag vs 4.4 index→flag→this = N18a）
};

// SigLoadHeroMissionData（sizeof 0x104）
class SigLoadHeroMissionData
{
public:
    SigLoadHeroMissionData();             // ORIG W 0x081639f0：调 init()
    void init();                          // ORIG W 0x08163a04

private:
    unsigned int m_field0;                // +0x00
    unsigned char m_data[0x100];          // +0x04
};

// SigSaveHeroMissionData（sizeof 0x104；与 Load 版同构）
class SigSaveHeroMissionData
{
public:
    SigSaveHeroMissionData();             // ORIG W 0x081639ac：调 init()
    void init();                          // ORIG W 0x081639c0

private:
    unsigned int m_field0;                // +0x00
    unsigned char m_data[0x100];          // +0x04
};

// ---------------------------------------------------------------------------
// Garena 自由函数
// ---------------------------------------------------------------------------

// ORIG T 0x0817305a：(cafeFlag & 1) != 0。网咖标志位检测。
bool GarenaCyberCafe(unsigned short cafeFlag);

// ORIG T 0x08173080：时间窗校验（语义标注【推断】——差值 0x12c=300 秒）：
//   (nowA + 300 <= nowB) 时若 serverA != serverC 则 false；
//   否则 ((serverD + 300 >= nowB)) 为真；首条件不满足直接真。
bool GarenaCheckTimeStamp(unsigned int nowA, unsigned int nowB,
                          unsigned int serverC, unsigned int serverD);

// ORIG T 0x08471c74：user 为空直接返回；否则组 Packet_Pcroom_Response
// （mobileAuth=1、gcaType=11、patchTime=time()+0x93a80）经 StreamPool 投递
// DISPATCH_Q（msg 头 0x17e，附 uid）。转写自 objdump 全量指令序。
void GarenaAuthPCRoom(CUser* user);

// ---------------------------------------------------------------------------
// Packet_Pcroom_Response —— PCRoom 响应包（ORIG W ctor 0x0822be14：
// PacketHeader(0xfaf,0x1f) 展开 + 清零）。无权威 PacketHeader 类；本类按
// ELF _ZN12PacketHeaderC1Ett @0x08110b70 手工排布（type/len/清零区），槽位
// 偏移全部 ORIG 实测，访问器经 memcpy 保证对齐无关。
// ---------------------------------------------------------------------------
#include <string.h>

class Packet_Pcroom_Response
{
public:
    Packet_Pcroom_Response();             // ORIG W 0x0822be14

    void setType(unsigned short t)        { wr16(0x00, t); }
    void setLen(unsigned short l)         { wr16(0x02, l); }
    unsigned short getLen() const         { return rd16(0x02); }
    void clearHeaderTail()                { memset(m_buf + 4, 0, 6);
                                            wr32(0x0a, 0); wr32(0x0e, 0);
                                            m_buf[0x1e] = 0; }
    void setResult(unsigned int v)        { wr32(0x12, v); }
    void setPatchTime(unsigned int t)     { wr32(0x16, t); }
    void setMobileAuth(unsigned char c)   { m_buf[0x1a] = c; }
    void setGcaType(unsigned char c)      { m_buf[0x1b] = c; }
    void setUid(unsigned int v)           { wr32(0x0a, v); }
    void setAccId(unsigned int v)         { wr32(0x0e, v); }

private:
    unsigned short rd16(int off) const;
    void wr16(int off, unsigned short v);
    void wr32(int off, unsigned int v);

    unsigned char m_buf[0x20];            // 总窗（ORIG len 0x1f + 对齐尾）
};

static_assert(sizeof(Packet_Pcroom_Response) == 0x20,
              "Packet_Pcroom_Response ABI drift");

}  // namespace Taiwan

#endif  // DNF_GAME_TAIWAN_QUERIES_H_
