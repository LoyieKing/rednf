// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ExchangeServer_Exchange/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// ExchangeServer_Exchange.cpp —— exchange_server::CExchangeServer 全方法
// OnlyOrig_Exchange.ExExchangeTU 批（Wave5）。
// 逐指令转写自 ORIG 反汇编快照 /tmp/es_disasm.txt：
//   段 _ZN15exchange_server15CExchangeServer* @0x8491004..0x8493a7e +
//   ExchangeServerInfo C1 @0x8491004。
// 权威布局以只读头 ExchangeServer.h（#9F8A 修订版）为准；vtable 实证：
//   CExchangeServer vtable 0x8c7a1d0 = [DispatchPacket, UnregisterExchangeServer,
//   AllocSession, FreeSession, GetServerType]（5 槽，无虚析构）。
// ============================================================================
#include "ExchangeServer.h"

#include <arpa/inet.h>
#include <time.h>

#include "CacheCharacterType.h"
#include "CMonitorServerProxy.h"      // SendExchangeServerInfo(int,int,short)
#include "CServerProxyMgr.h"
#include "CStreamGuard.h"
#include "GlobalData.h"               // s_msgq_mgr / s_*_stream_pool / s_monitor_proxy_mgr
#include "InterfacePacketBuf.h"
#include "MsgQueueMgr.h"
#include "StaticPool.h"

namespace exchange_server
{

// ---------------------------------------------------------------------------
// 便捷名（ORIG 直访全局 0x940bd68/0x940bd6c/0x940bda4/0x940bda8/0x940be28）
// ---------------------------------------------------------------------------
typedef MsgQueueMgr::QUEUE_IDX QIdx;

// ---------------------------------------------------------------------------
// OnRecvMessageServerInfo 线格式视图（ORIG 0x8493220 起 `mov 0xa(%eax)` /
// `movzwl 0xe(%eax)` / `mov 0x10(%eax)` 逐字段重读同一具名指针）——§4.3 具名化，
// 非对齐偏移 0x0a/0x0e ⇒ packed。
// ---------------------------------------------------------------------------
struct ExchangeServerInfoMsgView
{
    char m_pad00[0x0a];
    int m_channel0a;
    unsigned short m_port0e;
    unsigned int m_ip10;
} __attribute__((packed));

// ---------------------------------------------------------------------------
// ExchangeServerInfo::ExchangeServerInfo   ORIG 0x8491004（逐域清零）
// ---------------------------------------------------------------------------
#include "ExchangeServer_Exchange/ExchangeServerInfo.inc"
}

// ---------------------------------------------------------------------------
// GetServerType / SetServerType            ORIG 0x8491030 / 0x849103c
// ---------------------------------------------------------------------------
#include "ExchangeServer_Exchange/GetServerType.inc"
}

#include "ExchangeServer_Exchange/SetServerType.inc"
}

// ---------------------------------------------------------------------------
// AllocSession / FreeSession               ORIG 0x849104a / 0x8491060
// （StaticPool<CSession,300>@+0x6c）
// ---------------------------------------------------------------------------
#include "ExchangeServer_Exchange/AllocSession.inc"
}

#include "ExchangeServer_Exchange/FreeSession.inc"
}

// ---------------------------------------------------------------------------
// Reset                                    ORIG 0x849107e
// ---------------------------------------------------------------------------
#include "ExchangeServer_Exchange/Reset.inc"
}

// ---------------------------------------------------------------------------
// GetNetworkEvent                          ORIG 0x84910a6
// （WaitForSessionEvent(10ms)；返回值忽略，恒 true）
// ---------------------------------------------------------------------------
#include "ExchangeServer_Exchange/GetNetworkEvent.inc"
}

// ---------------------------------------------------------------------------
// GetExchangeServerCount                   ORIG 0x84910ca
// （遍历 map 比较 value.m_type08 == type）
// ---------------------------------------------------------------------------
#include "ExchangeServer_Exchange/GetExchangeServerCount.inc"
}

// ---------------------------------------------------------------------------
// SetMaster                                ORIG 0x849114c
// ---------------------------------------------------------------------------
#include "ExchangeServer_Exchange/SetMaster.inc"
}

// ---------------------------------------------------------------------------
// SetSlave                                 ORIG 0x84911a4
// ---------------------------------------------------------------------------
#include "ExchangeServer_Exchange/SetSlave.inc"
}

// ---------------------------------------------------------------------------
// TrySend                                  ORIG 0x84911fc
// （map[serverId].m_pSession 存在则经 SessionManager 转发）
// ---------------------------------------------------------------------------
#include "ExchangeServer_Exchange/TrySend.inc"
}

