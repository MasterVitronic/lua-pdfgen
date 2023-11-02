/*

  lua-pdfgen.c - Lua bindings to PDFgen

  Copyright (c) 2021, Díaz Devera Víctor <mastervitronic@gmail.com>

  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

/***
 * Lua bindings to PDFgen
 *
 * This documentation is partial, and doesn't cover all functionality yet.
 * @module pdfgen
 * @author Díaz Devera Víctor (Máster Vitronic) <mastervitronic@gmail.com>
 */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "compat.h"
#include "pdfgen.h"

#define PDFGEN "PDFGEN"

typedef struct ctx_t{
  lua_State *L ;
  struct pdf_doc *pdf;
  struct pdf_info info;
} ctx_t;

static ctx_t * ctx_check(lua_State *L, int i) {
	return (ctx_t *) luaL_checkudata(L, i, PDFGEN);
}

/**
 * Initializes the library
 * @function new
 * @treturn object the object pdfgen
 */
static int l_new (lua_State *L) {
  ctx_t *ctx = (ctx_t *)lua_newuserdata(L, sizeof(ctx_t));
  ctx->L = L;
  luaL_getmetatable(L, PDFGEN);
  lua_setmetatable(L, -2);
  return 1;
}

/**
 * Create a new PDF object, with the given page width/height
 * @function create
 * @param width Width of the page
 * @param height Height of the page
 * @param table info Optional information to be put into the PDF header
 */
static int l_pdf_create( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  char *creator  = "IspCore";
  char *producer = "IspCore";
  char *title    = "Default";
  char *author   = "IspCore";
  char *subject  = "Test";
  char *date     = "Default";
  float width    = luaL_checknumber(L, 2);
  float height   = luaL_checknumber(L, 3);

  if ( lua_istable(L, 4) ) {
    lua_getfield(L, 4, "creator");
      creator = (char *)luaL_checkstring(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 4, "producer");
      producer = (char *)luaL_checkstring(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 4, "title");
      title = (char *)luaL_checkstring(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 4, "author");
      author = (char *)luaL_checkstring(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 4, "subject");
      subject = (char *)luaL_checkstring(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, 4, "date");
      date = (char *)luaL_checkstring(L, -1);
    lua_pop(L, 1);
  }

  strcpy( ctx->info.creator, creator );
  strcpy( ctx->info.producer, producer );
  strcpy( ctx->info.title, title );
  strcpy( ctx->info.author, author );
  strcpy( ctx->info.subject, subject );
  strcpy( ctx->info.date, date );

  ctx->pdf = pdf_create(width,height, &ctx->info);

  luaL_getmetatable(L, PDFGEN);
  lua_setmetatable(L, -2);

  return 1;
}

/**
 * Sets the font to use for text objects. Default value is Times-Roman if
 * this function is not called.
 * Note: The font selection should be done before text is output,
 * and will remain until pdf_set_font is called again.
 * @function set_font
 * @param font New font to use. This must be one of the standard PDF fonts:
 *  Courier, Courier-Bold, Courier-BoldOblique, Courier-Oblique,
 *  Helvetica, Helvetica-Bold, Helvetica-BoldOblique, Helvetica-Oblique,
 *  Times-Roman, Times-Bold, Times-Italic, Times-BoldItalic,
 *  Symbol or ZapfDingbats
 * @treturn boolean false on failure, true on success
 */
static int l_pdf_set_font( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  char const  * font  = luaL_checkstring(L, 2);
  int result = pdf_set_font(ctx->pdf, font);
  if ( result < 0 ){
    lua_pushboolean(L, 0);
  }else{
    lua_pushboolean(L, 1);
  }

  return 1;
}

/**
 * Calculate the width of a given string in the current font
 * @function get_font_text_width
 * @param font_name Name of the font to get the width of.
 *  This must be one of the standard PDF fonts:
 *  Courier, Courier-Bold, Courier-BoldOblique, Courier-Oblique,
 *  Helvetica, Helvetica-Bold, Helvetica-BoldOblique, Helvetica-Oblique,
 *  Times-Roman, Times-Bold, Times-Italic, Times-BoldItalic,
 *  Symbol or ZapfDingbats
 * @param text Text to determine width of
 * @param size Size of the text, in points
 * @treturn mixed false on failure, text_width calculated width in
 */
