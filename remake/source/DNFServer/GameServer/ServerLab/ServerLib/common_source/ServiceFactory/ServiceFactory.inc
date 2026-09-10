ServiceFactory::ServiceFactory(char* _ServiceFactoryName, char* _ServiceConfigName)
{
    setTick();
    printf("_ServiceFactoryName-%s\n", _ServiceFactoryName);
    printf("_ServiceConfigName-%s\n", _ServiceConfigName);
    std::string serviceConfigName(_ServiceConfigName);
    // ORIG 二进制实测：第一个 find_last_of 用 "."（去扩展名），后两个用 "/"。
    // ORIG: iVar1 常驻 ebx；size 有栈槽 -0xc（store+reload）。
    register int iVar1 = (int)serviceConfigName.find_last_of(".");
    int size = (iVar1 - (int)serviceConfigName.find_last_of("/")) + -1;
    // ORIG: 无 (int) 强转（lea 0x1(%eax),%edx 形态）。
    serviceConfigName =
        serviceConfigName.substr(serviceConfigName.find_last_of("/") + 1, size);
    memset(ServiceFactoryName, 0, 0x15);
    memset(ServiceConfigName, 0, 0x51);
    strncpy(ServiceFactoryName, _ServiceFactoryName, 0x14);
    strncpy(ServiceConfigName, serviceConfigName.c_str(), 0x50);
