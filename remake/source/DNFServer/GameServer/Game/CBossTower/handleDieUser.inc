// ORIG T 0x08143a62：
//   user==0 → 0；CBossPlay::handleDieUser 非 1 → 0；虚调 _checkMemberDie()
//   （ORIG call *(vptr+8)）为真时 onFinishStage()；m_prepareFinishFlag(+0xafc)
//   != 1 时 _onPrepareFinishTower(false)；返回 1。
// [b28d-54 N] ORIG 0x08143a62：CBossPlay::handleDieUser 保持 int（bool 化实测致
//   callee epilogue %ebx 合并回归，CBossPlay.h:61 在册）⇒ 调用点恒 test/sete
//   （P197④/N18b）；m_prepareFinishFlag bool 化已翻正（本波）、`call *%edx` vs
//   %eax 为 N7。语义等价。
// [b28d-solo-CBossTower] AE（41:41，Main 代落 bool 化后翻正）：CBossPlay.h:61
//   handleDieUser int→bool（ORIG 调用点 0x08143a62 `call; xor $1,%eax; test
//   %al,%al; je` 无 setne ⇒ bool，P292）+ 字段 bool 化 + 裸 if(!f)。
bool CBossTower::handleDieUser(CUser* user)
{
    if (user == 0)
        return 0;
    if (!CBossPlay::handleDieUser(user))
        return 0;
    if (_checkMemberDie())
    {
        onFinishStage();
        // b28d-54 翻正：ORIG 0x08143ab6 `movzbl 0xafc(%eax),%eax; xor $1,%eax;
        // test %al,%al; je` ⇒ 字段 bool 化（CBossTower.h）+ 裸 `if(!f)`（P248 档②）。
        if (!m_prepareFinishFlag)
            _onPrepareFinishTower(false);
    }
    return 1;
}