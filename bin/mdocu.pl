#!/usr/bin/perl -w

# $Id: mdocu.pl,v 1.2 2002/11/25 05:14:20 markus Exp $

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.


=head1 NAME

mdocu - Extracts the function comments out of a (C++) source

=head1 SYNOPSIS

  mdocu [OPTIONS] [sourcefile [...]]

=head1 OPTIONS

  --verbose, -v .... Be verbose
  --help, -?, -h ... Show this help and exit

=head1 ARGUMENTS

  sourcefile ... Sources to inspect; if missing read from stdin

=head1 DESCRIPTION

Script to extract the function description out of a C(++) source. Those
descriptions are identified by a C++ comment starting directly with
B<Purpose:> (no whitespaces in between the comment characters and the keyword).

Then the next lines starting with the C++ comment characters are checked for
the keywords B<Parameters>, B<Returns>, B<Requires> and/or B<Throws>.

Their respective contents is printed out in a HTML format.

=cut


package main;
use strict;
use Pod::Usage;
use Getopt::Long;

my $verbose=0;
my $help=0;

Getopt::Long::Configure ("bundling", "gnu_getopt", "no_ignore_case");
Getopt::Long::Configure ("prefix_pattern=(--|-)");
GetOptions ('verbose+' => \$verbose, 'help|h|?' => \$help) or pod2usage(2);

pod2usage (1) if ($help);

my $purpose;
my %values;

while (<>) {
    if (m!^//Purpose   :!) {
        $purpose = $_;
        $purpose =~ s#^//[^:]+:\s*(.*)#$1#;

        my $lastVar = \$purpose;
        # Read next lines til end of header
    READLINE:
        while (<>) {
            last if !m!^//!;
            # Check for other values or if previous continues
            {
                if (m!^//Parameters:!) {$lastVar = \$values{'Parameters'}; last; }
                if (m!^//Returns   :!) {$lastVar = \$values{'Returns'}; last; }
                if (m!^//Requires  :!) {$lastVar = \$values{'Requires'}; last; }
                if (m!^//Throws    :!) {$lastVar = \$values{'Throws'}; last; }
                if (m!^//\s*$!) { $$lastVar .= "<br>"; last READLINE; }
                s#^//\s*(.*)#$1#;
                $$lastVar .= $_; last;
            }
            s#^//[^:]+:\s*(.*)#$1#;
            $$lastVar .= $_;
        }

        # Now skip over trailing comment line
        $_ = <>;
        $_ = <> if m!/\*-*\*/!;
        my $fnc = $_;

        # ... and parse finally function name (until an open curly brace)
        while (!/\{|:/) { $fnc .= $_; print "Parsed: $fnc\n"; $_ = <>; }

        # Strip the part after the brace or a ':'
        print ("Fnc : $fnc");
        $fnc =~ s/\s\s+/ /g;
        $fnc =~ /(.*\s)*\w*::\s*([^\s]*)(.*)(\{|:|\n)/g;
        my $type = $1 ? $1 : "";
        print ("Split: ", $type, " - ", $2, " - ", $3, "\n");

        print ("    \<pre\>\<a name=\"$2\"\>\</a>$type\<b\>$2\</b\>$3\</pre\>");
        print $purpose;
        foreach (%values) { print ($_); }
        print ("\n\n");
        %values = ();
    }
}

END { close (STDOUT); }
