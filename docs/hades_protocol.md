# DNF 游戏服 × Hades 反外挂系统通信协议还原文档

> 还原对象：`df_game_r`（md5 `7b2b22caea80752deaa403356ea867c0`，36,360,954 字节，elf32-i386）
> 还原方法：Ghidra 反编译（60 个 Hades 相关函数，见 [tools/hades/dump_hades.java](../tools/hades/dump_hades.java)）+ objdump 全量反汇编交叉验证。
> 完成日期：2026-08-16。所有结论均有二进制地址证据；无证据处明确标注“待定”。

---

## 1. 概述

Hades 是台服 DNF 的反非法程序 / 金币管控处置系统（反外挂服务器）。游戏服 `df_game_r` 内嵌 Hades 客户端：

- `XNuclear::CHades`（每用户）：采集并上报登录、登出、金币变动、疲劳、经验、副本、外挂检测、安全状态等事件；
- `CHadesServerProxy`（全局单例）：与 Hades 服务器的 TCP（数据）+ UDP（心跳）连接管理；
- 客户端 → 游戏服上报外挂动作（opcode `0x2b6`）→ 游戏服落库 `member_punish_info*` 表（reason 固定 `'HADES'`）；
- 游戏服 → 客户端告警回执（opcode `0x1bf`，调试串 `ENUM_NOTIPACKET_HADES_ALERT`）。

外部公开资料为零（GitHub/Gitee/Sourcegraph 无 `CHadesServerProxy` 命中），以下全部来自二进制还原。

---

## 2. 组件与对象布局

### 2.1 XNuclear::CHades（每用户，内嵌于 CUser）

- 位置：`CUser + 0x8e954`（`CUser::getHades` @ 0x8230800 返回该地址）；对象跨度 **0x2c = 44 字节**（相邻对象 `Secu_GoldControl` @ CUser+0x8e980）。
- 无虚表；仅 C1/D1；`CUser::C1` @ 0x864e7a6 构造、`CUser::D1` @ 0x864ebf4 析构。
- `Init(CUser*)` @ 0x84b91e8：唯一调用点 `CUser::InitOnAccept` @ 0x86499c5。

| 偏移 | 大小 | 类型 | 用途 / 证据 |
|---|---|---|---|
| +0x00 | 0x18 | `std::map<ushort,ushort>` | 基类 map（allocator 槽 + _Rb_tree_header 24 字节）。**本 build 为死成员**：全二进制仅 ctor/dtor 触碰，无 insert/find/operator[]，原始 hacktype→count 聚合逻辑已移除 |
| +0x1c | 4 | int | 疲劳消耗计数：`Init` 清零；`UseFatigue` 自增，>1 时发 `StdCateUpdate(1,0)` 并清零 |
| +0x20 | 4 | CUser* | `Init` 写入；所有 Send_* 经此取 CUser |
| +0x24 | 4 | uint | Hades TCP 连接秒数快照（`Send_Login` 时存 `GetHadesSocketConnSec()`）；`needLogin` 比较用 |
| +0x28 | 4 | — | 未访问（对象尾部对齐） |

### 2.2 CHadesServerProxy（全局单例 `GlobalData::s_hades_proxy` @ BSS 0x940be34）

- 构造：`GlobalData::Init` @ 0x829a851-0x829a8b5：`new CHadesServerProxy(udp_ip, tcp_port, udp_port)`（0x30e1c 字节），随后 ctor 内 `HadesSvrInit()`。
- ctor @ 0x8470510：`BaseServerProxy::BaseServerProxy(this, ip, tcp_port, "HadesSvr")` → 写 vtable → `+0x30e0c = udp_port` → `CUdpHandler` @ +0x30e10 → `+0x30e18 = 0` → `HadesSvrInit()`。

| 偏移 | 大小 | 用途 |
|---|---|---|
| +0x00 | 4 | vtable @ 0x8c69fb0（[0]=OnDisconnect 0x84707b6, [4]=Disconnect 0x82fd6e4, [8]=Parsing 0x8470794） |
| +0x04 | — | `CNetwork<100000,100000>`（TCP handler） |
| +0x30dfc | 4 | socket state（0=断开, 2=已连接） |
| +0x30e00 | — | std::string 服务器 IP（= `udp_ip_of_hades`） |
| +0x30e04 | 4 | TCP 端口（= `tcp_port_of_hades`） |
| +0x30e08 | — | std::string `"HadesSvr"` |
| +0x30e0c | 4 | UDP 端口（= `udp_port_of_hades`，ctor param_3） |
| +0x30e10 | — | CUdpHandler（client fd @ +0x30e14） |
| +0x30e18 | 4 | 连接时间戳 curSec（`ConnTcpHadesSvr` 刷新；`GetHadesSocketConnSec` 返回） |

