#!/usr/bin/perl -w

# $Id: mdocu.pl,v 1.3 2002/11/25 06:08:32 markus Exp $

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
        $purpose =~ s#^//[^:]+:\s*(.*)\n*#$1#;

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
                if (m!^//\s*$!) { $$lastVar .= "\n"; last READLINE; }
                s#^//\s*(.*)#$1#;
                $$lastVar .= $_; next READLINE;
            }
            s#^//[^:]+:\s*(.*)#$1#;
            $$lastVar .= $_;
        }

        # Now skip over the trailing comment line (if available) and start
        # start parsing with what should be the function name
        $_ = <>;
        $_ = <> if m!/\*-*\*/!;
        my $fnc = $_;

        # ... and continue unt an open curly brace or colon
        while (!/\{|:/) { $fnc .= $_; print "Parsed: $fnc\n"; $_ = <>; }

        # Strip the part after the brace or the colon
        $fnc =~ s/\s\s+/ /g;
        $fnc =~ /(.*\s|^)(\w*::|^|\s)\s*([^\s]*)([^:\{\n]*)/g;

        # Print purpose and parameters
        print ("    <pre><a name=\"$3\"></a>$1<b>$3</b>$4</pre>\n");
        print ("    <dl>\n");
        print ("      <dd><p>$purpose</p></dd>\n");
        my @params;
        if ($values{'Parameters'}) {
            print ("      <dd><dl><dt><b>Parameters</b></dt>\n");
            foreach (split /\n+/, $values{'Parameters'}) {
                /(\w+)(.*)/g;
                print ("          <dd><code>$1</code>$2<dd>\n");
                $params[++$#params] = $1;
            }
            print ("      </dl><dd>\n");
        }

        # Print out the rest of the stuff
        foreach my $i ('Returns', 'Requires', 'Throws') {
            if ($values{$i}) {
                print ("      <dd><dl><dt><b>$i</b></dt>\n");
                foreach (@params) {
                    $values{$i} =~ s/(.*)($_)(.*)/$1<code>$2<\/code>$3/g; }
                print ("          <dd>$values{$i}</dd></dl></dd>\n");
            }
        }
        print ("    </dl>\n");
        %values = ();
    }
}

END { close (STDOUT); }
