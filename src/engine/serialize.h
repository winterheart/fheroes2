/***************************************************************************
 *   Copyright (C) 2012 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef H2SERIALIZE_H
#define H2SERIALIZE_H

#include <cstdio>
#include <list>
#include <map>
#include <string>
#include <vector>

#include "math_base.h"
#include "types.h"

struct Point;
struct Rect;
struct Size;

class StreamBase
{
protected:
    size_t flags;

    virtual u8 get8() = 0;
    virtual void put8( char ) = 0;

    virtual size_t sizeg( void ) const = 0;
    virtual size_t sizep( void ) const = 0;
    virtual size_t tellg( void ) const = 0;
    virtual size_t tellp( void ) const = 0;

    void setconstbuf( bool );
    void setfail( bool );

public:
    StreamBase()
        : flags( 0 )
    {}
    virtual ~StreamBase() = default;

    void setbigendian( bool );

    bool isconstbuf( void ) const;
    bool fail( void ) const;
    bool bigendian( void ) const;

    virtual void skip( size_t ) = 0;

    virtual u16 getBE16() = 0;
    virtual u16 getLE16() = 0;
    virtual u32 getBE32() = 0;
    virtual u32 getLE32() = 0;

    virtual void putBE32( u32 ) = 0;
    virtual void putLE32( u32 ) = 0;
    virtual void putBE16( u16 ) = 0;
    virtual void putLE16( u16 ) = 0;

    virtual std::vector<u8> getRaw( size_t = 0 /* all data */ ) = 0;
    virtual void putRaw( const char *, size_t ) = 0;

    u16 get16();
    u32 get32();

    void put16( u16 );
    void put32( u32 );

    uint8_t get()
    {
        return get8();
    }

    void put( char ch )
    {
        put8( ch );
    }

    StreamBase & operator>>( bool & );
    StreamBase & operator>>( char & );
    StreamBase & operator>>( u8 & );
    StreamBase & operator>>( int8_t & );
    StreamBase & operator>>( u16 & );
    StreamBase & operator>>( int16_t & );
    StreamBase & operator>>( u32 & );
    StreamBase & operator>>( s32 & );
    StreamBase & operator>>( float & );
    StreamBase & operator>>( std::string & );

    StreamBase & operator>>( fheroes2::Point & point_ );

    StreamBase & operator<<( const bool );
    StreamBase & operator<<( const char );
    StreamBase & operator<<( const u8 );
    StreamBase & operator<<( const int8_t );
    StreamBase & operator<<( const u16 );
    StreamBase & operator<<( const int16_t );
    StreamBase & operator<<( const u32 );
    StreamBase & operator<<( const s32 );
    StreamBase & operator<<( const float );
    StreamBase & operator<<( const std::string & );

    StreamBase & operator<<( const fheroes2::Point & point_ );

    template <class Type1, class Type2>
    StreamBase & operator>>( std::pair<Type1, Type2> & p )
    {
        return *this >> p.first >> p.second;
    }

    template <class Type>
    StreamBase & operator>>( std::vector<Type> & v )
    {
        const u32 size = get32();
        v.resize( size );
        for ( typename std::vector<Type>::iterator it = v.begin(); it != v.end(); ++it )
            *this >> *it;
        return *this;
    }

    template <class Type>
    StreamBase & operator>>( std::list<Type> & v )
    {
        const u32 size = get32();
        v.resize( size );
        for ( typename std::list<Type>::iterator it = v.begin(); it != v.end(); ++it )
            *this >> *it;
        return *this;
    }

    template <class Type1, class Type2>
    StreamBase & operator>>( std::map<Type1, Type2> & v )
    {
        const u32 size = get32();
        v.clear();
        for ( u32 ii = 0; ii < size; ++ii ) {
            std::pair<Type1, Type2> pr;
            *this >> pr;
            v.insert( pr );
        }
        return *this;
    }

    template <class Type1, class Type2>
    StreamBase & operator<<( const std::pair<Type1, Type2> & p )
    {
        return *this << p.first << p.second;
    }

    template <class Type>
    StreamBase & operator<<( const std::vector<Type> & v )
    {
        put32( static_cast<u32>( v.size() ) );
        for ( typename std::vector<Type>::const_iterator it = v.begin(); it != v.end(); ++it )
            *this << *it;
        return *this;
    }

    template <class Type>
    StreamBase & operator<<( const std::list<Type> & v )
    {
        put32( static_cast<u32>( v.size() ) );
        for ( typename std::list<Type>::const_iterator it = v.begin(); it != v.end(); ++it )
            *this << *it;
        return *this;
    }

    template <class Type1, class Type2>
    StreamBase & operator<<( const std::map<Type1, Type2> & v )
    {
        put32( static_cast<u32>( v.size() ) );
        for ( typename std::map<Type1, Type2>::const_iterator it = v.begin(); it != v.end(); ++it )
            *this << *it;
        return *this;
    }
};

