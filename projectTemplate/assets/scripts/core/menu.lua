declare("Menu", {})

local DRAW_HIGHLIGHT = true

local BUTTON = 1
local OPTION = 2
local SLIDER = 3
local SPACER = 4

local ARROW_UP = -1
local ARROW_DOWN = 1

local ARROW_WIDTH = 120
local ARROW_HEIGHT = 20

local SLIDER_WIDTH = 260
local SLIDER_HEIGHT = 26
local SLIDER_GAP = SLIDER_HEIGHT * 0.3

declare("menuInput", {
    --  keyboard and gamepad are unified
    keyboardEnabled = true,
    mouseEnabled = true,
})

declare("menuOptions", {
    --  if true, menu title and item labels are localization keys rather than display literals
    localized = false,

    width = videoConfig.xRes,
    height = videoConfig.yRes,
    ofsX = 0,
    ofsY = 0,
    paddingX = 100,
    paddingY = 50,
    spacing = 1.5,

    titleFont = nil,
    titleSize = 1.5,
    titleAlignment = AB.font.CENTER,
    titleGap = 0.5,

    font = nil,
    size = 1,
    alignment = AB.font.CENTER,

    sfx = {
        menuSelect = 0,
        menuBack = 0,
        menuMove = 0
    },

    back = function() end
})

local uiState = {
    mouseX = 0,
    mouseY = 0,
    mouseDown = false,
    
    oldMouseX = 0,
    oldMouseY = 0,
    mouseMoved = false,

    up = false,
    down = false,
    left = false,
    right = false,
    select = false,
}

function Menu:new(title, options)
    title = title or ""
    options = options or menuOptions

    if (not options.font) then
        error("Need to specify a font for menu!")
    end

    local menu = {
        title = {
            label = title,
            font = options.titleFont or options.font,
            size = options.titleSize,
            alignment = options.titleAlignment,
            gap = options.titleGap,
            x = 0,
            y = 0,
        },

        items = {},
        built = false,

        scroll = false,
        scrollTop = 0,
        scrollBottom = 0,
        scrollMin = 0,
        scrollMax = 0,
        scrollOffset = 0,
        scrollTarget = 0,

        hot = 0,
        active = 0,
        lastWidget = 1,
    }
    --  shallow copy menu options
    for k, v in pairs(options) do
        menu[k] = v
    end

    setmetatable(menu, self)
    self.__index = self

    return menu
end

function Menu:clear()
    self.items = {}
    self.built = false
    self.hot = 0
    self.active = 0
end

