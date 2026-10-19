// zen_singleton.h, reconstructed from libtss_sdk.so.1.0.394.
//
// ORIG DWARF for both instantiations:
//   struct Singleton<T> {
//       struct ObjectCreator {                 // line 22, sizeof == 1
//           ObjectCreator();                   // line 23
//           void do_nothing() const;           // line 28
//       };
//   private:
//       static ObjectCreator object_creator;   // line 35
//   public:
//       static T *instance();                  // line 13
//   };
//
// instance() is the guarded function-local static emitted at 0x0008af00
// (TssSdkCommSvc, storage 0x28 bytes) and 0x000914f0 (TssSdkPluginMgr,
// storage 0x38 bytes):
//   __cxa_guard_acquire(guard(obj)); T::T(); __cxa_guard_release;
//   __cxa_atexit(T::~T, &obj, __dso_handle); return &obj;
//
// ObjectCreator's constructor calls instance(), so the static data member
// pre-creates the singleton during dynamic initialization (ORIG
// 0x8ae3e..0x8ae9a and 0x8d595..0x8d60f).  instance() itself never reads
// object_creator, which is what keeps the pre-creation free of recursion.
//
// ORIG zen_singleton.h defines instance() inside the class, so every TU that
// runs ObjectCreator during dynamic initialization inlines the guarded static
// into its own init block (__cxa_guard_acquire -> T::T() ->
// __cxa_guard_release -> __cxa_atexit -> __cxa_guard_abort/_Unwind_Resume)
// while the weak out-of-line copy `Singleton<T>::instance()` still exists once
// per instantiation after linking.  A declaration-only member made the
// reconstructed blocks call the merged symbol and lose the guard/ctor calls,
// so the body lives in-class.  A .cpp must not define it out-of-line any more
// (that is a redefinition); explicit instantiations are still allowed.
#ifndef TSS_RECON_ZEN_SINGLETON_H_
#define TSS_RECON_ZEN_SINGLETON_H_

template <typename T>
class Singleton {
public:
    // ORIG line 13: guarded function-local static, defined in the header.
    // always_inline is required for the ORIG codegen: without it GCC 13 emits
    // a call to the out-of-line weak instance() from the object_creator
    // initializer, so the TU's static-init block loses the guard/ctor/atexit
    // sequence that ORIG inlined into every TU.
    __attribute__((always_inline)) static T *instance() {
        static T obj;
        return &obj;
    }

    class ObjectCreator {
    public:
        ObjectCreator() { Singleton<T>::instance(); }
        void do_nothing() const {}
    };

private:
    static ObjectCreator object_creator;
};

// zen_singleton.h line 35.  Instantiating ObjectCreator makes every TU emit
// the weak object_creator root and inline the guarded instance() body above.
template <typename T>
typename Singleton<T>::ObjectCreator Singleton<T>::object_creator;

#endif  // TSS_RECON_ZEN_SINGLETON_H_
