#ifndef NSL_OBJECTPOOLBYBOOSTPOOL_H_
#define NSL_OBJECTPOOLBYBOOSTPOOL_H_

#include <new>
#include <boost/pool/pool.hpp>

#include "ThreadLock.h"

namespace nsl {

template <class T, class A, class B, class C>
class object_pool_by_boost_pool
{
public:
    // Member init order must match orig: mTotalMallocNum=0 then lock_ ctor, then body.
    explicit object_pool_by_boost_pool(unsigned int next_size)
        : mTotalMallocNum(0)
    {
        pPool = new boost::pool<boost::default_user_allocator_new_delete>(sizeof(T), next_size, 0);
    }
    ~object_pool_by_boost_pool()
    {
        delete pPool;
    }
    object_pool_by_boost_pool& operator=(const object_pool_by_boost_pool&);

    T* malloc()
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > lock(lock_);
        mTotalMallocNum = mTotalMallocNum + 1;
        return (T*)pPool->malloc();
    }

    void free(T* p)
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > lock(lock_);
        mTotalMallocNum = mTotalMallocNum - 1;
        pPool->free(p);
    }

    bool is_from(T* p)
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > lock(lock_);
        return pPool->is_from(p);
    }

    T* construct()
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > lock(lock_);
        T* __p = (T*)pPool->malloc();
        if (__p == 0)
        {
            return 0;
        }
        mTotalMallocNum = mTotalMallocNum + 1;
        // value-init: POD zero-fill (rep stos); non-POD calls default ctor
        new (__p) T();
        return __p;
    }

    template <class U1>
    T* construct(const U1& u1)
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > lock(lock_);
        void* p = pPool->malloc();
        if (p == 0)
            return 0;
        mTotalMallocNum = mTotalMallocNum + 1;
        new (p) T(u1);
        return static_cast<T*>(p);
    }

    template <class U1, class U2>
    T* construct(const U1& u1, const U2& u2)
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > lock(lock_);
        void* p = pPool->malloc();
        if (p == 0)
            return 0;
        mTotalMallocNum = mTotalMallocNum + 1;
        new (p) T(u1, u2);
        return static_cast<T*>(p);
    }

    template <class U1, class U2, class U3>
    T* construct(const U1& u1, const U2& u2, const U3& u3)
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > lock(lock_);
        void* p = pPool->malloc();
        if (p == 0)
            return 0;
        mTotalMallocNum = mTotalMallocNum + 1;
        new (p) T(u1, u2, u3);
        return static_cast<T*>(p);
    }

    void destroy(T* p)
    {
        TScopedLock<TThreadLock<ThreadLock_linux> > lock(lock_);
        p->~T();
        mTotalMallocNum = mTotalMallocNum - 1;
        pPool->free(p);
    }

    size_t GetTotalMallocNum()
    {
        return mTotalMallocNum;
    }

private:
    boost::pool<boost::default_user_allocator_new_delete>* pPool;
    size_t mTotalMallocNum;
    ThreadLock lock_;
};

} // namespace nsl

#endif // NSL_OBJECTPOOLBYBOOSTPOOL_H_