### 2.3 其它组件

| 组件 | 地址 | 作用 |
|---|---|---|
| Timer_HadesHeartBeat | C1 0x863cfa6 / dispatch_sig 0x863a760 / registNextTimer 0x863a7c2 | TCP 断开后 60s UDP 心跳 |
| Inter_HadesAction | C1 0x84eeca8 / send_alert 0x84e5e62 / dispatch_sig 0x84e5f02 | 客户端外挂上报（sig 0x2b6） |
| Inter_HadesDisConnect | C1 0x84eeacc / dispatch_sig 0x84e4342 | TCP 断连通知（sig 0x23a）→ 重启心跳 |
| DB_HadesPunish | C1 0x844fab2 / makeRequest 0x8445dd4 / dispatch 0x8445aa6 | 惩罚落库（sig 0x2b6，DBMW 线程） |
| cUserHistoryLog::HadesHistoryLog | 0x8685418 | 用户历史日志 `"Hades,%d,%d,%d,%d"` |
| SIG_HADES_PUNISH | GetOutBuffer 0x8454232 / GetInBuffer 0x8457d48（0x24=36 字节） | DB 流结构 |

---

## 3. 网络拓扑与连接管理

```
server_str.xml（运行时配置，键名已在二进制确认）
  udp_ip_of_hades   (rodata 0x8c163de, id 0x44c) → CEnv+0x200 [char[16]]  get_udp_ip_hades @0x82a6e04
  udp_port_of_hades (rodata 0x8c163ee, id 0x44f) → CEnv+0x210 [int]       get_udp_port_hades @0x82a6e12
  tcp_port_of_hades (rodata 0x8c16400, id 0x452) → CEnv+0x214 [int]       get_tcp_port_hades @0x82a6e20

游戏服 CHadesServerProxy
  TCP：主动 connect → udp_ip_of_hades : tcp_port_of_hades   （数据通道，游戏服→Hades 上报）
  UDP：client socket（仅 sendto，无接收）→ udp_ip_of_hades : udp_port_of_hades （心跳）
  注意：TCP 与 UDP 共用同一个 IP 配置键 udp_ip_of_hades；不存在独立 "tcp ip" 键。
```

### 3.1 连接状态机

| state | 语义 | 证据 |
|---|---|---|
| 0 | 断开 | BaseServerProxy ctor 初值；Disconnect 显式写 0；心跳以 state==0 作为“未连接”判断 |
| 1 | 连接中 | 其它 proxy 使用；Hades 不用 |
| 2 | 已连接 | `ConnTcpHadesSvr` 置 2；`IsConnect()==(state==2)`；`SendHadesTcpPacket` 仅 state==2 时真正发送；MonitorTcpThread 仅 state==2 时收发 |

- `HadesSvrInit` @ 0x847059a = `ConnTcpHadesSvr`（Init → SetSocketState(2) → 记录 curSec）+ `InitUdp`（失败日志 `"Can't init udp client socket"`，DF_HadesServerProxy.cpp:0xd5）。
- `GlobalData::Init` 构造后再次 `BaseServerProxy::Init`（fd 已开则 no-op）；失败日志 `"[GlobalData::Init] Fail Init (Hades server proxy)"`。

### 3.2 TCP 断线 → UDP 心跳 → （重连后）补发登录

```
MonitorTcpThread::dispatch (0x857d95a) @0x857e7f1/0x857e843/0x857e8b6（仅 state==2 时 OnTcpRecv/OnTcpSend）
  → recv/send 出错 → BaseServerProxy::Disconnect (0x82fd6e4)：CNetwork::disconnect → state=0
      → 虚调 CHadesServerProxy::OnDisconnect (0x84707b6) → RunConnTimer (0x84707ca)
  → RunConnTimer：Stream 写 (sig=0x23a, -1) → MsgQueueMgr::put(queue=1)
  → DispatchThread::dispatch → InterDispatcher::dispatch(sig=0x23a)
      → array[0x23a*4=0x8e8] = Inter_HadesDisConnect（ctor 0x84eeacc 注册于 InterDispatcher ctor @0x84be1cd）
  → Inter_HadesDisConnect::dispatch_sig (0x84e4342) → Timer_HadesHeartBeat::registNextTimer (0x863a7c2)
```

