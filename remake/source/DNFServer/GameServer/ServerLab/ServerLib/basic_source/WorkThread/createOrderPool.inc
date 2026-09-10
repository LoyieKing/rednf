Message* WorkThread::createOrderPool()
{
    Message* msg = ((boost::object_pool<Message, boost::default_user_allocator_new_delete>*)OrderPool)->construct();
    if (msg != NULL)
    {
        msg->initialize();
    }
    return msg;
