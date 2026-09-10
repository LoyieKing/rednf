// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 TaiwanQueries/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
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
#include "TaiwanQueries/base64_decode.inc"
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
#include "TaiwanQueries/COpensslRSA.inc"
}

// ORIG W 0x0816c246：key 非空 RSA_free 后置空。
// 第三方 libcrypto（OpenSSL）豁免（§4.5 例外，不转不违例）：C 符号需 extern "C" 链接。
extern "C" void RSA_free(void* rsa);

#include "TaiwanQueries/dtor_COpensslRSA.inc"
}

// ORIG T 0x0816c26e：Init("publickey.pem")。
#include "TaiwanQueries/Init.inc"
}

// ORIG T 0x0816c28a：path 空 → false；BIO_new(BIO_s_file()) →
// BIO_ctrl(bio,0x6c,3,(GetAppFolder()+path).c_str())（BIO_C_SET_FILENAME）→
// PEM_read_bio_RSA_PUBKEY(bio,0,0,0) 存 m_rsa → BIO_free_all → key 非空真。
// b28d-36 判词（ORIG 0x0816c28a）已被 b28d-solo-D02 推翻并修真：
// **IDENTICAL（83=83，difflines=0）** —— 修法 = 把 full/rel 两 string 收进内层
// 作用域（析构点=BIO_ctrl 之后、PEM 之前，与 ORIG 0x0816c31d/0x0816c345 一致）
// ⇒ 后续 return 语句脱离 EH region，4.4 不再用 %ebx 汇聚返回值、unwind 块位置
// 同时归位（原判词的 N104 EH 尾块序 + P78 返回寄存器两处残差一次性消失）。
// 原判词保留备查：
// ①EH 尾块相对序——ORIG 正常路径尾块居最末、unwind 块居中（0x816c308..0x816c345），
// 4.4 把同一段 epilogue 就地内联、unwind 压最末，difflib 放大成 ~24 行 rotate；
// ②返回汇聚寄存器——ORIG 各 return 直接 `mov $0/1,%eax`（0x816c298/0x816c38c/0x816c393），
// 4.4 因 return 前需析构 full/rel 两个 string 而用 %ebx 汇聚、尾 `mov %ebx,%eax`。
#include "TaiwanQueries/Init_1.inc"
}

// ORIG T 0x0816c532：rsaSize=RSA_size(key)；pos=0 起按块
// min(len-pos, rsaSize) RSA_public_decrypt(...,RSA_PKCS1_PADDING=1)，块失败
// 即整体失败；成功块 insert 到 out 尾部，直至 pos>=len。
#include "TaiwanQueries/Decrypt.inc"
}

// ORIG T 0x0816c3a2：rsaSize=RSA_size(key)；maxChunk=rsaSize-0xc；pos=0 起按块
// min(len-pos, maxChunk) RSA_private_encrypt(...,RSA_PKCS1_PADDING=1)（块长上限
// rsaSize-12 预留 PKCS#1 填充），块失败即整体失败；成功块 insert 到 out 尾部，
// 剩余不足一块时返回 true。
#include "TaiwanQueries/Encrypt.inc"
}

namespace Taiwan {

// ---------------------------------------------------------------------------
// GarenaAuthData getter/setter（读侧网络序转换与 ORIG ntohl/ntohs 调用一致）
// ---------------------------------------------------------------------------

// ORIG T 0x08172fc8
#include "TaiwanQueries/getUid.inc"
}

// ORIG T 0x08172fde
#include "TaiwanQueries/getUname.inc"
}

// ORIG T 0x08173008
#include "TaiwanQueries/getGcaType.inc"
}

// ORIG T 0x08173022
#include "TaiwanQueries/getMobileAuth.inc"
}

// ORIG T 0x0817304e
#include "TaiwanQueries/getGoldMemberType.inc"
}

// ORIG T 0x08172fea
#include "TaiwanQueries/getTimeStamp.inc"
}

// ORIG T 0x08173042
#include "TaiwanQueries/getPatchTime.inc"
}

// ORIG T 0x08173034
#include "TaiwanQueries/setPatchTime.inc"
}

// ---------------------------------------------------------------------------
// AuthGarenaToken
// ---------------------------------------------------------------------------

// ORIG B 0x09404754（静态对象；本体由 __static_initialization 构造）
COpensslRSA AuthGarenaToken::openSSLRSA_;

// ORIG T 0x081730c2
#include "TaiwanQueries/init.inc"
}

