/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef UA_CONFIG_H_
#define UA_CONFIG_H_

/**
 * open62541 Version
 * ----------------- */
#define UA_OPEN62541_VER_MAJOR 1
#define UA_OPEN62541_VER_MINOR 4
#define UA_OPEN62541_VER_PATCH 10
#define UA_OPEN62541_VER_LABEL "-undefined" /* Release candidate label, etc. */
#define UA_OPEN62541_VER_COMMIT "unknown-commit"
#define UA_OPEN62541_VERSION "v1.4.10-undefined"

/**
 * Architecture
 * ------------
 * Define one of the following. */

#define UA_ARCHITECTURE_WIN32
/* #undef UA_ARCHITECTURE_POSIX */

/* Select default architecture if none is selected */
#if !defined(UA_ARCHITECTURE_WIN32) && !defined(UA_ARCHITECTURE_POSIX)
# ifdef _WIN32
#  define UA_ARCHITECTURE_WIN32
# else
#  define UA_ARCHITECTURE_POSIX
# endif
#endif

/**
 * Feature Options
 * ---------------
 * Changing the feature options has no effect on a pre-compiled library. */

#define UA_LOGLEVEL 100
#ifndef UA_ENABLE_AMALGAMATION
/* #undef UA_ENABLE_AMALGAMATION */
#endif
#define UA_ENABLE_METHODCALLS
#define UA_ENABLE_NODEMANAGEMENT
#define UA_ENABLE_SUBSCRIPTIONS
#define UA_ENABLE_PUBSUB
/* #undef UA_ENABLE_PUBSUB_ENCRYPTION */
/* #undef UA_ENABLE_PUBSUB_FILE_CONFIG */
#define UA_ENABLE_PUBSUB_INFORMATIONMODEL
#define UA_ENABLE_DA
#define UA_ENABLE_DIAGNOSTICS
#define UA_ENABLE_HISTORIZING
#define UA_ENABLE_PARSING
#define UA_ENABLE_SUBSCRIPTIONS_EVENTS
#define UA_ENABLE_JSON_ENCODING
/* #undef UA_ENABLE_XML_ENCODING */
/* #undef UA_ENABLE_MQTT */
/* #undef UA_ENABLE_NODESET_INJECTOR */
/* #undef UA_INFORMATION_MODEL_AUTOLOAD */
/* #undef UA_ENABLE_ENCRYPTION_MBEDTLS */
/* #undef UA_ENABLE_CERT_REJECTED_DIR */
/* #undef UA_ENABLE_TPM2_SECURITY */
/* #undef UA_ENABLE_ENCRYPTION_OPENSSL */
/* #undef UA_ENABLE_ENCRYPTION_LIBRESSL */
#if defined(UA_ENABLE_ENCRYPTION_MBEDTLS) || defined(UA_ENABLE_ENCRYPTION_OPENSSL) || defined(UA_ENABLE_ENCRYPTION_LIBRESSL)
#define UA_ENABLE_ENCRYPTION
#endif
/* #undef UA_ENABLE_SUBSCRIPTIONS_ALARMS_CONDITIONS */

/* Multithreading */
/* #undef UA_ENABLE_IMMUTABLE_NODES */
#define UA_MULTITHREADING 100

/* Advanced Options */
#define UA_ENABLE_STATUSCODE_DESCRIPTIONS
#define UA_ENABLE_TYPEDESCRIPTION
/* #undef UA_ENABLE_INLINABLE_EXPORT */
#define UA_ENABLE_NODESET_COMPILER_DESCRIPTIONS
/* #undef UA_ENABLE_DETERMINISTIC_RNG */
#define UA_ENABLE_DISCOVERY
/* #undef UA_ENABLE_DISCOVERY_MULTICAST */
/* #undef UA_ENABLE_QUERY */
/* #undef UA_ENABLE_MALLOC_SINGLETON */
#define UA_ENABLE_DISCOVERY_SEMAPHORE
#define UA_GENERATED_NAMESPACE_ZERO
/* #undef UA_GENERATED_NAMESPACE_ZERO_FULL */
/* #undef UA_ENABLE_PUBSUB_MONITORING */
/* #undef UA_ENABLE_PUBSUB_BUFMALLOC */
/* #undef UA_ENABLE_PUBSUB_SKS */

