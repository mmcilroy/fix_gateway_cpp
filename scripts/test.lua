
function encode()
  local request = {
    { 128, 'exchange' },
    { 146, '3' },
    {  55, 'symbol1 '},
    {  55, 'symbol2 '},
    {  55, 'symbol3 '}
  }
  for k0,v0 in pairs( request ) do
    print( v0[1] .. ' = ' .. v0[2] )
  end
end

encode()
