## Lua-pdfgen: Lua bindings for PDFgen

Lua-pdfgen is a Lua binding library for [PDFGen](https://github.com/AndreRenaud/PDFGen).

It runs on GNU/Linux and requires [Lua](http://www.lua.org/) (>=5.1)
and [PDFGen](https://github.com/AndreRenaud/PDFGen).

_Authored by:_ _[Díaz Devera Víctor Diex Gamar (Máster Vitronic)](https://www.linkedin.com/in/Master-Vitronic)_

[![Lua logo](./docs/powered-by-lua.gif)](http://www.lua.org/)

#### License

MIT license . See [LICENSE](./LICENSE).

#### Documentation

See the [Reference Manual](https://vitronic.gitlab.io/lua-pdfgen/).

#### Motivation:

I needed a fast, convenient and compatible way to create PDF files with the
most commonly used Spanish characters.


#### Getting and installing

```sh
$ git clone https://gitlab.com/vitronic/lua-pdfgen.git
$ cd lua-pdfgen
lua-pdfgen$ make
lua-pdfgen$ make install # or 'sudo make install' (Ubuntu)
```

#### Example

```lua
#!/usr/bin/env lua

local pdfgen = require("pdfgen")
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

```

The script can be executed at the shell prompt with the standard Lua interpreter:

```shell
$ lua test.lua
```

Other examples can be found in the **examples/** directory contained in the release package

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.
