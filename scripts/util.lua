function val( request, tag )
  for i=1,#request do
    if tag == request[i][1] then
      return request[i][2]
    end
  end
  return nil
end
