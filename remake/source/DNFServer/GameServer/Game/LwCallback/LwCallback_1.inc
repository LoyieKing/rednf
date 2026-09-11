LwCallback::LwCallback(std::function<void (char const*, char const*, int, char const*)> callback)
    : m_callback(callback)
{
}