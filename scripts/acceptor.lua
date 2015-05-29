
function handler( session, request )
    if request[35] == 'A' then
        fix.send( session, 'A', {} )
    elseif request[35] == '0' then
        fix.send( session, '0', {} )
    elseif request[35] == 'D' then
        fix.send( session, '8', { [55]=request[55] } )
    end
end

fix.acceptor( ':14002', handler )
