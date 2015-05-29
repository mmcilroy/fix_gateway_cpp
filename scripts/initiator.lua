
function handler( session, request )
end

session = fix.initiator( 'localhost:14002', handler )

request = {
    [55]='VOD.L',
    [128]='XLON'
}

fix.send( session, 'A', request )
fix.send( session, '0', request )
fix.send( session, 'D', request )
