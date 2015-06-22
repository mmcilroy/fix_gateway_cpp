
function on_init()
  request = {
    { 11, 'BBBBBBBB' },
    { 55, 'VOD.L' }
  }
  session = fix.connect( 'localhost:14000' )
  fix.send( session, 'D', request )
end

function on_event( session, request )
end
