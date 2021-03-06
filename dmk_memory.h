#pragma once

#include "dmk.h"
#include "dmk_assert.h"
#include <memory>
#include <type_traits>
#if defined( DMK_OS_WIN )
#include <windows.h>
#endif

namespace dmk
{
    template <size_t alignment>
    void* aligned_malloc( size_t size )
    {
        if ( alignment <= sizeof( size_t ) )
            return malloc( size );
        else
            return _mm_malloc( size, alignment );
    }

    template <size_t alignment>
    void aligned_free( void* memory )
    {
        if ( alignment <= sizeof( size_t ) )
            free( memory );
        else
            _mm_free( memory );
    }

    struct allocator_base
    {
    public:
        typedef size_t size_type;
        typedef void* pointer;
        typedef const void* const_pointer;
        typedef void* zeroed_pointer;

    public:
        template <size_t alignment>
        static size_t align_down( size_t size )
        {
            return ( size ) & ~( alignment - 1 );
        }
        template <size_t alignment>
        static size_t align_up( size_t size )
        {
            return ( size + alignment - 1 ) & ~( alignment - 1 );
        }
        static void zeroize( pointer begin, pointer end )
        {
            std::memset( begin, 0, ( size_t )end - ( size_t )begin );
        }
        static void move( const_pointer begin, const_pointer end, pointer destination )
        {
            std::memmove( destination, begin, ( size_t )end - ( size_t )begin );
        }
        template <size_t itemsize>
        static void fill( pointer begin, pointer end, const_pointer source )
        {
            for ( ; begin != end; begin++ )
            {
                std::memmove( begin, source, itemsize );
            }
        }

    public:
    };

    struct malloc_allocator : public allocator_base
    {
    public:
        static pointer allocate( size_type& size )
        {
            size = align_up<sizeof( size_t )>( size );
            return std::malloc( size );
        }
        static zeroed_pointer allocate_zeroed( size_type& size )
        {
            zeroed_pointer memory = allocate( size );
            zeroize( memory, ( uint8_t* )memory + size );
            return memory;
        }
        static void deallocate( pointer memory )
        {
            std::free( memory );
        }
    };

    enum
    {
        PageSize  = 4096,
        CacheSize = 64,
        SSESize   = 16,
        AVXSize   = 32
    };

    enum
    {
        PageAllocationGranularity = 65536
    };

    DMK_ALIGNED_ALLOCATOR( PageSize ) void* paged_malloc( size_t size )
    {
        return ::VirtualAlloc( NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
    }

    void paged_free( void* memory )
    {
        ::VirtualFree( ( PVOID )memory, 0, MEM_RELEASE );
    }

    template <size_t alignment>
    struct aligned_allocator : public allocator_base
    {
    public:
        static pointer allocate( size_type& size )
        {
            size = align_up<sizeof( size_t )>( size );
            return aligned_malloc<alignment>( size );
        }
        static zeroed_pointer allocate_zeroed( size_type& size )
        {
            zeroed_pointer memory = allocate( size );
            zeroize( memory, ( uint8_t* )memory + size );
            return memory;
        }
        static void deallocate( pointer memory )
        {
            aligned_free<alignment>( memory );
        }
    };

    template <>
    struct aligned_allocator<PageSize> : public allocator_base
    {
    public:
        static pointer allocate( size_type& size )
        {
            size = align_up<PageAllocationGranularity>( size );
            return paged_malloc( size );
        }
        static zeroed_pointer allocate_zeroed( size_type& size )
        {
            return allocate( size );
        }
        static void deallocate( pointer memory )
        {
            paged_free( memory );
        }
    };
} // namespace dmk
