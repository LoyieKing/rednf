// GetServerChannelNo ORIG 0x848f27b：*(int*)(G_CEnvironment()+0x1b0)
//  == G_CEnvironment()->m_serverEnvir.m_channelNo（Server_Envir@+0x198 内 +0x18）。
int GetServerChannelNo()
{
    return G_CEnvironment()->m_serverEnvir.m_channelNo;
}