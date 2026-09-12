local Serialization = {}

function Serialization:clone()
    local nt = {}
    for k, v in pairs(self) do
        nt[k] = v
    end
    return nt
end

declare("serialize", function(root)
    return Serialization:clone():serialize(root)
end)

function Serialization:serialize(root)
    if (type(root) ~= "table") then
        error("Can only serialize tables")
    end

    self.tableToRef = {}
    self.refToTable = {}

    self:ref(root)

    local out = {}
    local savecount = 0

    while #self.refToTable > savecount do
        savecount = savecount + 1
        local t = self.refToTable[savecount]

        table.insert(out, "{")
        for k, v in pairs(t) do
            local vk = self:value(k)
            local vv = self:value(v)
            if (vk and vv) then
                table.insert(out, string.format("[%s]=%s,", vk, vv))
            end
        end
        table.insert(out, "},")
    end

    return "{" .. table.concat(out) .. "}"
end

function Serialization:value(v)
    local t = type(v)

    if (t == "string") then
        return string.format("%q", v)
    elseif (t == "number") then
        return tostring(v)
    elseif (t == "boolean") then
        return tostring(v)
    elseif (t == "table") then
        return "{" .. self:ref(v) .. "}"
    else
        -- skip functions, userdata, threads, nil
        return nil
    end
end

function Serialization:ref(t)
    local ref = self.tableToRef[t]
    if (not ref) then
        if (t == self) then
            error("Can't serialize the serialization object")
        end
        table.insert(self.refToTable, t)
        ref = #self.refToTable
        self.tableToRef[t] = ref
    end
    return ref
end

declare("restoreTable", function(dest, source, visited)
    --  avoid circular references
    visited = visited or {}
    if (visited[source]) then
        return visited[source]
    end
    visited[source] = dest

    for k, v in pairs(source) do
        local vt = type(v)

        if (vt == "table") then
            if (type(dest[k])) ~= "table" then
                dest[k] = {}
            end
            restoreTable(dest[k], v, visited)

        elseif (vt ~= "function") then
            -- numbers, strings, booleans
            dest[k] = v
        end
    end

    return dest
end)

declare("unserialize", function(s)
    if (type(s) ~= "string") then
        error("String expected")
    end

    local chunk, err = load("return " .. s, "unserialize", "t", {})
    if (not chunk) then
        error(err)
    end

    local tables = chunk()

    for i = 1, #tables do
        local t = tables[i]
        local copy = {}
        for k, v in pairs(t) do
            copy[k] = v
        end

        for k, v in pairs(copy) do
            local nk = (type(k) == "table") and tables[k[1]] or k
            local nv = (type(v) == "table") and tables[v[1]] or v
            t[k] = nil
            t[nk] = nv
        end
    end

    return tables[1]
end)