// ---------------------------------------------------------------------------
// AttachSessionInExchangeServer            ORIG 0x8491296
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] AE：P221/P227 临时链复现 serverId 跨 ctor 保活 + trace 槽 -0x18。
#include "ExchangeServer_Exchange/AttachSessionInExchangeServer.inc"
}

// ---------------------------------------------------------------------------
// RegisterExchangeServer                   ORIG 0x849135c
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] AE：P293 具名 it（find 先/end 后）+ P221 临时链保活。
#include "ExchangeServer_Exchange/RegisterExchangeServer.inc"
}

// ---------------------------------------------------------------------------
// UnregisterExchangeServer                 ORIG 0x849144c
// （移除后若无 SLAVE（CONNECT_TYPE=1）剩留则自动转 Master）
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] AE：返回类型 void→bool（ORIG 成功 mov $1/失败 mov $0）+ P221 临时链。
#include "ExchangeServer_Exchange/UnregisterExchangeServer.inc"
}

// ---------------------------------------------------------------------------
// SendRequestCharacInfo                    ORIG 0x849152a
// 包：header(1,0x84) + a,b,c,d + extraLen + binary(extra)
// 发往 serverId = 第三个整参（与 ORIG 栈槽一致）
// ---------------------------------------------------------------------------
#include "ExchangeServer_Exchange/SendRequestCharacInfo.inc"
}

// ---------------------------------------------------------------------------
// SendResponseCharacInfo（带 session 版）  ORIG 0x8491644
// 包：header(1,0x85) + f + a + b + len + binary(data)
// ---------------------------------------------------------------------------
#include "ExchangeServer_Exchange/SendResponseCharacInfo.inc"
}

// ---------------------------------------------------------------------------
// SendResponseCharacInfo（无 session 版）  ORIG 0x849173e → _forchina
// ---------------------------------------------------------------------------
#include "ExchangeServer_Exchange/SendResponseCharacInfo_1.inc"
}

// ---------------------------------------------------------------------------
// SendResponseCharacInfo_forchina          ORIG 0x849176e
// 三档池取流（total=len+8：<=0xfff Small / <=0x3fff Default / else Big），
// 写 a、b、binary(data) 后投递 MsgQueueMgr idx=1（DISPATCH_Q）。
// 注：ORIG 返回值取 MsgQueueMgr::put 尾部 eax（`mov %eax,%ebx` 整字直传，非规范化），
// 本函数返回 put 的结果（权威头 MsgQueueMgr::put 声明 bool，转发 Stream* 版返回值）。
// 判词：else if 写成 (total > 0xfff && total <= 0x3fff) 复现 ORIG 第二分支入口的
// 冗余重测 `cmpl $0xfff; jle`（4.1 不消除短路链中已知条件，4.4 复用前次比较结果
// 需显式保留 && 前项）。check_tu 实测 identical 口径。
// ---------------------------------------------------------------------------
#include "ExchangeServer_Exchange/SendResponseCharacInfo_forchina.inc"
}

// ---------------------------------------------------------------------------
// ReturnCacheCharacter                     ORIG 0x84918d8
// 本 channel 命中缓存并回包；参数 e 控制命中后是否归还池内存。
// ---------------------------------------------------------------------------
// [b28e 2026-09-09] 126/127（p167 CLEAR）：P231 纯 register bool 复现 ORIG 的
// `&&` 短路物化岛（P226 语句表达式对内存第二条件被 4.4 jump-thread，探针 p1-t2；
// register 形探针 p7-t47 出岛，仅多 1 条 `mov %eax,%ebx`，ORIG 值在 %al）。
// 真 bug 已修：GetCacheCharacter 实参 c→d。
// 残差=块内局部槽向（pCache -0x34 vs -0x14、trace -0x30/-0x20 vs -0x2c/-0x3c，
// N93 附）+ 1 条寄存器中转 mov 封顶。
#include "ExchangeServer_Exchange/ReturnCacheCharacter.inc"
}

// ---------------------------------------------------------------------------
// ForwardRequestCharac_forchina            ORIG 0x8491ab6
// 同 _forchina 但：入参 len>0x20 时先计 CacheLoadFailCount；
// 投递队列改为 idx=2（DB_Q）。
// ---------------------------------------------------------------------------
#include "ExchangeServer_Exchange/ForwardRequestCharac_forchina.inc"
}

// ---------------------------------------------------------------------------
// ForwardRequestCharac                     ORIG 0x8491c32 → _forchina
// ---------------------------------------------------------------------------
#include "ExchangeServer_Exchange/ForwardRequestCharac.inc"
}

