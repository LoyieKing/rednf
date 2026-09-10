int Threads::getScopedLittleWorkIdx(int start, int end)
{
    int ret = start;
    // ORIG DWARF：most_little_queue_size 为 unsigned int。
    unsigned int most_little_queue_size = threadWork_[start]->orderQueue.size();
    for (int i = start; i < end; i = i + 1)
    {
        if (threadWork_[i]->orderQueue.size() < most_little_queue_size)
        {
            most_little_queue_size = threadWork_[i]->orderQueue.size();
            ret = i;
        }
    }
    return ret;