- `registNextTimer`：`TimerQueue::InsertTimer(2, 0, 0x96, 0x3c, 0, 0)`。`0x96=150` 是**定时器消息号**（TimerDispatcher 数组下标 150×4=0x258）；`0x3c=60` 经 `SECOND_TO_MILISECONDi`（×1000）作触发延迟。数据节 `HADES_HEARTBEAT_SEC` @ 0x8c04a34 = 60，一致。
- 心跳 `dispatch_sig` @ 0x863a760：仅 `GetSocketState(s_hades_proxy)==0` 时发送 UDP 心跳并自我续期；TCP 恢复后心跳自然停止。
- **重连**：Hades 服务器 → 游戏服 TCP 指令 opcode **0xfe7**（`CDispatchServerPacket::dispatchPacket` @ 0x8473660 二叉 switch `cmp $0xfe7` @0x8473e96 → 0x847a140）→ `ConnTcpHadesSvr()`：重跑 Init + state=2 + 刷新 +0x30e18。
- **补发登录**：`+0x30e18` 变化 ⇒ 每个在线用户的 `CHades::needLogin`（比较 `CHades+0x24` 快照）检测到差异 → 自动 `Send_Login()` + `StdCateUpdate(3,0)`。断线期间 `SendHadesTcpPacket` 丢弃（state≠2）。

> 待定：硬断线后 TCP 如何重建（本二进制中 MonitorTcpThread 对 Hades 无 state==0 自动重连逻辑；心跳只发 UDP 不建 TCP；0xfe7 只能经已建立的 TCP 到达）——依赖 Hades 侧主动建连或运维指令，未在二进制内发现。

---

## 4. 公共包格式

所有 Hades 包使用 `PacketHeader`（ctor @ 0x8110b70，10 字节）：

| 偏移 | 大小 | 字段 |
|---|---|---|
| +0x00 | u16 | opcode / pid |
| +0x02 | u16 | 总长度 size（发送方按 `*(u16*)(buf+2)` 读取） |
| +0x04 | u16 | 0 |
| +0x06 | u32 | 0 |

payload 自 +0x0a 起。游戏服→Hades 的包全部走 TCP（`SendHadesTcpPacket`），仅心跳走 UDP（`SendHadesUdpPacket`）。

---

## 5. 游戏服 → Hades 服务器（TCP，9 个包）

### 5.1 Packet_Hades_Login — pid 0xfd2, size 0x64=100

发送：`CHades::Send_Login` @ 0x84b9200；末尾总是跟随 `Send_UserStatus`。

| 偏移 | 大小 | 类型 | 来源 | 建议字段名 |
|---|---|---|---|---|
| +0x0a | 4 | u32 | `CUser::get_acc_id`（CUser+0x704ac） | m_accId |
| +0x0e | 16 | char[16] | `CHades::GetIP`（见下） | m_ip |
| +0x1e | 65 | char[65] | `strncpy(..., CNetwork::get_mac_addr(), 0x41)`（源 @ CNetwork+0x6ee2c） | m_mac |
| +0x5f | 1 | u8 | `CEnvironment::get_channel_no`（int→byte） | m_channel |
| +0x60 | 4 | u32 | `CUser::GetUID`（CUser+0x704a4/0x704a6 两个 u16 拼装） | m_uid |

**IP 机制（强证据纠正）**：`GetIP` @ 0x84b9774 调 `GetPeerIP2(buf, 4)`，而 GetPeerIP2 对 maxlen≤15 恒失败（仅清零）→ 回退读 `CUser+0x8e418`（登录时 Inter_LoginInfo @ 0x84c725c 以长度 0x10 写入的 peer IP）。即 **Login 包 IP 是登录时抓取的客户端 IP，不是发送时实时 getpeername**。

### 5.2 Packet_Hades_Logout — pid 0xfd3, size 0x0e

发送：`CHades::Send_Logout` @ 0x84b92ea；唯一调用点 `CUser::UpdateLogout` @ 0x865560d。

| 偏移 | 大小 | 类型 | 来源 | 建议字段名 |
|---|---|---|---|---|
| +0x0a | 4 | u32 | `CUser::get_acc_id` | m_accId |

### 5.3 Packet_Hades_AddHackType — pid 0xfd7, size 0x1a=26

发送：`CHades::AddHackTypeCnt` @ 0x84b9328（先 `needLogin`）；唯一调用点 `CUser::onUpdateHackCount` @ 0x867fdcc（由 `CHackAnalyzer::onUpdateHackCount` 转发，且 `!isHackUserTimer()` 时）。

| 偏移 | 大小 | 类型 | 来源 | 建议字段名 |
|---|---|---|---|---|
| +0x0a | 4 | u32 | `CUser::get_acc_id` | m_accId |
| +0x0e | 2 | u16 | param_1（ENUM_HACKTYPE，`(u16)` 截断） | m_hackType |
| +0x10 | 2 | u16 | param_2（hack 次数） | m_count |
| +0x12 | 4 | u32 | param_3 原样落包 | m_field12（待定） |
| +0x16 | 4 | u32 | param_4 原样落包 | m_field16（待定） |

### 5.4 Packet_Hades_StdUpdate — pid 0xfd9, size 0x12=18

