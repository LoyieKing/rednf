int RegenerationROI::_0_Load_RegenerationRandomOptionItem()
{
    int err = 0;                                            // ORIG movl $0,-0xc 预清
    err = importRegenerationRandomOptionScript(
        m_regenerator, "Etc/RandomOption/RegenerationRandomOption.etc");
    // P225/P295：ORIG `cmpl $0,-0xc; setne %al; test %al,%al; je` 无槽物化 ⇒ 源形
    // 是 while+return（g3 探针实证；具名 bool 局部必落槽 -0x9 → DIFF+1）。
    while (err != 0)
    {
        printf("Fail to Load Random Option Item Regenerator Line : %d\n", err);
        return err;
    }
    return 0;
