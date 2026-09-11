Arad_Script::Arad_Script(const char* path)
{
    // ORIG 0x0817b580：mov 0xc(%ebp),%eax; mov %eax,(%esp); call make_file_path
    // → mov 0x8(%ebp),%edx; add $0x20,%edx; mov %eax,0x4(%esp); mov %edx,(%esp);
    // call std::string::append(char const*)@0x08708000（nm 实证）。原实现写
    // pathC_ = full 生成 operator=(char const*)@0x08708060 且多一个 full 槽（P0：
    // ORIG 无槽往返 ⇒ 右值内联）。
    pathC_.append(make_file_path(path));
}