static int l_pdf_get_font_text_width( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  const char *font_name = luaL_checkstring(L, 2);
  const char *text = luaL_checkstring(L, 3);
  float size = luaL_checknumber(L, 4);
  float text_width  ;

  int result = pdf_get_font_text_width(
    ctx->pdf, font_name, text, size, &text_width
  );

  if ( result < 0 ){
    lua_pushboolean(L, 0);
  }else{
    lua_pushnumber(L, text_width);
  }

  return 1;
}

/**
 * Add a text string to the document
 * @function add_text
 * @param page Page to add object to (NULL => most recently added page)
 * @param text String to display
 * @param size Point size of the font
 * @param xoff X location to put it in
 * @param yoff Y location to put it in
 * @param colour Colour to draw the text
 * @treturn boolean true on success, false on failure
 */
static int l_pdf_add_text( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  struct pdf_object *page = NULL;
  if ( lua_islightuserdata(L, 2) ){
    page = lua_touserdata(L, 2);
  }
  char const  *text  = luaL_checkstring(L, 3);
  float size  = luaL_checknumber(L, 4);
  float xoff  = luaL_checknumber(L, 5);
  float yoff  = luaL_checknumber(L, 6);
  uint32_t colour= luaL_checknumber(L, 7);

  int result = pdf_add_text(ctx->pdf,page,text,size,xoff,yoff,colour);
  if ( result == 0 ){
    lua_pushboolean(L, 1);
  }else{
    lua_pushboolean(L, 0);
  }

  return 1;
}

/**
 * Add an outline rectangle to the document
 * @function add_rectangle
 * @param page Page to add object to (NULL => most recently added page)
 * @param x X offset to start rectangle at
 * @param y Y offset to start rectangle at
 * @param width Width of rectangle
 * @param height Height of rectangle
 * @param border_width Width of rectangle border
 * @param colour Colour to draw the rectangle
 * @treturn boolean true success, false on failure
 */
static int l_pdf_add_rectangle( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  struct pdf_object *page = NULL;
  if ( lua_islightuserdata(L, 2) ){
    page = lua_touserdata(L, 2);
  }
  float xoff  = luaL_checknumber(L, 3);
  float yoff  = luaL_checknumber(L, 4);
  float width  = luaL_checknumber(L, 5);
  float height  = luaL_checknumber(L, 6);
  float border_width  = luaL_checknumber(L, 7);
  uint32_t colour= luaL_checknumber(L, 8);

  int result = pdf_add_rectangle(
    ctx->pdf,page,xoff,yoff,width,height,border_width,colour
  );
  if ( result == 0 ){
    lua_pushboolean(L, 1);
  }else{
    lua_pushboolean(L, 0);
  }

  return 1;
}

/**
 * Add a filled rectangle to the document
 * @function add_filled_rectangle 
 * @param page Page to add object to (NULL => most recently added page)
 * @param x X offset to start rectangle at
 * @param y Y offset to start rectangle at
 * @param width Width of rectangle
 * @param height Height of rectangle
 * @param border_width Width of rectangle border
 * @param colour_fill Colour to fill the rectangle
 * @param colour_border Colour to draw the rectangle
 * @treturn boolean true success, false on failure
 */
static int l_pdf_add_filled_rectangle( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  struct pdf_object *page = NULL;
  if ( lua_islightuserdata(L, 2) ){
    page = lua_touserdata(L, 2);
  }
  float xoff  = luaL_checknumber(L, 3);
  float yoff  = luaL_checknumber(L, 4);
  float width  = luaL_checknumber(L, 5);
  float height  = luaL_checknumber(L, 6);
  float border_width  = luaL_checknumber(L, 7);
  uint32_t colour_fill= luaL_checknumber(L, 8);
  uint32_t colour_border= luaL_checknumber(L, 9);

  int result = pdf_add_filled_rectangle(
    ctx->pdf,page,xoff,yoff,width,height,
    border_width,colour_fill,colour_border
  );
  if ( result == 0 ){
    lua_pushboolean(L, 1);
  }else{
    lua_pushboolean(L, 0);
  }

  return 1;
}