// ---------------------------------------------------------------------------
// OnRecvPacketCharacInfo                   ORIG 0x84922ee（返回失败行号/0）
// 包字段：type, idx, channel_no, charac_no[, msg_size, binary]
// ---------------------------------------------------------------------------
// [b28e 2026-09-09] 199/200（p167 CLEAR）：P225 while 形补 setne 物化 + 去具名
// fromChannel（P0）+ trace 全表达式临时链（P227，复现 ORIG 的 4 对深槽求值/回读）。
// 残差=循环出口对齐 nop（N112）+ SendResponseCharacInfo 实参装载序 1 条 mov（P75）
// + CACHE_CHARACTER_TYPE 帧基 -0x3f8b5 vs -0x3f8bc（N82）封顶。
#include "ExchangeServer_Exchange/OnRecvPacketCharacInfo.inc"
}

// ---------------------------------------------------------------------------
// OnRecvPacketCharacInfoRet                ORIG 0x84925e8
// 包字段：ret, type, idx[, msg_size, binary]
// ret==0 且本地回包失败（或 ret!=0）→ ForwardRequestCharac 兜底
// ---------------------------------------------------------------------------
// [b28e 2026-09-09] DIFF→NEAR（165/166，p167 CLEAR）：P225 while 形补 setne 物化
// + 去具名 channel（P0）+ trace 全表达式临时链（P227，复现 ctor 前深槽/寄存器求值）。
// 残差=N82/N93 槽号平移（ORIG -0x38..-0x40 vs 我方 -0x2c..-0x34）封顶。
#include "ExchangeServer_Exchange/OnRecvPacketCharacInfoRet.inc"
}

// ---------------------------------------------------------------------------
// OnRecvPacketServerInfo                   ORIG 0x8492854
// 包字段：serverId, ip, port(short)；Master 回全表、Slave 回空注册响应包
// ---------------------------------------------------------------------------
// [b28e 2026-09-09] 282/282（gap 0，p167 CLEAR）：P227 临时链 + 具名 ipv 补回
// push %edi/esi/ebx 与帧 0x9c；真 bug 修复=trace 实参序（见函数内注释）。
// 残差=N82/N98 槽号差（ORIG info -0x5c/pkt -0x6c/trace -0x48 vs 我方
// -0x3c/-0x64/-0x3c）封顶。
#include "ExchangeServer_Exchange/OnRecvPacketServerInfo.inc"
}

// ---------------------------------------------------------------------------
// OnRecvPacketServerInfoRet                ORIG 0x8492c5e
// Master 广播的 server 表回执：跳过自身表项，其余尝试互连并登记；
// 最后 Attach 发起方会话并转 Slave 角色。
// ---------------------------------------------------------------------------
// [b28e 2026-09-09] 228/228（gap 0，p167 CLEAR）：P227 临时链 + 具名 ipv 复现
// ctor 前 %esi/%ebx/%edi 保活；真 bug 修复=trace 实参序两处（见函数内注释）。
// 残差=N82/N93 槽号差（ORIG -0x84/-0x70/-0x60 vs 我方 -0x80/-0x64/-0x54）封顶。
#include "ExchangeServer_Exchange/OnRecvPacketServerInfoRet.inc"
}

// ---------------------------------------------------------------------------
// DispatchPacket                           ORIG 0x8493002
// data[0]=类型（当前仅识别 1），data[1]=子码 0x82..0x85；
// 返回类型 int（ORIG 尾 mov %ebx,%eax 无 bool 归一，0x315 行号直传；
// 调用点 ExchangeServer_Session.cpp 存 int moved 后 cmpl $0）。
// ---------------------------------------------------------------------------
// [b28e 2026-09-09] 129/129（p167 CLEAR）：唯一残差=外层 default 的 pktType
// 求值时机（ORIG 0x8493150 `mov -0x10(%ebp),%ebx` 在 ctor 前，4.4 恒 ctor 后读槽）。
// 已试无效：去 const、`(int)pktType`、语句表达式实参、纯 register（消掉 ORIG 的
// -0x10 槽 ⇒ 帧 0x50→0x40）⇒ 判 N80/P87（单实参 varargs 4.4 不提前求值）封顶。
#include "ExchangeServer_Exchange/DispatchPacket.inc"
}

// ---------------------------------------------------------------------------
// OnRecvMessageServerInfo                  ORIG 0x84931de
// stream buffer：+0x0a channel_no(int)、+0x0e port(short)、+0x10 ip(uint)
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] NEAR 判 N7：p167 CLEAR(17/17)；残差=callee-saved 角色
// （ORIG edi=channelNo/esi=port vs 4.4 esi/ebx）+ 缺 push %edi。真 bug 已修：
// SetServerType(MASTER) 移出内层 if（ORIG je 直达）。
#include "ExchangeServer_Exchange/OnRecvMessageServerInfo.inc"
}