发送：`CHades::StdCateUpdate` @ 0x84b938c（先 `needLogin`）。4 个调用点（param_2 恒 0）：

| 偏移 | 大小 | 类型 | 来源 | 字段名 |
|---|---|---|---|---|
| +0x0a | 4 | u32 | `CUser::get_acc_id` | m_accId |
| +0x0e | 2 | u16 | param_1（类别） | m_stdCate |
| +0x10 | 2 | u16 | param_2（恒 0） | m_fieldC（待定） |

| m_stdCate 值 | 触发点 | 地址 |
|---|---|---|
| 1 | 疲劳消耗（每 2 次 UseFatigue） | CHades::UseFatigue @ 0x84b9593 |
| 2 | 组队副本开始（逐成员循环） | CParty::dungeon_start @ 0x85a1fc5 |
| 3 | 登录 / 重连补发登录 | Inter_LoginInfo @ 0x84c6f19；CHades::needLogin @ 0x84b975c |

### 5.5 Packet_Hades_Gold_Plus / Gold_Minus — pid 0xfda / 0xfdb, size 0x14=20

发送：`GoldPlus` @ 0x84b93fa / `GoldMinus` @ 0x84b944a（先 `needLogin`）。

| 偏移 | 大小 | 类型 | 来源 | 建议字段名 |
|---|---|---|---|---|
| +0x0a | 4 | u32 | `CUser::get_acc_id` | m_accId |
| +0x0e | 4 | u32 | param_1（金币增减量） | m_gold |
| +0x12 | 2 | u16 | param_3（ENUM_GOLD_CONTROL_REASON） | m_reason |

### 5.6 Packet_Hades_GoldTracking — pid 0xfdc, size 0x29=41

发送：`GoldTracking` @ 0x84b949a（先 `needLogin`）；+0x12 字段先 memset 0x10 再 `strncpy(...,0x10)`（param_3 可为 NULL → 全 0）。

| 偏移 | 大小 | 类型 | 来源 | 建议字段名 |
|---|---|---|---|---|
| +0x0a | 4 | u32 | `CUser::get_acc_id` | m_accId |
| +0x0e | 4 | u32 | param_2（AddAuctionMoney 路径实传 acc_id） | m_characId（待定） |
| +0x12 | 16 | char[16] | param_3（**IP 字符串**，非角色名：AddAuctionMoney 传 GetIP；选角 Inter 传硬编码 `"127.0.0.1"` @ 0x8c86f40；sendGold 可 NULL） | m_ip |
| +0x22 | 4 | u32 | param_4（金币量） | m_gold（待定） |
| +0x26 | 2 | u16 | param_5（3 个调用点均传 `getCurCharacUsedFatigue`） | m_usedFatigue |
| +0x28 | 1 | u8 | param_1（事件类型：2=邮寄/选角，4=拍卖） | m_eventType |

### 5.7 Packet_Hades_UserStatus — pid 0xfdd, size 0x12=18

发送：`Send_UserStatus` @ 0x84b97fe；唯一调用点 `Send_Login` 末尾 @ 0x84b92e3。

| 偏移 | 大小 | 类型 | 来源 | 建议字段名 |
|---|---|---|---|---|
| +0x0a | 4 | u32 | `CUser::get_acc_id` | m_accId |
| +0x0e | 4 | u32 | status 位域（初值 0） | m_status |

status 位（`getSecuType` @ 0x84b9aa2 读 CUser+0x8e97d u8）：
- bit0 = secuType & (1|2|0x20)（任一命中）
- bit1 = secuType & 8
- bit2 = secuType & 0x10

### 5.8 Packet_Hades_Udp_HeartBeat — pid 0xfd8, size 0x0e=14（UDP）

发送：`SendHeartBeatPacket` @ 0x8470682 → `SendHadesUdpPacket` @ 0x84706be → `CUdpHandler::SendToServer` @ 0x8483b68（`sendto(fd@+0x30e14, pkt, len=*(u16*)(pkt+2), port=+0x30e0c, ip=GetServerIP())`）。

| 偏移 | 大小 | 类型 | 来源 | 字段名 |
|---|---|---|---|---|
| +0x0a | 2 | u16 | 本服 `CEnvironment::get_udp_port()` | m_udpPort |
| +0x0c | 2 | u16 | 本服 `CEnvironment::get_channel_no()` | m_channelNo |

唯一调用点：Timer_HadesHeartBeat::dispatch_sig @ 0x863a760（TCP state==0 时每 60s 发送并续期）。

### 5.9 空桩函数（上报逻辑已删，保留调用点）

