// ORIG 符号: _ZN11CGM_Manager4IsGmEj @ 0x829948c
// readGuard(m_lock)；find 结果与 end() 用 operator== 比较，相等（命中）返回 true。
// [B24D判词 N] 语义正确：readGuard+find，it==end() 返回 true 否则 false（ORIG
//   operator==）；diff=mov $0x0/mov $0x1 分支汇聚顺序互换（GCC4.1 vs 4.4），
//   55/55 纯 lowering，语义等价。
bool CGM_Manager::IsGm(unsigned int accId)
{
    readGuard<rwLock> guard(&m_lock);
    std::map<unsigned int, CGMRecord>::iterator it = m_gmMap.find(accId);
    // ORIG 0x082994ee：`test %al,%al; je +0x6b(mov $1)` ⇒ al==0（find 未命中）
    // 返回 **1**；命中返回 0。原实现 `if(it==end()) return true;` 语义与 ORIG
    // **相反**（真 bug：命中 GM 反而返回 false）——按 ORIG 汇编改正极性。
    if (it == m_gmMap.end())
        return false;
    return true;
}