#!/usr/bin/env lua

--[[
 @filename  massive_file.lua
 @version   1.0
 @autor     Máster Vitronic <mastervitronic@gmail.com>
 @date      lun oct 16 09:50:42 -04 2023
 @licence   MIT licence
]]--

local pdfgen = require("../pdfgen")
local pdf = pdfgen:new()
local pagecount = 10;
PDF_LETTER_WIDTH  =  612.0
PDF_LETTER_HEIGHT = 792.0

local options = {
  creator = 'Vitronic',
  producer= 'producer',
  title   = 'title',
  author  = 'author',
  subject = 'subject',
  date    = os.date('%Y%m%d%H%M%SZ')
}

pdf:create(
  PDF_LETTER_WIDTH,
  PDF_LETTER_HEIGHT,
  options
)
pdf:set_font('Times-Roman')

local filename = ("massive-%d.pdf"):format(pagecount);
for i=1, pagecount do
  local str = ("page %d"):format(i);
  pdf:append_page();
  pdf:add_text(nil, str, 12, 50, 20, '0x000000');
  pdf:add_image_file(nil, 100, 500, 50, 150, "data/penguin.jpg");
end

print(pdf:save(filename))
pdf:destroy()
