bool WindowsCodeProtectorServer::IsDifferentExeVersionRoughly() // 0x08575970
{
    if (m_exeFamily != 0)
    {
        // [b28d-64] P50 复活：ORIG 0x0857598a 独立 `iterator::C1Ev` 默认构造 +
        // begin() 临时槽（-0x2c）再拷入 it（-0x1c）⇒ 先声明后赋值。
        std::vector<WindowsExeFile*>::iterator it;
        WindowsExeFile* f;
        unsigned long maxFileSize = 0;
        it = m_exeFamily->m_exeFiles.begin();
        for (; it != m_exeFamily->m_exeFiles.end(); ++it)
        {
            // [b28d-64] P0：ORIG 0x085759c1 具名 f（-0x14 槽往返）且每轮两读；
            // 比较方向按 ORIG `cmp -0x10(%ebp),%eax; jbe`（m_size > maxFileSize）。
            f = *it;
            if (f->m_size > maxFileSize)
            {
                maxFileSize = f->m_size;
            }
        }
        // [b28d-64] P47/P191：ORIG 0x08575a1c `ja <尾 mov $0>` + fall `mov $1` ⇒
        // if(cond) return false; return true;（原三目/直返产 setbe 物化 +4 条）。
        unsigned long total = m_exeFamily->GetTotalIntervalSize();
        if (total * 2 <= maxFileSize)
        {
            return true;
        }
    }
    return false;
}