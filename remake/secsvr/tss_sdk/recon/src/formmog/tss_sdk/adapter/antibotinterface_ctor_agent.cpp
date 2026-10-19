// Semantic reconstruction from libtss_sdk.so.1.0.394:
//   AntibotInterface::C1/C2 at 0x00088580 / 0x00088d30
//   AntibotInterface::D0/D1/D2 at 0x00088b30 / 0x00088b60 / 0x00088bb0
//
// The object is 4 bytes: the vptr shared with the IAntibotInterface base.  The
// constructors install the derived vtable at 0x00292ca0 (address point
// 0x00292ca8, Initialize first); D0 additionally calls operator delete.  The
// destructor pair follows the 18 inherited business slots at 0x00292cf0/f4.
//
// The complete base/derived declarations live in iantibotinterface.h and are
// shared with the factory, every method implementation TU, and the closing
// base-vtable owner.  Defining the destructor out of line here also makes this
// TU the derived-vtable emission point (the virtual destructor is the first
// non-inline, non-pure virtual introduced by the derived class).

#include "iantibotinterface.h"

AntibotInterface::AntibotInterface() {
}

AntibotInterface::~AntibotInterface() {
}
