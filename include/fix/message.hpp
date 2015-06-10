#pragma once

#include "fix/constants.hpp"

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace fix {

// ----------------------------------------------------------------------------
typedef int tag;

// ----------------------------------------------------------------------------
struct header
{
    std::string protocol_;
    std::string sender_;
    std::string target_;
    int sequence_;
};

// ----------------------------------------------------------------------------
class message
{
public:
    template< typename T >
    void add( tag, T );

    template< typename H >
    void parse( H ) const;

    const std::string& str() const;

private:
    std::string buf_;
};

// ----------------------------------------------------------------------------
template< typename T >
void message::add( tag t, T v )
{
    std::stringstream ss;
    ss << t << "=" << v << delim_char;
    buf_.append( ss.str() );
}

template< typename H >
void message::parse( H handler ) const
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

const std::string& message::str() const
{
    return buf_;
}

}
