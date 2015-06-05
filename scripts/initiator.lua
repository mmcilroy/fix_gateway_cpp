
request = {
    [ 60]=fix.time(),
    [ 40]='2',
    [ 54]='1',
    [ 55]='VOD.L',
    [ 11]='12345',
    [ 21]='3',
    [ 38]='200',
    [ 44]='100',
    [100]='XOSL',
    [167]='CS',
    [528]='P'
}

function handler( session, response )
    if response[35] == 'A' then
        fix.send( session, 'D', request )
    end
end

header = {
    [ 8]='FIX.4.4',
    [49]='SENDER',
    [56]='TARGET'
}

session = fix.initiator( 'localhost:9107', header, handler )
fix.send( session, 'A', { [98]='0', [108]='30' } )
