void Threads::setMostLittleQueue()
{
    // ORIG DWARF 声明序：compare(54) 先于 most_little_queue(56)。
    int compare;
    int most_little_queue = (int)threadWork_[0]->orderQueue.size();
    for (int i = 1; i < mWorkThreadNum; i = i + 1)
    {
        // ORIG DWARF 局部：compare(54)。
        compare = (int)threadWork_[i]->orderQueue.size();
        if (compare < most_little_queue)
        {
            // ORIG：most_little_queue = compare 先于 mMostLittleOrderQueue = i。
            most_little_queue = compare;
            mMostLittleOrderQueue = i;
        }
    }
    most_little_queue = (int)threadTCPSend_[0]->sendQueue.size();
    for (int i = 1; i < 1; i = i + 1)
    {
        compare = (int)threadTCPSend_[i]->sendQueue.size();
        if (compare < most_little_queue)
        {
            // ORIG：同上顺序。
            most_little_queue = compare;
            mMostLittleSendQueue = i;
        }
    }
