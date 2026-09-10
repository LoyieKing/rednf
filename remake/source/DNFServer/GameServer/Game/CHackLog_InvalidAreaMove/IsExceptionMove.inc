bool CHackLog_InvalidAreaMove::IsExceptionMove(int a, int b, int c, int d)
{
    if ((a == 6 && b == 0 && c == 6 && d == 4) ||
        (a == 6 && b == 4 && c == 6 && d == 0) ||
        (a == 3 && b == 8 && c == 3 && d == 0) ||
        (a == 3 && b == 0 && c == 3 && d == 8) ||
        (a == 3 && b == 1 && c == 2 && d == 0) ||
        (a == 2 && b == 0 && c == 3 && d == 1) ||
        (a == 2 && b == 2 && c == 2 && d == 5) ||
        (a == 2 && b == 5 && c == 2 && d == 2) ||
        (a == 2 && b == 1 && c == 2 && d == 5) ||
        (a == 2 && b == 5 && c == 2 && d == 1))
        return 1;
    return 0;
