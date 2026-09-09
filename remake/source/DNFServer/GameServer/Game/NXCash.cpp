// ============================================================================
// nexon::cash::NCashImpl —— NEXON 计费协议实现（独立 TU）。
//
// [GI 拆分] 本文件由 nexon_cash.cpp 拆出：ORIG 侧 NCashImpl 属独立编译单元
// `NXCash.cpp`，其 _GLOBAL__I_ 键名 = 本 TU 首个「强符号 · 外部链接 · 本 TU
// 定义」实体 = _ZN5nexon4cash9NCashImplC2EPKcshts（ORIG @0x081ae511，
// 对应函数体 @0x081acfdc）。故 NCashImpl ctor 必须保持为本文件第一处外联
// 定义（P327：外联 ctor 取 C2 形）。
//
// NexonCash 薄包装仍留在 nexon_cash.cpp：ORIG 该 TU 内 NexonCash ctor 位于
// NCashImpl ctor 之后，而我方需 NexonCash ctor「只见 NCashImpl ctor 原型」
// 以复现 new-expression 的保守 EH cleanup（delete+_Unwind_Resume 岛，
// ORIG 0x081ae21b-0x081ae232）。若把 NexonCash 并入本文件则其定义必须先于
// NCashImpl ctor（同 TU 前置），GI 键名将变成 NexonCashC2 而非 want。
//
// 逐符号按 df_game_r ORIG 反汇编转写；地址见各函数注释。
// ============================================================================
#include "nexon_cash.h"

#include "GameTypes.h"

// ORIG 该 TU 含 file-scope 动态初始化（0x081ae4d1
// __static_initialization_and_destruction_0 → ios_base::Init::C1Ev +
// __cxa_atexit(D1)）；include <iostream> 是 GI 的发射前提（P327）。
#include <iostream>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <wchar.h>

namespace Encryption
{
unsigned int Adler32(char* data, unsigned int length);                      // _ZN10Encryption7Adler32EPcj
void EncodeDressStringWide(const wchar_t* in, wchar_t* out,
                           unsigned long param);                            // _ZN10Encryption21EncodeDressStringWideEPKwPwm
// [b28d-Main·检查点22] 删除原先的 `__attribute__((weak))` 空 stub：
//   ① 二者在 ORIG 里是 **U 引用**（`nm $O` = U，由 NEEDED `libnxencryption.so`
//      在运行期提供；0x0807e190/0x0807e8e0 为 PLT 调用点），我方在 Game/ 源码内
//      发射 W 定义 ⇒ 属「我方 .cpp 发明导出符号」（only_new 2 项）。
//   ② 空 stub 运行期返回 0/不写 out，与厂商实现不一致（行为差异）。
//   现由 `toolchains/nxencryption/libnxencryption.so`（厂商真实二进制，已入仓）
//   参与链接：本 TU 只留声明 ⇒ 我方 df_game_r 变成 U + NEEDED libnxencryption.so，
//   与 ORIG readelf/nm 完全同型。
}  // namespace Encryption