function Menu:build()
    local titleLineHeight, titleBase, titleMaxHeight = AB.font.getMetrics(self.title.font)
    local itemLineHeight, itemBase, itemMaxHeight, itemMaxUp, itemMaxDown = AB.font.getMetrics(self.font)
    local y = 0

    --  title
    if (self.title.label ~= "") then
        if (self.localized) then
            self.title.label = AB.l10n.getString(self.title.label)
        end

        if (self.title.alignment == AB.font.LEFT) then
            self.title.x = self.paddingX
        end
        if (self.title.alignment == AB.font.CENTER) then
            self.title.x = self.width / 2
        end
        if (self.title.alignment == AB.font.RIGHT) then
            self.title.x = self.width - self.paddingX
        end

        self.title.y = y + titleBase * self.title.size
        local titleAdvance = titleLineHeight * self.title.size + itemLineHeight * self.size * (self.spacing - 1)
        titleAdvance = titleAdvance + itemLineHeight * self.size * self.title.gap
        y = y + titleAdvance
    end

    --  this is an opportune point to set the up arrow's y position
    --  given the current state of y, although it may not be used
    self.upArrow = {
        y = y + self.ofsY + self.paddingY,
        pressed = false
    }
    self.downArrow = {
        y = self.height - self.paddingY + self.ofsY,
        pressed = false
    }
    local topOfItems = y

    --  items
    for _, item in pairs(self.items) do
        if (item.type == SPACER) then
            y = y + (itemLineHeight * self.size) * item.ratio
        else
            if (item.type == BUTTON) then
                if (self.alignment == AB.font.LEFT) then
                    item.x = self.paddingX
                end
                if (self.alignment == AB.font.CENTER) then
                    item.x = self.width / 2
                end
                if (self.alignment == AB.font.RIGHT) then
                    item.x = self.width - self.paddingX
                end
            else
                item.x = self.paddingX
                item.x2 = self.width - self.paddingX
            end

            item.y = y + itemBase * self.size
            y = y + itemLineHeight * self.size * self.spacing
        end
    end
    y = y - itemLineHeight * self.size * (self.spacing - 1)

    --  remove spacers
    for i = #self.items, 1, -1 do
        if (self.items[i].type == SPACER) then
            table.remove(self.items, i)
        end
    end

    --  check if a "scroll pane" is needed
    local shiftY = self.paddingY
    local marginY = (itemMaxUp + itemMaxDown) * self.size / 8
    local contentHeight = math.max(0, y - topOfItems)

    local scrollGap = ARROW_HEIGHT * 2
    self.bandHeight = (self.downArrow.y - scrollGap) - (self.upArrow.y + scrollGap)

    if (contentHeight > self.bandHeight) then
        self.scroll = true
        self.scrollTop = self.upArrow.y + scrollGap
        self.scrollBottom = self.downArrow.y - scrollGap
        self.scrollSpeed = (itemLineHeight * self.size * self.spacing) / 10
    else
        self.scroll = false
        shiftY = self.height / 2 - y / 2
    end        

    --  scooch everything over and down just a teensy bit
    self.title.x = self.title.x + self.ofsX
    self.title.y = self.title.y + self.ofsY + shiftY

    local marginX = itemLineHeight * self.size / 2
    if (self.paddingX > 0 and marginX > self.paddingX) then
        marginX = self.paddingX
    end
        
    for _, item in pairs(self.items) do
        item.x = item.x + self.ofsX
        item.y = item.y + self.ofsY + shiftY

        if (item.type == OPTION or item.type == SLIDER) then
            item.x2 = item.x2 + self.ofsX
        end
        if (item.type == SLIDER) then
            item.xMin = item.x2 - SLIDER_WIDTH + SLIDER_GAP
            item.xSpan = SLIDER_WIDTH - SLIDER_GAP * 2
        end

        item.bounds = {
            x1 = self.paddingX + self.ofsX - marginX,
            x2 = self.width - self.paddingX + self.ofsX + marginX,
            y1 = item.y - itemMaxUp * self.size - marginY,
            y2 = item.y + itemMaxDown * self.size + marginY
        }
        
        --  shrink buttons x bounds
        if (item.type == BUTTON) then
            local labelWidth = AB.font.stringLength(self.font, item.label, self.size)
            if (self.alignment == AB.font.CENTER) then
                item.bounds.x1 = self.width / 2 - labelWidth / 2 - marginX + self.ofsX
                item.bounds.x2 = self.width / 2 + labelWidth / 2 + marginX + self.ofsX
            end
            if (self.alignment == AB.font.LEFT) then
                item.bounds.x2 = item.bounds.x1 + labelWidth + marginX * 2
            end
            if (self.alignment == AB.font.RIGHT) then
                item.bounds.x1 = item.bounds.x2 - (labelWidth + marginX * 2)
            end
        end
    end    

    if (self.scroll) then
        --  adjust items for scroll pane
        local extent = self.items[#self.items].bounds.y2 - self.items[1].bounds.y1
        local align  = self.scrollTop - self.items[1].bounds.y1
        for _, item in pairs(self.items) do
            item.y = item.y + align
            item.bounds.y1 = item.bounds.y1 + align
            item.bounds.y2 = item.bounds.y2 + align
        end

        --  calculate scroll extents
        self.scrollMin = 0
        self.scrollMax = math.max(0, extent - self.bandHeight)
        self.scrollTarget = self.scrollMin
        self.scrollOffset = self.scrollTarget

        --  calculate arrow bounds
        local x1 = self.width / 2 + self.ofsX - ARROW_WIDTH / 2 - marginX
        local x2 = self.width / 2 + self.ofsX + ARROW_WIDTH / 2 + marginX
        self.upArrow.bounds = {
            x1 = x1,
            y1 = self.upArrow.y - marginY,
            x2 = x2,
            y2 = self.upArrow.y + ARROW_HEIGHT + marginY
        }
        self.downArrow.bounds = {
            x1 = x1,
            y1 = self.downArrow.y - ARROW_HEIGHT - marginY,
            x2 = x2,
            y2 = self.downArrow.y + marginY
        }
    end

    --  calculate vertical offset for sliders
    self.sliderOfsY = -(itemMaxDown - itemMaxUp) * self.size / 2

    self.built = true
end

function Menu:addButton(label, callback, enabled, backButton)
    table.insert(self.items, {
        type = BUTTON,
        label = self.localized and AB.l10n.getString(label) or label,
        callback = callback,
        enabled = enabled ~= false,
        backButton = backButton or false
    })
    self.built = false
end

function Menu:addOption(label, options, callback, enabled)
    table.insert(self.items, {
        type = OPTION,
        label = self.localized and AB.l10n.getString(label) or label,
        options = options,
        callback = callback,
        enabled = enabled ~= false,
        index = 1
    })
    self.built = false
end

-- target = { value = 0 }
function Menu:addSlider(label, target, callback, enabled)
    table.insert(self.items, {
        type = SLIDER,
        label = self.localized and AB.l10n.getString(label) or label,
        target = target,
        callback = callback,
        enabled = enabled ~= false
    })
    self.built = false
end

function Menu:addSpacer(ratio)
    table.insert(self.items, {
        type = SPACER,
        ratio = ratio or 0.5,
    })
    self.built = false
end

local function regionHit(x1, y1, x2, y2)
    if (uiState.mouseX < x1 or uiState.mouseY < y1 or
        uiState.mouseX > x2 or uiState.mouseY > y2) then

        return false
    else
        return true
    end
end

function Menu:playSound(index)
    if (self.sfx.menuMove and self.sfx.menuMove ~= 0) then
        AB.audio.stopSound(self.sfx.menuMove) 
    end
    if (self.sfx.menuSelect and self.sfx.menuSelect ~= 0) then
        AB.audio.stopSound(self.sfx.menuSelect) 
    end
    if (self.sfx.menuBack and self.sfx.menuBack ~= 0) then
        AB.audio.stopSound(self.sfx.menuBack) 
    end

    if (index and index ~= 0) then
        AB.audio.playSound(index)
    end
end

function Menu:setSliderValue(item)
    item.target.value = math.max(0, math.min(1, (uiState.mouseX - item.xMin) / item.xSpan))
end

function Menu:select(item)
    if (item.type == BUTTON) then
        if (item.backButton) then
            self:playSound(self.sfx.menuBack)
            if (self.scroll) then
                self.scrollTarget = self.scrollMin
                self.scrollOffset = self.scrollTarget
            end
            self.hot = 0
        else
            self:playSound(self.sfx.menuSelect)
        end
    else
        self:playSound(self.sfx.menuMove)
    end
    uiState.select = false
    if (item.type == OPTION) then
        item.index = item.index + 1
        if (item.index > #item.options) then
            item.index = 1
        end
    end
    if (item.type == SLIDER) then
        if (item.target.value > 0.999) then
            item.target.value = 0.0
        else
            item.target.value = item.target.value + 0.1
            if (item.target.value > 1.0) then
                item.target.value = 1.0
            end
        end
    end
    item.callback()
end

function Menu:selectSlider(item)
    if (uiState.mouseX < item.xMin) then
        self:select(item)
        return    
    end

    self:setSliderValue(item)
    self:playSound(self.sfx.menuMove)
    uiState.select = false
    item.callback()
end

function Menu:ensureHotVisible()
    if (not self.scroll or self.hot == 0 or not self.items[self.hot]) then
        return
    end

    local item = self.items[self.hot]
    if (item.bounds.y1 - self.scrollTarget < self.scrollTop) then
        self.scrollTarget = item.bounds.y1 - self.scrollTop
    elseif (item.bounds.y2 - self.scrollTarget > self.scrollBottom) then
        self.scrollTarget = item.bounds.y2 - self.scrollBottom
    end

    --  snap scroll to extents if first or last item
    if (item == self.items[1]) then
        self.scrollTarget = self.scrollMin
    end
    if (item == self.items[#self.items]) then
        self.scrollTarget = self.scrollMax
    end
end

function Menu:update()
    if (not self.built) then
        self:build()
    end

    --  uiState.prepare()
    if (menuInput.mouseEnabled) then
        uiState.mouseX, uiState.mouseY, uiState.mouseDown = AB.input.getMouseState()
        uiState.mouseX = (uiState.mouseX - videoConfig.xOffset) / videoConfig.xScale
        uiState.mouseY = (uiState.mouseY - videoConfig.yOffset) / videoConfig.yScale
    
        uiState.mouseMoved = uiState.mouseX ~= uiState.oldMouseX or uiState.mouseY ~= uiState.oldMouseY
        uiState.oldMouseX = uiState.mouseX
        uiState.oldMouseY = uiState.mouseY
    else
        uiState.mouseDown = false
        uiState.mouseMoved = false
    end

    uiState.up = menuInput.keyboardEnabled and AB.input.menuUp()
    uiState.down = menuInput.keyboardEnabled and AB.input.menuDown()
    uiState.left = menuInput.keyboardEnabled and AB.input.menuLeft()
    uiState.right = menuInput.keyboardEnabled and AB.input.menuRight()
    uiState.select = menuInput.keyboardEnabled and AB.input.menuSelect()

    --  later
    if (AB.input.menuBack() and menuInput.keyboardEnabled) then
        self:playSound(self.sfx.menuBack)
        self.back()
        return
    end

    if (self.scroll) then
        --  handle mouse wheel scrolling
        if (menuInput.mouseEnabled) then
            self.scrollTarget = self.scrollTarget - AB.input.getMouseWheelMovement() * self.scrollSpeed * 4
            self.scrollTarget = math.max(self.scrollMin, math.min(self.scrollTarget, self.scrollMax))
        end

        --  handle scroll arrows
        self.upArrow.pressed = false
        self.downArrow.pressed = false

        if (uiState.mouseDown) then
            if (regionHit(self.upArrow.bounds.x1, self.upArrow.bounds.y1,
                self.upArrow.bounds.x2, self.upArrow.bounds.y2) and
                self.scrollTarget > self.scrollMin) then

                self.scrollTarget = math.max(self.scrollMin, self.scrollTarget - self.scrollSpeed)
                self.upArrow.pressed = true
            end

            if (regionHit(self.downArrow.bounds.x1, self.downArrow.bounds.y1,
                self.downArrow.bounds.x2, self.downArrow.bounds.y2) and
                self.scrollTarget < self.scrollMax) then

                self.scrollTarget = math.min(self.scrollTarget + self.scrollSpeed, self.scrollMax)
                self.downArrow.pressed = true
            end
        end
    end

    --  update items
    for id = 1, #self.items do
        local item = self.items[id]

        if (item.enabled) then
            local yOffset = -self.scrollOffset
            local hit = regionHit(item.bounds.x1, item.bounds.y1 + yOffset, item.bounds.x2, item.bounds.y2 + yOffset)

            --  if mouse outside scissor box set hit to false
            if (self.scroll) then
                if (uiState.mouseY < self.scrollTop or uiState.mouseY > self.scrollBottom) then
                    hit = false
                end
            end

            --  check hover
            if (uiState.mouseMoved and hit) then
                if (self.hot ~= id) then
                    self:playSound(self.sfx.menuMove)
                end
                self.hot = id
                self:ensureHotVisible()
            end
    
            --  check press
            if (hit) then
                if (self.active == 0 and uiState.mouseDown) then
                    self.active = id
                    item.scrubbed = false
                end
            end
    
            --  scrub sliders
            if (item.type == SLIDER and self.active == id and hit and uiState.mouseDown) then
                if (item.scrubbed) then
                    self:setSliderValue(item)
                else
                    if (uiState.mouseX >= item.xMin) then
                        self:setSliderValue(item)
                        item.scrubbed = true
                    end
                end
            end            

            --  button hot and active, but mouse not down, user clicked!
            if (not uiState.mouseDown and self.hot == id and self.active == id) then
                if (item.type == SLIDER and not item.scrubbed) then
                    self:selectSlider(item)
                elseif (item.type == SLIDER) then
                    self:playSound(self.sfx.menuMove)
                    item.callback()
                 else
                    self:select(item)
                end
                break
            end
    
            --  take focus if no widget has it
            if (self.hot == 0) then
                self.hot = id;
                self:ensureHotVisible()
            end

            --  handle keyboard / gamepad
            if (self.hot == id) then
                if (uiState.up) then
                    self:playSound(self.sfx.menuMove)
                    self.hot = self.lastWidget
                    uiState.up = false
                    self:ensureHotVisible()
                end
                if (uiState.down) then
                    self:playSound(self.sfx.menuMove)
                    self.hot = 0
                    uiState.down = false
                end
                if (uiState.right) then
                    if (item.type == OPTION) then
                        self:playSound(self.sfx.menuMove)
                        item.index = item.index + 1
                        if (item.index > #item.options) then
                            item.index = 1
                        end
                    end
                    if (item.type == SLIDER) then
                        self:playSound(self.sfx.menuMove)
                        item.target.value = item.target.value + 0.1
                        if (item.target.value >= 1.0) then
                            item.target.value = 1.0
                        end
                    end
                end
                if (uiState.left) then
                    if (item.type == OPTION) then
                        self:playSound(self.sfx.menuMove)
                        item.index = item.index - 1
                        if (item.index < 1) then
                            item.index = #item.options
                        end
                    end
                    if (item.type == SLIDER) then
                        self:playSound(self.sfx.menuMove)
                        item.target.value = item.target.value - 0.1
                        if (item.target.value <= 0.0) then
                            item.target.value = 0.0
                        end
                    end
                end

                if (uiState.select) then
                    self:select(item)
                    break
                end
            end
        end
        if (item.enabled) then
            self.lastWidget = id
        end
    end

    --  update scrolling
    if (self.scroll) then
        if (math.abs(self.scrollOffset - self.scrollTarget) > 0.5) then
            self.scrollOffset = self.scrollOffset + (self.scrollTarget - self.scrollOffset) / 8
        else
            self.scrollOffset = self.scrollTarget
        end
    end    

    --  uiState.finish()
    if (not uiState.mouseDown) then
        if (self.active > 0) then
            self.items[self.active].scrubbed = false
        end
        self.active = 0
    else
        --  if the mouse is clicked, but no widget is active, we need to mark the active item
        --  unavailable so that we won't activate the next widget we drag the cursor onto
        if (self.active == 0) then
            self.active = -1
        end
    end
end

local function setItemColor(font, lit, enabled)
    if (lit) then
        AB.graphics.setColor(1, 1, 1)
        AB.font.setColor(font, 1, 1, 1)
    else
        if (DRAW_HIGHLIGHT) then
            AB.graphics.setColor(1, 1, 1)
            AB.font.setColor(font, 1, 1, 1)
        else
            AB.graphics.setColor(1, 1, 1, 0.5)
            AB.font.setColor(font, 1, 1, 1, 0.5)
        end
    end
    if (not enabled) then
        AB.graphics.setColor(1, 1, 1, 0.25)
        AB.font.setColor(font, 1, 1, 1, 0.25)
    end
end

function Menu:render()
    if (not self.built) then
        self:build()
    end

    --  render title
    if (self.title.label ~= "") then
        AB.font.setColor(self.title.font, 1, 1, 1)
        AB.font.printString(0, self.title.font, self.title.x, self.title.y, self.title.size, self.title.alignment, self.title.label)
    end

    --  render arrow buttons
    if (self.scroll) then
        local x = self.width / 2 + self.ofsX
        local y = self.upArrow.y + ARROW_HEIGHT
        if (self.scrollTarget > self.scrollMin) then
            AB.graphics.setColor()
        else
            AB.graphics.setColor(1, 1, 1, 0.25)
        end
        local scale = 1
        if (self.upArrow.pressed) then
            scale = 0.75
        end
        AB.graphics.renderTri(0,
            x - ARROW_WIDTH * scale / 2, y,
            x + ARROW_WIDTH * scale/ 2, y,
            x, y - ARROW_HEIGHT * scale
        )

        y = self.downArrow.y - ARROW_HEIGHT
        if (self.scrollTarget < self.scrollMax) then
            AB.graphics.setColor()
        else
            AB.graphics.setColor(1, 1, 1, 0.25)
        end
        scale = 1
        if (self.downArrow.pressed) then
            scale = 0.75
        end
        AB.graphics.renderTri(0,
            x - ARROW_WIDTH * scale / 2, y,
            x + ARROW_WIDTH * scale / 2, y,
            x, y + ARROW_HEIGHT * scale
        )
    end

    --  render all items
    if (self.scroll) then
        local x = self.ofsX * videoConfig.xScale + videoConfig.xOffset
        local y = self.scrollTop * videoConfig.yScale + videoConfig.yOffset
        local w = self.width * videoConfig.xScale
        local h = (self.scrollBottom - self.scrollTop) * videoConfig.yScale
        AB.graphics.flushGraphics()
        AB.graphics.setScissor(x, videoConfig.yRes - y - h, w, h)
    end
    for i = 1, #self.items do
        local item = self.items[i]

        local lit = false
        if (self.active <= 0) then
            lit = (i == self.hot)
        else
            lit = (i == self.active)
        end

        if (DRAW_HIGHLIGHT) then
            if (lit) then
                AB.font.setColor(1, 1, 1)
                local w = item.bounds.x2 - item.bounds.x1
                local h = item.bounds.y2 - item.bounds.y1
                AB.graphics.setColor()
                AB.graphics.renderQuad(0, w, h, item.bounds.x1 + w / 2, item.bounds.y1 + h / 2 - self.scrollOffset)
    
                AB.font.setColor(self.font, 0, 0, 0)
                AB.graphics.setColor(0, 0, 0)
            else
                if (item.enabled) then
                    AB.graphics.setColor()
                    AB.font.setColor(self.font)
                else
                    AB.graphics.setColor(1, 1, 1, 0.25)
                    AB.font.setColor(self.font, 1, 1, 1, 0.25)
                end
            end
        else
            setItemColor(self.font, lit, item.enabled)
        end

        --  shimmy all text over a smidge when item pressed
        local ofsX = 0
        local ofsY = 0
        local shift = i == self.active and lit
        if (item.type == SLIDER and item.scrubbed) then
            shift = false
        end
        if (shift) then
            ofsX = 1
            ofsY = 2
        end

        if (item.type == BUTTON) then
            AB.font.printString(0, self.font, item.x + ofsX, item.y + ofsY - self.scrollOffset, self.size, self.alignment, item.label)
        end

        if (item.type == OPTION) then
            AB.font.printString(0, self.font, item.x + ofsX, item.y + ofsY - self.scrollOffset, self.size, AB.font.LEFT, item.label)
            AB.font.printString(0, self.font, item.x2 + ofsX, item.y + ofsY - self.scrollOffset, self.size, AB.font.RIGHT, item.options[item.index])
        end

        if (item.type == SLIDER) then
            AB.font.printString(0, self.font, item.x + ofsX, item.y + ofsY - self.scrollOffset, self.size, AB.font.LEFT, item.label)

            setItemColor(self.font, lit, item.enabled)

            local yPos = item.y - self.sliderOfsY - self.scrollOffset
            AB.graphics.renderQuad(0, SLIDER_WIDTH, SLIDER_HEIGHT, item.x2 - SLIDER_WIDTH / 2, yPos)
            AB.graphics.setColor(0, 0, 0)
            AB.graphics.renderQuad(0, SLIDER_WIDTH - SLIDER_GAP, SLIDER_HEIGHT - SLIDER_GAP, item.x2 - SLIDER_WIDTH / 2, yPos)

            setItemColor(self.font, lit, item.enabled)
            local width = item.xSpan * item.target.value
            local xPos = item.xMin + width / 2
            AB.graphics.renderQuad(0, width, SLIDER_HEIGHT - SLIDER_GAP * 2, xPos, yPos)
        end
    end
    if (self.scroll) then
        AB.graphics.flushGraphics()
        AB.graphics.setScissor()
    end
end
