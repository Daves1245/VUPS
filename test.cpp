namespace nostd {
    // type_t, size_t
    template<class T>struct tag{using type=T;};
    template<class Tag>using type_t=typename Tag::type;
    using size_t=decltype(sizeof(int));
}
#ifndef _COMPILE_VS_NATIVE_
using nostd::size_t;
#endif

#ifdef _COMPILE_VS_NATIVE_

// Handle memory allocator that can keep track of memory leaks

#include <stdlib.h>
#include <stdio.h> 
#include <iostream>
#include <map>

//#define TRACK_MEMORY

std::map<void*, std::size_t> mem;
bool count_allocation = true;
int total_memory_usage = 0;

void* operator new(size_t size) 
{
    void* p = malloc(size); 
#ifdef TRACK_MEMORY
    if (count_allocation) {
        //std::cout << "Allocating " << size << " bytes at " << p << std::endl; 
        count_allocation = false;
        mem[p] = size;
        total_memory_usage += size;
        count_allocation = true;
    }
#endif
    return p; 
} 
  
void operator delete(void* p) 
{
    //printf("Deallocating %p\n", p);
#ifdef TRACK_MEMORY
    if (count_allocation && mem.count(p)) {
        size_t bytes = mem[p];
        mem.erase(p);
        total_memory_usage -= bytes;
        //std::cout << "Deallocating " << bytes << " bytes at " << p << std::endl; 
        std::cout << total_memory_usage << " remaining" << std::endl;
    }
#endif
    free(p); 
}

#endif

#ifndef _COMPILE_VS_NATIVE_
// Externs
struct string;
extern "C" {
    [[noreturn]] extern void panic(string* s);
    extern void print(string* s);
}

// Default placement versions of operator new.
inline void* operator new(nostd::size_t, void* __p) throw() { return __p; }
inline void* operator new[](nostd::size_t, void* __p) throw() { return __p; }

// Default placement versions of operator delete.
inline void  operator delete  (void*, void*) throw() { }
inline void  operator delete[](void*, void*) throw() { }
#endif

namespace nostd {
    // https://stackoverflow.com/questions/32074410/stdfunction-bind-like-type-erasure-without-standard-c-library

    // Move
    template<class T>
    T&& move(T&t){return static_cast<T&&>(t);}

    // Swap
    template<typename T> void swap(T& t1, T& t2) {
        T temp = nostd::move(t1); // or T temp(std::move(t1));
        t1 = nostd::move(t2);
        t2 = nostd::move(temp);
    }

    // Forward
    template<class T>
    struct remove_reference:tag<T>{};
    template<class T>
    struct remove_reference<T&>:tag<T>{};
    template<class T>using remove_reference_t=type_t<remove_reference<T>>;

    template<class T>
    T&& forward( remove_reference_t<T>& t ) {
    return static_cast<T&&>(t);
    }
    template<class T>
    T&& forward( remove_reference_t<T>&& t ) {
    return static_cast<T&&>(t);
    }

    // Decay
    template<class T>
    struct remove_const:tag<T>{};
    template<class T>
    struct remove_const<T const>:tag<T>{};

    template<class T>
    struct remove_volatile:tag<T>{};
    template<class T>
    struct remove_volatile<T volatile>:tag<T>{};

    template<class T>
    struct remove_cv:remove_const<type_t<remove_volatile<T>>>{};


    template<class T>
    struct decay3:remove_cv<T>{};
    template<class R, class...Args>
    struct decay3<R(Args...)>:tag<R(*)(Args...)>{};
    template<class T>
    struct decay2:decay3<T>{};
    template<class T, size_t N>
    struct decay2<T[N]>:tag<T*>{};

    template<class T>
    struct decay:decay2<remove_reference_t<T>>{};

    template<class T>
    using decay_t=type_t<decay<T>>;

    // is_convertable
    template<class T>
    T declval(); // no implementation

    template<class T, T t>
    struct integral_constant{
    static constexpr T value=t;
    constexpr integral_constant() {};
    constexpr operator T()const{ return value; }
    constexpr T operator()()const{ return value; }
    };
    template<bool b>
    using bool_t=integral_constant<bool, b>;
    using true_type=bool_t<true>;
    using false_type=bool_t<false>;

    template<class...>struct voider:tag<void>{};
    template<class...Ts>using void_t=type_t<voider<Ts...>>;

    namespace details {
    template<template<class...>class Z, class, class...Ts>
    struct can_apply:false_type{};
    template<template<class...>class Z, class...Ts>
    struct can_apply<Z, void_t<Z<Ts...>>, Ts...>:true_type{};
    }
    template<template<class...>class Z, class...Ts>
    using can_apply = details::can_apply<Z, void, Ts...>;

    namespace details {
    template<class From, class To>
    using try_convert = decltype( To{declval<From>()} );
    }
    template<class From, class To>
    struct is_convertible : can_apply< details::try_convert, From, To > {};
    template<>
    struct is_convertible<void,void>:true_type{};

    // enable_if
    template<bool, class=void>
    struct enable_if {};
    template<class T>
    struct enable_if<true, T>:tag<T>{};
    template<bool b, class T=void>
    using enable_if_t=type_t<enable_if<b,T>>;

    // result_of
    namespace details {
    template<class F, class...Args>
    using invoke_t = decltype( declval<F>()(declval<Args>()...) );

    template<class Sig,class=void>
    struct result_of {};
    template<class F, class...Args>
    struct result_of<F(Args...), void_t< invoke_t<F, Args...> > >:
        tag< invoke_t<F, Args...> >
    {};
    }
    template<class Sig>
    using result_of = details::result_of<Sig>;
    template<class Sig>
    using result_of_t=type_t<result_of<Sig>>;

    // aligned_storage
    template<size_t size, size_t align>
    struct alignas(align) aligned_storage_t {
    char buff[size];
    };

    // is_same
    template<class A, class B>
    struct is_same:false_type{};
    template<class A>
    struct is_same<A,A>:true_type{};

    // functional
    template<class Sig, size_t sz, size_t algn>
    struct small_task;

