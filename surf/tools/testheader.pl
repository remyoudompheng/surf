#!/usr/bin/perl -w

use strict;
use Term::ANSIColor;
$|=1;

my $header;
while ($header = shift) {
    print $header . "\n";
    check($header);
}


sub check()
{
    my $header = shift;
    open FILE, ">testheader_dummy.cc" or die "Can't create cc file";
    print FILE <<END;
#include "$header"
END
    close FILE;
    compile ("testheader_dummy.cc");
 
    open FILE, ">testheader_dummy.cc" or die "Can't create cc file";
    print FILE <<END;
#include "$header"
#include "$header"
END
    close FILE;
    compile ("testheader_dummy.cc");
    
}

sub compile ()
{
    my $command = 'c++ -DHAVE_EXPLICIT_TEMPLATE_INSTANTIATION=1 -DSTDC_HEADERS=1 -DYYTEXT_POINTER=1 -DGMP2_PREFIX=1  -I. -I../image-formats  -I../src -I../drawfunc -I../yaccsrc -I../image-formats -I../curve -I../gtkgui -I../mt -I../draw -I../misc -I../debug -I../xpm  -D_REENTRANT  -fno-rtti -fno-exceptions -Wall -W -Wwrite-strings -Wpointer-arith -Wconversion -Wno-unused -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Woverloaded-virtual -Wnested-externs -I/usr/X11R6/include -I/usr/local/lib/glib/include -I/usr/local/include  -I/usr/X11R6/include -c testheader_dummy.cc';
#    $command = 'c++';
#    print "executing: $command\n";
    print color("red");
    my $retval = system ($command);
    print color("reset");
    unlink ("testheader_dummy.cc");
    unlink ("testheader_dummy.o");

    return $retval;
}