// ORIG T 0x081730d6：param==""（operator== 成立）走默认公钥，否则指定路径；
// 返回对应 Init 的布尔结果。
// [b28d-solo-D02] verdict=DIFF（96 vs 91，N95/N104 族封顶）。本轮已修：
// ①arg 去判空三目（P171 照抄：ORIG 0x08173115 直接压 path 形参）；
// ②empty/arg 的 allocator 临时形与 ORIG 同构（0x8b83f20 空串字面量）。
// 残差 = 两 string 与 allocator 临时槽的**绝对槽位**（ORIG empty -0x18 /
// alloc -0x11 / arg -0x10 / alloc -0x9；我方 empty -0x10 / arg -0x14，方向相反）
// 与 unwind 清理链块序（ORIG -0x10→-0x9→-0x18→-0x11）——即 4.1「先声明深」
// vs 4.4「先声明浅」的槽序（P309/N95）+ EH 块排列（N104）。交换声明序会反转
// 构造指令序（ORIG 先构造 empty），两形互斥；探针三形不可兼得 ⇒ 封顶。
#include "TaiwanQueries/init_1.inc"
}

// ORIG T 0x081731f2
// [b28d-solo-D02] verdict=DIFF（227 vs 229，N95/N104 族封顶）。本轮已修：
// ①两处 `size()?&v[0]:0` 三目删（P171 直取，ORIG 0x08173334/0x08173423）；
// ②日志参数去 `&0xffff`/`&0xff` 冗余掩码（ORIG movzbl/movzwl 已零扩展）；
// ③两处 cMyTrace 改全表达式临时链（P221：失败 trace 槽 -0x64→-0x4c）；
// ④getUname/getUid 改实参内联（P0：ORIG 无槽、值留 %edi/%esi 跨 ctor）；
// ⑤token 具名局部照抄（ORIG 0x08173244 `mov %eax,-0x20`）。
// 残差 = ①标量槽区分配序（ORIG ok -0x19/token -0x20/gold -0x74/gca -0x70/
// stamp -0x6c 与我方相反 ⇒ 4.1 先声明深 vs 4.4 先声明浅，N95/N126）；
// ②帧 0x8c vs 0x80 + %edi push 缺失（4.4 不把 uname 升格 callee-saved；
// 全表达式形已是 P226/P227 上限）；③EH 清理链块序（N104）。
// p167 仅 C1/C2、D1/D2 构造/析构变体差（clean 归一口径 CLEAR）⇒ 非缺段。
#include "TaiwanQueries/decrypt.inc"
}

// ---------------------------------------------------------------------------
// usp Query 族
// ---------------------------------------------------------------------------

// ---- EntranceQuery --------------------------------------------------------

// ORIG T 0x08173ffa（C2 同址别名）
#include "TaiwanQueries/EntranceQuery.inc"

#include "TaiwanQueries/setQuery.inc"
#include "TaiwanQueries/execQuery.inc"
#include "TaiwanQueries/getResult.inc"
#include "TaiwanQueries/BalanceQuery.inc"
#include "TaiwanQueries/setQuery_1.inc"
}

// ORIG T 0x08174076
#include "TaiwanQueries/execQuery_1.inc"
}

// ORIG T 0x08174092
#include "TaiwanQueries/getResult_1.inc"
}

// ---- BalancePointQuery ----------------------------------------------------

// ORIG T 0x08174290（C2 同址别名）
#include "TaiwanQueries/BalancePointQuery.inc"
#include "TaiwanQueries/setQuery_2.inc"
}

#include "TaiwanQueries/execQuery_2.inc"
}

// ORIG T 0x081742fc
#include "TaiwanQueries/getResult_2.inc"
}

// ---- GiftQuery ------------------------------------------------------------

// ORIG T 0x081747fe（C2 同址别名）
// N76 封顶：uid 写入块 4.4 恒 LHS(this/ecx) 先物化、ORIG 4.1 恒 RHS(槽回读)先，
// 3 行时序差不可翻；函数体赋值序（price,pay,uid）已复原，帧 0x28 一致。
#include "TaiwanQueries/GiftQuery.inc"
}

// ORIG T 0x08174866
#include "TaiwanQueries/setQuery_3.inc"
}

#include "TaiwanQueries/execQuery_3.inc"
}

// ORIG T 0x081748d6
#include "TaiwanQueries/getResult_3.inc"
}

