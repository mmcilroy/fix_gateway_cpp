
engine = fix.engine()

function handler( session, request )
    engine:send( session, request )
end

engine:acceptor( 'tcp@localhost:14002', handler )
engine:start()
