DataPool::DataPool()
{
    LogSendMessagePool = new boost::object_pool<Message, boost::default_user_allocator_new_delete>(0x400, 0);
    BufferLogSend = new boost::object_pool<SendBuffer, boost::default_user_allocator_new_delete>(0x400, 0);
    TCPSocketPool = new object_pool_by_boost_pool<TCPSocket, int, int, int>(0x800);
    TCPUserPool = new object_pool_by_boost_pool<TCPUser, int, int, int>(0x800);
    mTcpUserCount = 0;