namespace nexon {
namespace cash {

// ---------------------------------------------------------------------------
// NCashMessage 定义必须前置于 NCashImpl ctor 且保持 inline（P210 帧继承）
//
// NCashImpl ctor 调用 m_inMsg()/m_outMsg()（NCashMessage::C1）。拆 TU 前二者
// 同处 nexon_cash.cpp 且 NCashMessage 定义在前 ⇒ 4.4 精确继承 callee outgoing；
// 拆分后若本 TU 只见声明，4.4 转保守形，C1 prologue 由 ORIG 的
// `push %ebx; sub $0x24` 退化为 `push %esi; push %ebx; sub $0x20`（DIFF）。
// 实测三形（均定义前置）：
//   · 普通 out-of-line 定义 ⇒ C1 恢复，但 NCashMessageC2 成为 TU 首强符号，
//     GI 键名被顶成 NCashMessageC2（≠ want）；
//   · __attribute__((weak)) 定义 ⇒ C1 仍 DIFF（4.4 对 weak callee 不做帧继承）；
//   · inline 定义 ⇒ C1 恢复且符号为 W（不参与 GI 命名）⇒ 唯一可行形。
// 符号类型随之与 ORIG 一致（ORIG 该 8 符号全为 W）；代价为 4.4 额外发射
// local 别名 `_ZN5nexon4cash12NCashMessageC5Ev`（n 类型、地址 0、结构性噪音）。
// ---------------------------------------------------------------------------
inline NCashMessage::NCashMessage()
    : m_magic('U'), m_bodyLength(0)
{
}

inline unsigned char* NCashMessage::data()
{
    return &m_wireMagic;  // this+4
}

inline unsigned char* NCashMessage::body()
{
    return &m_bodyBytes[0];  // this+7
}

inline unsigned short NCashMessage::body_length()
{
    return m_bodyLength;
}

// ORIG 0x081ae54e：return u16(this+2)+3。
inline int NCashMessage::length() const
{
    return static_cast<int>(m_bodyLength) + 3;
}

// ORIG 0x081ae578（W）：wireMagic='U'; *(u16*)netLen=htons(bodyLength) 指针直存
// （dst 0x081ae585 add $5 单步 + 0x081ae588 mov %eax,%ebx 先载入，技巧27）。
inline bool NCashMessage::encode_header()
{
    m_wireMagic = 'U';
    *reinterpret_cast<unsigned short*>(m_netLenB) = htons(m_bodyLength);
    return true;
}

// ORIG 0x081ae5ae：bodyLength 参数直存 +0x02 后调用 encode_header()。
inline void NCashMessage::encode_header(unsigned int bodyLength)
{
    m_bodyLength = static_cast<unsigned short>(bodyLength);
    encode_header();
}

// ORIG 0x081ae5ce（W）：m_bodyLength=ntohs(*(u16*)netLen)（0x081ae5d7 add $5
// 单步后 movzwl 直读，无 copy 槽）；u16 无符号 cmp $0x200 超 → 清零返 false。
inline bool NCashMessage::decode_header()
{
    m_bodyLength = ntohs(*reinterpret_cast<unsigned short*>(m_netLenB));
    if (m_bodyLength > 0x200)
    {
        m_bodyLength = 0;
        return false;
    }
    return true;
}
// ORIG 0x081acfdc：NCashImpl ctor 本体（TU 首符号 ⇒ GI 键名 C2 形）。
NCashImpl::NCashImpl(const char* serverAdrs, short port, unsigned char paramB,
                     unsigned short fieldC, short fieldD)
    : m_socket(), m_inMsg(), m_outMsg()
{
    // ORIG 0x081acfdc：子对象构造（m_socket@+0x20、m_inMsg@+0x40、m_outMsg@+0x248）
    // 后依次写 m_requestID=0、m_paramB、m_fieldC、m_fieldD、m_connected=0、
    // strcpy(m_serverAdrs, serverAdrs)（无 NULL 检查/预清零）、m_port。
    m_requestID = 0;
    m_paramB = paramB;
    m_fieldC = fieldC;
    m_fieldD = fieldD;
    m_connected = false;
    strcpy(m_serverAdrs, serverAdrs);
    m_port = port;
}

void NCashImpl::_increaseRequestID()                 // ORIG 0x081aea32
{
    ++m_requestID;
}

// NCashImpl 四个 coupon 定义在文件尾：
// ORIG 编译单元内调用点仅见原型 → 宽松帧 sub $0x18（技巧31 扩展）。

// ORIG 0x081aea46（W）：MSG_WAITALL(0x100) 预读头并解码，随后转 recv_body。
// buf → %ebx（0x081aea58 call data 后直落寄存器，技巧2），getHandle 结果免槽
// 直填第1参（0x081aea66-0x081aea7b），n 槽 -0xc，frame 0x24 + pop ebx。
int NCashImpl::recv_msg()
{
    // [b28d-D43] P0/N96 清绑：register asm("ebx") 删除，改实参内联形。
    int n = static_cast<int>(::recv(m_socket.getHandle(), m_inMsg.data(), 3, 0x100));
    if (n == 0)
    {
        printf("[%s] connection lost: %s\n", __FUNCTION__,
               strerror(errno));
        m_connected = false;
        m_socket.close();
        return 2;
    }
    if (n < 0)
    {
        printf("[%s] recv error: %s\n", __FUNCTION__, strerror(errno));
        m_connected = false;
        m_socket.close();
        return 2;
    }

    m_inMsg.decode_header();
    return recv_body();
}

// ORIG 0x081aeb3c：按 body_length 继续收 body 区（flags 同 0x100）。
int NCashImpl::recv_body()
{
    // [b28 BM11] N96/P0 清绑：register asm("esi")/asm("ebx") 存量删除，改实参
    // 内联形（P200/P226 方向）：body_length→body→getHandle 的 R2L 求值序与
    // ORIG 装载序同构；跨 body call 存活的 ushort 零扩展值由 4.4 升格
    // callee-saved。
    int n = static_cast<int>(::recv(
        m_socket.getHandle(), m_inMsg.body(),
        m_inMsg.body_length(), 0x100));
    if (n == 0)
    {
        printf("[%s] connection lost: %s\n", __FUNCTION__, strerror(errno));
        m_connected = false;
        m_socket.close();
        return 2;
    }
    if (n < 0)
    {
        printf("[%s] recv error: %s\n", __FUNCTION__, strerror(errno));
        m_connected = false;
        m_socket.close();
        return 2;
    }
    return 0;
}

// ORIG 0x081aec2c（W）：encode(totalLen) → 缓存 length→%esi / data→%ebx
// （0x081aec5c/0x081aec6e 直用寄存器，技巧2），getHandle 结果免槽直填第1参
// （0x081aec79-0x081aec8e，技巧37）；n 槽 -0xc；比较对象是**重调用** length()
// （0x081aeca4-0x081aeca9 cmp -0xc,%eax + setne/test，技巧9 bool 形）；
// 错误分支 strerror 结果 → %ebx 后重 call length 填 printf（0x081aecc4-0x081aecd4）。
int NCashImpl::send_msg(unsigned int totalLen)
{
    m_outMsg.encode_header(totalLen);
    // [b28d-D43] P0/N96 清绑：register asm("esi")/asm("ebx") 删除，改实参内联形。
    int n = static_cast<int>(::send(m_socket.getHandle(), m_outMsg.data(),
                                    m_outMsg.length(), 0x100));
    if (n != static_cast<int>(m_outMsg.length()))   // 0x081aeca9 cmp -0xc,%eax
    {
        // [b28d-D43] P0/N96 清绑：err 改内联实参。
        printf("send error: send bytes: %d, message length: %d, error: %s\n",
               n, static_cast<int>(m_outMsg.length()), strerror(errno));
        m_connected = false;
        m_socket.close();
        return 2;
    }
    return 0;
}

// ORIG 0x081ad080：未连接则重开重连；返回连接态。
bool NCashImpl::reconnect()
{
    if (!m_connected)
    {
        m_socket.close();
        m_socket.open();
        m_connected = m_socket.connect(m_serverAdrs,
                                       static_cast<unsigned short>(m_port));
    }
    return m_connected;
}

// ORIG 0x081ad0e6（sret）：ByteBuffer(out.body(),0x200)<<pid<<flag<<requestID。
ByteBuffer NCashImpl::set_data_header(unsigned char packetId, unsigned char flag)
{
    ByteBuffer out(reinterpret_cast<char*>(m_outMsg.body()), 0x200);
    out << packetId;
    out << flag;
    _increaseRequestID();
    out << m_requestID;
    return out;
}

// ORIG 0x081ad168：从入包 body 读 pid/flag/requestId 三元组。
// [b23S3C] P192：sret 返回入包视图对象（NRVO 直构 caller 槽，无额外拷贝）。
// [b28d-D43] NEAR 帧量化：ORIG sub $0x10 vs 4.4 $0xc（P190附注②；体 39/39 全等）（ORIG 0x081ad168）。
ByteBuffer NCashImpl::get_data_header(unsigned char& a, unsigned char& b,
                                      unsigned int& c)
{
    // [b28d-D43] ORIG 0x081ad168 无 in 的 ctor call（wrap 的 sret 即 in 槽，
    // %ebx=get_data_header sret 槽）：直接初始化形令 4.4 把 in 地址传给 wrap。
    // [b28d-solo-nexon_cash] N 封顶（帧量化跨版本）：ORIG `sub $0x10` vs 4.4
    // `sub $0xc`；实测补 4 字节未用局部跳 $0x1c，$0x10 不可达（P190 附注②）。
    // 指令 39/39 全等，仅帧 sub/add 立即数差，语义等价。
    ByteBuffer in = ByteBuffer::wrap(reinterpret_cast<char*>(m_inMsg.body()),
                                     m_inMsg.body_length());
    in >> a;
    in >> b;
    in >> c;
    return in;
}

// ORIG 0x081ad1e0：switch(err-5) 跳转表分支（默认 16）。
int NCashImpl::handle_cash_error(int err)
{
    int result = 0x10;
    // ORIG 跳转表（0x8bb42d4）逐项：case0=err5→r3"E_GIFT_LIMIT_OVER Remain cash"、
    // case1=err6→r4"NO REMIN CASH"、case2=err7→default(r0x10)、case3=err8→r5"E_GIFT_NOT_READY
    // error"、case4=err9→r6"E_GIFT_UNDER_AGE error"、case5=err10→r7"Undefined cash error"。
    switch (err - 5)
    {
    case 1:      // 置于 case0 前，使块1=r4（NO REMIN）、块2=r3（GIFT_LIMIT）匹配 ORIG 块序
        result = 4;
        printf("[%s] NO REMIN CASH\n", __FUNCTION__);
        break;
    case 0:
        result = 3;
        printf("[%s] E_GIFT_LIMIT_OVER Remain cash\n", __FUNCTION__);
        break;
    case 3:
        result = 5;
        printf("[%s] E_GIFT_NOT_READY error\n", __FUNCTION__);
        break;
    case 4:
        result = 6;
        printf("[%s] E_GIFT_UNDER_AGE error\n", __FUNCTION__);
        break;
    case 5:
        result = 7;
        printf("[%s] Undefined cash error\n", __FUNCTION__);
        break;
    case 2:      // 与 default 同流（跳转表同项）
    default:
        result = 0x10;
        printf("[%s] Undefined cash error: %u\n", __FUNCTION__,
               static_cast<unsigned>(err));
        break;
    }
    return result;
}

// ORIG 0x081ad2c0：0..3 静默；default 打印；恒返回 16。
// case 0..3 → goto 共用出口标号（技巧23/24）：复现 ORIG 0x081ad2d0-0x081ad2ff
// 二分树 dispatch（cmp$1 je / cmp$1 jg / test je / jmp / cmp$2 je / cmp$3 je）
// + 每 case `nop; jmp merge` 岛；直接 break 会被 4.4 折成 jbe 范围形。
int NCashImpl::handle_coupon_error(int err)
{
    int result = 0x10;
    switch (err)
    {
    case 0: goto Lsilent;
    case 1: goto Lsilent;
    case 2: goto Lsilent;
    case 3: goto Lsilent;
    default:
        printf("[%s] Invalid coupon packet_id: %u\n", __FUNCTION__,
               static_cast<unsigned>(err));
        break;
    }
Lsilent:
    return result;
}

// ORIG 0x081ad316：手组包头（不走 set_data_header！仅自增 requestID 不写入）。
// [b28d-solo-nexon_cash] N 封顶：rc 预清/三元组无预清/声明序已归位后，残余
// 仅为 ORIG 0x081ad430 wrap 实参装载寄存器差（ORIG `lea in,%edx; mov body,%eax`
// vs 4.4 `mov body,%edx; lea in,%eax`，P75/N7 跨版本装载调度，our 107 vs
// orig 106），语义等价。
int NCashImpl::checkEntrance(const char* userName, char& outStatus)
{
    if (!reconnect())
        return 2;

    unsigned char typeByte = static_cast<unsigned char>(m_fieldD);

    ByteBuffer out(reinterpret_cast<char*>(m_outMsg.body()), 0x200);
    out << static_cast<unsigned char>(0x03);
    out << static_cast<unsigned char>(0x01);
    out << typeByte;
    out << userName;
    _increaseRequestID();  // ORIG: 自增但不入包

    // [b28d-solo-nexon_cash] P198①：ORIG 0x081ad3a9 预清 rc 后才 call send_msg。
    int rc = 0;
    rc = send_msg(out.length());
    if (rc != 0)
        return rc;

    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b28d-solo-nexon_cash] P212/P171：ackPid/statusByte 先声明占浅槽
    // （ORIG -0x21/-0x22）、in 后声明占深槽 -0x34；ORIG 无 movb $0 预清。
    unsigned char ackPid;
    unsigned char statusByte;
    ByteBuffer in = ByteBuffer::wrap(reinterpret_cast<char*>(m_inMsg.body()),
                                     m_inMsg.body_length());
    in >> ackPid;
    in >> statusByte;
    outStatus = static_cast<char>(statusByte);
    printf("[%s] CheckEntrace: %d\n", __FUNCTION__,
           static_cast<int>(outStatus));  // ORIG 格式串 typo 忠实保留
    return 0;
}

// ORIG 0x081ad49e：packet_id=0 应答 err==0 后读余额。
// [b28d-D43] N82/N95 帧/槽平移（81/104）（ORIG 0x081ad49e）。
int NCashImpl::getBalance(const char* userId, int& balanceOut)
{
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x00);
    msg << userId;