| 函数 | 地址 | 调用点 | 说明 |
|---|---|---|---|
| Send_HackType | 0x84b9322 | — | 空 ret |
| Send_ReturnToVillage | 0x84b9386 | GameWorld::out_from_dungeon @ 0x86c6be0 | 空 ret（出副本“回村”上报点） |
| Send_ChangeGold | 0x84b93ee | DisPatcher_CheckConn::dispatch_sig @ 0x81eaa50 | 空 ret（连接检查后“金币变化”上报点） |
| Send_ChangeSecurity | 0x84b93f4 | — | 空 ret（实际走 ChangeSecurity） |

---

## 6. 客户端 → 游戏服：外挂动作上报（opcode 0x2b6）

分发：`InterDispatcher` 构造 @ 0x84bbafa 将 `Inter_HadesAction` 注册到 `offset 0xad8`（= sig 0x2b6 × 4）；`DispatchThread::dispatch` @ 0x847e545 按流首 int（sig）分派。包总长 **0x22 = 34 字节**。

| 偏移 | 大小 | 字段 | 证据 |
|---|---|---|---|
| +0x00 | 2 | opcode = 0x2b6 | PacketHeader 约定 |
| +0x02 | 2 | size = 0x22 | 同上 |
| +0x04..+0x09 | 6 | 保留 0 | 同上 |
| +0x0a | 4 | accId | dispatch_sig 与 `CUser::get_acc_id` 比较 |
| +0x0e..+0x11 | 4 | 未使用（服务端不读） | 无引用 |
| +0x12 | 4 | actionType（位掩码） | → RunHadesAction param_1 |
| +0x16 | 4 | 参数 p1 | → RunHadesAction param_2 |
| +0x1a | 4 | 参数 p2 | → RunHadesAction param_3 |
| +0x1e | 4 | 参数 p3 | → RunHadesAction param_4 |

`Inter_HadesAction::dispatch_sig` @ 0x84e5f02：
- 在线（CUser* 非空且包内 accId 匹配）：`GetIP` 取用户 IP 作 reason → `RunHadesAction(action,p1,p2,p3,IP)` → `send_alert(user, action)`；
- 离线（CUser* 空且 accId≠0）：不查用户，直接按位掩码 `DB_HadesPunish::makeRequest(accId, type=2|1, p1,p2,p3, NULL)`（bit1→type2，bit0→type1）。

---

## 7. 游戏服 → 客户端：告警回执（opcode 0x1bf）

`Inter_HadesAction::send_alert` @ 0x84e5e62：

```
InterfacePacketBuf::put_header(0, 0x1bf)
put_int(actionType)     // 4 字节
finalize(true) → CUser::Send
```

调试串佐证：`ENUM_NOTIPACKET_HADES_ALERT`（Hades 告警通知包枚举）。仅在线成功路径回执；离线路径不回执。

---

## 8. Hades 服务器 → 游戏服：指令

- TCP 接收：`CHadesServerProxy::Parsing` @ 0x8470794 → `CNetwork<100000,100000>::Parsing_ServerPacket`。
- 已知指令 **0xfe7**：`CDispatchServerPacket::dispatchPacket` @ 0x8473660（opcode 二叉 switch）→ `CHadesServerProxy::ConnTcpHadesSvr()`——重置 TCP 会话（state=2 + 刷新 connSec），触发所有在线用户补发 Login + StdUpdate(3,0)。

> 待定：Hades 服务器 → 游戏服还有哪些其它指令（0xfe7 之外）。`Parsing_ServerPacket` 的完整 opcode 表未穷举。

---

## 9. 惩罚动作与落库

### 9.1 RunHadesAction 位掩码（@ 0x84b95bc，唯一调用点：Inter_HadesAction 在线分支）

| 位 | 动作 |
|---|---|
| == 0x40 | 跳过 `HadesHistoryLog`（不写用户历史日志） |
| 0x02 | `CUser::setHackUserFlag`（CUser+0x8d1a9=1）+ `SetTradePunishType(4)`（CUser+0x79628 位或 4）+ `DB_HadesPunish::makeRequest(accId, type=2, p1,p2,p3, reason)` |
| 0x01 | 若 `!isHackUserTimer()`（CUser+0x8d1a8）：`setHackUserFlag` + `setHackUserTimer` + `makeRequest(accId, type=1, ...)` |
| 0x04 | `CUser::DisConnSig(user, 0x39, 1, 0)` 强制断线（0x39=57 为客户端断线原因码；发送后记录崩溃统计并入队关闭） |

### 9.2 DB_HadesPunish::makeRequest（游戏/分发线程侧 @ 0x8445dd4）

```
Stream << sig 0x2b6 << -1
SIG_HADES_PUNISH *p = GetInBuffer<SIG_HADES_PUNISH>()   // 0x24 = 36 字节
p->m_id = accId; p->punishType; p->endTimeDays; p->applyFlag; p->punishValue;
if (reason) strcpy(p->reason, reason)                    // 在线 = 用户 IP 串；离线 = NULL 跳过
MsgQueueMgr::put(queue=2, stream)
```