// ---------------------------------------------------------------------------
// OnRecvMessageCharacInfo                  ORIG 0x849341c
// 本机（参数 a==0x20，ORIG 比较 0x10(%ebp)）走缓存回收路径；否则带请求体转发。
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] AE：真 bug `b==0x20`→`a==0x20`（ORIG 比较 0x10(%ebp)）；
// P198① localHit 两句式预清；P0 内联 length/get_buffer 实参。
#include "ExchangeServer_Exchange/OnRecvMessageCharacInfo.inc"
}

// ---------------------------------------------------------------------------
// DispatchMessage                          ORIG 0x8493530
// msgid 分流（ORIG 决策树实证）：{7..0xc,0x11,0x19,0x20,0xb3,0x282}→CharacInfo；
// {0xb8}→ServerInfo；其余→trace。
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] AE：真 bug 分支映射反转（0xb8→OnRecvMessageServerInfo）+
// P46/P261 switch 化（复现二分决策树）+ P221 临时链。
#include "ExchangeServer_Exchange/DispatchMessage.inc"
}

// ---------------------------------------------------------------------------
// GetMessage                               ORIG 0x849362a（队列 idx=5）
// ---------------------------------------------------------------------------
#include "ExchangeServer_Exchange/GetMessage.inc"
}

// ---------------------------------------------------------------------------
// FreeMessage                              ORIG 0x8493648
// 按 Stream.m_poolType(+0x20) 归还池；flag==0 直接放弃。
// ORIG 各越限分支打日志后仍归还原池。
// 判词：返回类型 bool（ORIG 尾 `mov $0x1,%eax`、stream==0 路径 `mov $0x0,%eax`，
// 4.1 不为 void 生成返回值链；ExchangeServer.h 已广播改型，调用点 Infra:572 忽略返回值）。
// [b28d-solo-Exchange] NEAR 判 N93：88=88 条；残差=三臂 cMyTrace 槽深方向
// （ORIG case1 -0x3c/case2 -0x1c vs 4.4 相反）；P221 临时链翻槽成功但 4.4 升格
// len→%ebx（push %ebx）88→91 反劣，已回退。
// ---------------------------------------------------------------------------
#include "ExchangeServer_Exchange/FreeMessage.inc"
}

// ---------------------------------------------------------------------------
// SendExchangeServerInfo                   ORIG 0x84937ba
// 条件：(!m_flag38 && m_flag39) 或距上次发送 <=9 秒 → 跳过上报，恒返回 true
// （ORIG 正极性守卫 m_flag38||!m_flag39 包裹发送段）。
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] AE：真 bug 两处 return false→true（ORIG 均跳 0x8493848 mov $1）；
// 正极性守卫 `m_flag38||!m_flag39` + time_t 同型相减（jle）+ P52 链式 receiver。
#include "ExchangeServer_Exchange/SendExchangeServerInfo.inc"
}

// ---------------------------------------------------------------------------
// PreparePassiveSocket                     ORIG 0x8493856
// 监听本服端口、把自身注册进 serverMap（即 ORIG 的三级“size over”告警链，
// 其比较立即数为编译期常量——忠实保留判断形态）。
// ---------------------------------------------------------------------------
// [b28d-solo-Exchange] AE：真 bug 日志第二实参 sizeProbe→0x3f869/0x3f86b（P97）+
// P221 五臂临时链 + m_ip04 int 化（P211 store 中转）。
#include "ExchangeServer_Exchange/PreparePassiveSocket.inc"
}

// ---------------------------------------------------------------------------
// C1 / D1                                  ORIG 0x8493ae6 / 0x8493b82
// 成员按地址序构造：sessionMgr(+0x0c) → localInfo(+0x40) → map(+0x50)
// → pool(+0x6c)，尾调 Reset。析构逆序由默认成员析构承担。
// ---------------------------------------------------------------------------
// ORIG 0x8493ae6：仅 IExchangeServer 基类 + 成员按序默认构造（sessionMgr
// → localInfo → map → pool）→ Reset()。标量成员（+0x04/+0x08/+0x0a/+0x38/
// +0x39/+0x3c/+0x68）不初始化（ORIG 无清零指令）。
#include "ExchangeServer_Exchange/CExchangeServer.inc"
}

#include "ExchangeServer_Exchange/dtor_CExchangeServer.inc"
}

}  // namespace exchange_server