    // [b28d-solo-nexon_cash] P198①：ORIG 0x081ad4de 预清 rc 后才 call send_msg。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;

    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b28d-solo-nexon_cash] 真语义 bug：ORIG 0x081ad58c 直接复用
    // get_data_header 的 sret 对象（-0x34）继续 `in >> balanceOut`（读 body
    // 第 4 个字段）；旧形重新 wrap 入包 body 从头读（字段错位 + 多 3 调用）。
    // P171/P212：三元组无预清；具名 in 承载 sret（与 msg@-0x1c 同布局）。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    ByteBuffer in = get_data_header(ackPid, errCode, reqId);
    if (errCode != 0)
        return handle_cash_error(errCode);

    in >> balanceOut;
    printf("[%s] Remain cash: %d\n", __FUNCTION__, balanceOut);
    return 0;
}

// ORIG 0x081ad5c2。
int NCashImpl::purchase(unsigned long long productSeq, int cashType,
                        int cashAmount, const char* param1, const char* param2,
                        NXTransaction& trOut)
{
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x01);
    msg << static_cast<short>(m_fieldD);
    msg << productSeq;
    msg << cashType;
    msg << cashAmount;
    msg << param1;
    msg << param2;
    msg << m_paramB;
    msg << static_cast<short>(m_fieldC);

    // [b27L8] ORIG send_msg call 前 movl $0x0,-0xc 预清 ⇒ P198① 两句式。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b27L8] ORIG 无 ackPid/errCode/reqId 预清（movb/movl 缺失）⇒ 未初始化声明。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    // [b23S3C] P192：以 sret 返回值续读 trOut（消本函数 C1/wrap/body/
    // body_length EXTRA×4）。
    ByteBuffer in = get_data_header(ackPid, errCode, reqId);
    if (errCode != 2)
        return handle_cash_error(errCode);

    in >> trOut.m_type;
    in >> trOut.m_id;
    printf("[%s] PURCHASE OK, db_id: %u, transaction_id: %u\n", __FUNCTION__,
           static_cast<unsigned>(trOut.m_type),
           trOut.m_id);
    return 0;
}

