require 'util'

function on_init()
  fix.accept( ':14001' )
end

function on_event( session, request )
  fix.send( session, '8', {
    {  11, val( request, 11 ) },
    {  55, val( request, 55 ) },
    {  39, '0' },
    { 150, '0' }
  } )
end
