#pragma once

#include "fix/constants.hpp"

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>

namespace fix {

// ----------------------------------------------------------------------------
typedef int tag;

// ----------------------------------------------------------------------------
struct header
{
    std::string protocol_;
    std::string sender_;
    std::string target_;
};

// ----------------------------------------------------------------------------
class message
{
public:
    message();

    message( const std::string& );

    void add( message& );

    template< typename T >
    void add( tag, T );

    template< typename H >
    void parse( H ) const;

    size_t size() const;

    void clear();

    char operator[]( size_t n ) const;

    const std::string& str() const;

private:
    std::string buf_;
};

// ----------------------------------------------------------------------------
inline std::ostream& operator<<( std::ostream& out, const header& hdr )
{
    out << "protocol=" << hdr.protocol_
        << ", sender=" << hdr.sender_
        << ", target=" << hdr.target_;
    return out;
}

inline std::ostream& operator<<( std::ostream& out, const message& msg )
{
    out << msg.str();
    return out;
}

inline message::message()
{
}

inline message::message( const std::string& buf ) :
    buf_( buf )
{
}

inline void message::add( message& msg )
{
    buf_.append( msg.str() );
}

template< typename T >
inline void message::add( tag t, T v )
{
    std::stringstream ss;
    ss << t << "=" << v << delim_char;
    buf_.append( ss.str() );
}

template< typename H >
inline void message::parse( H handler ) const
{
    static boost::char_separator<char> field_sep( delim_str );
    boost::tokenizer< boost::char_separator< char > > fields( buf_, field_sep );
    for( auto field : fields )
    {
        std::vector< std::string > strs;
        boost::split( strs, field, boost::is_any_of( "=" ) );
        handler( boost::lexical_cast< tag >( strs[0] ), strs[1] );
    }
}

inline const std::string& message::str() const
{
    return buf_;
}

inline size_t message::size() const
{
    return buf_.size();
}

inline void message::clear()
{
    buf_.clear();
}

inline char message::operator[]( size_t n ) const
{
    return buf_[n];
}

}
