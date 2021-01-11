Short:        Edit and view Markdown documents  
Uploader:     tyrrell.simon@gmail.com (Simon Tyrrell)  
Author:       tyrrell.simon@gmail.com (Simon Tyrrell)  
Type:         text/edit  
Version:      0.8  
Architecture: ppc-amigaos >= 4.1  
Distribution: Aminet  
Requires: dev/mui/MCC_TextEditor, dev/mui/MCC_BetterString, dev/mui/MCC_TheBar,  Launch-Handler  

This is a tool to allow you to edit and view Markdown documents.  It uses a customised version of 
md4c at https://github.com/mity/md4c by Martin Mitáš available to do the actual conversion of 
Markdown to HTML.

## USAGE:

The window consists of two pages; an Editor page and a Settings page.

### Editor:

The Editor page consists of a tool bar and a text editor gadget which is where the Markdown source
code will be shown. The text editor is a subclass of the [text editor custom class](https://github.com/amiga-mui/texteditor) so all of its key shortcuts are available too.

#### Toolbar

 * **Load**: Load a Markdown file. Alternatively you can either drag and drop a markdown file onto the window.
 * **Save**: Save the contents of the text editor to a file.
 * **Convert**: Convert to HTML and view in a browser. This will save a new file with ".html" appended to the name of the markdown file and open it in a browser.
 * **Undo**: Undo the latest changes.
 * **Redo**: Redo the latest reverted changes.
 * **Bold**: Make the selected text bold.
 * **Italic**: Make the selected text italic.
 * **Strikethrough**: Strike through the selected text.
 * **Code**: Make the selected text code.
 * **Horizontal Rule**: Insert a Horizontal Rule.
 * **Fenced Code**: Make a fenced code block.
 * **Hyperlink**: Insert a hyperlink. This will open a new window where you can enter the link text and address. 
 * **Image**: Insert an image. This will open a new window where you can enter the image path, its alternative text and an optional title. 
 * **Table**: Insert a table. This will open a new window where you can enter the number of rows and columns that you require. Also there is an *Alignments* string gadget where you can specify the alignment for each column. 
By default, each column is left-aligned, however you can alter that here. The string here will be read where the first character defines the alignment for the first column, the second character for the second column, *etc.* 
    * *l* will make the column left-aligned, 
    * *r* will make the column right-aligned
    * *c* will make the column centre-aligned.

If the *Alignments* is empty or has fewer characters than the number of specified columns, the remaining columns will default to being left-aligned. For example *crlc* would specify that the first four columns of the table would be centre-, right-, left- and centre-aligned respectively.
 
### Settings:

On this page you can configure some of the options for converting the md4c Markdown to HTML. For more information on these, view the [md4c documentation](https://github.com/mity/md4c) 

## FUTURE:

 * Getting the buttons to become selected for given bits of text, e.g. the bold button being highlighted when the cursor is in a piece of bold text, etc. 
 * Syntax highlighting would be nice too. 
 * Adding more elements to the md4c Markdown to HTML conversion code such as definition lists, footnotes, *etc.* This would then get added to md4c via pull requests.
 * The ability to save and load customised conversion settings.
 * Get the *Convert* function to re-use an existing browser window rather than launch a new one each time.

Any other ideas are welcome. :-)

## CHANGELOG

### 0.9
 * Added Blockquote function

### 0.8 - (10/01/2021)
 * Moved to using TheBar to give a graphical toolbar along with many more functions.
 * Added Undo, Redo, Strikethrough, Code, Fenced Code Blocks, Horizontal Rule, Table, Image and Link functions
	
### 0.5 - (07/01/2021):
 * Changed from using HTMLview to using Launch-Handler which comes with OS4.
 * Added the settings page to allow configuration of the Markdown to HTML conversion settings. 
 * Added a (very!) :-)) rudimentary set of buttons on the toolbar to allow the setting of bold, italic and code sections. 
 * Added a test Markdown file.
 * Bug fixes.

### 0.1 - (05/01/2021):
 * Initial release

## LICENSE:

The code is available under the MIT license. The md4c parts are copyright Martin Mitáš and the rest 
is copyright (c) 2021  by Simon Tyrrell. The emoticon_bigsmile image is copyright to Martin Merz, 
go to http://www.masonicons.info for more information and to get his awesome artwork.


 
