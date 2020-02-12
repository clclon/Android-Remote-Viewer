
#pragma once

#if !defined(__ANDROID__)
#  error "__ANDROID__ build only !"
#endif

#define UTILS_VECTOR_NO_CFI __attribute__((no_sanitize("cfi")))

namespace android {

template <typename T> struct trait_trivial_ctor { enum { value = false }; };
template <typename T> struct trait_trivial_dtor { enum { value = false }; };
template <typename T> struct trait_trivial_copy { enum { value = false }; };
template <typename T> struct trait_trivial_move { enum { value = false }; };
template <typename T> struct trait_pointer      { enum { value = false }; };    
template <typename T> struct trait_pointer<T*>  { enum { value = true }; };

template <typename TYPE>
struct traits {
    enum {
        // whether this type is a pointer
        is_pointer          = trait_pointer<TYPE>::value,
        // whether this type's constructor is a no-op
        has_trivial_ctor    = is_pointer || trait_trivial_ctor<TYPE>::value,
        // whether this type's destructor is a no-op
        has_trivial_dtor    = is_pointer || trait_trivial_dtor<TYPE>::value,
        // whether this type type can be copy-constructed with memcpy
        has_trivial_copy    = is_pointer || trait_trivial_copy<TYPE>::value,
        // whether this type can be moved with memmove
        has_trivial_move    = is_pointer || trait_trivial_move<TYPE>::value
    };
};

class VectorImpl
{
public:
    enum {
        HAS_TRIVIAL_CTOR    = 0x00000001,
        HAS_TRIVIAL_DTOR    = 0x00000002,
        HAS_TRIVIAL_COPY    = 0x00000004
    };

                            VectorImpl(size_t itemSize, uint32_t flags);
                            VectorImpl(const VectorImpl& rhs);
#   if defined(BUILD_FAKE_LIBRARY)
    virtual                 ~VectorImpl();
#   else
    virtual                 ~VectorImpl() = 0;
#   endif

            void            finish_vector();
            VectorImpl&     operator = (const VectorImpl& rhs);    
            
    inline  const void*     arrayImpl() const   { return mStorage; }
    inline  size_t          size() const        { return mCount; }
    inline  bool            isEmpty() const     { return mCount == 0; }
            size_t          capacity() const;
            ssize_t         setCapacity(size_t size);
            ssize_t         resize(size_t size);
            ssize_t         insertVectorAt(const VectorImpl& vector, size_t index);
            ssize_t         appendVector(const VectorImpl& vector);
            ssize_t         insertArrayAt(const void* array, size_t index, size_t length);
            ssize_t         appendArray(const void* array, size_t length);
            ssize_t         insertAt(size_t where, size_t numItems = 1);
            ssize_t         insertAt(const void* item, size_t where, size_t numItems = 1);
            void            pop();
            void            push();
            void            push(const void* item);
            ssize_t         add();
            ssize_t         add(const void* item);
            ssize_t         replaceAt(size_t index);
            ssize_t         replaceAt(const void* item, size_t index);
            ssize_t         removeItemsAt(size_t index, size_t count = 1);
            void            clear();
            const void*     itemLocation(size_t index) const;
            void*           editItemLocation(size_t index);
            void*           editArrayImpl();
            typedef int (*compar_t)(const void* lhs, const void* rhs);
            typedef int (*compar_r_t)(const void* lhs, const void* rhs, void* state);
            status_t        sort(compar_t cmp);
            status_t        sort(compar_r_t cmp, void* state);

protected:
            size_t          itemSize() const;
            void            release_storage();

    virtual void            do_construct(void* storage, size_t num) const;
    virtual void            do_destroy(void* storage, size_t num) const;
    virtual void            do_copy(void* dest, const void* from, size_t num) const;
    virtual void            do_splat(void* dest, const void* item, size_t num) const;
    virtual void            do_move_forward(void* dest, const void* from, size_t num) const;
    virtual void            do_move_backward(void* dest, const void* from, size_t num) const;
    
private:
        void* _grow(size_t where, size_t amount);
        void  _shrink(size_t where, size_t amount);

