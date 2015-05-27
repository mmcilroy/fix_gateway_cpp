
engine = fix.engine()

function handler( session, request )
    print( 'hello' )
    --[[
    if request[35] == 'D' then
        response[35] = '8'
        response[39] = '0'
        response[55] = request[55]
        engine:send( session, response )
    end
    ]]
end

engine:acceptor( 'tcp@localhost:14002', handler )
engine:start()
