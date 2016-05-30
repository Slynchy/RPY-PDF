#RPY-PDF Changelog

#### v0.7.0
- Removed PDF support
	* Tough decision, but it makes the program much easier to compile, doesn't require any external libs, and the outputted PDFs didn't work anyway.
- Now outputs a .txt file for further formatting.
	* As well as another .txt file containing all the "garbage" strings, so you can see if strings were removed when they shouldn't be.
- Bug/Todo list:
	* Needs a configurable chapter/act divider?
	* Try compiling to Linux

#### v0.6.0
- Added unicode support
	* Still dislikes ellipses, but accentuations work now.
- Can now output to .TXT as well as .PDF
- PDF support complete and will be unsupported in future versions.
	* This is because I started this project to read RPY games on Kindle, but PDF doesn't work well on Kindle for non-graphical content.
- Software now entirely customizable via configurable text files.
- Bug list:
	* No distinguishment between scenes/chapters
	* Haven't checked to see what strings are removed, in case some important ones are.

#### v0.5.0
- First commit
- Bug list:
	* No configuration possible to the page layout
		+ i.e. still has hard-coded fonts, etc.
	* Probably more; can't remember.