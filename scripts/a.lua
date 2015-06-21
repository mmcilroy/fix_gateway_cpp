
function on_init()
  fix.accept( ':8060' )
end

function on_event( session, request )
  fix.send( session, '8', {
    {  11, '12345' },
    {  39, '0' },
    {  55, 'VOD.L' },
    { 150, '0' }
  } )
end
