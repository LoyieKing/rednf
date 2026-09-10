bool checkValidIndex(int index, STScriptFileList& list)   // [b28d-Main·P292] int→bool（ORIG 调用点 `call; xor $0x1,%eax`）
{
    if (list.GetFileName(index) == 0)
        return 0;
    return 1;