    template<class R, class...Args, size_t sz, size_t algn>
    struct small_task<R(Args...), sz, algn>{
        struct vtable_t {
            void(*mover)(void* src, void* dest);
            void(*copyer)(const void* src, void* dest);
            void(*destroyer)(void*);
            R(*invoke)(void const* t, Args&&...args);
            template<class T>
            static vtable_t const* get() {
                static const vtable_t table = {
                    // Mover
                    [](void* src, void*dest) {
                        new(dest) T(nostd::move(*static_cast<T*>(src)));
                    },
                    // Copyer
                    [](const void* src, void*dest) {
                        new(dest) T(*static_cast<T const*>(src));
                    },
                    // Destroyer
                    [](void* t){ static_cast<T*>(t)->~T(); },
                    // Invoker
                    [](void const* t, Args&&...args)->R {
                        return (*static_cast<T const*>(t))(nostd::forward<Args>(args)...);
                    }
                };
                return &table;
            }
        };
        vtable_t const* table = nullptr;
        nostd::aligned_storage_t<sz, algn> data;
        template<class F,
            class dF=nostd::decay_t<F>,
            // don't use this ctor on own type:
            nostd::enable_if_t<!nostd::is_same<dF, small_task>{}>* = nullptr,
            // use this ctor only if the call is legal:
            nostd::enable_if_t<nostd::is_convertible<
            nostd::result_of_t<dF const&(Args...)>, R
            >{}>* = nullptr
        >
        small_task( F&& f ): table( vtable_t::template get<dF>() )
        {
            // a higher quality small_task would handle null function pointers
            // and other "nullable" callables, and construct as a null small_task

            static_assert( sizeof(dF) <= sz, "object too large" );
            static_assert( alignof(dF) <= algn, "object too aligned" );
            new(&data) dF(nostd::forward<F>(f));
        }
        // I find this overload to be useful, as it forces some
        // functions to resolve their overloads nicely:
        // small_task( R(*)(Args...) )
        ~small_task() {
            if (table) {
                table->destroyer(&data);
            }
        }
        small_task(const small_task& o):
            table(o.table)
        {
            if (table) {
                table->copyer(&o.data, &data);
            }
        }
        small_task(small_task&& o):
            table(o.table)
        {
            if (table) {
                table->mover(&o.data, &data);
            }
        }
        small_task(){}
        small_task& operator=(const small_task& o){
            // this is a bit rude and not very exception safe
            // you can do better:
            this->~small_task();
            new(this) small_task( o );
            return *this;
        }
        small_task& operator=(small_task&& o){
            // this is a bit rude and not very exception safe
            // you can do better:
            this->~small_task();
            new(this) small_task( nostd::move(o) );
            return *this;
        }
        explicit operator bool() const {
            return table;
        }
        R operator()(Args...args) const {
            return table->invoke(&data, nostd::forward<Args>(args)...);
        }
    };

    template<class Sig>
    using function = small_task<Sig, sizeof(void*)*4, alignof(void*) >;
}

#ifdef _COMPILE_VS_NATIVE_
#define standard std
#else
#define standard nostd
#endif

#ifdef _COMPILE_VS_NATIVE_
#include <functional>
#include <iostream>
#include <cstring>
#include <ostream>

#include <thread>
#include <chrono>
#include <time.h>
#endif

#ifdef _COMPILE_VS_NATIVE_
bool ostream_dummy = (std::cout << std::boolalpha) ? true : false;
using std::function;
#else
using nostd::function;
#endif

#ifndef _COMPILE_VS_NATIVE_
#endif

struct string {
    int _size;
    const char* _data;
    int size() {
        return _size;
    }
    const char* data() {
        return _data;
    }
    string() : _data(nullptr), _size(0) {}
    string(const char* str) : _data(str), _size(__builtin_strlen(str)) {}
    string(const char* str, int size) : _data(str), _size(size) {}
    string substr(int start, int len) {
        return string(_data + start, len);
    }
    void copy(char* str, int size) {
        __builtin_memcpy(str, _data, size);
    }
    int find(string str) {
        const char* s = __builtin_strstr(_data, str.data());
        if (s == nullptr) {
            return -1;
        } else {
            return (int)(s - _data);
        }
    }
};

#ifndef _COMPILE_VS_NATIVE_
[[noreturn]] void g_abort(string s) {
    panic(&s);
}
void g_print(string s) {
    print(&s);
}
#endif

struct string_buffer {
    string_buffer(int size) : _data(new char[size]), _size(size), _loc((char*)_data) {}
    ~string_buffer() {
        delete[] _data;
    }
    void write(const void* buf, int size) {
        __builtin_memcpy(_loc, buf, size);
        _loc += size;
    }
    void write(char c) {
        *_loc = c;
        _loc++;
    }
    const char* data() {
        return (char*)_data;
    }
    int size() {
        return _loc - _data;
    }
    string to_string() {
        return string(this->data(), this->size());
    }
    void flush() {
        *_loc = '\0';
    }
private:
    char* _data;
    int _size;
    char* _loc;
};

#ifdef _COMPILE_VS_NATIVE_
using std::ostream;
#else
typedef string_buffer ostream;
ostream& operator<<(ostream& os, const char* str) 
{
    os.write(str, __builtin_strlen(str));
    return os; 
}
ostream& operator<<(ostream& os, int integer) 
{
    char buf[16];
    int len = 0;
    int start = 0;
    if (integer < 0) {
        buf[len++] = '-';
        start++;
        integer = -integer;
    }
    if (integer == 0) {
        buf[len++] = '0';
    } else {
        while(integer > 0) {
            buf[len++] = '0' + (integer % 10);
            integer /= 10;
        }
    }
    for(int i = 0; i < (len-start)/2; i++) {
        char tmp = buf[start+i];
        buf[start+i] = buf[len-start-1-i];
        buf[len-start-1-i] = tmp;
    }
    os.write(buf, len);
    return os; 
}
ostream& operator<<(ostream& os, long long integer) 
{
    char buf[16];
    int len = 0;
    int start = 0;
    if (integer < 0) {
        buf[len++] = '-';
        start++;
        integer = -integer;
    }
    if (integer == 0) {
        buf[len++] = '0';
    } else {
        while(integer > 0) {
            buf[len++] = '0' + (integer % 10);
            integer /= 10;
        }
    }
    for(int i = 0; i < (len-start)/2; i++) {
        char tmp = buf[start+i];
        buf[start+i] = buf[len-start-1-i];
        buf[len-start-1-i] = tmp;
    }
    os.write(buf, len);
    return os; 
}
ostream& operator<<(ostream& os, bool val) 
{
    if (val) {
        os.write("true", 4);
    } else {
        os.write("false", 5);
    }
    return os;
}
ostream& operator<<(ostream& os, char c) 
{
    os.write(c);
    return os;
}
ostream& operator<<(ostream& os, float val) 
{
    int first = (int)val;
    os << first;
    os << '.';
    int second = (val - first) * 10000;
    os << second;
    return os; 
}
#endif

ostream& operator<<(ostream& os, string s) 
{
    os.write(s.data(), s.size());
    return os; 
}

[[noreturn]] void _abort(string message, const char* file, int start_line, int start_char, int end_line, int end_char) {
    int buf_size = sizeof(char) * ((message.size() + __builtin_strlen(file) + 2) + 128);
#ifdef _COMPILE_VS_NATIVE_
    ostream& out = std::cout;
#else
    ostream out(buf_size);
#endif

    out << file << ": " << start_line << ":" << start_char << " -> " << end_line << ":" << end_char << " " << message.data() << "\n";
    out.flush();

#ifdef _COMPILE_VS_NATIVE_
    exit(-1);
#else
    g_abort(out.to_string());
    while(true);
#endif
}

#include <initializer_list> // std::initializer_list<T>