/**
 * Add a line to the document
 * @function add_line 
 * @param page Page to add object to (NULL => most recently added page)
 * @param x1 X offset of start of line
 * @param y1 Y offset of start of line
 * @param x2 X offset of end of line
 * @param y2 Y offset of end of line
 * @param width Width of the line
 * @param colour Colour to draw the line
 * @treturn boolean true success, false on failure
 */
static int l_pdf_add_line( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  struct pdf_object *page = NULL;
  if ( lua_islightuserdata(L, 2) ){
    page = lua_touserdata(L, 2);
  }
  float x1  = luaL_checknumber(L, 3);
  float y1  = luaL_checknumber(L, 4);
  float x2  = luaL_checknumber(L, 5);
  float y2  = luaL_checknumber(L, 6);
  float width  = luaL_checknumber(L, 7);
  uint32_t colour= luaL_checknumber(L, 8);

  int result = pdf_add_line(
    ctx->pdf,page,x1,y1,x2,y2,
    width,colour
  );
  if ( result == 0 ){
    lua_pushboolean(L, 1);
  }else{
    lua_pushboolean(L, 0);
  }

  return 1;
}

/**
 * Add an image file as an image to the document.
 * Passing 0 for either the display width or height will
 * include the image but not render it visible.
 * Passing a negative number either the display height or width will
 * have the image be resized while keeping the original aspect ratio.
 * Supports image formats: JPEG, PNG, PPM, PGM & BMP
 * @function add_image_file 
 * @param page Page to add image to (NULL => most recently added page)
 * @param x X offset to put image at
 * @param y Y offset to put image at
 * @param display_width Displayed width of image
 * @param display_height Displayed height of image
 * @param image_filename Filename of image file to display
 * @treturn bbolean false on failure, true on success
 */
static int l_pdf_add_image_file( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  struct pdf_object *page = NULL;
  if ( lua_islightuserdata(L, 2) ){
    page = lua_touserdata(L, 2);
  }
  float x  = luaL_checknumber(L, 3);
  float y  = luaL_checknumber(L, 4);
  float display_width  = luaL_checknumber(L, 5);
  float display_height  = luaL_checknumber(L, 6);
  const char *image_filename  = luaL_checkstring(L, 7);

  int result = pdf_add_image_file(
    ctx->pdf,page,x,y,display_width,
    display_height,image_filename
  );
  if ( result < 0 ){
    lua_pushboolean(L, 0);
  }else if (result >= 0){
    lua_pushboolean(L, 1);
  }

  return 1;
}

/**
 * Add a bookmark to the document
 * @function add_bookmark 
 * @param page Page to jump to for bookmark
 * (or NULL for the most recently added page)
 * @param parent ID of a previously created bookmark that is the parent
 * of this one. -1 if this should be a top-level bookmark.
 * @param name String to associate with the bookmark
 * @return false on failure, new bookmark id on success
 */
static int l_pdf_add_bookmark( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  struct pdf_object *page = NULL;
  if ( lua_islightuserdata(L, 2) ){
    page = lua_touserdata(L, 2);
  }
  int parent  = luaL_checkinteger(L, 3);
  const char *name  = luaL_checkstring(L, 4);

  int result = pdf_add_bookmark(
    ctx->pdf,page,parent,name
  );
  if ( result < 0 ){
    lua_pushboolean(L, 0);
  }else{
    lua_pushinteger(L, result);
  }

  return 1;
}

/***
 * Add a link annotation to the document
 * @function add_link 
 * @param page Page that holds the clickable rectangle
 * (or NULL for the most recently added page)
 * @param x X coordinate of bottom LHS corner of clickable rectangle
 * @param y Y coordinate of bottom LHS corner of clickable rectangle
 * @param width width of clickable rectangle
 * @param height height of clickable rectangle
 * @param target_page Page to jump to for link
 * @param target_x X coordinate to position at the left of the view
 * @param target_y Y coordinate to position at the top of the view
 * @treturn boolean false on failure, new bookmark id on success
 */