消费者：`DBTask::DBProcess` @ 0x83ff3f4 读流首 int（sig，校验 ≤0x353）→ `handler = *(this+4+sig*4)`；`DB_HadesPunish` 注册于 `offset 0xadc`（= sig 0x2b6）。DB 句柄 `GetDBHandle(1,0)`。

**SIG_HADES_PUNISH 结构（36 字节）**：

| 偏移 | 类型 | 字段 | 说明 |
|---|---|---|---|
| +0x00 | u32 | m_id | 账号 ID |
| +0x04 | u32 | punishType | 1 → SQL punish_type=3；否则 0xc |
| +0x08 | u32 | endTimeDays | 0 → `'9999-12-31 23:59:59'`；否则 `adddate(now(),interval N day)` |
| +0x0c | u32 | applyFlag | 0 → 1；否则 2 |
| +0x10 | u32 | punishValue | dispatch 未使用（主表固定 0、历史表固定 6） |
| +0x14 | char[16] | reason | **dispatch 未读取**（历史表 reason 用固定韩文常量，见下） |

### 9.3 DB_HadesPunish::dispatch（DB 线程执行 @ 0x8445aa6）

1. **update 先行**：
```sql
upDate member_punish_info set occ_time=now(), start_time=now(), end_time=%s, apply_flag=%d
 where m_id=%s and punish_type=%d
-- %s = '9999-12-31 23:59:59' | adddate(now(),interval N day)
-- apply_flag = 1 | 2（SIG@0xc==0 → 1，否则 2）
-- punish_type = 3（SIG@4==1）| 0xc（否则）
```
2. **影响行数 0 → insert**：
```sql
inSert into member_punish_info (m_id, punish_type, occ_time, start_time, end_time, punish_value, apply_flag)
 values(%s, %d, now(), now(), %s, 0, %d)
```
3. **历史表**（按当前年分表）：
```sql
inSert into member_punish_info_history_%Y (m_id, punish_type, occ_time, punish_value, start_time,
 end_time, admin_id, apply_flag, reason)
 values(%s, %d, now(), 6, now(), %s, 'HADES', %d, '%s')
-- punish_value 固定 6；admin_id 固定 'HADES'；reason = 固定韩文常量（见下）
```
4. **ars_mail**（条件：punish_type==3 或 (punish_type==0xc 且 apply_flag==1)）：
```sql
inSert into member_punish_info_ars_mail (m_id, punish_type, apply_flag, occ_time) values(%s, %d, %d, now())
```

**固定 reason 常量**（EUC-KR 韩文）：
- `DAT_08c57930`（0xa6 字节，punishType==1 时）：`|*|HADES 제재 / 불법프로그램 및 유해 프로그램을 사용하여 이득을 취하거나, 불법 재화 생성 및 유통한 경우(퍼스트서버 포함) / 1차 : 해지불가 / 계정해킹 주장 시 확인요청`
  （≈ “|*|HADES 制裁 / 使用非法程序及有害程序牟利，或非法生成/流通金币（含先锋服）/ 1 次：不可解封 / 主张账号被盗时需核实”）
- `DAT_08c579d8`（0x3d 字节，punishType≠1 时）：`|*|HADES 제재 / 불법프로그램에 의한 처벌 / 계정보호제도 적용`
  （≈ “|*|HADES 制裁 / 因使用非法程序的处罚 / 适用账号保护制度”）

---

## 10. 用户历史日志

`cUserHistoryLog::HadesHistoryLog` @ 0x8685418：`CUser::LogHistory(user, "Hades,%d,%d,%d,%d", action, p1, p2, p3)`。
`CUser::LogHistory` @ 0x867c8fe 组装 CSV 前缀（accId、频道、端口、IP、webAddress、成长类型、职业、等级、角色名等）后写入 TimeLog 用户历史日志。cUserHistoryLog 位于 CUser+0x79700。

---

## 11. 上报事件全量索引（调用点 → CHades 方法）