template<typename T>
struct dynamic_array {
    dynamic_array() {}
    dynamic_array(int size) {
        resize(size);
    }
    dynamic_array(std::initializer_list<T> list) {
        reserve(list.size());
        _size = list.size();
        int i = 0;
        for(auto& a : list) {
            new(&data[i]) T(a);
            i++;
        }
    }
    ~dynamic_array() {
        resize(0);
        delete[] (char*)data;
    }
    void resize(int new_size) {
        reserve(new_size);
        // Destruct any elements if we must decrease in size
        for(int i = new_size; i < _size; i++) {
            data[i].~T();
        }
        // Default-Construct any new elements
        for(int i = _size; i < new_size; i++) {
            new(&data[i]) T;
        }
        _size = new_size;
    }
    void reserve(int new_capacity) {
        if (new_capacity > capacity) {
            if (capacity * 2 > new_capacity) {
                new_capacity = capacity * 2;
            }
            // Create new buffer
            T* new_data = (T*)(new char[new_capacity * sizeof(T)]);

            // We don't need to memset because we can leave the uninitialized data as undefined
            // They won't access it because .at() checks for size
            // __builtin_memset(new_data + capacity /* * sizeof(T) is automatic */, 0, (new_capacity - capacity) * sizeof(T));

            for(int i = 0; i < _size; i++) {
                // Construct new element from old element
                new(&new_data[i]) T(standard::move(data[i]));
                // Deconstruct dead element
                data[i].~T();
            }
            delete[] (char*)data;
            data = new_data;
            capacity = new_capacity;
        }
    }
    T& at(int index) {
        // std::cout << "\nAT INDEX: " << index << std::endl;
        if (index >= _size) {
            _abort("Array Index out-of-bounds", "??", 0, 0, 0, 0);
        }
        return data[index];
    }
    T& operator[](int index) {
        return data[index];
    }
    void push_back(T datum) {
        reserve(_size + 1);
        new(&data[_size]) T(datum);
        _size++;
    }
    int size() {
        return _size;
    }
public:
    T* data = nullptr;
    int _size = 0;
    unsigned int reference_count = 0;
    int capacity = 0;
};

#define id_type unsigned short

// All objects inherit from this
class Object {
public:
    // Keep track of reference count for deallocations
    unsigned int reference_count = 1;
    // Keep track of object id
    const id_type object_id;

    // Initialize a new object
    Object(id_type object_id) : object_id(object_id) {};
};

template<typename T>
class ObjectRef {
public:
    T* obj;
    ObjectRef() : obj(nullptr) {}
    ObjectRef(T* obj) : obj(obj) {
        obj->reference_count++;
    }
    // If we can implicitly convert from U* to T*,
    // then we can create a ObjectRef<T> from an ObjectRef<U>
    template<typename U>
    ObjectRef(ObjectRef<U> other) {
        // Grab reference to Object
        obj = other.obj;
        // Nullify the other ObjectRef
        other.obj = nullptr;
    }
    ~ObjectRef()
    {
        if (obj) {
            obj->reference_count--;
            if (obj->reference_count == 0) {
                delete obj;
            }
        }
    }
    ObjectRef(const ObjectRef<T>& other) // copy constructor
    : ObjectRef(other.obj)
    {}
    ObjectRef(ObjectRef<T>&& other) noexcept // move constructor
    {
        this->obj = other.obj;
        other.obj = nullptr;
    }
    ObjectRef<T>& operator=(const ObjectRef<T>& other) // copy assignment
    {
        return *this = ObjectRef(other);
    }
    ObjectRef<T>& operator=(ObjectRef<T>&& other) noexcept // move assignment
    {
        standard::swap(obj, other.obj);
        return *this;
    }
    T& operator*() const
    {
        if (obj == nullptr) {
            _abort("Null pointer exception", "??", 0, 0, 0, 0);
        }
        return *obj;
    }
    T* operator->() const
    {
        if (obj == nullptr) {
            _abort("Null pointer exception", "??", 0, 0, 0, 0);
        }
        return obj;
    }
    // Define implicit conversion to T*
    operator T*() const { return obj; }
};

// Trait instance
class Trait {
public:
    Object* obj;
    Trait(Object* obj) : obj(obj) {};
};

// Object Instance
typedef Object* ObjectInstance;

#define TRAIT_HEADER \
      class _Instance : public Trait { \
      public: \

#define TRAIT_MID1 \
          class _Vtable { \
          public:

#define TRAIT_MID \
          }; \
          _Instance() : Trait(nullptr) {} \
          _Instance(Object* obj) : Trait(obj) { \
          };

#define TRAIT_FOOTER \
      };

// Array of all vtables
static void* vtbls[1024][1024];

template<typename T>
bool is_class(Object* obj) {
    return obj->object_id == T::object_id;
}

template<typename T>
bool is_trait(Object* obj) {
    return vtbls[obj->object_id][T::trait_id] != nullptr;
}

template<typename T>
Object* cast_to_trait(Object* obj, const char* error_file, int error_start_line, int error_start_char, int error_end_line, int error_end_char) {
    if (!is_trait<T>(obj)) {
        _abort("Fail to cast!", error_file, error_start_line, error_start_char, error_end_line, error_end_char);
    }
    return obj;
}

template<typename T>
T* cast_to_class(Object* obj, const char* error_file, int error_start_line, int error_start_char, int error_end_line, int error_end_char) {
    if (!is_class<T>(obj)) {
        _abort("Fail to cast!", error_file, error_start_line, error_start_char, error_end_line, error_end_char);
    }
    return static_cast<T*>(obj);
}

namespace _Array_ {
  template<typename T>
  void push(dynamic_array<T>* arr, T val) {
      if (arr == nullptr) {
          _abort("Null pointer exception", "??", 0, 0, 0, 0);
      }
      arr->push_back(val);
  }
  template<typename T>
  T pop(dynamic_array<T>* arr, T val) {
      if (arr == nullptr) {
          _abort("Null pointer exception", "??", 0, 0, 0, 0);
      }
      T ret = arr->back();
      arr->pop_back();
      return ret;
  }
  template<typename T>
  T remove(dynamic_array<T>* arr, int index) {
      if (arr == nullptr) {
          _abort("Null pointer exception", "??", 0, 0, 0, 0);
      }
      T ret = arr->at(index);
      arr->erase(arr->begin() + index);
      return ret;
  }
  template<typename T>
  int size(dynamic_array<T>* arr) {
      if (arr == nullptr) {
          _abort("Null pointer exception", "??", 0, 0, 0, 0);
      }
      return arr->size();
  }
  template<typename T>
  void resize(dynamic_array<T>* arr, int size) {
      if (arr == nullptr) {
          _abort("Null pointer exception", "??", 0, 0, 0, 0);
      }
      arr->resize(size);
  }
  template<typename T>
  int get_capacity(dynamic_array<T>* arr) {
      if (arr == nullptr) {
          _abort("Null pointer exception", "??", 0, 0, 0, 0);
      }
      return arr->capacity();
  }
  template<typename T>
  void set_capacity(dynamic_array<T>* arr, int size) {
      if (arr == nullptr) {
          _abort("Null pointer exception", "??", 0, 0, 0, 0);
      }
      arr->reserve(size);
  }
  template<typename T>
  ObjectRef<dynamic_array<T>> clone(dynamic_array<T>* arr) {
      if (arr == nullptr) {
          _abort("Null pointer exception", "??", 0, 0, 0, 0);
      }
      dynamic_array<T>* ret = new dynamic_array<T>();
      ret->resize(arr->size());
      for(int i = 0; i < arr->size(); i++) {
          ret[i] = arr[i];
      }
      return ret;
  }
}

