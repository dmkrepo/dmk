#pragma once

#include "dmk.h"
#include "dmk_assert.h"
#include "dmk_memory.h"
#include <string>
#include <vector>
#include <iostream>

#include <cppformat/format.h>
#include <cppformat/format.cc>

#if defined( DMK_COMPILER_MSVC )
#pragma execution_character_set( "utf-8" )
#endif

namespace dmk
{
#define REPL_CHAR ( 0xFFFD ) //

    inline std::string asci_lowercase( const std::string& str )
    {
        std::string temp = str;
        std::transform( str.begin( ), str.end( ), temp.begin( ), ::tolower );
        return temp;
    }

    inline std::string asci_uppercase( const std::string& str )
    {
        std::string temp = str;
        std::transform( str.begin( ), str.end( ), temp.begin( ), ::toupper );
        return temp;
    }

    inline bool matches( const std::string& pattern, const std::string& text )
    {
        if ( pattern == "*" )
            return true;
        if ( pattern == text )
            return true;
        if ( pattern.empty( ) )
            return false;
        if ( pattern[0] == '!' )
        {
            return matches( pattern.substr( 1 ), text );
        }
        size_t len = pattern.size( ) - 1;
        if ( text.front( ) == '*' )
        {
            return pattern.substr( 1 ) == text.substr( text.size( ) - len, len );
        }
        else if ( text.back( ) == '*' )
        {
            return pattern.substr( 0, len ) == text.substr( 0, len );
        }
        else
        {
            return false;
        }
    }

    template <typename _T>
    inline std::string stringify( const _T& value )
    {
        std::stringstream mem;
        mem << value;
        return mem.str( );
    }

    namespace utf8
    {
        const char* decode( const char* begin, const char* end, char32_t& output );
        int charlen_unsafe( const char* begin );
        const char* next( const char* begin, const char* end );
        size_t length( const char* begin, const char* end );
        size_t length_unsafe( const char* begin );
        const char* advance( const char* begin, const char* end, size_t pos );
        char* encode( char* output, char32_t input );
        // std::string  uchar( char32_t ch );
    }

    std::wstring u8_w( const std::string& s );
    std::u16string u8_u16( const std::string& s );
    std::u32string u8_u32( const std::string& s );
    std::string c32_u8( size_t count, char32_t ch );
    std::string w_u8( const std::wstring& s );
    std::string u16_u8( const std::u16string& s );
    std::string u32_u8( const std::u32string& s );

    struct u8string
    {
    public:
        typedef char value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        struct const_iterator : public std::iterator<std::forward_iterator_tag, char32_t>
        {
        public:
            const_iterator( const_pointer current, const_pointer end ) noexcept : m_current( current ),
                                                                                  m_end( end )
            {
            }
            const_iterator( const_pointer current, size_type size ) noexcept : m_current( current ),
                                                                               m_end( current + size )
            {
            }
            const_iterator( ) noexcept : m_current( nullptr ), m_end( nullptr )
            {
            }
            inline bool valid( ) const
            {
                return m_current < m_end;
            }
            char32_t operator*( ) const
            {
                DMK_ASSERT_LT( m_current, m_end ); // <
                if ( m_current < m_end )
                {
                    uint8_t c = uint8_t( *m_current );
                    if ( c < 0x80 )
                    {
                        return c;
                    }
                    char32_t out;
                    utf8::decode( m_current, m_end, out );
                    return out;
                }
                return REPL_CHAR;
            }
            const_iterator& operator++( ) // ++it
            {
                DMK_ASSERT_LT( m_current, m_end );
                m_current = utf8::next( m_current, m_end );
                DMK_ASSERT_LE( m_current, m_end );
                return *this;
            }
            const_iterator operator++( int ) // it++
            {
                const_iterator tmp( *this );
                ++( *this );
                return tmp;
            }
            const_iterator& operator+=( size_t pos )
            {
                DMK_ASSERT_LT( m_current, m_end );
                m_current = utf8::advance( m_current, m_end, pos );
                DMK_ASSERT_LE( m_current, m_end );
                return *this;
            }
            const_iterator operator+( size_t pos ) const
            {
                return const_iterator( utf8::advance( m_current, m_end, pos ), m_end );
            }
            bool operator==( const const_iterator& it ) const
            {
                DMK_ASSERT_EQ( m_end, it.m_end );
                return it.m_current == m_current;
            }
            bool operator!=( const const_iterator& it ) const
            {
                return !operator==( it );
            }
            const_pointer ptr( ) const
            {
                return m_current;
            }

        private:
            friend struct u8string;
            const_pointer m_current;
            const_pointer m_end;
        };

