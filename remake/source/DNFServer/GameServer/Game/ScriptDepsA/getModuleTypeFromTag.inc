// ORIG getModuleTypeFromTag T 0x088c2233（报告全文转写）：步长 8 遍历 17 组，
// 与 open 标签比较；未命中 -1。
int getModuleTypeFromTag(std::string& tag)
{
    for (int i = 0; i < 17; ++i)
    {
        if (tag == MODULE_NAME_TAG[i][0])
            return i;
    }
    return -1;
}