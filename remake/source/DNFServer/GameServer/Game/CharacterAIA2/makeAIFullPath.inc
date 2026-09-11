// makeAIFullPath —— 报告 _lt_global_gt_/makeAIFullPath.md：dir+"ai/" 后接 makeFullPath。
// ORIG 0x08a38af4：临时直接作为按值参数（无额外拷贝构造）。
void makeAIFullPath(std::string& dir, std::string& name)
{
    makeFullPath(dir + "ai/", name);
}