#! /usr/bin/perl -w

# This is a little tool to change the header (copyright notice) in
# every source file. It will add a header to files which have none.
# The new header is loaded from "header.txt".
# example usage: header.pl */*.cc */*.h */*.l */*.y

use strict;

my $name = "header.txt";
open (NEWHEADERFILE, "<$name") or die "Can't open headerfile \"$name\":$!";
my @newheader = <NEWHEADERFILE>;
close NEWHEADERFILE or die "Can´t close headerfile \"$name\":$!";
my $newheader = join ("", @newheader);

processFiles (@ARGV);

sub processFiles
{
    my $string =  "visualizing algebraic curves and algebraic surfaces";
    my $name;
    while ($name = shift) {
	print <STDERR>, "Processing $name...";
	open (FILE, "<$name") || die "Can't open file $name:$!";
	my @lines = <FILE>;
	close(FILE) || die "Can't close file";


	my $tmpnam = $name . ".newheader";
	open (NEW, ">$tmpnam") || die "Can't open file $tmpnam";


	my $contents = join("", @lines);
	if ($contents =~ s#(/\*(?:(?!\*/).)*$string.*?\*/)#$newheader#s) {
	    print NEW $contents;
	} else {
	    print NEW $newheader . "\n" . $contents;
	}

	close NEW or die "Can´t close \"$tmpnam\":$!";
	rename ("$name", "$name.bak") or die "Can´t rename file:$!";
	rename ("$tmpnam", "$name") or die "Can´t rename file:$!";
	print "okay\n";
    }
}
