void atomic_flag::clear(memory_order) volatile
{
    register volatile atomic_flag* self = this;
    register pthread_mutex_t& __mutex = get_atomic_mutex();
    if ((void*)&pthread_cancel != 0)
    {
        if (int __e = pthread_mutex_lock(&__mutex))
            __throw_system_error(__e);
        self->_M_i = false;
        pthread_mutex_unlock(&__mutex);
    }
    else
    {
        self->_M_i = false;
    }
