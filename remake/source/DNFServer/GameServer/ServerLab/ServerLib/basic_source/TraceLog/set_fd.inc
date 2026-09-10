int TraceLog::set_fd(int line)
{
    FILE* fptr;
    // 原始：fopen 赋值在条件内 → cmpl/sete/test/je
    if ((fptr = fopen(logfname, "w+")) == NULL)
    {
        fprintf(stderr, "CAN'T OPEN OR CREATE THE FILE %s-%s\n", logfname, strerror(errno));
        return -1;
    }
    logfd = fileno(fptr);
    if ((fptr = fopen(errfname, "w+")) == NULL)
    {
        fprintf(stderr, "CAN'T OPEN OR CREATE THE FILE %s-%s\n", errfname, strerror(errno));
        return -1;
    }
    errfd = fileno(fptr);
    if ((fptr = fopen(statfname, "w+")) == NULL)
    {
        fprintf(stderr, "CAN'T OPEN OR CREATE THE FILE %s-%s\n", statfname, strerror(errno));
        return -1;
    }
    statfd = fileno(fptr);
    return 1;