| 事件 | 调用点 | CHades 方法 | 参数 |
|---|---|---|---|
| 登录 | Inter_LoginInfo::dispatch_sig @ 0x84c6ef6 | Send_Login（+Send_UserStatus）+ StdCateUpdate(3,0) | — |
| 登出 | CUser::UpdateLogout @ 0x865560d | Send_Logout | — |
| 升级 | CUser::_check_level_up @ 0x8662eeb / 0x86630de | ExpUp(expDelta)（参数未使用，仅 needLogin） | — |
| 疲劳消耗 | CUser::FatigueUp @ 0x8655ce9 | UseFatigue（每 2 次发 StdCateUpdate(1,0)） | — |
| 副本开始 | CParty::dungeon_start @ 0x85a1fc5（逐成员） | StdCateUpdate(2,0) | — |
| 普通金币+ | Secu_GoldControl::AddMoney @ 0x82877ae | GoldPlus(gold, 0) | gold = CUser+0x40/0x44 |
| 导入金币+ | Secu_GoldControl::AddImportMoney @ 0x828783e | GoldPlus(gold, 1) | gold = CUser+0x48/0x4c |
| 拍卖金币+ | Secu_GoldControl::AddAuctionMoney @ 0x82878dc（j>0x1387） | GoldPlus(gold, 2) + GoldTracking(4,...) | gold = CUser+0x60/0x64 |
| 普通金币− | Secu_GoldControl::SubMoney @ 0x8287a68 | GoldMinus(gold, 0) | CUser+0x50/0x54 |
| 导入金币− | Secu_GoldControl::SubImportMoney @ 0x8287af8 | GoldMinus(gold, 1) | CUser+0x58/0x5c |
| 拍卖金币− | Secu_GoldControl::SubAuctionMoney @ 0x82879d6（j>0x1387） | GoldMinus(gold, 2) | CUser+0x68/0x6c |
| 账号金币异常上报 | Secu_AccountHacking::sendGold @ 0x8278dd7 | GoldTracking(type, count, name, gold, fatigue) | type=h 参数 |
| 选角金币/疲劳 | Inter_SelectCharacMid::dispatch_sig @ 0x84e6246（DB 0x2ce 回复后） | GoldTracking(2, pkt+0x10, "127.0.0.1", pkt+0x08, fatigue) | 硬编码 IP |
| 外挂计数更新 | CUser::onUpdateHackCount @ 0x867fdcc（`!isHackUserTimer()` 时） | AddHackTypeCnt(hackType, count, u32, u32) | 参数 3/4 语义待定 |
| 安全卡激活/未激活 | Dispatcher_SecurityCardCertKey::dispatch_sig @ 0x820a924 / 0x820a949 | ChangeSecurity(1, 0x1b / 0x1a) | detail 码未使用 |
| 安全卡验证 | Dispatcher_SecurityCardAuthRpy::process @ 0x8262b59/2bdc/2c01/2c7a | ChangeSecurity(1, 0x17/0x15/0x16/0x19) | 0x17 附带 addServerHackCnt(0x343) |
| 密保验证 | Dispatcher_MouseRegister::doPassPadReplay @ 0x82634b3/35c5/35ea/3697 | ChangeSecurity(1, 0x21/0x1f/0x20/0x23) | 0x21 附带 addServerHackCnt(0x342) |
| 鼠标/密保注册 | Dispatcher_MouseRegister::dispatch_sig @ 0x8263be0/4111/4141 | ChangeSecurity(1, 0x26/0x25/0x24) | — |

---

## 12. 枚举汇总

**ENUM_HADES_SECURITY**：本 build 13 处调用首参恒为 1；第二参 detail 码 0x15–0x26（未使用，语义待定）。

**ENUM_GOLD_CONTROL_REASON**（GoldPlus/GoldMinus 第三参）：
| 值 | 语义 | 调用点 |
|---|---|---|
| 0 | 普通金币 | AddMoney / SubMoney |
| 1 | 导入金币 | AddImportMoney / SubImportMoney |
| 2 | 拍卖金币 | AddAuctionMoney / SubAuctionMoney |

**actionType 位掩码**（0x2b6 包 @+0x12 / RunHadesAction）：
| 值 | 语义 |
|---|---|
| 0x01 | DB punishType=1 惩罚（hack 标志 + 计时器，首次触发）；离线分支同样落库 |
| 0x02 | DB punishType=2 惩罚（hack 标志 + 交易惩罚 bit4）；离线分支同样落库 |
| 0x04 | DisConnSig(user, 0x39, 1, 0) 强制断线 |
| 0x40 | 跳过 HadesHistoryLog |

**ENUM_HACKTYPE（旁证）**：0x342 = 密保验证失败超限；0x343 = 安全卡验证失败超限；0x2ce = 账号金币异常。

**Hades 服务端包 opcode 汇总**：
| opcode | 包 | size | 方向/通道 |
|---|---|---|---|
| 0xfd2 | Login | 100 | 游戏服→Hades TCP |
| 0xfd3 | Logout | 14 | 同上 |
| 0xfd7 | AddHackType | 26 | 同上 |
| 0xfd8 | Udp_HeartBeat | 14 | 游戏服→Hades UDP |
| 0xfd9 | StdUpdate | 18 | 游戏服→Hades TCP |
| 0xfda | Gold_Plus | 20 | 同上 |
| 0xfdb | Gold_Minus | 20 | 同上 |
| 0xfdc | GoldTracking | 41 | 同上 |
| 0xfdd | UserStatus | 18 | 同上 |
| 0x2b6 | 客户端外挂上报 | 34 | 客户端→游戏服 |
| 0x1bf | Hades 告警回执 | 4+10 | 游戏服→客户端 |
| 0xfe7 | TCP 会话重置指令 | — | Hades 服务器→游戏服 |