/* Options for Debugging */
#define UA_DEBUG
/* #undef UA_DEBUG_DUMP_PKGS */
/* #undef UA_DEBUG_FILE_LINE_INFO */

/* Options for Tests */
/* #undef UA_ENABLE_ALLOW_REUSEADDR */

/**
 * Function Export
 * ---------------
 * On Win32: Define ``UA_DYNAMIC_LINKING`` and ``UA_DYNAMIC_LINKING_EXPORT`` in
 * order to export symbols for a DLL. Define ``UA_DYNAMIC_LINKING`` only to
 * import symbols from a DLL.*/
#define UA_DYNAMIC_LINKING

/* Shortcuts for extern "C" declarations */
#if !defined(_UA_BEGIN_DECLS)
# ifdef __cplusplus
#  define _UA_BEGIN_DECLS extern "C" {
# else
#  define _UA_BEGIN_DECLS
# endif
#endif
#if !defined(_UA_END_DECLS)
# ifdef __cplusplus
#  define _UA_END_DECLS }
# else
#  define _UA_END_DECLS
# endif
#endif

/**
 * POSIX Feature Flags
 * -------------------
 * These feature flags have to be set before including the first POSIX
 * header.
 *
 * Special note for FreeBSD: Defining _XOPEN_SOURCE will hide lots of socket
 * definitions as they are not defined by POSIX. Defining _BSD_SOURCE will
 * not help as the symbols undergo a consistency check and get adjusted in
 * the headers. */
#ifdef UA_ARCHITECTURE_POSIX
# if !defined(_XOPEN_SOURCE) && !defined(__FreeBSD__)
#  define _XOPEN_SOURCE 600
# endif
# ifndef _DEFAULT_SOURCE
#  define _DEFAULT_SOURCE
# endif

/* On older systems we need to define _BSD_SOURCE.
 * _DEFAULT_SOURCE is an alias for that. */
# ifndef _BSD_SOURCE
#  define _BSD_SOURCE
# endif

/* Define _GNU_SOURCE to get functions like poll. Comment this out to
 * only use standard POSIX definitions. */
# ifndef _GNU_SOURCE
#  define _GNU_SOURCE
# endif

#define UA_HAS_GETIFADDR 1
#endif

/**
 * C99 Definitions
 * --------------- */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <float.h>
#include <string.h>

/**
 * Inline Functions
 * ---------------- */
#ifdef _MSC_VER
# define UA_INLINE __inline
#else
# define UA_INLINE inline
#endif

/* An inlinable method is typically defined as "static inline". Some
 * applications, such as language bindings with a C FFI (foreign function
 * interface), can however not work with static inline methods. These can set
 * the global UA_ENABLE_INLINABLE_EXPORT macro which causes all inlinable
 * methods to be exported as a regular public API method.
 *
 * Note that UA_ENABLE_INLINABLE_EXPORT has a negative impact for both size and
 * performance of the library. */
#if defined(UA_ENABLE_INLINABLE_EXPORT) && defined(UA_INLINABLE_IMPL)
# define UA_INLINABLE(decl, impl) UA_EXPORT decl; decl impl
#elif defined(UA_ENABLE_INLINABLE_EXPORT)
# define UA_INLINABLE(decl, impl) UA_EXPORT decl;
#else
# define UA_INLINABLE(decl, impl) static UA_INLINE decl impl
#endif

/**
 * Thread-local variables
 * ---------------------- */
#if UA_MULTITHREADING >= 100
# if defined(__GNUC__) /* Also covers clang */
#  define UA_THREAD_LOCAL __thread
# elif defined(_MSC_VER)
#  define UA_THREAD_LOCAL __declspec(thread)
# endif
#endif
#ifndef UA_THREAD_LOCAL
# define UA_THREAD_LOCAL
#endif

/**
 * Atomic Operations
 * -----------------
 *
 * Atomic operations synchronize across processor cores and enable lockless
 * multi-threading. */

/* Intrinsic atomic operations are not available everywhere for MSVC.
 * Use the win32 API. Prevent duplicate definitions by via winsock2. */
#if UA_MULTITHREADING >= 100 && defined(_WIN32)
# ifndef _WINSOCKAPI_
#  define _NO_WINSOCKAPI_
# endif
# define _WINSOCKAPI_
# include <windows.h>
# ifdef _NO_WINSOCKAPI_
#  undef _WINSOCKAPI_
# endif
#endif

