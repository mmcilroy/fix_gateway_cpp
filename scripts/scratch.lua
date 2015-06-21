
function on_init()
  fix.accept( ':8080' )
end

function on_event( session, request )
  fix.send( session, response )             -- respond to sender
end
