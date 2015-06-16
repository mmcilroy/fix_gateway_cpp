#pragma once

void set_utc_time( std::string& s )
{
    time_t t = time( 0 );
    struct tm* utc = gmtime( &t );
    s.resize( 16 );
    s.clear();
    sprintf( (char*)s.c_str(), "%04d%02d%02d-%02d:%02d:%02d",
        utc->tm_year+1900,
        utc->tm_mon+1,
        utc->tm_mday,
        utc->tm_hour,
        utc->tm_min,
        utc->tm_sec );
}