static UA_INLINE void *
UA_atomic_xchg(void * volatile * addr, void *newptr) {
#if UA_MULTITHREADING >= 100 && defined(_WIN32) /* Visual Studio */
    return InterlockedExchangePointer(addr, newptr);
#elif UA_MULTITHREADING >= 100 && defined(__GNUC__) /* GCC/Clang */
    return __sync_lock_test_and_set(addr, newptr);
#else
# if UA_MULTITHREADING >= 100
#  warning Atomic operations not implemented
# endif
    void *old = *addr;
    *addr = newptr;
    return old;
#endif
}

static UA_INLINE void *
UA_atomic_cmpxchg(void * volatile * addr, void *expected, void *newptr) {
#if UA_MULTITHREADING >= 100 && defined(_WIN32) /* Visual Studio */
    return InterlockedCompareExchangePointer(addr, newptr, expected);
#elif UA_MULTITHREADING >= 100 && defined(__GNUC__) /* GCC/Clang */
    return __sync_val_compare_and_swap(addr, expected, newptr);
#else
    void *old = *addr;
    if(old == expected)
        *addr = newptr;
    return old;
#endif
}

/**
 * Memory Management
 * -----------------
 *
 * The flag ``UA_ENABLE_MALLOC_SINGLETON`` enables singleton (global) variables
 * with method pointers for memory management (malloc et al.). The method
 * pointers can be switched out at runtime. Use-cases for this are testing of
 * constrained memory conditions and arena-based custom memory management.
 *
 * If the flag is undefined, then ``UA_malloc`` etc. are set to the default
 * malloc, as defined in ``/arch/<architecture>/ua_architecture.h``.
 */

#ifdef UA_ENABLE_MALLOC_SINGLETON
extern UA_THREAD_LOCAL void * (*UA_mallocSingleton)(size_t size);
extern UA_THREAD_LOCAL void (*UA_freeSingleton)(void *ptr);
extern UA_THREAD_LOCAL void * (*UA_callocSingleton)(size_t nelem, size_t elsize);
extern UA_THREAD_LOCAL void * (*UA_reallocSingleton)(void *ptr, size_t size);
# define UA_malloc(size) UA_mallocSingleton(size)
# define UA_free(ptr) UA_freeSingleton(ptr)
# define UA_calloc(num, size) UA_callocSingleton(num, size)
# define UA_realloc(ptr, size) UA_reallocSingleton(ptr, size)
#else
# include <stdlib.h>
# define UA_free free
# define UA_malloc malloc
# define UA_calloc calloc
# define UA_realloc realloc
#endif

/* Stack-allocation of memory. Use C99 variable-length arrays if possible.
 * Otherwise revert to alloca. Note that alloca is not supported on some
 * plattforms. */
#ifndef UA_STACKARRAY
# if defined(__GNUC__) || defined(__clang__)
#  define UA_STACKARRAY(TYPE, NAME, SIZE) TYPE NAME[SIZE]
# else
# if defined(__GNUC__) || defined(__clang__)
#  define UA_alloca(size) __builtin_alloca (size)
# elif defined(_WIN32)
#  define UA_alloca(SIZE) _alloca(SIZE)
# else
#  include <alloca.h>
#  define UA_alloca(SIZE) alloca(SIZE)
# endif
#  define UA_STACKARRAY(TYPE, NAME, SIZE) \
    /* cppcheck-suppress allocaCalled */  \
    TYPE *(NAME) = (TYPE*)UA_alloca(sizeof(TYPE) * (SIZE))
# endif
#endif

/**
 * Assertions
 * ----------
 * The assert macro is disabled by defining NDEBUG. It is often forgotten to
 * include -DNDEBUG in the compiler flags when using the single-file release. So
 * we make assertions dependent on the UA_DEBUG definition handled by CMake. */
#ifdef UA_DEBUG
# include <assert.h>
# define UA_assert(ignore) assert(ignore)
#else
# define UA_assert(ignore) do {} while(0)
#endif

/* Outputs an error message at compile time if the assert fails.
 * Example usage:
 * UA_STATIC_ASSERT(sizeof(long)==7, use_another_compiler_luke)
 * See: https://stackoverflow.com/a/4815532/869402 */
