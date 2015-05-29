#pragma once

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

namespace fixtk {

typedef int tag;

typedef std::string value;

struct field
{
    field()
    {
    }

    field( tag t, const value& v ) :
        tag_( t ),
        value_( v )
    {
    }

    template< typename T >
    field( tag t, const T& v ) :
        tag_( t )
    {
        std::stringstream ss;
        ss << v;
        value_ = ss.str();
    }

    tag tag_;
    value value_;
};

struct header
{
    std::string type_;
    std::string protocol_;
    std::string sender_;
    std::string target_;
    int sequence_ = 0;
};

typedef std::vector< field > field_vector;

typedef std::map< int, std::string > field_map;

typedef std::string message;

}

inline std::ostream& operator<<( std::ostream& out, const fixtk::field_vector& flds )
{
    for( int i=0; i<flds.size(); i++ ) {
        out << flds[i].tag_ << "=" << flds[i].value_ << "|";
    }

    return out;
}
