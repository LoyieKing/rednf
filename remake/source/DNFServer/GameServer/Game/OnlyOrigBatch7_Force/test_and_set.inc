bool atomic_flag::test_and_set(memory_order) volatile
{
    register volatile atomic_flag* self = this;
    register pthread_mutex_t& __mutex = get_atomic_mutex();
    register bool __ret;
    if ((void*)&pthread_cancel != 0)
    {
        if (int __e = pthread_mutex_lock(&__mutex))
            __throw_system_error(__e);
        __ret = self->_M_i;
        self->_M_i = true;
        pthread_mutex_unlock(&__mutex);
    }
    else
    {
        __ret = self->_M_i;
        self->_M_i = true;
    }
    return __ret;