#if defined(__cplusplus) && __cplusplus >= 201103L /* C++11 or above */
# define UA_STATIC_ASSERT(cond,msg) static_assert(cond, #msg)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L /* C11 or above */
# define UA_STATIC_ASSERT(cond,msg) _Static_assert(cond, #msg)
#elif defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER) /* GCC, Clang, MSC */
# define UA_CTASTR2(pre,post) pre ## post
# define UA_CTASTR(pre,post) UA_CTASTR2(pre,post)
# ifndef __COUNTER__ /* PPC GCC fix */
#  define __COUNTER__ __LINE__
# endif
# define UA_STATIC_ASSERT(cond,msg)                             \
    typedef struct {                                            \
        unsigned int UA_CTASTR(static_assertion_failed_,msg) : !!(cond); \
    } UA_CTASTR(static_assertion_failed_,__COUNTER__)
#else /* Everybody else */
# define UA_STATIC_ASSERT(cond,msg) typedef char static_assertion_##msg[(cond)?1:-1]
#endif

/**
 * Locking for Multithreading
 * --------------------------
 * If locking is enabled, the locks must be reentrant. That is, the same thread
 * must be able to take the same lock several times. This is required because we
 * sometimes call a user-defined callback when the server-lock is still held.
 * The user-defined code then should be able to call (public) methods which
 * again take the server-lock. */

#if UA_MULTITHREADING < 100

# define UA_LOCK_INIT(lock)
# define UA_LOCK_DESTROY(lock)
# define UA_LOCK(lock)
# define UA_UNLOCK(lock)
# define UA_LOCK_ASSERT(lock, num)

#elif defined(UA_ARCHITECTURE_WIN32)

typedef struct {
    /* Critical sections on win32 are always recursive */
    CRITICAL_SECTION mutex;
    int mutexCounter;
} UA_Lock;

static UA_INLINE void
UA_LOCK_INIT(UA_Lock *lock) {
    InitializeCriticalSection(&lock->mutex);
    lock->mutexCounter = 0;
}

static UA_INLINE void
UA_LOCK_DESTROY(UA_Lock *lock) {
    DeleteCriticalSection(&lock->mutex);
}

static UA_INLINE void
UA_LOCK(UA_Lock *lock) {
    EnterCriticalSection(&lock->mutex);
    ++lock->mutexCounter;
}

static UA_INLINE void
UA_UNLOCK(UA_Lock *lock) {
    --lock->mutexCounter;
    LeaveCriticalSection(&lock->mutex);
}

static UA_INLINE void
UA_LOCK_ASSERT(UA_Lock *lock, int num) {
    UA_assert(num <= 0 || lock->mutexCounter > 0);
}

#elif defined(UA_ARCHITECTURE_POSIX)

#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex;
    int mutexCounter;
} UA_Lock;

static UA_INLINE void
UA_LOCK_INIT(UA_Lock *lock) {
    pthread_mutexattr_t mattr;
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&lock->mutex, &mattr);
    lock->mutexCounter = 0;
}

static UA_INLINE void
UA_LOCK_DESTROY(UA_Lock *lock) {
    pthread_mutex_destroy(&lock->mutex);
}

static UA_INLINE void
UA_LOCK(UA_Lock *lock) {
    pthread_mutex_lock(&lock->mutex);
    lock->mutexCounter++;
}

static UA_INLINE void
UA_UNLOCK(UA_Lock *lock) {
    lock->mutexCounter--;
    pthread_mutex_unlock(&lock->mutex);
}

static UA_INLINE void
UA_LOCK_ASSERT(UA_Lock *lock, int num) {
    UA_assert(num <= 0 || lock->mutexCounter > 0);
}

#endif

/**
 * Dynamic Linking
 * ---------------
 * Explicit attribute for functions to be exported in a shared library. */
#if defined(_WIN32) && defined(UA_DYNAMIC_LINKING)
# ifdef UA_DYNAMIC_LINKING_EXPORT /* export dll */
#  ifdef __GNUC__
#   define UA_EXPORT __attribute__ ((dllexport))
#  else
#   define UA_EXPORT __declspec(dllexport)
#  endif
# else /* import dll */
#  ifdef __GNUC__
#   define UA_EXPORT __attribute__ ((dllimport))
#  else
#   define UA_EXPORT __declspec(dllimport)
#  endif
# endif
#else /* non win32 */
# if __GNUC__ || __clang__
#  define UA_EXPORT __attribute__ ((visibility ("default")))
# endif
#endif
#ifndef UA_EXPORT
# define UA_EXPORT /* fallback to default */
#endif

