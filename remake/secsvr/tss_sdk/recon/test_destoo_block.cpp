#include <dlfcn.h>
#include <cstdio>
#include <cstring>
typedef void (*TenFn)(unsigned char*, bool, unsigned char*, unsigned int);
int main(){
 void* h=dlopen("/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394",RTLD_NOW|RTLD_LOCAL);
 void* mh=dlopen("/tmp/libtencrypt_new.so",RTLD_NOW|RTLD_LOCAL);
 TenFn o=(TenFn)dlsym(h,"_Z9TenDesTooPhbS_j"), m=(TenFn)dlsym(mh,"_Z9TenDesTooPhbS_j");
 unsigned char key[64]; for(int i=0;i<64;i++)key[i]=(unsigned char)(i*7+15);
 int lens[]={8,16,24,32};
 for(int li=0;li<4;li++) { int len=lens[li];
  unsigned char a[256],b[256]; for(int i=0;i<len;i++)a[i]=b[i]=(unsigned char)(i*13+45);
  o(key,true,a,len); m(key,true,b,len);
  std::printf("len=%d\norig:",len); for(int i=0;i<len;i++)std::printf(" %02x",a[i]);
  std::printf("\nmine:"); for(int i=0;i<len;i++)std::printf(" %02x",b[i]); std::printf("\n");
 }
}