        inline void _do_construct(void* storage, size_t num) const;
        inline void _do_destroy(void* storage, size_t num) const;
        inline void _do_copy(void* dest, const void* from, size_t num) const;
        inline void _do_splat(void* dest, const void* item, size_t num) const;
        inline void _do_move_forward(void* dest, const void* from, size_t num) const;
        inline void _do_move_backward(void* dest, const void* from, size_t num) const;
        void *      mStorage = nullptr;
        size_t      mCount = 0;
        const   uint32_t    mFlags = 0;
        const   size_t      mItemSize = 0;
};


class SortedVectorImpl : public VectorImpl
{
public:
                            SortedVectorImpl(size_t itemSize, uint32_t flags);
                            SortedVectorImpl(const VectorImpl& rhs);
    virtual                 ~SortedVectorImpl();
    
    SortedVectorImpl&     operator = (const SortedVectorImpl& rhs);    
    ssize_t         indexOf(const void* item) const;
    size_t          orderOf(const void* item) const;
    ssize_t         add(const void* item);
    ssize_t         merge(const VectorImpl& vector);
    ssize_t         merge(const SortedVectorImpl& vector);
    ssize_t         remove(const void* item);
        
protected:
    virtual int             do_compare(const void* lhs, const void* rhs) const;

private:
            ssize_t         _indexOrderOf(const void* item, size_t* order = 0) const;
            ssize_t         add();
            void            pop();
            void            push();
            void            push(const void* item);
            ssize_t         insertVectorAt(const VectorImpl& vector, size_t index);
            ssize_t         appendVector(const VectorImpl& vector);
            ssize_t         insertArrayAt(const void* array, size_t index, size_t length);
            ssize_t         appendArray(const void* array, size_t length);
            ssize_t         insertAt(size_t where, size_t numItems = 1);
            ssize_t         insertAt(const void* item, size_t where, size_t numItems = 1);
            ssize_t         replaceAt(size_t index);
            ssize_t         replaceAt(const void* item, size_t index);
};

/////////

template <typename TYPE>
class SortedVector;

template <class TYPE>
class Vector : private VectorImpl
{
public:
    typedef TYPE            value_type;
                            Vector();
                            Vector(const Vector<TYPE>& rhs);
    explicit                Vector(const SortedVector<TYPE>& rhs);
    virtual                 ~Vector();

    const Vector<TYPE>&     operator = (const Vector<TYPE>& rhs) const;
          Vector<TYPE>&           operator = (const Vector<TYPE>& rhs);    

    const Vector<TYPE>&     operator = (const SortedVector<TYPE>& rhs) const;
          Vector<TYPE>&           operator = (const SortedVector<TYPE>& rhs);

    inline  void            clear()             { VectorImpl::clear(); }

    inline  size_t          size() const                { return VectorImpl::size(); }
    inline  bool            isEmpty() const             { return VectorImpl::isEmpty(); }
    inline  size_t          capacity() const            { return VectorImpl::capacity(); }
    inline  ssize_t         setCapacity(size_t size)    { return VectorImpl::setCapacity(size); }
    inline  ssize_t         resize(size_t size)         { return VectorImpl::resize(size); }
    inline  const TYPE*     array() const;
            TYPE*           editArray();
    inline  const TYPE&     operator [] (size_t index) const;
    inline  const TYPE&     itemAt(size_t index) const;
            const TYPE&     top() const;
            TYPE&           editItemAt(size_t index);
            TYPE&           editTop();
            ssize_t         insertVectorAt(const Vector<TYPE>& vector, size_t index);
            ssize_t         appendVector(const Vector<TYPE>& vector);
            ssize_t         insertArrayAt(const TYPE* array, size_t index, size_t length);
            ssize_t         appendArray(const TYPE* array, size_t length);
    inline  ssize_t         insertAt(size_t index, size_t numItems = 1);
            ssize_t         insertAt(const TYPE& prototype_item, size_t index, size_t numItems = 1);
    inline  void            pop();
    inline  void            push();
            void            push(const TYPE& item);
    inline  ssize_t         add();
            ssize_t         add(const TYPE& item);            
    inline  ssize_t         replaceAt(size_t index);
            ssize_t         replaceAt(const TYPE& item, size_t index);
    inline  ssize_t         removeItemsAt(size_t index, size_t count = 1);
    inline  ssize_t         removeAt(size_t index)  { return removeItemsAt(index); }