    public:
        u8string( ) noexcept
        {
        }
        u8string( const char* str ) : m_str( str )
        {
        }
        u8string( const wchar_t* str ) : m_str( w_u8( str ) )
        {
        }
        u8string( const char16_t* str ) : m_str( u16_u8( str ) )
        {
        }
        u8string( const char32_t* str ) : m_str( u32_u8( str ) )
        {
        }
        u8string( const char* first, const char* last ) : m_str( first, last )
        {
        }
        u8string( const wchar_t* first, const wchar_t* last ) : m_str( w_u8( std::wstring( first, last ) ) )
        {
        }
        u8string( const char16_t* first, const char16_t* last )
            : m_str( u16_u8( std::u16string( first, last ) ) )
        {
        }
        u8string( const char32_t* first, const char32_t* last )
            : m_str( u32_u8( std::u32string( first, last ) ) )
        {
        }
        u8string( const std::string& str ) : m_str( str )
        {
        }
        u8string( const u8string& str ) : m_str( str.m_str )
        {
        }
        u8string( u8string&& str ) noexcept : m_str( std::move( str.m_str ) )
        {
        }
        u8string( std::string&& str ) noexcept : m_str( std::move( str ) )
        {
        }
        u8string( const std::wstring& str ) : m_str( w_u8( str ) )
        {
        }
        u8string( const std::u16string& str ) : m_str( u16_u8( str ) )
        {
        }
        u8string( const std::u32string& str ) : m_str( u32_u8( str ) )
        {
        }
        u8string( size_type count, char32_t ch ) : m_str( c32_u8( count, ch ) )
        {
        }
        u8string& operator=( const u8string& str )
        {
            m_str = str.m_str;
            return *this;
        }
        u8string& operator=( u8string&& str )
        {
            m_str = std::move( str.m_str );
            return *this;
        }
        u8string& operator=( const std::string& str )
        {
            m_str = str;
            return *this;
        }
        u8string& operator=( std::string&& str )
        {
            m_str = std::move( str );
            return *this;
        }
        u8string& operator=( const char* str )
        {
            m_str = std::string( str );
            return *this;
        }
        std::string& str( )
        {
            return m_str;
        }
        const std::string& str( ) const
        {
            return m_str;
        }
        std::string localestr( ) const
        {
            return m_str;
        }
        operator const std::string&( ) const
        {
            return m_str;
        }
        std::wstring wstr( ) const
        {
            return u8_w( m_str );
        }
        std::u16string u16str( ) const
        {
            return u8_u16( m_str );
        }
        std::u32string u32str( ) const
        {
            return u8_u32( m_str );
        }
        const char* c_str( ) const
        {
            return m_str.c_str( );
        }
        char* data( )
        {
            return const_cast<char*>( m_str.data( ) );
        }
        size_type size( ) const
        {
            return m_str.size( );
        }
        size_type length( ) const
        {
            const_pointer b = _begin( );
            size_type s = size( );
            return utf8::length( b, b + s );
        }
        const char* data( ) const
        {
            return m_str.data( );
        }
        const_iterator cbegin( ) const
        {
            const_pointer b = _begin( );
            size_type s = size( );
            return const_iterator( b, b + s );
        }
        const_iterator cend( ) const
        {
            const_pointer b = _begin( );
            size_type s = size( );
            return const_iterator( b + s, b + s );
        }
        const_iterator begin( ) const
        {
            return cbegin( );
        }
        const_iterator end( ) const
        {
            return cend( );
        }
        u8string substr( const const_iterator& first, const const_iterator& last ) const
        {
            const_pointer f = first.ptr( );
            const_pointer l = last.ptr( );
            if ( f == l )
            {
                return std::string( );
            }
            return std::string( f, l );
        }
        u8string substr( const const_iterator& first, size_type count ) const
        {
            return substr( first, first + count );
        }
        u8string substr( const const_iterator& first ) const
        {
            return substr( first, end( ) );
        }
        const_iterator find( char ch, size_type offset = 0 ) const
        {
            size_type pos = m_str.find( ch, offset );
            if ( pos == std::string::npos )
            {
                return end( );
            }
            return iterator_from_byte_pos( pos );
        }
        const_iterator find( char32_t ch, size_type offset = 0 ) const
        {
            return find( u8string( 1, ch ), offset );
        }
        const_iterator find( const u8string& str, size_type offset = 0 ) const
        {
            size_type pos = m_str.find( str.str( ), offset );
            if ( pos == std::string::npos )
            {
                return end( );
            }
            return iterator_from_byte_pos( pos );
        }
        bool empty( ) const
        {
            return m_str.empty( );
        }
        void clear( )
        {
            m_str.clear( );
        }
        const_iterator iterator_from_pointer( const_pointer ptr ) const
        {
            DMK_ASSERT_GE( ptr, _begin( ) );
            DMK_ASSERT_LE( ptr, _end( ) );
            return const_iterator( ptr, _end( ) );
        }
        const_iterator iterator_from_byte_pos( size_type pos ) const
        {
            DMK_ASSERT_LE( _begin( ) + pos, _end( ) );
            return const_iterator( _begin( ) + pos, _end( ) );
        }
        char32_t operator[]( size_type index ) const
        {
            return *( begin( ) + index );
        }
        u8string& operator+=( const u8string& other )
        {
            m_str += other.m_str;
            return *this;
        }
        u8string& operator+=( const char* other )
        {
            m_str += other;
            return *this;
        }

