#include <dlfcn.h>
#include <cstdio>
#include <cstring>
typedef void (*TenFn)(unsigned char*, bool, unsigned char*, unsigned int);
static void run(const char* name, TenFn o, TenFn m) {
    unsigned char key[64], a[128], b[128];
    for (int i=0;i<64;++i) key[i]=(unsigned char)(i*7+5);
    for (int i=0;i<128;++i) a[i]=b[i]=(unsigned char)(i*13+5*3);
    o(key,true,a,128); m(key,true,b,128);
    int d=std::memcmp(a,b,128);
    std::printf("%s %s firstdiff=%d a0=%02x b0=%02x\n", name, d?"DIFF":"same", d, a[0], b[0]);
}
int main(){
    void* h=dlopen("/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394",RTLD_NOW|RTLD_LOCAL);
    if(!h){std::printf("dlopen %s\n",dlerror());return 2;}
    struct {const char* n;} names[]={{"_Z13TenBlowFish56PhbS_j"},{"_Z9TenDesTooPhbS_j"},{"_Z13TenBlowFish32PhbS_j"},{"_Z13TenBlowFish16PhbS_j"}};
    for(auto& x:names){
      TenFn o=(TenFn)dlsym(h,x.n);
      void* mh=dlopen("/tmp/libtencrypt_new.so",RTLD_NOW|RTLD_LOCAL);
      TenFn m=mh?(TenFn)dlsym(mh,x.n):nullptr;
      std::printf("%s orig=%p mine=%p\n",x.n,(void*)o,(void*)m);
      if(o&&m) run(x.n,o,m);
    }
}
