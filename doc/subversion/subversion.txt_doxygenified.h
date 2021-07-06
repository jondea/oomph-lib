/**

\mainpage Subversion

A read-only
<a href="http://subversion.tigris.org/"> subversion</a> 
repository of the oomph-lib project is hosted
at <A HREF="http://oomph-lib.maths.man.ac.uk">
oomph-lib.maths.man.ac.uk</A>. 
If <a href="http://subversion.tigris.org/"> subversion</a> is
installed on your computer you can obtain the latest 
snapshot of the project by issuing the command

\code
svn checkout svn://oomph-lib.maths.man.ac.uk/release/trunk
\endcode

Updates to the latest version can then be obtained by

\code
svn update
\endcode

The location of our subversion repository has changed, if you were
using the old location and you get an error that you can't connect
then (in the top-level  directory) please issue the command
\code
svn relocate svn://oomph-lib.maths.man.ac.uk/release/trunk
\endcode


Note that the source tree in the subversion repository is not a working
distribution and you must run

\code
./autogen.sh
\endcode

in order to build and install the library; see
the <a href="../../the_distribution/html/index.html">
installation instructions</a> for further details. 
In addition, you must have current versions of the autotools: 
<A HREF="http://www.gnu.org/software/automake/">automake</a>, 
<A HREF="http://www.gnu.org/software/autoconf/">autoconf</A> and 
<A HREF="http://www.gnu.org/software/libtool/">libtool</A>, 
installed on your machine. 

For information on how to obtain and use subversion 
please see the <a href="http://subversion.tigris.org/"> subversion web
site. </a> 

In addition, a browesable version of the repository is available at the
access-controlled page  
<a href="http://oomph-lib.maths.man.ac.uk/viewvc">
http://oomph-lib.maths.man.ac.uk/viewvc </a>.
 If you wish to obtain a
password for this page please send an e-mail to the usual address,
oomph-lib AT maths DOT man DOT ac DOT uk.


<HR>
\section problems Troubleshooting
If the response to the checkout or update commands is 
\code
svn: Can't connect to host 'oomph-lib.maths.man.ac.uk': Network unreachable
\endcode
despite the fact that your web browser is working, say, the likely problem 
is that your network commands are being intercepted by a proxy server, 
and you are using a version of the subversion software
that does not have proxy settings for the svn protocol.
(Subversion can also use the http and https protocols, but these are
not serviced by our repository. Therefore the response to the command
\code
svn checkout http://oomph-lib.maths.man.ac.uk/release/trunk 
\endcode
will be 
\code 
svn: Server sent unexpected return value (400 Bad Request) in response to OPTIONS request
for 'http://oomph-lib.maths.man.ac.uk/release/trunk'
\endcode

The solution is to install tsocks (if you don't have it already 
on your computer). If you are running Ubuntu or
another Debian derivative, the appropriate command is
\code
apt-get install tsocks
\endcode 
Edit the IP addresses in \c /etc/tsocks.conf to refer to your 
local domain and proxy server (you may need to ask
your local network administrator for information). The checkout is 
then effected by issuing the command
\code
tsocks svn checkout svn://oomph-lib.maths.man.ac.uk/release/trunk
\endcode 
and the update is done with the command
\code
tsocks svn update
\endcode
We are grateful to Chris Bertram from the University of Sydney
for this information.

<hr>
<hr>
\section pdf PDF file
A <a href="../latex/refman.pdf">pdf version</a> of this document is available.
**/