---

## 13. 待定项

1. Hades 服务器 → 游戏服除 0xfe7 外的指令集（`Parsing_ServerPacket` 未穷举）；
2. 硬断线后 TCP 重建机制（二进制内无自动重连代码）；
3. AddHackType 包 +0x12/+0x16 两个 u32 语义（onUpdateHackCount 参数 3/4）；
4. GoldTracking +0xe（characId）与 +0x22（gold）的精确语义；
5. StdUpdate +0x10（param_2）恒 0 的子类别语义；
6. ChangeSecurity 第二参 detail 码（0x15–0x26）枚举名；
7. DisConnSig 0x39 的客户端文案；
8. UserStatus / StdUpdate 在 Hades 服务器侧的接收语义（服务端不可见）。

---

## 14. 关键地址索引

| 符号 | 地址 |
|---|---|
| XNuclear::CHades ctor/dtor | 0x8697bc6 / 0x8694f54 |
| XNuclear::CHades::Init / needLogin / GetIP | 0x84b91e8 / 0x84b9718 / 0x84b9774 |
| XNuclear::CHades::Send_Login / Send_Logout / Send_UserStatus | 0x84b9200 / 0x84b92ea / 0x84b97fe |
| XNuclear::CHades::StdCateUpdate / UseFatigue / ExpUp | 0x84b938c / 0x84b9552 / 0x84b953e |
| XNuclear::CHades::GoldPlus / GoldMinus / GoldTracking | 0x84b93fa / 0x84b944a / 0x84b949a |
| XNuclear::CHades::AddHackTypeCnt / ChangeSecurity / RunHadesAction | 0x84b9328 / 0x84b95a8 / 0x84b95bc |
| CHadesServerProxy ctor / HadesSvrInit / ConnTcpHadesSvr / InitUdp | 0x8470510 / 0x847059a / 0x84705be / 0x8470628 |
| CHadesServerProxy::SendHeartBeatPacket / SendHadesUdpPacket / SendHadesTcpPacket | 0x8470682 / 0x84706be / 0x8470752 |
| CHadesServerProxy::Parsing / OnDisconnect / RunConnTimer / GetHadesSocketConnSec | 0x8470794 / 0x84707b6 / 0x84707ca / 0x84b9ab2 |
| Timer_HadesHeartBeat::dispatch_sig / registNextTimer | 0x863a760 / 0x863a7c2 |
| Inter_HadesAction::send_alert / dispatch_sig | 0x84e5e62 / 0x84e5f02 |
| Inter_HadesDisConnect::dispatch_sig | 0x84e4342 |
| DB_HadesPunish::makeRequest / dispatch | 0x8445dd4 / 0x8445aa6 |
| cUserHistoryLog::HadesHistoryLog | 0x8685418 |
| CEnvironment::get_udp_ip_hades / get_udp_port_hades / get_tcp_port_hades | 0x82a6e04 / 0x82a6e12 / 0x82a6e20 |
| 0xfe7 指令分发（CDispatchServerPacket::dispatchPacket） | 0x8473660（cmp 0xfe7 @ 0x8473e96） |
| HADES_HEARTBEAT_SEC 数据 | 0x8c04a34（= 60） |

---

## 15. 相关头文件状态

| 头文件 | pid | 状态 |
|---|---|---|
| Packet_Hades_StdUpdate.h | 0xfd9 | 已完整（m_accId/m_stdCate/m_fieldC） |
| Packet_Hades_Udp_HeartBeat.h | 0xfd8 | 已完整（m_udpPort/m_channelNo） |
| Packet_Hades_Login.h | 0xfd2 | 已补全字段（2026-08-16） |
| Packet_Hades_Logout.h | 0xfd3 | 已补全字段（2026-08-16） |
| Packet_Hades_AddHackType.h | 0xfd7 | 已补全字段（2026-08-16） |
| Packet_Hades_Gold_Plus.h | 0xfda | 已补全字段（2026-08-16） |
| Packet_Hades_Gold_Minus.h | 0xfdb | 已补全字段（2026-08-16） |
| Packet_Hades_GoldTracking.h | 0xfdc | 已补全字段（2026-08-16） |
| Packet_Hades_UserStatus.h | 0xfdd | 已补全字段（2026-08-16） |