// ---- ConfirmQuery ---------------------------------------------------------
// ORIG T 0x08174ac6（C2 同址别名）：trans 前 12 字节逐 dword 拷贝进 m_trans；
// ORIG 尾部 movl $0x0,(%eax) 清零 m_resultCode（+0x00）。
#include "TaiwanQueries/ConfirmQuery.inc"
}

// ORIG T 0x08174aec
#include "TaiwanQueries/setQuery_4.inc"
}

#include "TaiwanQueries/execQuery_4.inc"
}

// ORIG T 0x08174b44
#include "TaiwanQueries/getResult_4.inc"
}

// ---- CancelQuery ----------------------------------------------------------

// ORIG T 0x08174ce2（C2 同址别名）；ORIG 尾部同样清零 m_resultCode（+0x00）。
#include "TaiwanQueries/CancelQuery.inc"
}

// ORIG T 0x08174d08
#include "TaiwanQueries/setQuery_5.inc"
}

#include "TaiwanQueries/execQuery_5.inc"
}

// ORIG T 0x08174d60
#include "TaiwanQueries/getResult_5.inc"
}

// ---- ChargeQuery ----------------------------------------------------------

// ORIG T 0x08174efe（C2 同址别名）
#include "TaiwanQueries/ChargeQuery.inc"
}

// ORIG T 0x08174f52：%d 打印有符号 chargeType。
#include "TaiwanQueries/setQuery_6.inc"
}

#include "TaiwanQueries/execQuery_6.inc"
}

// ORIG T 0x08174fc6
#include "TaiwanQueries/getResult_6.inc"
}

// ---- CeraPointQuery -------------------------------------------------------

// ORIG T 0x081751b6（C2 同址别名）
#include "TaiwanQueries/CeraPointQuery.inc"
}

// ORIG T 0x08175224：%d 参数序 price/c6/StringToNumber(uidStr)/c8 与 ORIG 一致。
// [b28BM13 N7] 封顶：指令/槽/助记符全等，仅 5 个 callee-saved 分配整体错位
// （4.1 首位 edx vs 4.4 首位 edi）。register 绑定按 P0 全域禁用（旧 asm("edx")
// 尝试注记作废，实测亦劣化），穷举后封顶。
#include "TaiwanQueries/setQuery_7.inc"
}

#include "TaiwanQueries/execQuery_7.inc"
}

// ORIG T 0x081752cc
#include "TaiwanQueries/getResult_7.inc"
}

// ---------------------------------------------------------------------------
// Sig 族
// ---------------------------------------------------------------------------

// ORIG W 0x0815df38
#include "TaiwanQueries/init_2.inc"
}

// ORIG W 0x0815df50
#include "TaiwanQueries/set.inc"
}

// ORIG W 0x0815de9c
#include "TaiwanQueries/init_3.inc"
}

// ORIG W 0x0815decc：dst = this+4+(count<<4)，strncpy 15 字节后 ++count。
#include "TaiwanQueries/set_1.inc"
}

// ORIG W 0x0815df06
#include "TaiwanQueries/setData.inc"
}

// ORIG W 0x0815dda8
#include "TaiwanQueries/init_4.inc"
}

// ORIG W 0x0815de02：restrictValue 写 +0；flag 写 m_extra[index]（0x119c 段）；
// 计数自增。index 无界（ORIG 同样直接下标）。
#include "TaiwanQueries/set_2.inc"
}

// ORIG W 0x0815de38：整结构拷贝（两段 memcpy 同型展开）。
#include "TaiwanQueries/set_3.inc"
}

// ORIG W 0x08163a04
#include "TaiwanQueries/init_5.inc"
}

// ORIG W 0x081639f0（C2 同址别名）
#include "TaiwanQueries/SigLoadHeroMissionData.inc"
}

// ORIG W 0x081639c0
#include "TaiwanQueries/init_6.inc"
}

// ORIG W 0x081639ac（C2 同址别名）
#include "TaiwanQueries/SigSaveHeroMissionData.inc"
}

// ---------------------------------------------------------------------------
// Garena 自由函数
// ---------------------------------------------------------------------------

// ORIG T 0x0817305a
#include "TaiwanQueries/GarenaCyberCafe.inc"
}

// ORIG T 0x08173080
#include "TaiwanQueries/GarenaCheckTimeStamp.inc"
}

// ORIG T 0x08471c74
#include "TaiwanQueries/GarenaAuthPCRoom.inc"
}   // guard 析构回收 stream（ORIG D1 调用一致）

}  // namespace Taiwan
