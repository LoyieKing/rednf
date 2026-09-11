// ----------------------------------------------------------------------------
// ORIG T 0x08146824 _ZN6Global14CBossTowerFunc18DieCharacInDungeonEP5CUser
// state==10(死亡塔) → CDeathTower::handleDieUser；state==12(BossTower) → 虚槽[4]
// handleDieUser；其它 state 返回 0。两分支均返回 1。
// b28c-L8 判词 [N(N7)]：原 if 链 + 具名 `const int state` 已改
// switch(user->get_state()) —— ORIG 0x08146824 无 state 栈槽、`cmp $0xa,%eax;je /
// cmp $0xc,%eax;je / jmp` 决策链 = P0「值无槽往返⇒右值内联」+ P17 switch 形（if 链下
// 4.4 必产 store+mem 比较）；翻形后 44=44 逐条等。残 7 行 N7：ORIG
// `mov (%eax),%edx … call *%edx`（vptr+0x10 handleDieUser），4.4 对间接 call 目标恒
// %eax、其余装载寄存器随之整体后移一格（助记符/长度全等，仅寄存器名差）。
// ----------------------------------------------------------------------------
// [K6 2026-09-11·P292 档①] char→bool（配合 BossTowerFunc.h:48）：ORIG 调用点
// 0x081c2b00 `call; test %al,%al; je 主体`（单字节判定，无 setne 归一化）；
// 定义体各分支为常量 return 0/1，4.4 探针实测 bool 与 int/char 形态全等 ⇒ 体不变。
bool Global::CBossTowerFunc::DieCharacInDungeon(CUser* user)
{
    switch (user->get_state())
    {
    case 0xa:
        {
            WongWork::CDeathTower* tower = user->getDeathTower();
            if (tower != 0)
            {
                tower->handleDieUser(user);
            }
        }
        return 1;
    case 0xc:
        {
            WongWork::CBossTower* tower = user->getBossTower();
            if (tower != 0)
            {
                tower->handleDieUser(user);  // ORIG vptr+0x10 = 槽[4] handleDieUser
            }
        }
        return 1;
    }
    return 0;
}