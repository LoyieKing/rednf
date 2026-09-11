bool CGameManager::init()
{
    m_pCraneMinigameMgr = new CraneMinigameManager;
    //（ORIG 0x08294231..：`call init; xor $0x1,%eax; test %al,%al; je mov1 /
    // 非零→mov 0` 即 return (r==1)（真值归一，非取反——原实现 !r 语义写反，
    // init 失败/成功与调用方判定相反，真实 BUG）。X1 形复现 xor 直落+0/1 物化。
    // N96/P0：禁 register 绑定。无槽折叠形（probe /tmp/l7_h.cpp h2）：
    // 4.4 产 `cmp$1,%al; setne; test; je` 直比寄存器，残差 xor vs cmp+setne
    // 2 行（setcc-fold 负例在册）；槽化 X2 会污染 frame（f1/f3 实测更差）。
    if ((((unsigned char)GetCraneMinigameManager()->init()) ^ 1) != 0)
        return 0;
    return 1;
}