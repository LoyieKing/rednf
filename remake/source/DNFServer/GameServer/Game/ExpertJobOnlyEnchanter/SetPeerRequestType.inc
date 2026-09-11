// ---------------------------------------------------------------------------
// SetPeerRequestType（ORIG W 0x849f316：写 +0x44）
// 头文件现签名为 (int)；ORIG mangled 为
// _ZN10expert_job10CEnchanter18SetPeerRequestTypeE22ENUM_PEER_REQUEST_TYPE，
// 以枚举重载为准（decl 要求删除旧 (int) 定义于 ExpertJobLink4.cpp:703）。
// ---------------------------------------------------------------------------
void CEnchanter::SetPeerRequestType(ENUM_PEER_REQUEST_TYPE type)
{
    m_peerRequestType = type;
}