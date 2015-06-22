require 'util'

function on_init()
  venue = fix.connect( 'localhost:14001' )
  fix.accept( ':14000' )
end

function send_to_venue( session, request )
  fix.send( venue, 'D', {
    { 11, session .. '#' .. val( request, 11 ) },
    { 55, val( request, 55 ) }
  } )
end

function send_to_client( session, request )
  local client = tonumber( string.sub( val( request, 11 ), 1, 1 ) )
  local id = string.sub( val( request, 11 ), 3 )

  fix.send( client, '8', {
    {  11, id },
    {  55, val( request, 55 ) },
    {  39, val( request, 39 ) },
    { 150, val( request, 150 ) }
  } )
end

function on_event( session, request )
  local msgtype = val( request, 35 )
  if msgtype == 'D' then
    send_to_venue( session, request )
  elseif msgtype == '8' then
    send_to_client( session, request )
  end
end
