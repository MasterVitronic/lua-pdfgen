#!/usr/bin/env lua

--[[
 @filename  main.lua
 @version   1.0
 @autor     Máster Vitronic <mastervitronic@gmail.com>
 @date      lun oct 16 09:50:42 -04 2023
 @licence   MIT licence
]]--

local pdfgen = require("pdfgen")
local pdf = pdfgen:new()

function rgbToHex(rgb)
	local hexadecimal = '0X'
	for key, value in pairs(rgb) do
		local hex = ''
		while(value > 0)do
			local index = math.fmod(value, 16) + 1
			value = math.floor(value / 16)
			hex = string.sub('0123456789ABCDEF', index, index) .. hex
		end
		if(string.len(hex) == 0)then
			hex = '00'
		elseif(string.len(hex) == 1)then
			hex = '0' .. hex
		end
		hexadecimal = hexadecimal .. hex
	end
	return hexadecimal
end
--print( rgbToHex({244, 164, 96}) )


--PDF_LETTER_WIDTH  =  612.0
--PDF_LETTER_HEIGHT = 792.0

local PDF_A4_WIDTH  = 595.27001953125
local PDF_A4_HEIGHT = 841.88000488281


local options = {
  creator = 'Vitronic',
  producer= 'producer',
  title   = 'title',
  author  = 'author',
  subject = 'subject',
  date    = os.date('%Y%m%d%H%M%SZ')
}

pdf:create(
  PDF_A4_WIDTH,
  PDF_A4_HEIGHT,
  options
)

if (pdf:width() ~= PDF_A4_WIDTH or pdf:height() ~= PDF_A4_HEIGHT) then
  print(("PDF Size mismatch: %fx%f\n"):format(
    pdf:width(),pdf:height()
  ))
    --return false
end


local width = pdf:get_font_text_width("Times-BoldItalic", "foo", 14);
if (width == false or width < 18) then
  print(("Font width invalid: %f\n"):format(width));
  return -1
end

--/* These calls should fail, since we haven't added a page yet */
if ( pdf:add_image_file(nil, 10, 10, 20, 30, "data/teapot.ppm") ) then
  return -1;
end
if (pdf:add_image_file(nil, 100, 500, 50, 150,"data/penguin.jpg")) then
  return -1;
end
if (pdf:add_image_file(nil, 200, 500, 100, 100, "data/coal.png")) then
  return -1;
end
if (pdf:add_image_file(nil, 300, 500, 243, 204, "data/bee.bmp")) then
  return -1;
end
if (pdf:add_text(nil, "Page One", 10, 20, 30,rgbToHex({0xff, 0, 0}) ) ) then
  return -1;
end
if (pdf:add_bookmark(nil, -1, "Another Page") ) then
  return -1;
end

--if ( not pdf:get_err() ) then
    --return -1;
--end
pdf:clear_err()


pdf:set_font("Times-BoldItalic");
pdf:append_page();
local first_page = pdf:get_page(1);
PDF_ALIGN_JUSTIFY=3 ;

local height = pdf:add_text_wrap( nil,
'This is a great big long string that I hope will wrap properly '
..'around several lines.\nThere are some odd length '
..'linesthatincludelongwords to check the justification. '
..'I\'ve put some embedded line breaks in to '
..'see how it copes with them. Hopefully it all works properly.\n\n\n'
..'We even include multiple breaks\n'
..'And special stuff €ÜŽžŠšÁ that áüöä should ÄÜÖß— “”‘’ break\n'
..'————————————————————————————————————————————————\n'
..'thisisanenourmouswordthatwillneverfitandwillhavetobecut',
16, 60, 800, 0, rgbToHex({0, 0, 0}), 300, PDF_ALIGN_JUSTIFY
);

pdf:add_rectangle(nil, 58, 800 + 16, 304, -height, 2,rgbToHex({0, 0, 0}));
pdf:add_image_file(nil, 10, 10, 20, 30, "data/teapot.ppm");
pdf:add_image_file(nil, 50, 10, 30, 30, "data/coal.png");
pdf:add_image_file(nil, 100, 10, 30, 30, "data/bee.bmp");
pdf:add_image_file(nil, 150, 10, 30, 30, "data/bee-32-flip.bmp");
pdf:add_image_file(nil, 150, 50, 50, 150, "data/grey.jpg");
pdf:add_image_file(nil, 200, 50, 50, -1, "data/bee.pgm");
pdf:add_image_file(nil, 400, 100, 100, 100, "data/grey.png");
pdf:add_image_file(nil, 400, 210, 100, 100, "data/indexed.png");
pdf:add_image_file(nil, 100, 500, 50, 150, "data/penguin.jpg");

pdf:add_text(nil, "Page One", 10, 20, 30, rgbToHex({0xff, 0, 0}));
pdf:add_text(nil, "PjGQji", 18, 20, 130, rgbToHex({0, 0xff, 0xff}));
pdf:add_line(nil, 10, 24, 100, 24, 4, rgbToHex({0xff, 0, 0}));

pdf:add_rectangle(nil, 150, 150, 100, 100, 4, rgbToHex({0, 0, 0xff}));
pdf:add_filled_rectangle(nil, 150, 450, 100, 100, 4,rgbToHex({0, 0xff, 0}), '0x78000000');
pdf:add_text(nil, "", 20, 20, 30, rgbToHex({0, 0, 0}));
pdf:add_text(nil, "Date (YYYY-MM-DD):", 20, 220, 30,rgbToHex({0, 0, 0}));
pdf:add_bookmark(nil, -1, "First page");