// ORIG 0x081ad7ae：subCmd∈{60,62} 时 streamType 替换为 subCmd，其余同上。
int NCashImpl::purchase(unsigned long long productSeq, int cashType,
                        int cashAmount, const char* param1, const char* param2,
                        NXTransaction& trOut, short subCmd)
{
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x01);
    // [b23S3C] 双臂各直推一次（ORIG +0x68/+0x7d 两块内各 call lsEs、jmp 共享
    // 后继）——具名 streamType 合流形少一条 lsEs（P167 实证 MISSING lsEs）。
    if (subCmd == 0x3c || subCmd == 0x3e)
        msg << subCmd;
    else
        msg << static_cast<short>(m_fieldD);
    msg << productSeq;
    msg << cashType;
    msg << cashAmount;
    msg << param1;
    msg << param2;
    msg << m_paramB;
    msg << static_cast<short>(m_fieldC);

    // [b23S3C] P197③：ORIG rc 槽两段式（length 前先 movl $0,-0xc）。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b23S3C] 三元组无预清（ORIG sret call 前无 movb/movl $0，槽仅由
    // get_data_header 填写）。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    // [b23S3C] P192：sret 返回值续读 trOut（消 EXTRA C1/wrap/body/body_length）。
    ByteBuffer in = get_data_header(ackPid, errCode, reqId);
    if (errCode != 2)
        return handle_cash_error(errCode);

    in >> trOut.m_type;
    in >> trOut.m_id;
    printf("[%s] PURCHASE OK, db_id: %u, transaction_id: %u\n", __FUNCTION__,
           static_cast<unsigned>(trOut.m_type),
           trOut.m_id);
    return 0;
}

