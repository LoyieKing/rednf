void emitResetValuesWeakSymbols()
{
    ResetValues a;             // C1/C2
    ResetValues b(a);          // C1ERKS/C2ERKS
    a.clear();                 // clear
    (void)b;
}