pdf:append_page(pdf);
second_page = pdf:get_page(2);

--pdf:add_text(second_page, "Page Two", 10, 20, 30, rgbToHex({0, 0, 0}));
pdf:add_text(second_page, "Page Two", 10, 20, 30, pdfgen.rgb(255, 0, 181));

pdf:add_text(nil, "This is some weird text () \\ # : - Wi-Fi 27°C",10, 50, 60, rgbToHex({0, 0, 0}));
pdf:add_text(
    nil,
    "Control characters ( ) < > [ ] { } / % \n \r \t \b \f ending", 10,
    50, 45, rgbToHex({0, 0, 0})
);
pdf:add_text(
  nil, "Special characters: €ÜŽžŠšÁáüöäÄÜÖß—“”‘’Æ", 10,
  50, 15, rgbToHex({0, 0, 0})
);
pdf:add_text(
  nil, "This one has a new line in it\nThere it was", 10,
  50, 80, rgbToHex({0, 0, 0})
);


pdf:add_text(
  nil,
  "This is a really long line that will go off the edge of the screen,\n\n\n "..
  "because it is so long. I like long text. The quick brown fox jumped\n\n\n "..
  "over the lazy dog. The quick brown fox jumped over the lazy dog\n\n\n",
  10, 100, 100, rgbToHex({0, 0, 0},300,3)
);

pdf:set_font("Helvetica-Bold");
pdf:add_text(
  nil,
  "This is a really long line that will go off the edge of the screen, "..
  "because it is so long. I like long text. The quick brown fox jumped "..
  "over the lazy dog. The quick brown fox jumped over the lazy dog",
  10, 100, 130, rgbToHex({0, 0, 0})
);

--pdf:set_font("ZapfDingbats");
pdf:set_font("Helvetica-Bold");
pdf:add_text(
  nil,
  "This is a really long line that will go off the edge of the screen, "..
  "because it is so long. I like long text. The quick brown fox jumped "..
  "over the lazy dog. The quick brown fox jumped over the lazy dog",
  10, 100, 150, rgbToHex({0, 0, 0})
);


local PDF_BARCODE_128A  = 0 --< Produce code-128A style barcodes
local PDF_BARCODE_39    = 1 --< Produce code-39 style barcodes
local PDF_BARCODE_EAN13 = 2 --< Produce EAN-13 style barcodes
local PDF_BARCODE_UPCA  = 3 --< Produce UPC-A style barcodes
local PDF_BARCODE_EAN8  = 4 --< Produce EAN-8 style barcodes
local PDF_BARCODE_UPCE  = 5 --< Produce UPC-E style barcodes

pdf:add_barcode(nil, PDF_BARCODE_39,
  pdf:mm_to_point(20),
  pdf:mm_to_point(240),
  pdf:mm_to_point(60),
  pdf:mm_to_point(20),
  "CODE39",rgbToHex({0, 0, 0})
);

pdf:add_barcode(nil, PDF_BARCODE_128A,
  pdf:mm_to_point(20),
  pdf:mm_to_point(210),
  pdf:mm_to_point(60),
  pdf:mm_to_point(20),
  "Code128", rgbToHex({0, 0, 0})
);
pdf:add_barcode(nil, PDF_BARCODE_UPCA,
  pdf:mm_to_point(100),
  pdf:mm_to_point(160),
  pdf:mm_to_point(60),
  pdf:mm_to_point(80),
  "003994155480", rgbToHex({0, 0, 0})
)

pdf:add_barcode(nil, PDF_BARCODE_EAN13,
  pdf:mm_to_point(20),
  pdf:mm_to_point(160),
  pdf:mm_to_point(60),
  pdf:mm_to_point(40),
  "4003994155486", rgbToHex({0, 0, 0})
);

pdf:set_font("Courier-Bold");
pdf:add_text(
  nil, "(5.6.5) RS232 shutdown", 8, 317, 546,
  rgbToHex({0, 0, 0})
);
pdf:add_text(nil, "", 8, 437, 546, rgbToHex({0, 0, 0}));
pdf:add_text(nil, "Pass", 8, 567, 556, rgbToHex({0, 0, 0}));
pdf:add_text(nil, "(5.6.3) RS485 pins", 8, 317, 556,rgbToHex({0, 0, 0}));

bm = pdf:add_bookmark(nil, -1, "Another Page");
bm = pdf:add_bookmark(nil, bm, "Another Page again");
pdf:add_bookmark(nil, bm, "A child page");
pdf:add_bookmark(nil, bm, "Another child page");
pdf:add_bookmark(nil, -1, "Top level again");


pdf:append_page();
pdf:set_font("Times-Roman");
local ai = 0
for i=0, 3000 do
  local xpos = math.floor(i / 100.0)*40.0
  local ypos = (i % 100.0)*10.0
  --print(xpos, ypos)
  --pdf:add_text(nil, "Text blob", 8, xpos, ypos,
    --rgbToHex({i, (i * 4) & 0xff, (i * 8) & 0xff})
  --);
  pdf:add_text(nil, "Text blob", 8, xpos, ypos,
    pdfgen.rgb(i, (i * 4) & 0xff, (i * 8) & 0xff)
  );
  ai=i
end

--print(ai)
pdf:add_text(nil,
  "Vitronic", 10, (ai / 100) * 100, (ai % 100) * 12, rgbToHex({0xff, 0, 0})
);
pdf:add_bookmark(nil, -1, "tercera page");

print(pdf:save('main.pdf'))

print(pdf:mm_to_point(100))

print(pdfgen.rgb(0xff, 0, 0))

pdf:destroy()
