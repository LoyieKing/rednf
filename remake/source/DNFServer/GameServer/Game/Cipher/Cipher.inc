Cipher::Cipher()
    : m_totalKeyLength(0)
{
    // [N7 封顶] push_back 元素 GetKeySize() 虚调用 callee 寄存器 %edx(ORIG)↔%eax(4.4)。
    m_graphs.reserve(CRYPTO_GRAPH_MAX);
    for (int i = 0; i < CRYPTO_GRAPH_MAX; ++i)
    {
        ICryptoGraph* graph = CreateCryptoGraph((CRYPTO_GRAPH_TYPE)i);
        m_graphs.push_back(graph);
        m_totalKeyLength += graph->GetKeySize();
    }