// ORIG 0x081ad9c4：两段附加串位于 m_paramB/m_fieldC 之后。
// [b28d-D43] N82 帧：ORIG sub $0x68 vs 4.4 push %ebx+sub $0x74（ORIG 0x081ad9c4）。
int NCashImpl::gift(unsigned long long productSeq, int cashType,
                    int cashAmount, const char* targetUserId,
                    const char* message, const char* extra1,
                    const char* extra2, NXTransaction& trOut)
{
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x07);
    msg << static_cast<short>(m_fieldD);
    msg << productSeq;
    msg << cashType;
    msg << cashAmount;
    msg << targetUserId;
    msg << message;
    msg << m_paramB;
    msg << static_cast<short>(m_fieldC);
    msg << extra1;
    msg << extra2;

    // [b28d-solo-nexon_cash] P198①：ORIG 0x081adae2 预清 rc 后才 call send_msg。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b28d-solo-nexon_cash] 真语义 bug（同 getBalance）：ORIG 0x081adb81 复用
    // get_data_header 的 sret 对象（-0x34）继续读 trOut.m_type/m_id；旧形重新
    // wrap 入包 body 从头读（字段错位 + 多 C1/body/body_length/wrap 4 调用）。
    // 三元组无预清（P171），具名 in 承载 sret（msg@-0x1c 同布局）。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    ByteBuffer in = get_data_header(ackPid, errCode, reqId);
    if (errCode != 2)
        return handle_cash_error(errCode);

    in >> trOut.m_type;
    in >> trOut.m_id;
    printf("[%s] PURCHASE OK, db_id: %u, transaction_id: %u\n", __FUNCTION__,
           static_cast<unsigned>(trOut.m_type),
           trOut.m_id);
    return 0;
}

