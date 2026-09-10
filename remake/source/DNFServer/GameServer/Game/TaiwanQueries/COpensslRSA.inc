COpensslRSA::COpensslRSA() : m_rsa(0) {}

// ---------------------------------------------------------------------------
// GetAppFolder —— ORIG W 0x0816c715（_Z12GetAppFolderv，COpensslRSA TU 内全局
// 函数，由 sret 返回空串 "\"\"" 字面量 @0x08b81360）。重建以空串最小实现满足
// Init 拼路径语义；真实路径来源（注册表/配置）缺省时返回空。
// ---------------------------------------------------------------------------
std::string GetAppFolder()
{
    return std::string("");
