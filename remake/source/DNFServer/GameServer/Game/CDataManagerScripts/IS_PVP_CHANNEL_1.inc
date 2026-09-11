// ORIG _Z14IS_PVP_CHANNELi 0x088dd255（W）：线性 cmpl/je 链，命中集合
// {4,6,5,7,8,9,10,13,14,15,16} 返回 1，否则 0（P45 短路线性判定同构）。
// 定义落本 TU：声明同文件（CDataManagerScripts.cpp 顶部），调用点在
// GetPvPChannelNumber 0x088dcc28（按 bool 语义消费）。
bool IS_PVP_CHANNEL(int channelType)
{
    return (channelType == 4) || (channelType == 6) ||
           (channelType == 5) || (channelType == 7) ||
           (channelType == 8) || (channelType == 9) ||
           (channelType == 10) || (channelType == 13) ||
           (channelType == 14) || (channelType == 15) ||
           (channelType == 16);
}