    typedef int (*compar_t)(const TYPE* lhs, const TYPE* rhs);
    typedef int (*compar_r_t)(const TYPE* lhs, const TYPE* rhs, void* state);
     
    inline status_t        sort(compar_t cmp);
    inline status_t        sort(compar_r_t cmp, void* state);

    inline size_t getItemSize() const { return itemSize(); }

     typedef TYPE* iterator;
     typedef TYPE const* const_iterator;

     inline iterator begin() { return editArray(); }
     inline iterator end()   { return editArray() + size(); }
     inline const_iterator begin() const { return array(); }
     inline const_iterator end() const   { return array() + size(); }
     inline void reserve(size_t n) { setCapacity(n); }
     inline bool empty() const{ return isEmpty(); }
     inline void push_back(const TYPE& item)  { insertAt(item, size(), 1); }
     inline void push_front(const TYPE& item) { insertAt(item, 0, 1); }
     inline iterator erase(iterator pos) {
         ssize_t index = removeItemsAt(pos-array());
         return begin() + index;
     }

protected:
    virtual void    do_construct(void* storage, size_t num) const;
    virtual void    do_destroy(void* storage, size_t num) const;
    virtual void    do_copy(void* dest, const void* from, size_t num) const;
    virtual void    do_splat(void* dest, const void* item, size_t num) const;
    virtual void    do_move_forward(void* dest, const void* from, size_t num) const;
    virtual void    do_move_backward(void* dest, const void* from, size_t num) const;
};

template<class TYPE> inline 
Vector<TYPE>::Vector() 
     : VectorImpl(sizeof(TYPE), 
                 ((traits<TYPE>::has_trivial_ctor   ? HAS_TRIVIAL_CTOR   : 0) 
                 |(traits<TYPE>::has_trivial_dtor   ? HAS_TRIVIAL_DTOR   : 0) 
                 |(traits<TYPE>::has_trivial_copy   ? HAS_TRIVIAL_COPY   : 0)) 
                 ) 
{ 
} 

template<class TYPE> inline
Vector<TYPE>::Vector(const Vector<TYPE>& rhs)
    : VectorImpl(rhs) {
}

template<class TYPE> inline
Vector<TYPE>::Vector(const SortedVector<TYPE>& rhs)
    : VectorImpl(static_cast<const VectorImpl&>(rhs)) {
}

template<class TYPE> inline
Vector<TYPE>::~Vector() {
    finish_vector();
}

template<class TYPE> inline
Vector<TYPE>& Vector<TYPE>::operator = (const Vector<TYPE>& rhs) {
    VectorImpl::operator = (rhs);
    return *this; 
}

template<class TYPE> inline
const Vector<TYPE>& Vector<TYPE>::operator = (const Vector<TYPE>& rhs) const {
    VectorImpl::operator = (static_cast<const VectorImpl&>(rhs));
    return *this;
}

template<class TYPE> inline
Vector<TYPE>& Vector<TYPE>::operator = (const SortedVector<TYPE>& rhs) {
    VectorImpl::operator = (static_cast<const VectorImpl&>(rhs));
    return *this;
}

template<class TYPE> inline
const Vector<TYPE>& Vector<TYPE>::operator = (const SortedVector<TYPE>& rhs) const {
    VectorImpl::operator = (rhs);
    return *this; 
}

template<class TYPE> inline
const TYPE* Vector<TYPE>::array() const {
    return static_cast<const TYPE *>(arrayImpl());
}

template<class TYPE> inline
TYPE* Vector<TYPE>::editArray() {
    return static_cast<TYPE *>(editArrayImpl());
}


template<class TYPE> inline
const TYPE& Vector<TYPE>::operator[](size_t index) const {
    LOG_FATAL_IF(index>=size(),
            "%s: index=%u out of range (%u)", __PRETTY_FUNCTION__,
            int(index), int(size()));
    return *(array() + index);
}

template<class TYPE> inline
const TYPE& Vector<TYPE>::itemAt(size_t index) const {
    return operator[](index);
}

template<class TYPE> inline
const TYPE& Vector<TYPE>::top() const {
    return *(array() + size() - 1);
}

template<class TYPE> inline
TYPE& Vector<TYPE>::editItemAt(size_t index) {
    return *( static_cast<TYPE *>(editItemLocation(index)) );
}

template<class TYPE> inline
TYPE& Vector<TYPE>::editTop() {
    return *( static_cast<TYPE *>(editItemLocation(size()-1)) );
}

template<class TYPE> inline
ssize_t Vector<TYPE>::insertVectorAt(const Vector<TYPE>& vector, size_t index) {
    return VectorImpl::insertVectorAt(reinterpret_cast<const VectorImpl&>(vector), index);
}

template<class TYPE> inline
ssize_t Vector<TYPE>::appendVector(const Vector<TYPE>& vector) {
    return VectorImpl::appendVector(reinterpret_cast<const VectorImpl&>(vector));
}

template<class TYPE> inline
ssize_t Vector<TYPE>::insertArrayAt(const TYPE* array, size_t index, size_t length) {
    return VectorImpl::insertArrayAt(array, index, length);
}

template<class TYPE> inline
ssize_t Vector<TYPE>::appendArray(const TYPE* array, size_t length) {
    return VectorImpl::appendArray(array, length);
}

template<class TYPE> inline
ssize_t Vector<TYPE>::insertAt(const TYPE& item, size_t index, size_t numItems) {
    return VectorImpl::insertAt(&item, index, numItems);
}

template<class TYPE> inline
void Vector<TYPE>::push(const TYPE& item) {
    return VectorImpl::push(&item);
}

template<class TYPE> inline
ssize_t Vector<TYPE>::add(const TYPE& item) {
    return VectorImpl::add(&item);
}

template<class TYPE> inline
ssize_t Vector<TYPE>::replaceAt(const TYPE& item, size_t index) {
    return VectorImpl::replaceAt(&item, index);
}

template<class TYPE> inline
ssize_t Vector<TYPE>::insertAt(size_t index, size_t numItems) {
    return VectorImpl::insertAt(index, numItems);
}

template<class TYPE> inline
void Vector<TYPE>::pop() {
    VectorImpl::pop();
}

template<class TYPE> inline
void Vector<TYPE>::push() {
    VectorImpl::push();
}

template<class TYPE> inline
ssize_t Vector<TYPE>::add() {
    return VectorImpl::add();
}

template<class TYPE> inline
ssize_t Vector<TYPE>::replaceAt(size_t index) {
    return VectorImpl::replaceAt(index);
}

template<class TYPE> inline
ssize_t Vector<TYPE>::removeItemsAt(size_t index, size_t count) {
    return VectorImpl::removeItemsAt(index, count);
}

template<class TYPE> inline
status_t Vector<TYPE>::sort(Vector<TYPE>::compar_t cmp) {
    return VectorImpl::sort((VectorImpl::compar_t)cmp);
}

template<class TYPE> inline
status_t Vector<TYPE>::sort(Vector<TYPE>::compar_r_t cmp, void* state) {
    return VectorImpl::sort((VectorImpl::compar_r_t)cmp, state);
}

// ---------------------------------------------------------------------------

template<class TYPE>
UTILS_VECTOR_NO_CFI void Vector<TYPE>::do_construct(void* storage, size_t num) const {
    LOGPF();
}

template<class TYPE>
void Vector<TYPE>::do_destroy(void* storage, size_t num) const {
    LOGPF();
}

template<class TYPE>
UTILS_VECTOR_NO_CFI void Vector<TYPE>::do_copy(void* dest, const void* from, size_t num) const {
    LOGPF();
}

template<class TYPE>
UTILS_VECTOR_NO_CFI void Vector<TYPE>::do_splat(void* dest, const void* item, size_t num) const {
    LOGPF();
}

template<class TYPE>
UTILS_VECTOR_NO_CFI void Vector<TYPE>::do_move_forward(void* dest, const void* from, size_t num) const {
    LOGPF();
}

template<class TYPE>
UTILS_VECTOR_NO_CFI void Vector<TYPE>::do_move_backward(void* dest, const void* from, size_t num) const {
    LOGPF();
}

/////////

template <class TYPE>
class SortedVector : private SortedVectorImpl
{
    friend class Vector<TYPE>;

public:
            typedef TYPE    value_type;
                            SortedVector();
                            SortedVector(const SortedVector<TYPE>& rhs);
    virtual                 ~SortedVector();

