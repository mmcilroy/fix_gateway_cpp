
typedef int tag;
typedef std::string value;
struct field { tag tag_; value value_; };
typedef std::vector< field > field_vector;
typedef std::map< tag, field* > field_map;

class message
{
public:
    message()
    {
    }

    message( const std::string& )
    {
    }

    field& operator[]( t )
    {
        field_map::iterator it = index_.find( t )
        if( it == index_.end() ) {
            
        } else {
            return it.second();
        }
    }

private:
    field_vector fields_;
    field_map index_;
};
