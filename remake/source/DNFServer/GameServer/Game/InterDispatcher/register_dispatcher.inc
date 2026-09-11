// ORIG 0x84be95e 逐指令转写。
void InterDispatcher::register_dispatcher(int code, ISigDispatcher* dispatcher)
{
    m_dispatchers[code] = dispatcher;
}