// ORIG 0x081adbd4：err!=3 时 handle_cash_error 结果被丢弃仍打印成功行返 0。
// [b28d-D43] N82/N95 同 cancel（帧/槽平移）（ORIG 0x081adbd4）。
int NCashImpl::confirm(unsigned char trFlag, unsigned int trId)
{
    if (trId == 0)
        return 0;
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x03);
    msg << trFlag;
    msg << trId;

    // [b28d-solo-nexon_cash] P198①：ORIG 0x081adc1d `movl $0x0,-0xc` 预清
    // rc 后才 call send_msg ⇒ 必须两句式；合并初始化式丢预清行。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b28d-solo-nexon_cash] P171：ORIG get_data_header 前无 movb/movl $0
    // 预清（槽仅由被调填写）⇒ 三元组声明不得带初始化。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    // [b28d-solo-nexon_cash] P212：具名 in 承载 get_data_header 的 sret，
    // 令 4.4 槽位与 ORIG 全等（msg@-0x1c / sret@-0x34）。
    ByteBuffer in = get_data_header(ackPid, errCode, reqId);
    (void)in;
    // [b28d-solo-nexon_cash] 真语义 bug：ORIG 0x081adce3..0x081adcf0 为
    // errCode!=3 → call handle_cash_error; jmp <leave>（跳过 printf 并返回
    // 其错误码）；旧形忽略返回值后继续打印成功行 ⇒ 改为 return。
    if (errCode != 3)
        return handle_cash_error(errCode);
    printf("[%s] E_PURCHASE_CONFIRM\n", __FUNCTION__);
    return 0;
}