    private:
        pointer _begin( )
        {
            return const_cast<char*>( m_str.data( ) );
        }
        pointer _end( )
        {
            return const_cast<char*>( m_str.data( ) ) + m_str.size( );
        }
        const_pointer _begin( ) const
        {
            return m_str.data( );
        }
        const_pointer _end( ) const
        {
            return m_str.data( ) + m_str.size( );
        }
        std::string m_str;
    };

    inline bool operator==( const u8string& lh, const u8string& rh )
    {
        return lh.str( ) == rh.str( );
    }

    inline bool operator!=( const u8string& lh, const u8string& rh )
    {
        return lh.str( ) != rh.str( );
    }

    inline bool operator<=( const u8string& lh, const u8string& rh )
    {
        return lh.str( ) <= rh.str( );
    }

    inline bool operator>=( const u8string& lh, const u8string& rh )
    {
        return lh.str( ) >= rh.str( );
    }

    inline bool operator<( const u8string& lh, const u8string& rh )
    {
        return lh.str( ) < rh.str( );
    }

    inline bool operator>( const u8string& lh, const u8string& rh )
    {
        return lh.str( ) > rh.str( );
    }

    inline u8string operator+( const u8string& lh, const u8string& rh )
    {
        return lh.str( ) + rh.str( );
    }

    inline u8string operator+( const char* lh, const u8string& rh )
    {
        return lh + rh.str( );
    }

    inline u8string operator+( const u8string& lh, const char* rh )
    {
        return lh.str( ) + rh;
    }

    inline u8string& u8( std::string& str )
    {
        return reinterpret_cast<u8string&>( str );
    }

    inline const u8string& u8( const std::string& str )
    {
        return reinterpret_cast<const u8string&>( str );
    }

    inline std::ostream& operator<<( std::ostream& os, const u8string& str )
    {
        os << str.str( );
        return os;
    }

    inline std::string hex( const std::string& str )
    {
        std::string result = "";
        for ( char c : str )
        {
            if ( c < 0x20 || c >= 0x7F )
            {
                result += fmt::format( "\\x{:02X}", uint8_t( c ) );
            }
            else
            {
                result += c;
            }
        }
        return result;
    }

