
function val( request, tag )
  for i=1,#request do
    if tag == request[i][1] then
      return request[i][2]
    end
  end
  return nil
end

function handler( session, request )
  local msgtype = val( request, 35 )
  if msgtype == 'A' then
    fix.send( session, 'A', { { 98, '0' }, { 108, '30' } } )
  elseif msgtype == '0' then
    fix.send( session, '0', {} )
  elseif msgtype == 'D' then
    response = {
      {  11, val( request, 11 ) },
      {  55, val( request, 55 ) },
      {  39, '0' },
      { 150, '0' }
    }
    fix.send( session, '8', response )
  end
end

fix.acceptor( ':8060', handler )

response = {
  {  11, 'clordid' },
  {  55, 'symbol' },
  {  39, '0' },
  { 150, '0' }
}
