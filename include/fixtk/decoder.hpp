#pragma once

#include "fixtk/message.hpp"

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace fixtk {

template< typename H >
void parse( const std::string& msg, H handler )
{
    static boost::char_separator<char> field_sep( "\x01" );
    boost::tokenizer< boost::char_separator< char > > fields( msg, field_sep );
    BOOST_FOREACH( const std::string& field, fields )
    {
        std::vector< std::string > strs;
        boost::split( strs, field, boost::is_any_of( "=" ) );
        handler( boost::lexical_cast< tag >( strs[0] ), strs[1] );
    }
}

class decoder
{
public:
    decoder();

    template< typename H >
    void decode( const char* msg, size_t size, H handler );

private:
    template< class H >
    void on_field( H& handler );

    void init_state();

    enum field_state {
        TAG, VALUE
    };

    enum message_state {
        SEEK_HEADER, SEEK_CHECKSUM
    };

    field_state field_state_;
    message_state message_state_;

    std::string tag_;
    std::string val_;
    std::string buf_;
};

decoder::decoder()
{
    init_state();
}

void decoder::init_state()
{
    tag_.clear();
    val_.clear();
    buf_.clear();

    field_state_ = TAG;
    message_state_ = SEEK_HEADER;
}

template< typename H >
void decoder::decode( const char* msg, size_t size, H handler )
{
    for( size_t i = 0; i < size; ++i )
    {
        char c = msg[i];

        if( field_state_ == TAG )
        {
            if( c == '=' )
            {
                // got the tag now look for the value
                field_state_ = VALUE;
            }
            else
            if( !isdigit( c ) )
            {
                // non numeric data should cause us to start looking for the tag afresh
                tag_.clear();
            }
            else
            {
                tag_.push_back( c );
            }
        }
        else
        {
            if( c == (char)1 )
            {
                // if we get a 1 assume we now have a complete fix field
                on_field( handler );

                // reset state so we start looking for next tag
                field_state_ = TAG;
                tag_.clear();
                val_.clear();
            }
            else
            {
                val_.push_back( c );
            }
        }
    }
}

template< typename H >
void decoder::on_field( H& handler )
{
    if( tag_.size() == 0 || val_.size() == 0 ) {
        return;
    }

    tag t = boost::lexical_cast< tag >( tag_.c_str() );

    if( t <= 0 ) {
        return;
    }

    buf_.append( tag_ );
    buf_.append( "=" );
    buf_.append( val_ );
    buf_.append( "\x1" );

    if( message_state_ == SEEK_HEADER )
    {
        if( t == 8 ) {
            message_state_ = SEEK_CHECKSUM;
        }
    }
    else
    {
        if( t == 10 )
        {
            handler( buf_ );
            init_state();
        }
    }
}

}