    struct string_iterator
    {
    public:
        string_iterator( const std::string& str )
            : m_ptr8( str.c_str( ) ), m_ptr32( nullptr ), m_length( utf8::length_unsafe( m_ptr8 ) )
        {
        }
        string_iterator( const u8string& str )
            : m_ptr8( str.c_str( ) ), m_ptr32( nullptr ), m_length( utf8::length_unsafe( m_ptr8 ) )
        {
        }
        string_iterator( const char* str )
            : m_ptr8( str ), m_ptr32( nullptr ), m_length( utf8::length_unsafe( str ) )
        {
        }
        string_iterator( const char* str, size_t length )
            : m_ptr8( str ), m_ptr32( nullptr ), m_length( length )
        {
        }
        string_iterator( const std::u32string& str )
            : m_ptr8( nullptr ), m_ptr32( str.c_str( ) ), m_length( str.size( ) )
        {
        }
        string_iterator( const char32_t* str )
            : m_ptr8( nullptr ), m_ptr32( str ), m_length( std::char_traits<char32_t>::length( str ) )
        {
        }
        string_iterator( const string_iterator& str )
            : m_ptr8( str.m_ptr8 ), m_ptr32( str.m_ptr32 ), m_length( str.m_length )
        {
        }
        uint32_t length( ) const
        {
            return m_length;
        }
        inline bool empty( ) const
        {
            return m_ptr8 ? !*m_ptr8 : ( m_ptr32 ? !*m_ptr32 : true );
        }
        inline char32_t character( ) const
        {
            return m_ptr8 ? character8( ) : character32( );
        }
        inline char32_t next( )
        {
            return m_ptr8 ? next8( ) : next32( );
        }
        inline char32_t character8( ) const
        {
            char c = *m_ptr8;
            if ( uint8_t( c ) < 0x80 )
            {
                return c;
            }
            char32_t result;
            utf8::decode( m_ptr8, m_ptr8 + 5, result );
            return result;
        }
        inline char32_t next8( )
        {
            char c = *m_ptr8;
            if ( c == 0 )
            {
                return 0;
            }
            if ( uint8_t( c ) < 0x80 )
            {
                ++m_ptr8;
                return c;
            }
            char32_t result;
            m_ptr8 = utf8::decode( m_ptr8, m_ptr8 + 5, result );
            return result;
        }
        inline char32_t character32( ) const
        {
            if ( !m_ptr32 )
            {
                return 0;
            }
            return *m_ptr32;
        }
        inline char32_t next32( )
        {
            if ( !m_ptr32 )
            {
                return 0;
            }
            char32_t c = *m_ptr32;
            if ( c == 0 )
            {
                return 0;
            }
            m_ptr32++;
            return c;
        }

    private:
        const char* m_ptr8;
        const char32_t* m_ptr32;
        const uint32_t m_length;
    };

    std::vector<std::string> tokenize( const std::string& str )
    {
        size_t param_end = 0;
        std::vector<std::string> args;
        while ( param_end < str.size( ) )
        {
            size_t param_start = str.find_first_not_of( ' ', param_end );
            if ( param_start == std::string::npos )
                break;
            param_end = str.find_first_of( ' ', param_start );
            if ( param_end == std::string::npos )
                param_end = str.size( );
            args.push_back( str.substr( param_start, param_end - param_start ) );
        }
        return args;
    }

    std::string replace_one( const std::string& str, const std::string& from, const std::string& to )
    {
        std::string r    = str;
        size_t start_pos = 0;
        if ( ( start_pos = r.find( from, start_pos ) ) != std::string::npos )
        {
            r.replace( start_pos, from.size( ), to );
        }
        return r;
    }

    std::string replace_all( const std::string& str, const std::string& from, const std::string& to )
    {
        std::string r    = str;
        size_t start_pos = 0;
        while ( ( start_pos = r.find( from, start_pos ) ) != std::string::npos )
        {
            r.replace( start_pos, from.size( ), to );
            start_pos += to.size( );
        }
        return r;
    }

    inline std::string q( const std::string& str )
    {
        return '"' + str + '"';
    }

    inline u8string q( const u8string& str )
    {
        return q( str.str( ) );
    }

    inline std::string qo( const std::string& str )
    {
        if ( !str.empty( ) && str.find_first_of( " \"'<>&|?*$;" ) == std::string::npos )
            return str;
        return '"' + str + '"';
    }

    inline u8string qo( const u8string& str )
    {
        return qo( str.str( ) );
    }

    template <typename _Type>
    inline std::string operator%( const std::string& left, const _Type& value )
    {
        return replace_one( left, "%", stringify( value ) );
    }

} // namespace dmk
