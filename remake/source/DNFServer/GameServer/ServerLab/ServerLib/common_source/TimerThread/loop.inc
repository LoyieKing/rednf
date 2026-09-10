void TimerThread::loop(void* temp)
{
    // ORIG: sign-extend time() to 64-bit via sar, sub/sbb into locals, cmpl mem for unsigned >
    do
    {
        long long startTime = (long long)time(NULL);
        PopTimeReqEvent();
        onTime();
        long long endTime = (long long)time(NULL);
        // ORIG：无显式 unsigned 转型（-O0 先装 startTime 再装 endTime 的
        // 64 位减法装载顺序；显式转型会先装 endTime，顺序相反）。
        unsigned long long elapsedTime = endTime - startTime;
        if (elapsedTime > (unsigned long long)0x14)
        {
            puts("TimerThread spent more time than NEXT_CHECK_TIME");
            printf("Elapsed Time: %lld\n", (long long)elapsedTime);
            printf("Map size: %d\n", timePeriodMap.Size());
            // ORIG: explicit continue reproduces nop; jmp back-edge merge
            continue;
        }
        else
        {
            if (0x14 - (int)elapsedTime < 0)
            {
                TSystem<LinuxSystem>::sleep(0);
                printf("\xBD\xBD\xB8\xB3 \xC5\xB8\xC0\xD3 : %d\n", 0x14 - (int)elapsedTime);
            }
            else
            {
                TSystem<LinuxSystem>::sleep(0x14 - (int)elapsedTime);
            }
        }
    } while (true);