namespace _String_ {
  string substring(string str, int start, int end) {
      start = ((start % str.size()) + str.size()) % str.size();
      end = ((end % str.size()) + str.size()) % str.size();
      if (start < end) {
          return str.substr(start, end - start);
      } else {
          return string("", 0);
      }
  }
  ObjectRef<dynamic_array<string>> split(string str, string split_on) {
      dynamic_array<string>* ret = new dynamic_array<string>();
      int s;
      int split_size = split_on.size();
      while((s = str.find(split_on)) != -1) {
          ret->push_back(str.substr(0, s));
          str = string(str.data(), s + split_size);
      }
      ret->push_back(str);
      return ret;
  }
  int match(string str, string match_str) {
      return str.find(match_str);
  }
  int size(string str) {
      return str.size();
  }
  string concat(string self, string str) {
      char* buf = new char[self.size() + str.size()];
      self.copy(buf, self.size());
      str.copy(buf + self.size(), str.size());

      return string(buf, self.size() + str.size());
  }
  bool is_equal(string self, string str) {
      if (self.size() != str.size()) {
          return false;
      } else {
          return __builtin_memcmp(self.data(), str.data(), self.size()) == 0;
      }
  }
  int to_integer(string self) {
    unsigned int ret = 0;
    bool neg = false;
    for(int i = 0; i < self.size(); i++) {
        if (i == 0 && self.data()[i] == '-') neg = true;
        char c = self.data()[i];
        if (c < '0' || c > '9') {
            return false;
        }
        ret *= 10;
        ret += (c - '0');
    }
    return neg ? -(int)ret : (int)ret;
  }
  bool is_integer(string self) {
    unsigned int ret = 0;
    for(int i = 0; i < self.size(); i++) {
        if (i == 0 && self.data()[i] == '-') continue;
        char c = self.data()[i];
        if (c < '0' || c > '9') {
            return false;
        }
        ret *= 10;
        ret += (c - '0');
    }
    return true;
  }
}

/*
Export environment using these structs
*/

struct _Env_String {
    int size;
    void* str;
    _Env_String(int size, void* str) : size(size), str(str) {}
};

struct _Env_Array {
    int size;
    void* buf;
    _Env_Array(int size, void* buf) : size(size), buf(buf) {}
};

// **************
// Global Traits
// **************

namespace _Trait_Printable {
  TRAIT_HEADER
  static const id_type trait_id = 1;
  TRAIT_MID1
      typedef void (*_Function_print_type)(Object*);
      _Vtable(
          _Function_print_type _Function_print
      ) :
          _Function_print(_Function_print)
      {};
      _Function_print_type _Function_print;
  TRAIT_MID
      // Dynamic dispatch of trait function calls
      static void _Function_print(Object* object) {
          ((_Vtable*)vtbls[object->object_id][trait_id])->_Function_print(object);
      }
  TRAIT_FOOTER
}

// **************
// Global functions
// **************

// Overload cout for arrays

ostream& operator<<(ostream& os, Object* v) 
{
    if (is_trait<_Trait_Printable::_Instance>(v)) {
        ((_Trait_Printable::_Instance::_Vtable*)vtbls[v->object_id][_Trait_Printable::_Instance::trait_id])->_Function_print(v);
    } else {
        if (v) {
            long long a = (long long)v;
            os << "Object<id=" << v->object_id << ",instance=" << a << ">";
        } else {
            os << "Object<null>";
        }
    }
    return os; 
}

template <typename T> 
ostream& operator<<(ostream& os, const dynamic_array<T>* v) 
{ 
    os << "["; 
    for (int i = 0; i < v->size(); ++i) { 
        os << (*v)[i]; 
        if (i != v->size() - 1) 
            os << ", "; 
    } 
    os << "]\\n";
    return os; 
}

// Variadic print statement

template<typename Value, typename... Values>
void _VS_print( Value v, Values... vs )
{
#ifdef _COMPILE_VS_NATIVE_
    ostream& out = std::cout;
#else
    ostream out(4096);
#endif
    using expander = int[];
    out << v; // first
    (void) expander{ 0, (out << " " << vs, void(), 0)... };
    out << "\n";
    out.flush();
#ifndef _COMPILE_VS_NATIVE_
    g_print(out.to_string());
#endif
}
void _VS_print()
{
#ifdef _COMPILE_VS_NATIVE_
    ostream& out = std::cout;
#else
    ostream out(4096);
#endif
    out << "\n";
    out.flush();
#ifndef _COMPILE_VS_NATIVE_
    g_print(out.to_string());
#endif
}

template<typename Value, typename... Values>
void _VS_raw_print( Value v, Values... vs )
{
#ifdef _COMPILE_VS_NATIVE_
    ostream& out = std::cout;
#else
    ostream out(4096);
#endif
    using expander = int[];
    out << v; // first
    (void) expander{ 0, (out << vs, void(), 0)... };
    out.flush();
#ifndef _COMPILE_VS_NATIVE_
    g_print(out.to_string());
#endif
}
void _VS_raw_print()
{
}

string _VS_input()
{
#ifdef _COMPILE_VS_NATIVE_
    std::string* ret = new std::string();
    std::getline(std::cin, *ret);
    return string(ret->c_str(), ret->size());
#else
    return string("", 0);
#endif
}

void _VS_sleep(int ms) {
  if (ms > 0) {
#ifdef _COMPILE_VS_NATIVE_
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#endif
  }
}

int _VS_time() {
#ifdef _COMPILE_VS_NATIVE_
  return (int)time( nullptr );
#else
  return 0;
#endif
}

#ifdef _COMPILE_VS_NATIVE_
#define MAIN int main() {return 0;}
#else
#define MAIN
#endif

