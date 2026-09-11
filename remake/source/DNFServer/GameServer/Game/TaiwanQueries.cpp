// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TaiwanQueries/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "TaiwanQueries/base64_decode.inc"
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

#include "TaiwanQueries/COpensslRSA.inc"

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

#include "TaiwanQueries/dtor_COpensslRSA.inc"


#include "TaiwanQueries/Init.inc"


#include "TaiwanQueries/Init_1.inc"


#include "TaiwanQueries/Decrypt.inc"


#include "TaiwanQueries/Encrypt.inc"


namespace Taiwan {

// ---------------------------------------------------------------------------
// GarenaAuthData getter/setter（读侧网络序转换与 ORIG ntohl/ntohs 调用一致）
// ---------------------------------------------------------------------------

#include "TaiwanQueries/getUid.inc"


#include "TaiwanQueries/getUname.inc"


#include "TaiwanQueries/getGcaType.inc"


#include "TaiwanQueries/getMobileAuth.inc"


#include "TaiwanQueries/getGoldMemberType.inc"


#include "TaiwanQueries/getTimeStamp.inc"


#include "TaiwanQueries/getPatchTime.inc"


#include "TaiwanQueries/setPatchTime.inc"


// ---------------------------------------------------------------------------
// AuthGarenaToken
// ---------------------------------------------------------------------------

// ORIG B 0x09404754（静态对象；本体由 __static_initialization 构造）
// [SplitC batch10 · 档2] openSSLRSA_ 定义已迁出至 TaiwanAuth.cpp（ORIG GI 键对齐；构造序敏感）

#include "TaiwanQueries/init.inc"


#include "TaiwanQueries/init_1.inc"


#include "TaiwanQueries/decrypt.inc"


// ---------------------------------------------------------------------------
// usp Query 族
// ---------------------------------------------------------------------------

// ---- EntranceQuery --------------------------------------------------------

#include "TaiwanQueries/EntranceQuery.inc"

#include "TaiwanQueries/setQuery.inc"
#include "TaiwanQueries/execQuery.inc"
#include "TaiwanQueries/getResult.inc"

// ---- BalanceQuery ---------------------------------------------------------

#include "TaiwanQueries/BalanceQuery.inc"


#include "TaiwanQueries/setQuery_1.inc"


#include "TaiwanQueries/execQuery_1.inc"


#include "TaiwanQueries/getResult_1.inc"


// ---- BalancePointQuery ----------------------------------------------------

#include "TaiwanQueries/BalancePointQuery.inc"


#include "TaiwanQueries/setQuery_2.inc"


#include "TaiwanQueries/execQuery_2.inc"


#include "TaiwanQueries/getResult_2.inc"


// ---- GiftQuery ------------------------------------------------------------

#include "TaiwanQueries/GiftQuery.inc"


#include "TaiwanQueries/setQuery_3.inc"


#include "TaiwanQueries/execQuery_3.inc"


#include "TaiwanQueries/getResult_3.inc"


#include "TaiwanQueries/ConfirmQuery.inc"


#include "TaiwanQueries/setQuery_4.inc"


#include "TaiwanQueries/execQuery_4.inc"


#include "TaiwanQueries/getResult_4.inc"


// ---- CancelQuery ----------------------------------------------------------

#include "TaiwanQueries/CancelQuery.inc"


#include "TaiwanQueries/setQuery_5.inc"


#include "TaiwanQueries/execQuery_5.inc"


#include "TaiwanQueries/getResult_5.inc"


// ---- ChargeQuery ----------------------------------------------------------

#include "TaiwanQueries/ChargeQuery.inc"


#include "TaiwanQueries/setQuery_6.inc"


#include "TaiwanQueries/execQuery_6.inc"


#include "TaiwanQueries/getResult_6.inc"


// ---- CeraPointQuery -------------------------------------------------------

#include "TaiwanQueries/CeraPointQuery.inc"


#include "TaiwanQueries/setQuery_7.inc"


#include "TaiwanQueries/execQuery_7.inc"


#include "TaiwanQueries/getResult_7.inc"


// ---------------------------------------------------------------------------
// Sig 族
// ---------------------------------------------------------------------------

#include "TaiwanQueries/init_2.inc"


#include "TaiwanQueries/set.inc"


#include "TaiwanQueries/init_3.inc"


#include "TaiwanQueries/set_1.inc"


#include "TaiwanQueries/setData.inc"


#include "TaiwanQueries/init_4.inc"


#include "TaiwanQueries/set_2.inc"


#include "TaiwanQueries/set_3.inc"


#include "TaiwanQueries/init_5.inc"


#include "TaiwanQueries/SigLoadHeroMissionData.inc"


#include "TaiwanQueries/init_6.inc"


#include "TaiwanQueries/SigSaveHeroMissionData.inc"


// ---------------------------------------------------------------------------
// Garena 自由函数
// ---------------------------------------------------------------------------

#include "TaiwanQueries/GarenaCyberCafe.inc"


#include "TaiwanQueries/GarenaCheckTimeStamp.inc"


#include "TaiwanQueries/GarenaAuthPCRoom.inc"
   // guard 析构回收 stream（ORIG D1 调用一致）

}  // namespace Taiwan