// ORIG 0x081add04：与 confirm 同构，packet_id=4、__FUNCTION__="cancel"。
// [b28d-D43] N82/N95 帧/槽平移：set_data_header 返回槽 ORIG -0x1c vs 4.4 -0x2c（ORIG 0x081add04）。
int NCashImpl::cancel(unsigned char trFlag, unsigned int trId)
{
    if (trId == 0)
        return 0;
    if (!reconnect())
        return 2;

    ByteBuffer msg = set_data_header(0x00, 0x04);
    msg << trFlag;
    msg << trId;

    // [b28d-solo-nexon_cash] P198①：同 confirm（ORIG 0x081add86 预清）。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    unsigned char ackPid, errCode;
    unsigned int reqId;
    // [b28d-solo-nexon_cash] P212：具名 in 承载 get_data_header 的 sret，令 4.4
    // 槽位与 ORIG 全等（msg@-0x1c / sret@-0x34）⇒ AE。
    ByteBuffer in = get_data_header(ackPid, errCode, reqId);
    (void)in;
    // [b28d-solo-nexon_cash] 真语义 bug：同 confirm（ORIG 0x081ade11 后
    // 0x081ade16 jmp <leave>，errCode!=3 时不打印）。
    if (errCode != 3)
        return handle_cash_error(errCode);
    printf("[%s] E_PURCHASE_CONFIRM\n", __FUNCTION__);  // 共用格式串
    return 0;
}