/**
 * Threadsafe functions
 * --------------------
 * Functions that can be called from independent threads are marked with
 * the UA_THREADSAFE macro. This is currently only an information for the
 * developer. It can be used in the future for instrumentation and static
 * code analysis. */
#define UA_THREADSAFE

/**
 * Non-aliasing pointers
 * -------------------- */
#ifdef _MSC_VER
# define UA_RESTRICT __restrict
#elif defined(__GNUC__)
# define UA_RESTRICT __restrict__
#elif defined(__CODEGEARC__)
# define UA_RESTRICT _RESTRICT
#else
# define UA_RESTRICT restrict
#endif

/**
 * Likely/Unlikely Conditions
 * --------------------------
 * Condition is likely/unlikely, to help branch prediction. */
#if defined(__GNUC__) || defined(__clang__)
# define UA_LIKELY(x) __builtin_expect((x), 1)
# define UA_UNLIKELY(x) __builtin_expect((x), 0)
#else
# define UA_LIKELY(x) x
# define UA_UNLIKELY(x) x
#endif

/**
 * Function attributes
 * ------------------- */
#if defined(__GNUC__) || defined(__clang__)
# define UA_FUNC_ATTR_MALLOC __attribute__((malloc))
# define UA_FUNC_ATTR_PURE __attribute__ ((pure))
# define UA_FUNC_ATTR_CONST __attribute__((const))
# define UA_FUNC_ATTR_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
# define UA_FORMAT(X,Y) __attribute__ ((format (printf, X, Y)))
#elif defined(_MSC_VER) && _MSC_VER >= 1800
# include <sal.h>
# define UA_FUNC_ATTR_MALLOC
# define UA_FUNC_ATTR_PURE
# define UA_FUNC_ATTR_CONST
# define UA_FUNC_ATTR_WARN_UNUSED_RESULT _Check_return_
# define UA_FORMAT(X,Y)
#else
# define UA_FUNC_ATTR_MALLOC
# define UA_FUNC_ATTR_PURE
# define UA_FUNC_ATTR_CONST
# define UA_FUNC_ATTR_WARN_UNUSED_RESULT
# define UA_FORMAT(X,Y)
#endif

#if defined(__GNUC__) || defined(__clang__)
# define UA_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
# define UA_DEPRECATED __declspec(deprecated)
#else
# define UA_DEPRECATED
#endif

/**
 * Internal Attributes
 * -------------------
 * These attributes are only defined if the macro UA_INTERNAL is defined. That
 * way public methods can be annotated (e.g. to warn for unused results) but
 * warnings are only triggered for internal code. */

#if defined(UA_INTERNAL) && (defined(__GNUC__) || defined(__clang__))
# define UA_INTERNAL_DEPRECATED \
    _Pragma ("GCC warning \"Macro is deprecated for internal use\"")
#else
# define UA_INTERNAL_DEPRECATED
#endif

#if defined(UA_INTERNAL) && (defined(__GNUC__) || defined(__clang__))
# define UA_INTERNAL_FUNC_ATTR_WARN_UNUSED_RESULT \
    __attribute__((warn_unused_result))
#else
# define UA_INTERNAL_FUNC_ATTR_WARN_UNUSED_RESULT
#endif

/**
 * Detect Endianness and IEEE 754 floating point
 * ---------------------------------------------
 * Integers and floating point numbers are transmitted in little-endian (IEEE
 * 754 for floating point) encoding. If the target architecture uses the same
 * format, numeral datatypes can be memcpy'd (overlayed) on the network buffer.
 * Otherwise, a slow default encoding routine is used that works for every
 * architecture.
 *
 * Integer Endianness
 * ^^^^^^^^^^^^^^^^^^
 * The definition ``UA_LITTLE_ENDIAN`` is true when the integer representation
 * of the target architecture is little-endian. */
#if defined(_WIN32)
# define UA_LITTLE_ENDIAN 1
#elif defined(__i386__) || defined(__x86_64__) || defined(__amd64__)
# define UA_LITTLE_ENDIAN 1
#elif (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && \
      (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__))