static int l_pdf_add_link( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  struct pdf_object *page = NULL;
  if ( lua_islightuserdata(L, 2) ){
    page = lua_touserdata(L, 2);
  }
  float x  = luaL_checknumber(L, 3);
  float y  = luaL_checknumber(L, 4);
  float width  = luaL_checknumber(L, 5);
  float height  = luaL_checknumber(L, 6);
  struct pdf_object *target_page = lua_touserdata(L, 7);
  float target_x  = luaL_checknumber(L, 8);
  float target_y  = luaL_checknumber(L, 9);

 int result = pdf_add_link(
    ctx->pdf, page, x, y,
    width,height, target_page,
    target_x,target_y
  );

  if ( result < 0 ){
    lua_pushboolean(L, 0);
  }else{
    lua_pushinteger(L, result);
  }

  return 1;
}

/**
 * Add a new page to the given pdf
 * @return NULL on failure, new page object on success
 */
static int l_pdf_append_page( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  struct pdf_object *page = pdf_append_page(ctx->pdf);
  if ( !page ){
    lua_pushnil(L);
  }else{
    lua_pushlightuserdata(L, page);
  }

  return 1;
}

/**
 * Retrieve a page by its number.
 * @function get_page
 * Note: The page must have already been created via \ref pdf_append_page
 * @param page_number Page number to retrieve, starting from 1.
 * @return Page object if the given page is found, NULL otherwise
 */
static int l_pdf_get_page( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  int page_number  = luaL_checkinteger(L, 2);
  struct pdf_object *result = pdf_get_page(ctx->pdf,page_number);
  if ( !result ){
    lua_pushnil(L);
  }else{
    lua_pushlightuserdata(L, result);
  }

  return 1;
}

/***
 * Adjust the width/height of a specific page
 * @function page_set_size
 * @param page object returned from @ref pdf_append_page
 * @param width Width of the page in points
 * @param height Height of the page in points
 * @treturn boolean false on failure, true on success
 */
static int l_pdf_page_set_size( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  struct pdf_object *page = NULL;
  if ( lua_islightuserdata(L, 2) ){
    page = lua_touserdata(L, 2);
  }
  float width   = luaL_checknumber(L, 3);
  float height  = luaL_checknumber(L, 4);

  int result = pdf_page_set_size(
    ctx->pdf, page, width, height
  );

  if ( result < 0 ){
    lua_pushboolean(L, 0);
  }else if (result >= 0) {
    lua_pushboolean(L, 1);
  }

  return 1;
}

/**
 * Add a text string to the document, making it wrap if it is too long
 * @function add_text_wrap
 * @param page Page to add object to (NULL => most recently added page)
 * @param text String to display
 * @param size Point size of the font
 * @param xoff X location to put it in
 * @param yoff Y location to put it in
 * @param angle Rotation angle of text (in radians)
 * @param colour Colour to draw the text
 * @param wrap_width Width at which to wrap the text
 * @param align Text alignment (see PDF_ALIGN_xxx)
 * @treturn mixed false on failure, the final height on success
 */
static int l_pdf_add_text_wrap( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  struct pdf_object *page = NULL;
  if ( lua_islightuserdata(L, 2) ){
    page = lua_touserdata(L, 2);
  }
  const char *text = luaL_checkstring(L, 3);
  float size       = luaL_checknumber(L, 4);
  float xoff       = luaL_checknumber(L, 5);
  float yoff       = luaL_checknumber(L, 6);
  float angle      = luaL_checknumber(L, 7);
  uint32_t colour  = luaL_checknumber(L, 8);
  float wrap_width = luaL_checknumber(L, 9);
  int align        = luaL_checkinteger(L, 10);
  float height;

  int result = pdf_add_text_wrap(
    ctx->pdf,page,text,size,xoff,yoff,
    angle,colour,wrap_width,align,&height
  );

  if ( result < 0 ){
    lua_pushboolean(L, 0);
  }else if (result >= 0) {
    lua_pushnumber(L, height);
  }

  return 1;
}

/**
 * Add a text string to the document at a rotated angle
 * @function add_text_rotate
 * @param page Page to add object to (NULL => most recently added page)
 * @param text String to display
 * @param size Point size of the font
 * @param xoff X location to put it in
 * @param yoff Y location to put it in
 * @param angle Rotation angle of text (in radians)
 * @param colour Colour to draw the text
 * @treturn boolean true on success, false on failure
 */
