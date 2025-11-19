-- Utility helpers: dump (safe table serialization) and expand (Cartesian
-- expansion of path templates using '{}' placeholders).

local function dump(o, seen)
  if type(o) == 'table' then
    seen = seen or {}
    if seen[o] then return '<cycle>' end
    seen[o] = true
    local parts = {}
    for k, v in pairs(o) do
      local key = type(k) == 'number' and k or ('"'..k..'"')
      parts[#parts+1] = '['..key..'] = ' .. dump(v, seen)
    end
    return '{ ' .. table.concat(parts, ', ') .. ' }'
  else
    return tostring(o)
  end
end

local function count_placeholders(s)
  local n = 0
  s:gsub("{}", function() n = n + 1 end)
  return n
end

local function expand(path, ...)
  -- Expected output for path "{}/src/{}.html" with { 0, 1 } and { 'a', 'b' }:
  -- { "0/src/a.html", "0/src/b.html", "1/src/a.html", "1/src/b.html" }
  --
  -- Expected output for path "{}/src/{}/{}.html" with { 0, 1 }, { 'a', 'b' }, { 0, 2 }:
  -- {
  --   "0/src/a/0.html", "0/src/a/2.html", "0/src/b/0.html", "0/src/b/2.html",
  --   "1/src/a/0.html", "1/src/a/2.html", "1/src/b/0.html", "1/src/b/2.html"
  -- }
  local args = {...}
  assert(count_placeholders(path) == #args, "placeholder count must match arg list count")
  local results = {}
  local function helper(current, depth)
    if depth > #args then
      results[#results+1] = current
      return
    end
    local current_args = args[depth]
    assert(type(current_args) == 'table', 'arg '..depth..' must be a table')
    if #current_args == 0 then return end
    for _, v in ipairs(current_args) do
      helper(current:gsub("{}", v, 1), depth + 1)
    end
  end
  helper(path, 1)
  return results
end

local test = expand("{}/src/{}/{}.html", { 0, 1 }, { 'a', 'b' }, { 0, 2 } )
print(dump(test))
