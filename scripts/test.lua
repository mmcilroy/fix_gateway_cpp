
engine = fix.engine()

function handler( session, message )
    engine:send( session, message )
end

engine:acceptor( 'tcp@localhost:14002', handler )
engine:start()
