function before_output()
  print("<script 99>")
  for _, v in ipairs(get_pages_with_config('title')) do
    print(v)
  end

  print('---')

  for _, v in ipairs(get_pages_with_config('title', 'Custom Title')) do
    print(v)
  end
  print("</script 99>")
end