// **************
// End Global functions
// **************
// Start Module Mergesort.vs
    namespace _Class_Mergesort {
        class _Instance : public Object {
        public:
            static const id_type object_id = 1;
            static ObjectRef<_Class_Mergesort::_Instance> _create_class() {
                ObjectRef<_Class_Mergesort::_Instance> ret = new _Class_Mergesort::_Instance();
                ret->reference_count--;
                return ret;
            }
            _Instance() : Object(object_id) {
                ObjectRef<_Class_Mergesort::_Instance> self = this;
            }
            void _Function_mergesort(ObjectRef<dynamic_array<int>> _VS_arr) {
                ObjectRef<_Class_Mergesort::_Instance> self = this;
                (((self)->_Function__mergesort)((_VS_arr), (0), (((_Array_::size)((_VS_arr).obj)) - (1))));
            }
            void _Function_p(ObjectRef<dynamic_array<int>> _VS_arr) {
                ObjectRef<_Class_Mergesort::_Instance> self = this;
                for(int _VS_i = (0);
                 ((_VS_i) < ((_Array_::size)((_VS_arr).obj))); ((_VS_i)++)) {
                    ((_VS_raw_print)(((_VS_arr)->at((_VS_i))), (string(" ", strlen(" ")))));
                }
                
                ((_VS_print)());
            }
            void _Function_merge(ObjectRef<dynamic_array<int>> _VS_arr, int _VS_l, int _VS_m, int _VS_r) {
                ObjectRef<_Class_Mergesort::_Instance> self = this;
                int _VS_a = (0);
                int _VS_b = (0);
                int _VS_c = (0);
                int _VS_n1 = (((_VS_m) - (_VS_l)) + (1));
                int _VS_n2 = ((_VS_r) - (_VS_m));
                ObjectRef<dynamic_array<int>> _VS_left = (new dynamic_array<int>({}));
                ObjectRef<dynamic_array<int>> _VS_right = (new dynamic_array<int>({}));
                for(int _VS_i = (0);
                 ((_VS_i) < (_VS_n1)); ((_VS_i)++)) {
                    ((_Array_::push)((_VS_left).obj, ((_VS_arr)->at(((_VS_l) + (_VS_i))))));
                }
                
                for(int _VS_i = (0);
                 ((_VS_i) < (_VS_n2)); ((_VS_i)++)) {
                    ((_Array_::push)((_VS_right).obj, ((_VS_arr)->at((((_VS_m) + (1)) + (_VS_i))))));
                }
                
                while (((_VS_a) < (_VS_n1)) && ((_VS_b) < (_VS_n2))) {
                    (((self)->_Function_p)((_VS_arr)));
                    if (((_VS_left)->at((_VS_a))) <= ((_VS_right)->at((_VS_b)))) {
                        ((_VS_print)((_VS_c), (string(" = ", strlen(" = "))), ((_VS_left)->at((_VS_a)))));
                        (((_VS_arr)->at((_VS_c))) = ((_VS_left)->at((_VS_a))));
                        ((_VS_a)++);
                    }
                     else {
                        ((_VS_print)((_VS_c), (string(" = ", strlen(" = "))), ((_VS_right)->at((_VS_b)))));
                        std::cout << _VS_c << " = " << _VS_right->at(_VS_b);
                        (((_VS_arr)->at((_VS_c))) = ((_VS_right)->at((_VS_b))));
                        ((_VS_b)++);
                    }
                    
                    (((self)->_Function_p)((_VS_arr)));
                    ((_VS_c)++);
                }
                
                while ((_VS_a) < (_VS_n1)) {
                    ((_VS_print)((_VS_c), (string(" = ", strlen(" = "))), ((_VS_left)->at((_VS_a)))));
                    (((_VS_arr)->at((_VS_c))) = ((_VS_left)->at((_VS_a))));
                    ((_VS_a)++);
                    ((_VS_c)++);
                }
                
                while ((_VS_b) < (_VS_n2)) {
                    ((_VS_print)((_VS_c), (string(" = ", strlen(" = "))), ((_VS_right)->at((_VS_b)))));
                    (((_VS_arr)->at((_VS_c))) = ((_VS_right)->at((_VS_b))));
                    ((_VS_b)++);
                    ((_VS_c)++);
                }
                
            }
            void _Function__mergesort(ObjectRef<dynamic_array<int>> _VS_arr, int _VS_l, int _VS_r) {
                ObjectRef<_Class_Mergesort::_Instance> self = this;
                if ((_VS_l) < (_VS_r)) {
                    int _VS_m = ((_VS_l) + (((_VS_r) - (_VS_l)) / (2)));
                    (((self)->_Function__mergesort)((_VS_arr), (_VS_l), (_VS_m)));
                    (((self)->_Function__mergesort)((_VS_arr), ((_VS_m) + (1)), (_VS_r)));
                    (((self)->_Function_merge)((_VS_arr), (_VS_l), (_VS_m), (_VS_r)));
                }
                
            }
        };
    }
// End Module Mergesort.vs
// Start Module Quicksort.vs
    namespace _Class_Quicksort {
        class _Instance : public Object {
        public:
            static const id_type object_id = 2;
            static ObjectRef<_Class_Quicksort::_Instance> _create_class() {
                ObjectRef<_Class_Quicksort::_Instance> ret = new _Class_Quicksort::_Instance();
                ret->reference_count--;
                return ret;
            }
            _Instance() : Object(object_id) {
                ObjectRef<_Class_Quicksort::_Instance> self = this;
            }
            void _Function_quicksort(ObjectRef<dynamic_array<int>> _VS_arr) {
                ObjectRef<_Class_Quicksort::_Instance> self = this;
                (((self)->_Function___quick_sort)((_VS_arr), (0), (((_Array_::size)((_VS_arr).obj)) - (1))));
            }
            void _Function_swap(ObjectRef<dynamic_array<int>> _VS_arr, int _VS_a, int _VS_b) {
                ObjectRef<_Class_Quicksort::_Instance> self = this;
                int _VS_tmp = ((_VS_arr)->at((_VS_a)));
                (((_VS_arr)->at((_VS_a))) = ((_VS_arr)->at((_VS_b))));
                (((_VS_arr)->at((_VS_b))) = (_VS_tmp));
            }
            int _Function_partition(ObjectRef<dynamic_array<int>> _VS_arr, int _VS_l, int _VS_h) {
                ObjectRef<_Class_Quicksort::_Instance> self = this;
                int _VS_p = ((_VS_arr)->at((_VS_h)));
                int _VS_ei = ((_VS_l) - (1));
                for(int _VS_i = (_VS_l);
                 ((_VS_i) < (_VS_h)); ((_VS_i)++)) {
                    if (((_VS_arr)->at((_VS_i))) <= (_VS_p)) {
                        ((_VS_ei)++);
                        (((self)->_Function_swap)((_VS_arr), (_VS_ei), (_VS_i)));
                    }
                    
                }
                
                (((self)->_Function_swap)((_VS_arr), ((_VS_ei) + (1)), (_VS_h)));
                return ((_VS_ei) + (1));
                _abort("Did not return from function, when it should have returned a int", "Quicksort.vs", 13, 5, 27, 5);
            }
            void _Function___quick_sort(ObjectRef<dynamic_array<int>> _VS_arr, int _VS_l, int _VS_h) {
                ObjectRef<_Class_Quicksort::_Instance> self = this;
                if ((_VS_l) < (_VS_h)) {
                    int _VS_pi = (((self)->_Function_partition)((_VS_arr), (_VS_l), (_VS_h)));
                    (((self)->_Function___quick_sort)((_VS_arr), (_VS_l), ((_VS_pi) - (1))));
                    (((self)->_Function___quick_sort)((_VS_arr), ((_VS_pi) + (1)), (_VS_h)));
                }
                
            }
        };
    }
