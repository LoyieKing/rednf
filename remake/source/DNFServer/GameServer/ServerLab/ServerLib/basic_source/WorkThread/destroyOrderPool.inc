void WorkThread::destroyOrderPool(Message* msg)
{
    ((boost::object_pool<Message, boost::default_user_allocator_new_delete>*)OrderPool)->free(msg);