    const SortedVector<TYPE>&   operator = (const SortedVector<TYPE>& rhs) const;    
    SortedVector<TYPE>&         operator = (const SortedVector<TYPE>& rhs);    

    inline  void            clear()             { VectorImpl::clear(); }
    inline  size_t          size() const                { return VectorImpl::size(); }
    inline  bool            isEmpty() const             { return VectorImpl::isEmpty(); }
    inline  size_t          capacity() const            { return VectorImpl::capacity(); }
    inline  ssize_t         setCapacity(size_t size)    { return VectorImpl::setCapacity(size); }
    inline  const TYPE*     array() const;
            TYPE*           editArray();
            ssize_t         indexOf(const TYPE& item) const;
            size_t          orderOf(const TYPE& item) const;
    inline  const TYPE&     operator [] (size_t index) const;
    inline  const TYPE&     itemAt(size_t index) const;
            const TYPE&     top() const;
            ssize_t         add(const TYPE& item);
            TYPE&           editItemAt(size_t index) {
                return *( static_cast<TYPE *>(VectorImpl::editItemLocation(index)) );
            }
            ssize_t         merge(const Vector<TYPE>& vector);
            ssize_t         merge(const SortedVector<TYPE>& vector);
            ssize_t         remove(const TYPE&);
    inline  ssize_t         removeItemsAt(size_t index, size_t count = 1);
    inline  ssize_t         removeAt(size_t index)  { return removeItemsAt(index); }
            
protected:
    virtual void    do_construct(void* storage, size_t num) const;
    virtual void    do_destroy(void* storage, size_t num) const;
    virtual void    do_copy(void* dest, const void* from, size_t num) const;
    virtual void    do_splat(void* dest, const void* item, size_t num) const;
    virtual void    do_move_forward(void* dest, const void* from, size_t num) const;
    virtual void    do_move_backward(void* dest, const void* from, size_t num) const;
    virtual int     do_compare(const void* lhs, const void* rhs) const;
};

template<typename T> struct trait_trivial_move<SortedVector<T> > { enum { value = true }; };

template<class TYPE> inline
SortedVector<TYPE>::SortedVector()
    : SortedVectorImpl(sizeof(TYPE),
                ((traits<TYPE>::has_trivial_ctor   ? HAS_TRIVIAL_CTOR   : 0)
                |(traits<TYPE>::has_trivial_dtor   ? HAS_TRIVIAL_DTOR   : 0)
                |(traits<TYPE>::has_trivial_copy   ? HAS_TRIVIAL_COPY   : 0))
                )
{
}

template<class TYPE> inline
SortedVector<TYPE>::SortedVector(const SortedVector<TYPE>& rhs)
    : SortedVectorImpl(rhs) {
}

template<class TYPE> inline
SortedVector<TYPE>::~SortedVector() {
    finish_vector();
}

template<class TYPE> inline
SortedVector<TYPE>& SortedVector<TYPE>::operator = (const SortedVector<TYPE>& rhs) {
    SortedVectorImpl::operator = (rhs);
    return *this; 
}

template<class TYPE> inline
const SortedVector<TYPE>& SortedVector<TYPE>::operator = (const SortedVector<TYPE>& rhs) const {
    SortedVectorImpl::operator = (rhs);
    return *this; 
}

template<class TYPE> inline
const TYPE* SortedVector<TYPE>::array() const {
    return static_cast<const TYPE *>(arrayImpl());
}

template<class TYPE> inline
TYPE* SortedVector<TYPE>::editArray() {
    return static_cast<TYPE *>(editArrayImpl());
}


template<class TYPE> inline
const TYPE& SortedVector<TYPE>::operator[](size_t index) const {
    return *(array() + index);
}

template<class TYPE> inline
const TYPE& SortedVector<TYPE>::itemAt(size_t index) const {
    return operator[](index);
}

template<class TYPE> inline
const TYPE& SortedVector<TYPE>::top() const {
    return *(array() + size() - 1);
}

template<class TYPE> inline
ssize_t SortedVector<TYPE>::add(const TYPE& item) {
    return SortedVectorImpl::add(&item);
}

template<class TYPE> inline
ssize_t SortedVector<TYPE>::indexOf(const TYPE& item) const {
    return SortedVectorImpl::indexOf(&item);
}

template<class TYPE> inline
size_t SortedVector<TYPE>::orderOf(const TYPE& item) const {
    return SortedVectorImpl::orderOf(&item);
}

template<class TYPE> inline
ssize_t SortedVector<TYPE>::merge(const Vector<TYPE>& vector) {
    return SortedVectorImpl::merge(reinterpret_cast<const VectorImpl&>(vector));
}

template<class TYPE> inline
ssize_t SortedVector<TYPE>::merge(const SortedVector<TYPE>& vector) {
    return SortedVectorImpl::merge(reinterpret_cast<const SortedVectorImpl&>(vector));
}

template<class TYPE> inline
ssize_t SortedVector<TYPE>::remove(const TYPE& item) {
    return SortedVectorImpl::remove(&item);
}

template<class TYPE> inline
ssize_t SortedVector<TYPE>::removeItemsAt(size_t index, size_t count) {
    return VectorImpl::removeItemsAt(index, count);
}

template<class TYPE>
void SortedVector<TYPE>::do_construct(void* storage, size_t num) const {
    construct_type( reinterpret_cast<TYPE*>(storage), num );
}

template<class TYPE>
void SortedVector<TYPE>::do_destroy(void* storage, size_t num) const {
    destroy_type( reinterpret_cast<TYPE*>(storage), num );
}

template<class TYPE>
void SortedVector<TYPE>::do_copy(void* dest, const void* from, size_t num) const {
    copy_type( reinterpret_cast<TYPE*>(dest), reinterpret_cast<const TYPE*>(from), num );
}

template<class TYPE>
void SortedVector<TYPE>::do_splat(void* dest, const void* item, size_t num) const {
    splat_type( reinterpret_cast<TYPE*>(dest), reinterpret_cast<const TYPE*>(item), num );
}

template<class TYPE>
void SortedVector<TYPE>::do_move_forward(void* dest, const void* from, size_t num) const {
    move_forward_type( reinterpret_cast<TYPE*>(dest), reinterpret_cast<const TYPE*>(from), num );
}

template<class TYPE>
void SortedVector<TYPE>::do_move_backward(void* dest, const void* from, size_t num) const {
    move_backward_type( reinterpret_cast<TYPE*>(dest), reinterpret_cast<const TYPE*>(from), num );
}

template<class TYPE>
int SortedVector<TYPE>::do_compare(const void* lhs, const void* rhs) const {
    return compare_type( *reinterpret_cast<const TYPE*>(lhs), *reinterpret_cast<const TYPE*>(rhs) );
}

};