static int l_pdf_add_text_rotate( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  struct pdf_object *page = NULL;
  if ( lua_islightuserdata(L, 2) ){
    page = lua_touserdata(L, 2);
  }
  const char *text = luaL_checkstring(L, 3);
  float size       = luaL_checknumber(L, 4);
  float xoff       = luaL_checknumber(L, 5);
  float yoff       = luaL_checknumber(L, 6);
  float angle      = luaL_checknumber(L, 7);
  uint32_t colour  = luaL_checknumber(L, 8);

  int result = pdf_add_text_rotate(
    ctx->pdf,page,text,size,xoff,yoff,
    angle,colour
  );

  if ( result < 0 ){
    lua_pushboolean(L, 0);
  }else if (result == 0) {
    lua_pushboolean(L, 1);
  }

  return 1;
}

/**
 * Save the given pdf document to the supplied filename.
 * @function save
 * @param filename Name of the file to store the PDF into (NULL for stdout)
 * @treturn boolean false on failure, true on success
 */
static int l_pdf_save( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  char const  * filename = NULL;
  if ( lua_isstring(L, 2) ) {
    filename  = luaL_checkstring(L, 2);
  }
  int result = pdf_save(ctx->pdf, filename);
  if ( result >= 0 ){
    lua_pushboolean(L, 1);
  }else{
    lua_pushboolean(L, 0);
  }
  return 1;
}

/**
 * Retrieves a PDF document height
 * @function height
 * @treturn number height of PDF document (in points)
 */
static int l_pdf_height( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  lua_pushnumber(L, pdf_height(ctx->pdf));

  return 1;
}

/**
 * Retrieves a PDF document width
 * @function width
 * @treturn number width of PDF document (in points)
 */
static int l_pdf_width( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  lua_pushnumber(L, pdf_width(ctx->pdf));

  return 1;
}

/**
 * Retrieves page height
 * @function page_height
 * @param page Page object to get height of
 * @treturn number height of page (in points)
 */
static int l_pdf_page_height( lua_State * L ) {
  struct pdf_object *page = lua_touserdata(L, 2);
  lua_pushinteger(L, pdf_page_height(page) );
  return 1;
}

/**
 * Retrieves page width
 * @function page_width
 * @param page Page object to get width of
 * @treturn number width of page (in points)
 */
static int l_pdf_page_width( lua_State * L ) {
  struct pdf_object *page = lua_touserdata(L, 2);
  lua_pushinteger(L, pdf_page_width(page));
  return 1;
}

/**
 * Add a barcode to the document
 * @function add_barcode
 * @param page Page to add barcode to (NULL => most recently added page)
 * @param code Type of barcode to add (PDF_BARCODE_xxx)
 * @param x X offset to put barcode at
 * @param y Y offset to put barcode at
 * @param width Width of barcode
 * @param height Height of barcode
 * @param string Barcode contents
 * @param colour Colour to draw barcode
 * @return false on failure, true on success
 */
static int l_pdf_add_barcode( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  struct pdf_object *page = NULL;
  if ( lua_islightuserdata(L, 2) ){
    page = lua_touserdata(L, 2);
  }
  int code      = luaL_checkinteger(L, 3);
  float x       = luaL_checknumber(L, 4);
  float y       = luaL_checknumber(L, 5);
  float width   = luaL_checknumber(L, 6);
  float height  = luaL_checknumber(L, 7);
  const char *string = luaL_checkstring(L, 8);
  uint32_t colour  = luaL_checknumber(L, 9);

  int result = pdf_add_barcode(ctx->pdf,page,code,
    x, y, width, height,string, colour
  );

  if ( result >= 0 ){
    lua_pushboolean(L, 1);
  }else{
    lua_pushboolean(L, 0);
  }

  return 1;
}


/**
 * Retrieve the error message if any operation fails
 * @function get_err
 * @param errval optional pointer to an integer to be set to the error code
 * @treturn mixed NULL if no error message, string description of error otherwise
 */
static int l_pdf_get_err( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  int *errval  = (int *)luaL_checkinteger(L, 2);
  const char *err = pdf_get_err(ctx->pdf, errval);
  if ( err ){
    luaL_error(L, err);
  }else{
    lua_pushnil(L);
  }

  return 1;
}

