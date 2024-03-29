Short:        Edit and view Markdown documents  
Uploader:     tyrrell.simon@gmail.com (Simon Tyrrell)  
Author:       tyrrell.simon@gmail.com (Simon Tyrrell), Martin Mitas  
Type:         text/edit  
Version:      1.2
Architecture: ppc-amigaos >= 4.1  
Distribution: Aminet  
Requires: dev/mui/MCC_TextEditor, dev/mui/MCC_BetterString, dev/mui/MCC_TheBar
Requires: dev/c/jansson_library.lha, net/misc/curl.lha 

This is a tool to allow you to edit and view Markdown documents. It uses a 
customised version of md4c at https://github.com/mity/md4c by Martin Mitáš 
available to do the actual conversion of Markdown to HTML.

##�Installation

You need to install the bug-fixed version of the MUI HTMLView library courtesy of Edgar 
Schwan. You also need to have an up to date version of curl in your default path.

## Usage:

The program can either be started from a Shell or from Workbench. 

### Shell Arguments

Earmark has the arguments template  SETTINGS/K,FILENAME

 * **SETTINGS**: Set this to the path of the settings file that you wish to 
 load when the program starts. 
 * **FILENAME**: This is the initial markdown file to load into the editor.
 
 
### Tooltypes

The icon can take a single tooltype:
 
 * **SETTINGS**: Set this to the path of the settings file that you wish to 
 load when the program starts.

### Program layout

The window consists of two pages; an Editor page and a Settings page.

### Editor:

