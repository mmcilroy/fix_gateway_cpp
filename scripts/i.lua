
function on_init()
  request = {
    { 11, '12345' },
    { 55, 'VOD.L' }
  }
  session = fix.connect( 'localhost:8060' )
  fix.send( session, 'D', request )
end

function on_event( session, request )
end