# define UA_LITTLE_ENDIAN 1
#elif defined(__linux__) /* Linux (including Android) */
# include <endian.h>
# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define UA_LITTLE_ENDIAN 1
# endif
#elif defined(__OpenBSD__) /* OpenBSD */
# include <sys/endian.h>
# if BYTE_ORDER == LITTLE_ENDIAN
#  define UA_LITTLE_ENDIAN 1
# endif
#elif defined(__NetBSD__) || defined(__FreeBSD__) || defined(__DragonFly__) /* Other BSD */
# include <sys/endian.h>
# if _BYTE_ORDER == _LITTLE_ENDIAN
#  define UA_LITTLE_ENDIAN 1
# endif
#elif defined(__APPLE__) /* Apple (MacOS, iOS) */
# include <libkern/OSByteOrder.h>
# if defined(__LITTLE_ENDIAN__)
#  define UA_LITTLE_ENDIAN 1
# endif
#elif defined(__QNX__) || defined(__QNXNTO__) /* QNX */
# include <gulliver.h>
# if defined(__LITTLEENDIAN__)
#  define UA_LITTLE_ENDIAN 1
# endif
#elif defined(_OS9000) /* OS-9 */
# if defined(_LIL_END)
#  define UA_LITTLE_ENDIAN 1
# endif
#endif
#ifndef UA_LITTLE_ENDIAN
# define UA_LITTLE_ENDIAN 0
#endif

/* Can the integers be memcpy'd onto the network buffer? Add additional checks
 * here. Some platforms (e.g. QNX) have sizeof(bool) > 1. Manually disable
 * overlayed integer encoding if that is the case. */
#if (UA_LITTLE_ENDIAN == 1)
UA_STATIC_ASSERT(sizeof(bool) == 1, cannot_overlay_integers_with_large_bool);
# define UA_BINARY_OVERLAYABLE_INTEGER 1
#else
# define UA_BINARY_OVERLAYABLE_INTEGER 0
#endif

/**
 * Float Endianness
 * ^^^^^^^^^^^^^^^^
 * The definition ``UA_FLOAT_IEEE754`` is set to true when the floating point
 * number representation of the target architecture is IEEE 754. The definition
 * ``UA_FLOAT_LITTLE_ENDIAN`` is set to true when the floating point number
 * representation is in little-endian encoding. */

#ifndef UA_FLOAT_IEEE754
#if defined(_WIN32)
# define UA_FLOAT_IEEE754 1
#elif defined(__i386__) || defined(__x86_64__) || defined(__amd64__) || \
    defined(__ia64__) || defined(__powerpc__) || defined(__sparc__) || \
    defined(__arm__)
# define UA_FLOAT_IEEE754 1
#elif defined(__STDC_IEC_559__)
# define UA_FLOAT_IEEE754 1
#elif defined(ESP_PLATFORM)
# define UA_FLOAT_IEEE754 1
#else
# define UA_FLOAT_IEEE754 0
#endif
#endif

/* Wikipedia says (https://en.wikipedia.org/wiki/Endianness): Although the
 * ubiquitous x86 processors of today use little-endian storage for all types of
 * data (integer, floating point, BCD), there are a number of hardware
 * architectures where floating-point numbers are represented in big-endian form
 * while integers are represented in little-endian form. */
#if defined(_WIN32)
# define UA_FLOAT_LITTLE_ENDIAN 1
#elif defined(__i386__) || defined(__x86_64__) || defined(__amd64__)
# define UA_FLOAT_LITTLE_ENDIAN 1
#elif defined(__FLOAT_WORD_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && \
    (__FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__) /* Defined only in GCC */
# define UA_FLOAT_LITTLE_ENDIAN 1
#elif defined(__FLOAT_WORD_ORDER) && defined(__LITTLE_ENDIAN) && \
    (__FLOAT_WORD_ORDER == __LITTLE_ENDIAN) /* Defined only in GCC */
# define UA_FLOAT_LITTLE_ENDIAN 1
#endif
#ifndef UA_FLOAT_LITTLE_ENDIAN
# define UA_FLOAT_LITTLE_ENDIAN 0
#endif

/* Only if the floating points are litle-endian **and** in IEEE 754 format can
 * we memcpy directly onto the network buffer. */
#if (UA_FLOAT_IEEE754 == 1) && (UA_FLOAT_LITTLE_ENDIAN == 1)
# define UA_BINARY_OVERLAYABLE_FLOAT 1
#else
# define UA_BINARY_OVERLAYABLE_FLOAT 0
#endif

#endif /* UA_CONFIG_H_ */