The Editor page consists of a tool bar and a text editor gadget which is where
 the Markdown source code will be shown. The text editor is a subclass of the 
 [text editor custom class](https://github.com/amiga-mui/texteditor) so all of 
 its key shortcuts are available too.

#### Toolbar

 * **Load**: Load a Markdown file. Alternatively you can either drag and drop 
 a markdown file onto the window.
 * **Save**: Save the contents of the text editor to a file.
 * **Convert**: Convert to HTML and view in a browser. This will save a new 
 file with ".html" appended to the name of the markdown file and open it in a 
 browser.
 * **Undo**: Undo the latest changes.
 * **Redo**: Redo the latest reverted changes.
 * **Cut**: Cut the selected text to the clipboard.
 * **Copy**: Copy the selected text to the clipboard.
 * **Paste**: Paste the selected text from the clipboard. 
 * **Bold**: Make the selected text bold.
 * **Italic**: Make the selected text italic.
 * **Strikethrough**: Strike through the selected text.
 * **Code**: Make the selected text code.
 * **Search**: Search within the text. This will open a new window where you 
 can enter a search string and choose where to search from within the 
 editor, whether to do case sensitive search and whether to search forwards
 or backwards.
 * **Horizontal Rule**: Insert a Horizontal Rule.
 * **Fenced Code**: Make a fenced code block.
 * **Blockquote**: This will either insert a blockquote or if you have some 
 selected text it will insert a blockquote marker at the start of each of the 
 selected lines.
 * **Hyperlink**: Insert a hyperlink. This will open a new window where you 
 can enter the link text and address. 
 * **Image**: Insert an image. This will open a new window where you can 
 enter the image path, its alternative text and an optional title. 
 * **Table**: Insert a table. This will open a new window where you can enter 
 the number of rows and columns that you require. Also there is an 
 *Alignments* string gadget where you can specify the alignment for each 
 column. 
 By default, each column is left-aligned, however you can alter that here. The 
 string here will be read where the first character defines the alignment for 
 the first column, the second character for the second column, *etc.* 
    * *l* will make the column left-aligned, 
    * *r* will make the column right-aligned
    * *c* will make the column centre-aligned.

If *Alignments* is empty or has fewer characters than the number of 
specified  columns, the remaining columns will default to being left-aligned. 
For example *crlc* would specify that the first four columns of the table 
would be centre-, right-, left- and centre-aligned respectively.
 
At the bottom of this page is an information bar which shows the following
information:

 * **Row**: Current row that the cursor is on.
 * **Column**: Current column that the cursor is on.
 * **Changed**: A checkmark that will be ticked if the file is changed from 
 its last saved position.

 
### Settings:

On this page you can configure some of the options for converting the md4c 
Markdown to HTML. For more information on these, view the 
[md4c documentation](https://github.com/mity/md4c).You can use the 
*Save Settings* and *Load Settings* buttons to save and load any customised 
conversion settings if you wish. 


### Menu

The menu options mostly duplicate the options available on the toolbar. There
are however some exceptions:

 * **Edit - Heading**: This submenu has the options for inserting a heading 
 tag into the document. These range from H1, H2 through to H6 with their 
 shortuct keys being 1, 2, through to 6.
 * **View - Toolbar rows**: This submenu allows you to specify the number
 of rows that the toolbar will use. This can be useful if you find that the
 window is too wide for your display.
 * **View - Internal**: This menuitem can be toggled on and off. If it is on, 
 then the preview pane in the right-hand area of the application will be used
 to display the HTML that is generated from the Markdown source. If this is off,
 then the OS Launch Handler will be used instead where you can choose a 
 fully-fledged browser to use.


## Future:

 * Getting the buttons to become selected for given bits of text, *e.g.* the 
 bold button being highlighted when the cursor is in a piece of bold 
 text, *etc.* 
 * Syntax highlighting would be nice too. 
 * Adding more elements to the md4c Markdown to HTML conversion code such as 
 definition lists, footnotes, *etc.* This would then get added to md4c via pull 
 requests.
 * Get the *Convert* function to re-use an existing browser window rather than 
 launch a new one each time.

Any other ideas are welcome. :-)

## Changes

### 1.2 - (28/07/2021)
 * Added the option to use the application HTML viewer window to display the 
 generated HTML.


### 1.1 - (19/01/2021)
 * Added command line arguments and icon tooltypes.
 * Set a default name for saved settings files. Also added a tooltype where 
 the user can set the default config file. (@walkero)
 * Added option for setting the number of rows that the toolbar spans so
 the window can require less width (@capehill).
 * Added shortcuts for inserting any of the H1, H2, ..., H6 tags into the
 editor.
 * Added Edit and View menus including option for changing the number of rows
 in the toolbar to reduce the minimum window width if needed (@capehill)
 * Added status bar gadget for showing current cursor position and whether the
 file has been changed. (@walkero)
 * Menu images added. 
 * BUGFIX: The convert button had the same shortcut as the Hyperlink button 
(@walkero)
 * BUGFIX: Fenced code blocks were automatically adding opening and closing 
braces (@walkero)
 * BUGFIX: The table button bubble had a semicolon after the shortcut, 
instead of a colon (@walkero)
 * BUGFIX: Menu Quit option was not working (@Capehill)
 * BUGFIX: If you marked a line, then chose bold, italic, *etc.* and that
 line started/ended with a space, then the markdown was broken. This was 
 because md4c included that space and the asterisks were staying as 
 characters. 

 *E.g.*

\*\*this is a broken bold line, not translated by md4c \*\* 

\*\* this is also a broken bold line, not translated by md4c\*\*

 (@walkero)
* BUGFIX: The filename for converting to html was getting ".html" 
 appended upon each conversion.

### 1.0 - (14/01/2021)
 * Added Blockquote function.
 * Added Search functionality.
 * Added Cut, Copy and Paste buttons.
 * Added shortcut for Update, now named Convert (@walkero).
 * Image, table and link editors now close when you click OK (@walkero).
 * Conversion settings can be saved/loaded (@walkero). 
 * The editor button shortcuts are now shown on the help bubbles as well 
 (@walkero).
 * Gladly accepted a much better name for the program :-) (@trixie).
 * BUGFIX: Choosing the Github dialect conversion setting was not being set.
 * BUGFIX: New projects would not save the converted html and launch the 
 preview. (@walkero).
 * BUGFIX: Load and Save menu options had the same shortcut (@walkero). 
 * BUGFIX: "Allow blocks" and "Allow spans" had the same help bubble text
 (@walkero).
  
### 0.8 - (10/01/2021)
 * Moved to using TheBar to give a graphical toolbar along with many more 
 functions.
 * Added Undo, Redo, Strikethrough, Code, Fenced Code Blocks, Horizontal 
 Rule, Table, Image and Link functions.
	
### 0.5 - (07/01/2021):
 * Changed from using HTMLview to using Launch-Handler which comes with OS4.
 * Added the settings page to allow configuration of the Markdown to HTML 
 conversion settings. 
 * Added a (very!) :-)) rudimentary set of buttons on the toolbar to allow the 
 setting of bold, italic and code sections. 
 * Added a test Markdown file.
 * Bug fixes.

### 0.1 - (05/01/2021):
 * Initial release

### Thanks

Thanks to all of the people who have given suggestions and feedback at amigans.net.
Special thanks got to @walkero, @trixie and @kas1e for their testing and feedback. 
Major thanks to Edgar Schwan for his bug-fixed version of the MUI HTMLView class and 
his tips on how I could I use it.


## License:

The code is available under the MIT license. The md4c parts are copyright 
Martin Mitas. The program icon and emoticon_bigsmile image are copyright to 
Martin Merz, go to http://www.masonicons.info for more information and to 
get his awesome artwork. The rest is copyright (c) 2021 by Simon Tyrrell. 


 
