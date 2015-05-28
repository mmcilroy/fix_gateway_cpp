
function handler( session, request )
    print( request )
end

fix.acceptor( 'tcp@localhost:14002', handler )