// End Module Quicksort.vs
// Start Module Hashable.vs
    namespace _Trait_Hash {
        TRAIT_HEADER
            static const id_type trait_id = 2;
        TRAIT_MID1
            typedef int (*_Function_hash_type)(Object*);
            _Vtable(
                _Function_hash_type _Function_hash
            ) :
                _Function_hash(_Function_hash)
            {};
            _Function_hash_type _Function_hash;
        TRAIT_MID
            // Dynamic dispatch of trait function calls
            static int _Function_hash(ObjectRef<Object> object) {
                return ((_Vtable*)vtbls[object->object_id][trait_id])->_Function_hash(object);
            }
        TRAIT_FOOTER
    }
// End Module Hashable.vs
// Start Module Operators.vs
    namespace _Trait_Eq {
        TRAIT_HEADER
            static const id_type trait_id = 3;
        TRAIT_MID1
            typedef bool (*_Function_is_equal_type)(Object*, ObjectRef<Object>);
            _Vtable(
                _Function_is_equal_type _Function_is_equal
            ) :
                _Function_is_equal(_Function_is_equal)
            {};
            _Function_is_equal_type _Function_is_equal;
        TRAIT_MID
            // Dynamic dispatch of trait function calls
            static bool _Function_is_equal(ObjectRef<Object> object, ObjectRef<Object> a) {
                return ((_Vtable*)vtbls[object->object_id][trait_id])->_Function_is_equal(object, a);
            }
        TRAIT_FOOTER
    }
    namespace _Class_Pair {
        template <typename T0, typename T1>
        class _Instance : public Object {
        public:
            static const id_type object_id = 3;
            static ObjectRef<_Class_Pair::_Instance<T0, T1>> _create_class(T0 _VS_first, T1 _VS_second) {
                ObjectRef<_Class_Pair::_Instance<T0, T1>> ret = new _Class_Pair::_Instance<T0, T1>(_VS_first, _VS_second);
                ret->reference_count--;
                return ret;
            }
            _Instance(T0 _VS_first, T1 _VS_second) : Object(object_id) {
                ObjectRef<_Class_Pair::_Instance<T0, T1>> self = this;
                (((self)->_VS_first) = (_VS_first));
                (((self)->_VS_second) = (_VS_second));
            }
            T0 _VS_first;
            T1 _VS_second;
        };
    }
    namespace _Trait_Comparable {
        TRAIT_HEADER
            static const id_type trait_id = 4;
        TRAIT_MID1
            typedef int (*_Function_compareTo_type)(Object*, ObjectRef<Object>);
            _Vtable(
                _Function_compareTo_type _Function_compareTo
            ) :
                _Function_compareTo(_Function_compareTo)
            {};
            _Function_compareTo_type _Function_compareTo;
        TRAIT_MID
            // Dynamic dispatch of trait function calls
            static int _Function_compareTo(ObjectRef<Object> object, ObjectRef<Object> a) {
                return ((_Vtable*)vtbls[object->object_id][trait_id])->_Function_compareTo(object, a);
            }
        TRAIT_FOOTER
    }
// End Module Operators.vs
// Start Module BigInteger.vs
    namespace _Class_BigInteger {
        class _Instance : public Object {
        public:
            static const id_type object_id = 4;
            static ObjectRef<_Class_BigInteger::_Instance> _create_class(int _VS_val) {
                ObjectRef<_Class_BigInteger::_Instance> ret = new _Class_BigInteger::_Instance(_VS_val);
                ret->reference_count--;
                return ret;
            }
            _Instance(int _VS_val) : Object(object_id) {
                ObjectRef<_Class_BigInteger::_Instance> self = this;
                (((self)->_VS_val) = (_VS_val));
            }
            int _VS_val;
        };
    }
    namespace _Class_BigInteger {
        namespace _Implement_Comparable_On_BigInteger {
            int _Function_compareTo(Object* self_void, ObjectRef<Object> _VS_i);
            int _Function_compareTo(Object* self_void, ObjectRef<Object> _VS_i) {
                ObjectRef<_Class_BigInteger::_Instance> const self = static_cast<_Class_BigInteger::_Instance*>(self_void);
                return (((self)->_VS_val) - ((cast_to_class<_Class_BigInteger::_Instance>((_VS_i), "BigInteger.vs", 16, 29, 16, 39))->_VS_val));
                _abort("Did not return from function, when it should have returned a int", "BigInteger.vs", 15, 5, 18, 1);
            }
            static _Trait_Comparable::_Instance::_Vtable _Vtable = _Trait_Comparable::_Instance::_Vtable(_Function_compareTo);
            static bool dummy = (vtbls[_Class_BigInteger::_Instance::object_id][_Trait_Comparable::_Instance::trait_id] = &_Vtable);
        }
    }
