#pragma once

#include "fix/message.hpp"

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace fix {

class decoder
{
public:
    decoder();

    template< typename H >
    void decode( const char* msg, size_t size, H handler );

private:
    template< class H >
    void on_field( H handler );

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

    fix::message msg_;
};

#include "decoder.inl"

}
