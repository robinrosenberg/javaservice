--------------------------------------------------------------------
   README File
   Mathieu.Peltier <mathieu.peltier@objectweb.org>   
--------------------------------------------------------------------

--------------------------------------------------------------------
Introduction 
--------------------------------------------------------------------
This document explains how to build an ObjectWeb Project Web site with
a little XML common framework (Ant targets and XSLT stylesheets). The
latest version can be found at:
http://www.objectweb.org/wws/d_read/webmaster/public/web.tgz

This framework is used by some ObjectWeb projects (all needed files
are in general managed by CVS and kept in a web directory in the root
of the CVS repository).

The XSLT stylesheets used are inspired by the jakarta-site2
(http://jakarta.apache.org/site/jakarta-site2.html) stylesheets. The
DTD used is inspired by the Cocoon (http://xml.apache.org/cocoon/)
documentation DTD.

External libraries used:
Xerces XML Parser (http://xml.apache.org/xerces-j/)
Xalan XSLT processor (http://xml.apache.org/xalan-j/)

--------------------------------------------------------------------
Presentation
--------------------------------------------------------------------

Directory Organization
----------------------
	
The Web site files are included in general in a directory called
web. This directory contains the following files:
	
  - /build.xml: Ant build.xml file.
  - /build.properties: Ant properties file.
  - /common_web.zip: contains all commun files (see below).
  - /lib/: contains all necessarily jars file.
  - /xml/: contains the XML source files, in particular the menu file
    (/project/project.xml).
  - /html/: contains the static HTML files, images, etc.
  - /cgi-bin/: contains the cgi-bin scripts.
	
The common_web.zip archive contains:

  - /dtd/: contains the DTD files.
  - /js/: contains the common JavaScript files.
  - /stylesheets/: contains the XSLT stylesheets.
  - /images/: contains the common images.

ObjectWeb DTD
-------------
	
The XML files must be conform to a simple DTD (see the
common/dtd/objectweb.dtd file). Where possible, great care has been
used to reutilize HTML tag names to reduce learning efforts and to
allow HTML editors to be used for complex authorings like tables and
lists. Please refer to the DTD file if you are confused. If you use
(X)Emacs, you should install the powerful PSGML mode.

The XML files must contain a properties section, where some meta
properties are defined (document title, author(s), filename and path
to root project). The body section can then contain one or several
sections, subsections and subtitles (s1, s2 and subtitle tags).
	
The following tags have the same signification than the HTML tags
(with some restrictions for the attributes):

   - paragraph tags: <p> (normal paragraph),

   - table tags: <table>, <tr> (row tag), <th> (row header) and <td>
   (row description tag),

   - lists tags: <ol> (ordered list), <ul> (unordered list) and <li>
   (list item),

   - common tag: <em> (emphasis, typically italic), <strong> (strong,
   typically bold), <img> tag (image), <code> (typically monospaced),
   <sup> (typically smaller and higher), <inf> (typically smaller and
   lower), <hr> (horizontal rule) and <br> (forced line break).
	
Sections and subsections can contains several paragraphs which are
defined using the classic tag p. You can also insert source code
thanks to the new tag source and copyright disclaimer using the tag
copyright. Be careful, if you are embedding other XML/XHTML markup
within source tag, then you need to place a CDATA section within the
source section. For example:
	
<p><![CDATA[ XML/XHTML content here ]]></p>
	
The hyperlinks tags are a bit special. The anchor tag is the
equivalent of <a name=""> HTML tag. The <a> tag is used to insert a
hard replacing link (equivalent of <a href=""> that can be external or
internal depending of the value of the fork attribute. The <connect>
tag defines a soft link between processed pages (no equivalent in
HTML).
		
The ObjectWeb DTD contains also a minimal support of forms. In
general, forms must be formatted manually (within tables).

The Project File
----------------
	
Each menu is defined in the "project file" stored in the project
directory (in general project/project.xml). The DTD used
(common/dtd/project.dtd) is quite simple. First the project title must
be defined thanks to a <a>, <connect> or <legend> tag. Each menu tag
can contain several submenu: use for these the <a> or <connect> tags
included in a <menuitem> tag (same signification as above).

You can also defined in this files a short description of your project
and a keywords list. These informations will be used to set meta-tags
in the HTML files.

--------------------------------------------------------------------
Building your Web site
--------------------------------------------------------------------

Prerequisites: a JVM, Ant (http://jakarta.apache.org/ant/)

You can download the project skeleton provided at
http://www.objectweb.org/wws/d_read/webmaster/public/web.tgz
To see the available targets of the Ant build file, type:

 ant -projecthelp

To build the Web site, just run: 

 ant 

By default, this will generate the WWW files in the
web/dist/public_html and web/dist/cgi-bin directories. You can modify
this using the Ant property file (web/build.properties). Then you
should able to modify this template site without problem.

--------------------------------------------------------------------
IMPORTANT NOTE
--------------------------------------------------------------------

If you want to use this framework to build your ObjectWeb project,
please do not modify the common_web.zip archive (this file can be
modify by ObjectWeb webmasters). The latest version can be found at:
http://www.objectweb.org/wws/d_read/webmaster/public/common_web.zip
