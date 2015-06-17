inline decoder::decoder()
{
    init_state();
}

inline void decoder::init_state()
{
    tag_.clear();
    val_.clear();
    msg_.clear();

    field_state_ = TAG;
    message_state_ = SEEK_HEADER;
}

template< typename H >
inline void decoder::decode( const char* msg, size_t size, H handler )
{
    for( size_t i = 0; i < size; ++i )
    {
        char c = msg[i];

        if( field_state_ == TAG )
        {
            if( c == '=' ) {
                field_state_ = VALUE; // got the tag now look for the value
            } else if( !isdigit( c ) ) {
                tag_.clear(); // ignore non numeric tags
            } else {
                tag_.push_back( c );
            }
        }
        else
        {
            if( c == delim_char )
            {
                on_field( handler );
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
inline void decoder::on_field( H handler )
{
    if( tag_.size() == 0 || val_.size() == 0 ) {
        return;
    }

    tag t = boost::lexical_cast< tag >( tag_.c_str() );
    msg_.add( t, val_ );

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
            handler( msg_ );
            init_state();
        }
    }
}