// End Module BigInteger.vs
// Start Module Heap.vs
    namespace _Class_Heap {
        template <typename T0>
        class _Instance : public Object {
        public:
            static const id_type object_id = 5;
            static ObjectRef<_Class_Heap::_Instance<T0>> _create_class() {
                ObjectRef<_Class_Heap::_Instance<T0>> ret = new _Class_Heap::_Instance<T0>();
                ret->reference_count--;
                return ret;
            }
            _Instance() : Object(object_id) {
                ObjectRef<_Class_Heap::_Instance<T0>> self = this;
            }
            void _Function_swap(ObjectRef<dynamic_array<T0>> _VS_arr, int _VS_a, int _VS_b) {
                ObjectRef<_Class_Heap::_Instance<T0>> self = this;
                T0 _VS_tmp = ((_VS_arr)->at((_VS_a)));
                (((_VS_arr)->at((_VS_a))) = ((_VS_arr)->at((_VS_b))));
                (((_VS_arr)->at((_VS_b))) = (_VS_tmp));
            }
            int _Function_parent(int _VS_i) {
                ObjectRef<_Class_Heap::_Instance<T0>> self = this;
                return ((_VS_i) / (2));
                _abort("Did not return from function, when it should have returned a int", "Heap.vs", 15, 5, 18, 5);
            }
            int _Function_left(int _VS_i) {
                ObjectRef<_Class_Heap::_Instance<T0>> self = this;
                return ((2) * (_VS_i));
                _abort("Did not return from function, when it should have returned a int", "Heap.vs", 18, 5, 21, 5);
            }
            int _Function_right(int _VS_i) {
                ObjectRef<_Class_Heap::_Instance<T0>> self = this;
                return (((2) * (_VS_i)) + (1));
                _abort("Did not return from function, when it should have returned a int", "Heap.vs", 21, 5, 25, 5);
            }
            void _Function_percolateDown(ObjectRef<dynamic_array<T0>> _VS_arr, int _VS_i) {
                ObjectRef<_Class_Heap::_Instance<T0>> self = this;
                int _VS_swapi = (_VS_i);
                if (((((self)->_Function_left)((_VS_i))) <= ((self)->_VS_size)) && (((_Trait_Comparable::_Instance::_Function_compareTo)(((_VS_arr)->at((_VS_i))), ((_VS_arr)->at((((self)->_Function_left)((_VS_i))))))) < (0))) {
                    ((_VS_swapi) = (((self)->_Function_left)((_VS_i))));
                }
                
                if (((((self)->_Function_right)((_VS_i))) <= ((self)->_VS_size)) && (((_Trait_Comparable::_Instance::_Function_compareTo)(((_VS_arr)->at((_VS_i))), ((_VS_arr)->at((((self)->_Function_right)((_VS_i))))))) < (0))) {
                    ((_VS_swapi) = (((self)->_Function_right)((_VS_i))));
                }
                
                if ((_VS_swapi) != (_VS_i)) {
                    (((self)->_Function_swap)((_VS_arr), (_VS_swapi), (_VS_i)));
                }
                
            }
            void _Function_percolateUp(ObjectRef<dynamic_array<T0>> _VS_arr, int _VS_i) {
                ObjectRef<_Class_Heap::_Instance<T0>> self = this;
                while (((_VS_i) != (0)) && (((_Trait_Comparable::_Instance::_Function_compareTo)(((_VS_arr)->at((_VS_i))), ((_VS_arr)->at((((self)->_Function_parent)((_VS_i))))))) != (0))) {
                    (((self)->_Function_swap)((_VS_arr), (_VS_i), (((self)->_Function_parent)((_VS_i)))));
                    ((_VS_i) = (((self)->_Function_parent)((_VS_i))));
                }
                
            }
            void _Function_insert(ObjectRef<dynamic_array<T0>> _VS_arr, T0 _VS_val) {
                ObjectRef<_Class_Heap::_Instance<T0>> self = this;
                if (((self)->_VS_size) == ((_Array_::size)((_VS_arr).obj))) {
                    ((_Array_::resize)((_VS_arr).obj, ((2) * ((_Array_::size)((_VS_arr).obj)))));
                }
                
                (((self)->_VS_size)++);
                (((_VS_arr)->at(((self)->_VS_size))) = (_VS_val));
                (((self)->_Function_percolateUp)((_VS_arr), ((self)->_VS_size)));
            }
            T0 _Function_pop(ObjectRef<dynamic_array<T0>> _VS_arr) {
                ObjectRef<_Class_Heap::_Instance<T0>> self = this;
                T0 _VS_ret = ((_VS_arr)->at((0)));
                (((self)->_Function_swap)((_VS_arr), (0), ((self)->_VS_size)));
                (((self)->_VS_size)--);
                (((self)->_Function_percolateDown)((_VS_arr), (0)));
                return (_VS_ret);
                _abort("Did not return from function, when it should have returned a template parameter #0 of class Heap", "Heap.vs", 54, 5, 62, 5);
            }
            void _Function_build_heap(ObjectRef<dynamic_array<T0>> _VS_arr) {
                ObjectRef<_Class_Heap::_Instance<T0>> self = this;
                (((self)->_VS_size) = ((_Array_::size)((_VS_arr).obj)));
                for(int _VS_i = ((self)->_VS_size);
                 ((_VS_i) >= (0)); ((_VS_i)--)) {
                    (((self)->_Function_percolateDown)((_VS_arr), (_VS_i)));
                }
                
            }
            void _Function_heapsort(ObjectRef<dynamic_array<T0>> _VS_arr) {
                ObjectRef<_Class_Heap::_Instance<T0>> self = this;
                (((self)->_Function_build_heap)((_VS_arr)));
                for(int _VS_i = ((_Array_::size)((_VS_arr).obj));
                 ((_VS_i) >= (0)); ((_VS_i)--)) {
                    (((self)->_Function_swap)((_VS_arr), (_VS_i), (0)));
                    (((self)->_VS_size)--);
                    (((self)->_Function_percolateDown)((_VS_arr), (0)));
                }
                
            }
            ObjectRef<dynamic_array<T0>> _VS_arr;
            int _VS_size;
        };
    }
// End Module Heap.vs
// Start Module Main.vs
    namespace _Class_Main {
        class _Instance : public Object {
        public:
            static const id_type object_id = 6;
            static ObjectRef<_Class_Main::_Instance> _create_class() {
                ObjectRef<_Class_Main::_Instance> ret = new _Class_Main::_Instance();
                ret->reference_count--;
                return ret;
            }
            _Instance() : Object(object_id) {
                ObjectRef<_Class_Main::_Instance> self = this;
                ObjectRef<_Class_Heap::_Instance<ObjectRef<Object>>> _VS_heap = (_Class_Heap::_Instance<ObjectRef<Object>>::_create_class());
                ObjectRef<dynamic_array<int>> _VS_arr = (new dynamic_array<int>({(10), (9), (8), (7), (6), (5), (4), (3), (2), (1)}));
                ObjectRef<_Class_Quicksort::_Instance> _VS_q = (_Class_Quicksort::_Instance::_create_class());
                ObjectRef<_Class_Mergesort::_Instance> _VS_m = (_Class_Mergesort::_Instance::_create_class());
                (((_VS_m)->_Function_mergesort)((_VS_arr)));
                for(int _VS_i = (0);
                 ((_VS_i) < (10)); ((_VS_i)++)) {
                    ((_VS_raw_print)(((_VS_arr)->at((_VS_i))), (string(" ", strlen(" ")))));
                }
                
            }
        };
    }
    ObjectRef<_Class_Main::_Instance> _VS_m = (_Class_Main::_Instance::_create_class());
