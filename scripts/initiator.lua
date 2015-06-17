
header = {
  {  8, 'FIX.4.4' },
  { 49, 'SENDER' },
  { 56, 'TARGET' }
}

request = {
  { 11, '12345' },
  { 55, 'VOD.L' }
}

session = fix.initiator( 'localhost:8060' )
fix.send( session, 'D', request )
