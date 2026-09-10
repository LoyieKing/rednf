void TimerGeneratePowerUp::generatePowerUp(std::vector<CUser*>& users, int count)
{
    const int total = static_cast<int>(users.size());
    int selected = 0;
    int idx = 0;                 // ORIG -0x14：rand()%total 具名槽（P0 槽往返）
    CUser* picked = 0;           // ORIG -0x30：预清零（P198①）
    int maxLoop = 10000;         // ORIG -0x10：死写入照抄（P171；条件仍用立即数）
    int loopCnt = 0;
    std::vector<CUser*> buf;
    // ORIG 0x08638a0b-0x08638a4e：循环体尾 `++loopCnt; cmp $0x2710; jle <cond>`，
    // 超限落 trace 块 + break ⇒ 源码形为 while(count>selected){…;++loopCnt;
    // if(loopCnt>10000){trace;break;}}（旧 && 合并形产 mov$1/$0/test 物化 + jle/jg）。
    while (count > selected)
    {
        idx = rand() % total;
        picked = users[idx];
        // N96/P0：uchar callee 无槽折叠形（xor 寄存器物化属 setcc-fold 负例）。
        if ((unsigned char)(picked->IsPowerUp() ^ 1))
        {
            picked->SetPowerUp(true);
            buf.push_back(picked);
            ++selected;
        }
        ++loopCnt;
        if (loopCnt > 10000)
        {
            cMyTrace(__PRETTY_FUNCTION__,
                     0x104e, 5)("Generate Power Up Party Failure");
            break;
        }
    }
    G_GameWorld()->SendPowerUpList(ENUM_POWER_UP_ACTIVATE_TYPE_ACTIVATE, buf);