/**
 * Convert a value in milli-meters into a number of points.
 * @function mm_to_point
 * @param mm millimeter value to convert to points
 * @treturn number 
 */
static int l_pdf_mm_to_point( lua_State * L ) {
  int mm = luaL_checkinteger(L, 2);
  lua_pushnumber(L, PDF_MM_TO_POINT(mm));

  return 1;
}

/**
 * Convert a value in inches into a number of points.
 * @function inch_to_point
 * @param inch inches value to convert to points
 * @treturn number 
 */
static int l_pdf_inch_to_point( lua_State * L ) {
  int inch = luaL_checkinteger(L, 2);
  lua_pushnumber(L, PDF_INCH_TO_POINT(inch));

  return 1;
}

/**
 * Convert three 8-bit RGB values into a single packed 32-bit
 * colour. These 32-bit colours are used by various functions
 * in PDFGen
 * @function rgb
*/
static int l_pdf_rgb( lua_State * L ) {
  int r   = luaL_checkinteger(L, 1);
  int g   = luaL_checkinteger(L, 2);
  int b   = luaL_checkinteger(L, 3);
  lua_pushnumber(L, PDF_RGB(r, g, b));

  return 1;
}

/**
 * Convert four 8-bit ARGB values into a single packed 32-bit
 * colour. These 32-bit colours are used by various functions
 * in PDFGen. Alpha values range from 0 (opaque) to 0xff
 * (transparent)
 * @function argb
 */
static int l_pdf_argb( lua_State * L ) {
  int a   = luaL_checkinteger(L, 1);
  int r   = luaL_checkinteger(L, 2);
  int g   = luaL_checkinteger(L, 3);
  int b   = luaL_checkinteger(L, 4);
  lua_pushnumber(L, PDF_ARGB(a, r, g, b));

  return 1;
}

/**
 * Acknowledge an outstanding pdf error
 * @function clear_err
 */
static int l_pdf_clear_err( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  pdf_clear_err(ctx->pdf);
  return 1;
}

/**
 * Destroy the pdf object, and all of its associated memory
 * @function destroy
 */
static int l_pdf_destroy( lua_State * L ) {
  ctx_t *ctx = ctx_check(L, 1);
  pdf_destroy(ctx->pdf);

  /* remove all methods operating on ctx */
	lua_newtable(L);
	lua_setmetatable(L, -2);

  return 1;
}

static const struct luaL_Reg funcs [] = {
  {"new", l_new},
  {"rgb", l_pdf_rgb},
  {"argb", l_pdf_argb},
  {NULL, NULL}
};

static const struct luaL_Reg meths [] = {
  {"create", l_pdf_create},
  {"set_font", l_pdf_set_font},
  {"append_page", l_pdf_append_page},
  {"add_text", l_pdf_add_text},
  {"add_rectangle", l_pdf_add_rectangle},
  {"add_image_file", l_pdf_add_image_file},
  {"height", l_pdf_height},
  {"width", l_pdf_width},
  {"page_width", l_pdf_page_width},
  {"page_height", l_pdf_page_height},
  {"add_bookmark", l_pdf_add_bookmark},
  {"add_link", l_pdf_add_link},
  {"get_page", l_pdf_get_page},
  {"page_set_size", l_pdf_page_set_size},
  {"add_text_wrap", l_pdf_add_text_wrap},
  {"add_text_rotate", l_pdf_add_text_rotate},
  {"add_filled_rectangle", l_pdf_add_filled_rectangle},
  {"get_font_text_width", l_pdf_get_font_text_width},
  {"add_line", l_pdf_add_line},
  {"save", l_pdf_save},
  {"get_err", l_pdf_get_err},
  {"mm_to_point", l_pdf_mm_to_point},
  {"inch_to_point", l_pdf_inch_to_point},
  {"add_barcode", l_pdf_add_barcode},
  {"clear_err", l_pdf_clear_err},
  {"destroy", l_pdf_destroy},
  {NULL, NULL}
};

int luaopen_pdfgen (lua_State *L) {
  luaL_newmetatable(L, PDFGEN);
  lua_pushvalue(L, -1);

  lua_setfield(L, -2, "__index");
  luaL_setfuncs(L, meths, 0);

  luaL_newlib(L, funcs);

  return 1;
}