// ORIG 0x081ade34：无 reconnect 前置；组"%s|%i|%i|%i|%s|%s|%s|%s|%i" 明文 →
// mbstowcs → Adler32(宽串字节) → EncodeDressStringWide(key=0x1b1bde64) →
// wcstombs 回 ASCII 密文入包 + 协议键 qword + adler qword。
// [b28d-solo-nexon_cash] 本波修真 bug 3 处：①4 个函数级 static 缓冲（ORIG 符号
// _ZZ...E4data/E9wide_data/E8enc_data/E9send_data，尺寸 0x3e8/0xfa0/0xfa0/0x3e8）
// 替代 struct+getter（旧形发明符号 + memset 长度 0x1f40 错）；②格式串 9 token
// （iv/code 为 %i，旧形漏传 pD/code 且 iv 当 char*）；③adler 为 unsigned long long
// （8 字节槽 0x081adfdd）。帧/槽已全对齐；N 封顶残余 = 4.1 局部物化/EH 段差
// （P0/N82 族，our 200 vs orig 196，多数为 .o 重定位伪差）。
int NCashImpl::chargeCash(char* sessionKey, unsigned char h1, unsigned char h2,
                          int iv, char* pA, char* pB, char* pC, char* pD,
                          int code, NXTransaction& trOut)
{
    // [b28d-solo-nexon_cash] ORIG 4 个函数级 static 缓冲（nm 实证）：
    // _ZZ...chargeCash...E4data@0x09405640(0x3e8) / E9wide_data@0x9405a40(0xfa0)
    // / E8enc_data@0x94069e0(0xfa0) / E9send_data@0x9407980(0x3e8)，均 b 段。
    // 旧 struct+ccBufs() getter 造发明符号且访问经局部引用缓存；ORIG 每次直压
    // 静态地址立即数 ⇒ 必须函数内 static 声明。
    // [b28d-solo-nexon_cash] nm 尺寸实证：data/send_data 0x3e8、wide_data/enc_data
    // 0xfa0（wchar_t=4B ⇒ wchar_t[1000]；旧 [2000] 使 memset 长度 0x1f40 与 ORIG
    // 的 0xfa0 不符且符号尺寸翻倍）。
    static char data[1000];
    static wchar_t wide_data[1000];
    static wchar_t enc_data[1000];
    static char send_data[1000];

    memset(data, 0, sizeof(data));
    memset(wide_data, 0, sizeof(wide_data));
    memset(enc_data, 0, sizeof(enc_data));
    memset(send_data, 0, sizeof(send_data));

    ByteBuffer msg = set_data_header(0x02, 0x01);

    // [b28d-solo-nexon_cash] ORIG 0x081ade75..0x081ade83：set_data_header 后
    // 立即 3 条 movl（-0x2c=0xe8c3f80b 未消费槽 + -0x28/-0x24=qword key
    // 0x00000000F3D8266F）⇒ 原码为具名局部（P232 dead 标量照样占槽）。
    int deadField = static_cast<int>(0xe8c3f80bu);
    unsigned long long frameKey = 0x00000000F3D8266FULL;
    (void)deadField;

    // [b28d-solo-nexon_cash] 真语义 bug：ORIG 0x081adefa..0x081adf2a 在栈上
    // 构造 27 字节 fmt = "%s|%i|%i|%i|%s|%s|%s|%s|%i"（9 token），并在
    // 0x081adf3c..0x081adf6a 压 9 个变参 sessionKey,h1,h2,iv,pA,pB,pC,pD,code
    // （0x18(%ebp)=iv 与 0x2c(%ebp)=code 均参与）⇒ 旧形把 iv 当 char* cast、
    // 少传 pD/code 且 fmt 仅 8 token（输出串错位）。
    char fmt[] = "%s|%i|%i|%i|%s|%s|%s|%s|%i";
    snprintf(data, sizeof(data), fmt,
             sessionKey,
             static_cast<int>(h1),
             static_cast<int>(h2),
             iv,
             pA, pB, pC, pD,
             code);

    size_t len = strlen(data) + 1;
    mbstowcs(wide_data, data, len);

    // [b28d-solo-nexon_cash] P0/P211：ORIG 0x081adfc4 的 wcslen 结果直接
    // `lea (%eax,%eax,1),%edx`（无 wlen 具名槽）；Adler32 返回值零扩展后存
    // 8 字节槽（0x081adfdd `mov $0x0,%edx` + -0x18/-0x14）⇒ 原码是
    // unsigned long long adler。
    unsigned long long adler = Encryption::Adler32(
        reinterpret_cast<char*>(wide_data),
        static_cast<unsigned int>(wcslen(wide_data) * 2));

    Encryption::EncodeDressStringWide(wide_data, enc_data,
                                      0x1b1bde64u);

    size_t enclenChars = wcslen(enc_data);
    wcstombs(send_data, enc_data, enclenChars);

    msg << send_data;
    msg << frameKey;                      // [-0x28/-0x24] 帧 key
    msg << adler;

    // [b28d-solo-nexon_cash] P198①：ORIG 预清 rc 后才 call send_msg。
    int rc = 0;
    rc = send_msg(msg.length());
    if (rc != 0)
        return rc;
    rc = recv_msg();
    if (rc != 0)
        return rc;

    // [b28d-solo-nexon_cash] P171：三元组无预清。
    unsigned char ackPid, errCode;
    unsigned int reqId;
    get_data_header(ackPid, errCode, reqId);

    if (errCode != 2)
    {
        // [b28d-solo-nexon_cash] §4.1/P154：删发明符号
        // _ZN5nexon4cash12_GLOBAL__N_114traceRecvErrorEiPKc（ORIG nm 无此符号）。
        // ORIG 0x081ae105..0x081ae145 为就地具名 cMyTrace 两句式（ctor + op()）。
        cMyTrace trace(__FUNCTION__, 0x303, 0);
        trace("NCashImpl::chargeCash Recv Error!! packet_id:%d, data:%s",
              static_cast<int>(errCode), data);
        return static_cast<int>(errCode);
    }

    ByteBuffer in = ByteBuffer::wrap(reinterpret_cast<char*>(m_inMsg.body()),
                                     m_inMsg.body_length());
    in >> trOut.m_type;
    in >> trOut.m_id;
    printf("[%s] CHARGE_CASH OK, db_id: %u, transaction_id: %u\n", __FUNCTION__,
           static_cast<unsigned>(trOut.m_type), trOut.m_id);
    return 0;
}

bool NCashImpl::checkCoupon()     { return false; }   // ORIG 0x081aea0a xor eax,eax
bool NCashImpl::useCoupon()       { return false; }   // ORIG 0x081aea14
bool NCashImpl::checkCafeCoupon() { return false; }   // ORIG 0x081aea1e
bool NCashImpl::useCafeCoupon()   { return false; }   // ORIG 0x081aea28

}  // namespace cash
}  // namespace nexon
