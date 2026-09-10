// 返回类型 bool：ORIG prett 串 `_ZZN21CCommunityServerProxy7InitTcpEvE19__PRETTY_FUNCTION__`
// @0x08c662e0 内容 = "bool CCommunityServerProxy::InitTcp()"（strall 审计 + objdump -s 实读），
// 且同族 BaseServerProxy/CGuildServerProxy/CMonitorServerProxy/CPCRoomServerProxy 的
// InitTcp 均为 bool。返回类型不入 mangled ⇒ 零 ABI 风险；调用点
// （CCommunityServerProxy::Init 转发、GlobalData.cpp:3124 重连）均丢弃返回值 ⇒ 行为等价。
bool CCommunityServerProxy::InitTcp()
