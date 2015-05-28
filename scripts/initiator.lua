
function handler( session, request )
    print( session )
    print( request )
end

session = fix.initiator( 'tcp@localhost:14002', handler )

request = {
    [35]='D',
    [55]='VOD.L'
}

fix.send( session, request )
