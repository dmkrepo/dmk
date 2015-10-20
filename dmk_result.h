#pragma once

#include "dmk.h"

#include <string>
#include <memory>

namespace dmk
{
    // boolean type with optional error message (memory efficient - one pointer)
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
} // namespace dmk