class ZStreamBuf;

class StreamBuf : public StreamBase
{
public:
    explicit StreamBuf( size_t = 0 );
    StreamBuf( const StreamBuf & );
    explicit StreamBuf( const std::vector<u8> & );
    StreamBuf( const u8 *, size_t );

    ~StreamBuf() override;

    StreamBuf & operator=( const StreamBuf & );

    const u8 * data( void ) const;
    size_t size( void ) const;
    size_t capacity( void ) const;

    void seek( size_t );
    void skip( size_t ) override;

    u16 getBE16() override;
    u16 getLE16() override;
    u32 getBE32() override;
    u32 getLE32() override;

    void putBE32( u32 ) override;
    void putLE32( u32 ) override;
    void putBE16( u16 ) override;
    void putLE16( u16 ) override;

    std::vector<u8> getRaw( size_t = 0 /* all data */ ) override;
    void putRaw( const char *, size_t ) override;

    std::string toString( size_t = 0 /* all data */ );

protected:
    void reset( void );

    size_t tellg( void ) const override;
    size_t tellp( void ) const override;
    size_t sizeg( void ) const override;
    size_t sizep( void ) const override;

    void copy( const StreamBuf & );
    void reallocbuf( size_t );
    void setfail( void );

    u8 get8() override;
    void put8( char ) override;

    friend class ZStreamBuf;

    u8 * itbeg;
    u8 * itget;
    u8 * itput;
    u8 * itend;
};

class StreamFile : public StreamBase
{
    StreamFile( const StreamFile & ) {}

public:
    StreamFile();
    StreamFile( const std::string &, const char * mode );
    ~StreamFile() override;

    size_t size( void ) const;
    size_t tell( void ) const;

    bool open( const std::string &, const std::string & mode );
    void close( void );

    StreamBuf toStreamBuf( size_t = 0 /* all data */ );

    void seek( size_t );
    void skip( size_t ) override;

    uint16_t getBE16() override;
    uint16_t getLE16() override;
    uint32_t getBE32() override;
    uint32_t getLE32() override;

    void putBE16( uint16_t ) override;
    void putLE16( uint16_t ) override;
    void putBE32( uint32_t ) override;
    void putLE32( uint32_t ) override;

    std::vector<uint8_t> getRaw( size_t = 0 /* all data */ ) override;
    void putRaw( const char *, size_t ) override;

    std::string toString( size_t = 0 /* all data */ );

protected:
    StreamFile & operator=( const StreamFile & )
    {
        return *this;
    }

    size_t sizeg( void ) const override;
    size_t sizep( void ) const override;
    size_t tellg( void ) const override;
    size_t tellp( void ) const override;

    u8 get8() override;
    void put8( char ) override;

private:
    std::FILE * _file;

    template <typename T>
    T getUint()
    {
        if ( !_file )
            return 0;
        T val;
        return std::fread( &val, sizeof( T ), 1, _file ) == 1 ? val : 0;
    }

    template <typename T>
    void putUint( const T val )
    {
        if ( _file )
            std::fwrite( &val, sizeof( T ), 1, _file );
    }
};

#endif
