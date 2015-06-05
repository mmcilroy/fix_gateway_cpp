
function handler( session, request )
  response = {
    [  6]='0',
    [ 11]=request[11],
    [ 14]='0',
    [ 17]='12345',
    [ 37]='12345',
    [ 39]='0',
    [ 54]='1',
    [ 55]=request[55],
    [150]='0',
    [151]='100'
  }

  if request[35] == 'A' then
    fix.send( session, 'A', { [98]='0', [108]='30' } )
  elseif request[35] == '0' then
    fix.send( session, '0', {} )
  elseif request[35] == 'D' then
    fix.send( session, '8', response )
  end
end

fix.acceptor( ':8060', handler )
