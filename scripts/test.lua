
local request = {
  { 128, 'exchange' },
  { 146, '3' },
  {  55, 'symbol1' },
  {  55, 'symbol2' },
  {  55, 'symbol3' }
}

for i=1,#request do
  print( request[i][1] )
end
