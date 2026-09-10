void TCPSendThread::SendDataPoolFree(Message* msg, TCPUser* pUser)
{
    pUser->SetSending(false);
    msg->initialize();
    // ORIG 二进制实测（0x81608d4，EUC-KR）："SEND 요청 끝------------- msg=%d"
    G_TraceLog()->sysLog(8,
                         "SEND \xbf\xe4\xc3\xbb \xb3\xa1------------- msg=%d",
                         Message::ident);
    pApp->super_DataPools.getCommonDataPool(msg->mOwnerWorkId)->destroySendMessage(msg);
