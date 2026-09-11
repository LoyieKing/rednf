// ORIG T 0x08630c46：pthread_join(m_tid)，成功(rc==0)返回 1、失败返回 0。
int ThreadInterface::join()
{
    if (pthread_join(m_tid, 0) != 0)
        return 0;
    return 1;
}