void DestroyCryptoGraph(ICryptoGraph*& graph)
{
    if (graph)
    {
        delete graph;
        graph = 0;
    }
