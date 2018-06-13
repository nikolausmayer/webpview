
webpview
========
<b>webpview</b> is a viewer for WebP images.

Requirements
============
* autoconf (req'd by WebP)
* automake (req'd by WebP)
* libtools (req'd by WebP)
* libX11-dev (req'd by CImg)

Known issues
============
* crashes if window is resized
* is slow (single-threaded WebP decoding)
* might not work if multiple images with different sizes are loaded
* `¯\_(ツ)_/¯`

License
=======
The webpview program core is under MIT license. [CImg](http://cimg.eu/)is under[CeCILL-C](Licence_CeCILL-C_V1-en.txt) license (open source, LGPL-like). WebP (downloaded during build) is under [WebM's](https://www.webmproject.org/license/software/) license.

