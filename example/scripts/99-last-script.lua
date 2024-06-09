function before_output()
  print("<script 99>")
  print("showing config.title of pages")
  for _, v in ipairs(get_pages_with_config('title')) do
    print(v .. ": " .. get_config_from_page(v).title)
  end

  print('---')

  for _, v in ipairs(get_pages_with_config('title', 'Custom Title')) do
    print(v)
  end
  print("</script 99>")
end