// End Module Main.vs
MAIN
#ifndef _COMPILE_VS_NATIVE_
ObjectRef<_Class_Main::_Instance> _entry_point = new _Class_Main::_Instance();
extern "C" {
}
#endif// Start Module Hashmap.vs
    namespace _Class_HashMap {
        template <typename T0, typename T1>
        class _Instance : public Object {
        public:
            static const id_type object_id = 7;
            static ObjectRef<_Class_HashMap::_Instance<T0, T1>> _create_class() {
                ObjectRef<_Class_HashMap::_Instance<T0, T1>> ret = new _Class_HashMap::_Instance<T0, T1>();
                ret->reference_count--;
                return ret;
            }
            _Instance() : Object(object_id) {
                ObjectRef<_Class_HashMap::_Instance<T0, T1>> self = this;
                (((self)->_VS_size) = (0));
                (((self)->_VS_capacity) = (25));
                (((self)->_VS_buckets) = (new dynamic_array<ObjectRef<dynamic_array<ObjectRef<_Class_Pair::_Instance<T0, T1>>>>>({})));
                ((_Array_::resize)(((self)->_VS_buckets).obj, (25)));
                for(int _VS_i = (0);
                 ((_VS_i) < (25)); ((_VS_i)++)) {
                    ((((self)->_VS_buckets)->at((_VS_i))) = (new dynamic_array<ObjectRef<_Class_Pair::_Instance<T0, T1>>>({})));
                }
                
            }
            T1 _Function_get(T0 _VS_k) {
                ObjectRef<_Class_HashMap::_Instance<T0, T1>> self = this;
                int _VS_hash = ((_Trait_Hash::_Instance::_Function_hash)((_VS_k)));
                ObjectRef<dynamic_array<ObjectRef<_Class_Pair::_Instance<T0, T1>>>> _VS_bucket = (((self)->_VS_buckets)->at(((_VS_hash) % ((self)->_VS_capacity))));
                for(int _VS_i = (0);
                 ((_VS_i) < ((_Array_::size)((_VS_bucket).obj))); ((_VS_i)++)) {
                    if ((_Trait_Eq::_Instance::_Function_is_equal)((_VS_k), (((_VS_bucket)->at((_VS_i)))->_VS_first))) {
                        return (((_VS_bucket)->at((_VS_i)))->_VS_second);
                    }
                    
                }
                
                _abort((string("Key not found!", strlen("Key not found!"))), "Hashmap.vs", 37, 9, 38, 5);
                _abort("Did not return from function, when it should have returned a template parameter #1 of class HashMap", "Hashmap.vs", 29, 5, 40, 5);
            }
            bool _Function_is_set(T0 _VS_k) {
                ObjectRef<_Class_HashMap::_Instance<T0, T1>> self = this;
                int _VS_hash = ((_Trait_Hash::_Instance::_Function_hash)((_VS_k)));
                ObjectRef<dynamic_array<ObjectRef<_Class_Pair::_Instance<T0, T1>>>> _VS_bucket = (((self)->_VS_buckets)->at(((_VS_hash) % ((self)->_VS_capacity))));
                for(int _VS_i = (0);
                 ((_VS_i) < ((_Array_::size)((_VS_bucket).obj))); ((_VS_i)++)) {
                    if ((_Trait_Eq::_Instance::_Function_is_equal)((_VS_k), (((_VS_bucket)->at((_VS_i)))->_VS_first))) {
                        return (true);
                    }
                    
                }
                
                return (false);
                _abort("Did not return from function, when it should have returned a bool", "Hashmap.vs", 40, 5, 51, 5);
            }
            void _Function_set(T0 _VS_k, T1 _VS_v) {
                ObjectRef<_Class_HashMap::_Instance<T0, T1>> self = this;
                int _VS_hash = ((_Trait_Hash::_Instance::_Function_hash)((_VS_k)));
                ObjectRef<dynamic_array<ObjectRef<_Class_Pair::_Instance<T0, T1>>>> _VS_bucket = (((self)->_VS_buckets)->at(((_VS_hash) % ((self)->_VS_capacity))));
                int _VS_found = (-(1));
                for(int _VS_i = (0);
                 ((_VS_i) < ((_Array_::size)((_VS_bucket).obj))); ((_VS_i)++)) {
                    if ((_Trait_Eq::_Instance::_Function_is_equal)((_VS_k), (((_VS_bucket)->at((_VS_i)))->_VS_first))) {
                        ((((_VS_bucket)->at((_VS_found)))->_VS_second) = (_VS_v));
                        return ;
                    }
                    
                }
                
                (((self)->_VS_size)++);
                ((_Array_::push)((_VS_bucket).obj, (_Class_Pair::_Instance<T0, T1>::_create_class((_VS_k), (_VS_v)))));
            }
            void _Function_unset(T0 _VS_k) {
                ObjectRef<_Class_HashMap::_Instance<T0, T1>> self = this;
                int _VS_hash = ((_Trait_Hash::_Instance::_Function_hash)((_VS_k)));
                ObjectRef<dynamic_array<ObjectRef<_Class_Pair::_Instance<T0, T1>>>> _VS_bucket = (((self)->_VS_buckets)->at(((_VS_hash) % ((self)->_VS_capacity))));
                int _VS_found = (-(1));
                for(int _VS_i = (0);
                 ((_VS_i) < ((_Array_::size)((_VS_bucket).obj))); ((_VS_i)++)) {
                    if ((_Trait_Eq::_Instance::_Function_is_equal)((_VS_k), (((_VS_bucket)->at((_VS_i)))->_VS_first))) {
                        (((self)->_VS_size)--);
                        ((_Array_::remove)((_VS_bucket).obj, (_VS_i)));
                        return ;
                    }
                    
                }
                
            }
            ObjectRef<dynamic_array<ObjectRef<dynamic_array<ObjectRef<_Class_Pair::_Instance<T0, T1>>>>>> _VS_buckets;
            int _VS_capacity;
            int _VS_size;
        };
    }
// End Module Hashmap.vs
// Start Module Optional.vs
    namespace _Class_Optional {
        template <typename T0>
        class _Instance : public Object {
        public:
            static const id_type object_id = 8;
            static ObjectRef<_Class_Optional::_Instance<T0>> _create_class() {
                ObjectRef<_Class_Optional::_Instance<T0>> ret = new _Class_Optional::_Instance<T0>();
                ret->reference_count--;
                return ret;
            }
            _Instance() : Object(object_id) {
                ObjectRef<_Class_Optional::_Instance<T0>> self = this;
                (((self)->_VS_is_set) = (false));
            }
            void _Function_set_value(T0 _VS_val) {
                ObjectRef<_Class_Optional::_Instance<T0>> self = this;
                (((self)->_VS_val) = (_VS_val));
                (((self)->_VS_is_set) = (true));
            }
            bool _Function_has_value() {
                ObjectRef<_Class_Optional::_Instance<T0>> self = this;
                return ((self)->_VS_is_set);
                _abort("Did not return from function, when it should have returned a bool", "Optional.vs", 21, 5, 24, 5);
            }
            T0 _Function_get_value() {
                ObjectRef<_Class_Optional::_Instance<T0>> self = this;
                if (!((self)->_VS_is_set)) {
                    _abort((string("Tried to get_value when there was no value!", strlen("Tried to get_value when there was no value!"))), "Optional.vs", 26, 13, 27, 9);
                }
                
                return ((self)->_VS_val);
                _abort("Did not return from function, when it should have returned a template parameter #0 of class Optional", "Optional.vs", 24, 5, 30, 1);
            }
            void _Function_clear_value() {
                ObjectRef<_Class_Optional::_Instance<T0>> self = this;
                (((self)->_VS_is_set) = (false));
            }
            bool _VS_is_set;
            T0 _VS_val;
        };
    }
// End Module Optional.vs
