#pragma once

#include "dmk.h"
#include "dmk_string.h"

#include <string>
#include <memory>
#include <tuple>
#include <iostream>

namespace dmk
{
    // boolean type with two state: true or error message (memory efficient - one pointer)
    struct bool_result
    {
    public:
        // default state - ok
        bool_result( ) = default;

        // copy
        bool_result( const bool_result& other )
            : m_error( other ? nullptr : new std::string( other.error( ) ) )
        {
        }

        // allow moving
        bool_result( bool_result&& ) = default;

        // prevent assigning
        bool_result& operator=( const bool_result& ) = delete;
        bool_result& operator=( bool_result&& ) = delete;

        // constructor(bool)
        bool_result( bool ok ) DMK_NOEXCEPT : m_error( ok ? nullptr : new std::string( ) )
        {
        }

        // constructor(string)
        bool_result( const std::string& error ) : m_error( new std::string( error ) )
        {
        }
        // constructor(string): move semantic
        bool_result( std::string&& error ) DMK_NOEXCEPT_OP( DMK_NOEXCEPT_OP( std::move( error ) ) )
            : m_error( new std::string( std::move( error ) ) )
        {
        }

        // prevent automatic conversion int->bool, pointer->bool
        bool_result( int value ) = delete;
        bool_result( nullptr_t ) = delete;

        // bool conversion: bool_result result; ...  if(result) {...}
        explicit operator bool( ) const
        {
            return !( bool )m_error;
        }

        // get error message (no copy, returns const reference)
        const std::string& error( ) const
        {
            static std::string empty;
            return m_error ? *m_error : empty;
        }

    private:
        const std::unique_ptr<std::string> m_error;
    };

    // boolean type with optional message
    struct result
    {
    public:
        typedef std::tuple<const bool, const std::string> tuple;

    public:
        result( ) : m_ok( true )
        {
        }
        result( bool ok ) : m_ok( ok )
        {
        }
        result( bool ok, const u8string& message ) : m_ok( ok ), m_message( message )
        {
        }
        result( bool ok, u8string&& message ) : m_ok( ok ), m_message( std::move( message ) )
        {
        }
        result( const u8string& message ) : m_ok( false ), m_message( message )
        {
        }
        result( u8string&& message ) : m_ok( false ), m_message( std::move( message ) )
        {
        }
        result( const char* message ) : m_ok( false ), m_message( u8string( message ) )
        {
        }
        template <typename... Args>
        result( bool ok, const std::string& message, const Args&... args )
            : m_ok( ok ), m_message( fmt::format( message, args... ) )
        {
        }
        template <typename... Args>
        result( bool ok, std::string&& message, const Args&... args )
            : m_ok( ok ), m_message( fmt::format( std::move( message ), args... ) )
        {
        }

        result( result&& ) = default;
        result( const result& ) = default;

        result& operator=( const result& ) = default;
        result& operator=( result&& ) = default;

        // bool conversion: bool_result result; ...  if(result) {...}
        explicit operator bool( ) const
        {
            return m_ok;
        }

        // get message (no copy, returns const reference)
        const std::string& message( ) const
        {
            return m_message;
        }

        // get message (no copy, returns reference)
        std::string& message( )
        {
            return m_message.str( );
        }

        result& operator<<( const result& right )
        {
            m_ok = m_ok && right.m_ok;
            if ( !m_message.empty( ) )
            {
                m_message += "; ";
            }
            m_message += right.m_message;
            return *this;
        }

        static result exit_code( int code )
        {
            if ( code == 0 )
                return true;
            return result( false, fmt::format( "ExitCode = {}", code ) );
        }

    private:
        u8string m_message;
        bool m_ok;
    };

} // namespace dmk
