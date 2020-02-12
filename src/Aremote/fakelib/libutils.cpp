
#include "libfake.h"
#include "libfakeVector.h"
#include "libfakeDisplayInfo.h"

namespace android {

class RefBase
{
public:
            void            incStrong(const void* id) const;
            void            decStrong(const void* id) const;
            void            forceIncStrong(const void* id) const;
            int32_t         getStrongCount() const;


    typedef RefBase basetype;

protected:
                            RefBase();
    virtual                 ~RefBase();
    
    enum {
        OBJECT_LIFETIME_STRONG  = 0x0000,
        OBJECT_LIFETIME_WEAK    = 0x0001,
        OBJECT_LIFETIME_MASK    = 0x0001
    };
    
};

RefBase::RefBase() {
    LOGPF();
}
RefBase::~RefBase() {
    LOGPF();
}
void RefBase::incStrong(const void* id) const {
    LOGPF();
}
void RefBase::decStrong(const void* id) const {
    LOGPF();
}
void RefBase::forceIncStrong(const void* id) const {
    LOGPF();
}
int32_t RefBase::getStrongCount() const {
    LOGPF();
    return 0;
}

///  Vector(s)

VectorImpl::VectorImpl(size_t itemSize, uint32_t flags) { LOGPF(); }
VectorImpl::VectorImpl(const VectorImpl& rhs) { LOGPF(); }

#   if defined(BUILD_FAKE_LIBRARY)
    /* VectorImpl::~VectorImpl() only in library linked !!! */ 
    VectorImpl::~VectorImpl() { LOGPF(); }
#   endif

VectorImpl& VectorImpl::operator = (const VectorImpl& rhs) { LOGPF(); return *this; }
void*      VectorImpl::editArrayImpl() { LOGPF(); return nullptr; }
size_t     VectorImpl::capacity() const { LOGPF(); return 0; }
ssize_t    VectorImpl::insertVectorAt(const VectorImpl& vector, size_t index)  { LOGPF(); return 0; }
ssize_t    VectorImpl::appendVector(const VectorImpl& vector)  { LOGPF(); return 0; }
ssize_t    VectorImpl::insertArrayAt(const void* array, size_t index, size_t length)  { LOGPF(); return 0; }
ssize_t    VectorImpl::appendArray(const void* array, size_t length)  { LOGPF(); return 0; }
ssize_t    VectorImpl::insertAt(size_t index, size_t numItems)  { LOGPF(); return 0; }
ssize_t    VectorImpl::insertAt(const void* item, size_t index, size_t numItems)  { LOGPF(); return 0; }
status_t   VectorImpl::sort(VectorImpl::compar_t cmp)  { LOGPF(); return 0; }
status_t   VectorImpl::sort(VectorImpl::compar_r_t cmp, void* state)  { LOGPF(); return 0; }
void       VectorImpl::pop() { LOGPF(); }
void       VectorImpl::push() { LOGPF(); }
void       VectorImpl::push(const void* item) { LOGPF(); }
ssize_t    VectorImpl::add()   { LOGPF(); return 0; }
ssize_t    VectorImpl::add(const void* item)   { LOGPF(); return 0; }
ssize_t    VectorImpl::replaceAt(size_t index)   { LOGPF(); return 0; }
ssize_t    VectorImpl::replaceAt(const void* prototype, size_t index)    { LOGPF(); return 0; }
ssize_t    VectorImpl::removeItemsAt(size_t index, size_t count)   { LOGPF(); return 0; }
void       VectorImpl::finish_vector()  { LOGPF(); }
void       VectorImpl::clear()  { LOGPF(); }
void*      VectorImpl::editItemLocation(size_t index)  { LOGPF(); return nullptr; }
const void* VectorImpl::itemLocation(size_t index) const  { LOGPF(); return nullptr; }
ssize_t    VectorImpl::setCapacity(size_t new_capacity)  { LOGPF(); return 0; }
ssize_t    VectorImpl::resize(size_t size)  { LOGPF(); return 0; }
void       VectorImpl::release_storage() { LOGPF(); }
void*      VectorImpl::_grow(size_t where, size_t amount)  { LOGPF(); return nullptr; }
void       VectorImpl::_shrink(size_t where, size_t amount) { LOGPF(); }
size_t     VectorImpl::itemSize() const  { LOGPF(); return 0; }

void       VectorImpl::_do_construct(void* storage, size_t num) const  { LOGPF(); }
void       VectorImpl::_do_destroy(void* storage, size_t num) const  { LOGPF(); }
void       VectorImpl::_do_copy(void* dest, const void* from, size_t num) const   { LOGPF(); }
void       VectorImpl::_do_splat(void* dest, const void* item, size_t num) const  { LOGPF(); }
void       VectorImpl::_do_move_forward(void* dest, const void* from, size_t num) const  { LOGPF(); }
void       VectorImpl::_do_move_backward(void* dest, const void* from, size_t num) const  { LOGPF(); }

void       VectorImpl::do_construct(void* storage, size_t num) const { LOGPF(); }
void       VectorImpl::do_destroy(void* storage, size_t num) const  { LOGPF(); }
void       VectorImpl::do_copy(void* dest, const void* from, size_t num) const  { LOGPF(); }
void       VectorImpl::do_splat(void* dest, const void* item, size_t num) const  { LOGPF(); }
void       VectorImpl::do_move_forward(void* dest, const void* from, size_t num) const  { LOGPF(); }
void       VectorImpl::do_move_backward(void* dest, const void* from, size_t num) const  { LOGPF(); }

///

SortedVectorImpl::SortedVectorImpl(size_t itemSize, uint32_t flags)
 : VectorImpl(itemSize, flags) { LOGPF(); }
SortedVectorImpl::SortedVectorImpl(const VectorImpl& rhs) 
 : VectorImpl(rhs) { LOGPF(); }
SortedVectorImpl::~SortedVectorImpl() { LOGPF(); }
SortedVectorImpl& SortedVectorImpl::operator = (const SortedVectorImpl& rhs) { LOGPF(); return *this; }

ssize_t    SortedVectorImpl::indexOf(const void* item) const { LOGPF(); return 0; }
size_t     SortedVectorImpl::orderOf(const void* item) const { LOGPF(); return 0; }
ssize_t    SortedVectorImpl::add(const void* item) { LOGPF(); return 0; }
ssize_t    SortedVectorImpl::merge(const VectorImpl& vector) { LOGPF(); return 0; }
ssize_t    SortedVectorImpl::merge(const SortedVectorImpl& vector) { LOGPF(); return 0; }
ssize_t    SortedVectorImpl::remove(const void* item) { LOGPF(); return 0; }

int        SortedVectorImpl::do_compare(const void* lhs, const void* rhs) const { LOGPF(); return 0; }

ssize_t    SortedVectorImpl::_indexOrderOf(const void* item, size_t* order) const { LOGPF(); return 0; }
ssize_t    SortedVectorImpl::add() { LOGPF();  return 0; }
void       SortedVectorImpl::pop() { LOGPF(); }
void       SortedVectorImpl::push() { LOGPF(); }
void       SortedVectorImpl::push(const void* item) { LOGPF(); }
ssize_t    SortedVectorImpl::insertVectorAt(const VectorImpl& vector, size_t index) { LOGPF(); return 0; }
ssize_t    SortedVectorImpl::appendVector(const VectorImpl& vector) { LOGPF(); return 0; }
ssize_t    SortedVectorImpl::insertArrayAt(const void* array, size_t index, size_t length) { LOGPF(); return 0; }
ssize_t    SortedVectorImpl::appendArray(const void* array, size_t length) { LOGPF(); return 0; }
ssize_t    SortedVectorImpl::insertAt(size_t where, size_t numItems) { LOGPF(); return 0; }
ssize_t    SortedVectorImpl::insertAt(const void* item, size_t where, size_t numItems) { LOGPF(); return 0; }
ssize_t    SortedVectorImpl::replaceAt(size_t index) { LOGPF(); return 0; }
ssize_t    SortedVectorImpl::replaceAt(const void* item, size_t index) { LOGPF(); return 0; }

// shit..
android::Vector<android::DisplayInfo> __export_template_Vector_DisplayInfo() {
    LOGPF();
    android::Vector<android::DisplayInfo> vd;
    return vd;
}
/*
*/

};

