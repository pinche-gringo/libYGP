#!/usr/bin/perl -w

# $Id: mdocu.pl,v 1.11 2003/06/19 03:34:18 markus Rel $

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

  --verbose, -v .... Be verbose (changes also output of --version)
  --version, -V .... Print version and exit
  --help, -?, -h ... Show this help and exit

=head1 ARGUMENTS

  sourcefile ... Sources to inspect; if missing read from stdin

=head1 DESCRIPTION

Script to extract the function description out of a (C++) source. Those
descriptions are identified by a C++ comment starting directly with
B<Purpose:> (no whitespaces in between the comment characters and the keyword).

Then the next lines starting with the C++ comment characters are checked for
the keywords B<Parameters>, B<Returns>, B<Requires> and/or B<Throws>.

Their respective contents is printed out in a HTML format. Note that uppercase
words in the purpose and parameter section are replaced by their equivalent
bold lowercase version.

Furthermore every parameter needs to be described in one line, as must every
requirement.

=cut


package main;
use strict;
use Pod::Usage;
use Getopt::Long;

my $verbose=0;
my $help=0;
my $version=0;

# Make some conversions for HTML
sub convertToHTML ($) {
    if ($_[0]) {
        $_[0] =~ s/&/&amp;/g;
        $_[0] =~ s/</&lt;/g;
        $_[0] =~ s/>/&gt;/g;
        $_[0] =~ s/\"/&quot;/g;
    }
    return $_[0]
}

Getopt::Long::Configure ("bundling", "gnu_getopt", "no_ignore_case");
Getopt::Long::Configure ("prefix_pattern=(--|-)");
GetOptions ('verbose|v+' => \$verbose, 'version|V' => \$version,
            'help|h|?' => \$help) or pod2usage(2);

pod2usage (1) if ($help);

if ($version) {
    $0 =~ s!.*/(.*)!$1!;
    my $rev = '$Revision: 1.11 $ ';
    $rev =~ s/\$(\w+:\s+\d+\.\d+).*\$.*/$1/;
    print "$0 - V0.3.00     ($rev)\n";
    print "Author: Markus Schwab; e-mail: g17m0\@lycos.com\n\n",
          "Distributed under the terms of the GNU General Public License\n" if ($verbose);
    exit (1);
}

my $purpose;
my %values;

while (<>) {
    if (m!^//Purpose *:!) {
        $purpose = $_;
        $purpose =~ s#^//[^:]+:\s*(.*)#$1#;

        my $lastVar = \$purpose;
        # Read next lines til end of header
      READLINE:
        while (<>) {
            last if !m!^//!;
            # Check for other values or if previous continues
            {
                if (m!^//Parameters *:!) {$lastVar = \$values{'Parameters'}; last; }
                if (m!^//Returns *:!) {$lastVar = \$values{'Returns'}; last; }
                if (m!^//Requires *:!) {$lastVar = \$values{'Requires'}; last; }
                if (m!^//Remarks *:!) {$lastVar = \$values{'Remarks'}; last; }
                if (m!^//Throws *:!) {$lastVar = \$values{'Throws'}; last; }
                if (m!^//\s*$!) { $$lastVar .= "\n"; next READLINE; }
                s#^//\s*(.*)#$1#;
                $$lastVar .= "$_"; next READLINE;
            }
            s#^//[^:]*:\s*(.*)#$1#;
            $$lastVar .= $_;
        }

        # Now skip over the trailing comment line (if available) and start
        # start parsing with what should be the function name
        $_ = <>;
        $_ = <> if m!/\*-*\*/!;
        my $fnc = $_;

        # ... and continue until an open curly brace or a single colon
        while (!/\{|[^:]:[^:]/) { $_ = <>; $fnc .= $_; }

        # Replace whitespaces with a single space and strip the part after
        # the brace or the colon
        $fnc =~ s/\s+/ /g;
        $fnc =~ s/(.*?)\s*\{.*/$1/;
        $fnc =~ s/(.*?)\s*[^:]:[^:].*/$1/;

        # Now separate the function in its subparts
        # type [class::]name rest
        my $type;
        my $name;
        my $rest;
        if ($fnc =~ /([\w\d_]+::.+\()/) {
           ($type, $name, $rest) = ($fnc =~ /(.*?)\s*[_\w\d]+\s*::\s*([\w\d_~=\-\*\/\+]+)\s*(\(.*)/); }
        else {
           ($type, $name, $rest) = ($fnc =~ /(.+?)\s+([\w\d_]+|operator[-+*\/])\s*(\(.*)/); }
        # $fnc =~ /([^\s]*\s+|^)(\w+::)?([^\s\(]+)(.*)/;

        convertToHTML ($type);
        convertToHTML ($name);
        convertToHTML ($rest);

        # Print purpose and parameters
        convertToHTML ($purpose);

        # Convert uppercase words in the description into <b>word</b>
        $purpose =~ s/([[:upper:]][[:upper:]]+)/<b>\L$1\E<\/b>/g;

        # Exchange double \n's with paragraphs in the purpose
        $purpose =~ s#\n\n\s*#</p>\n        </p>#g;
        # ... and later single \n's with \n and identation
        $purpose =~ s/\n([^\s])/\n          $1/g;
        $purpose =~ s/\s*$//g;

        print ("    <pre><a name=\"$name\"></a>$type <b>$name</b> $rest</pre>\n");
        print ("    <dl>\n");
        print ("      <dd><p>$purpose</p></dd>\n");
        my @params;
        if ($values{'Parameters'}) {
           $values{'Parameters'} =~ s/\s*$//g;
           print ("      <dd><dl><dt><b>Parameters</b></dt>\n");
           my $follow = 0;
           foreach (split /\n+/, $values{'Parameters'}) {
               convertToHTML ($_);

                if (/^\w+:/) {
                  /^(\w+)(.*)/;
                  my $var = $1;
                  my $desc = $2;

                  # Convert uppercase words in the parameter description into <b>word</b>
                  $desc =~ s/ ([[:upper:]][[:upper:]]+) / <b>\L$1\E<\/b> /g;

                  print ("</dd>\n") if ($follow);
                  $follow = 1;

                  print ("          <dd><code>$var</code>$desc");
                  @params[@params] = $1;
                }
                else {
                  print ("\n            $_");
                }
            }
            print ("</dd></dl></dd>\n");
        }

        # Print out the rest of the stuff
        foreach my $i ('Returns', 'Requires', 'Remarks', 'Throws') {
            if ($values{$i}) {
                $values{$i} =~ s/\s*$//g;
                convertToHTML ($values{$i});
                print ("      <dd><dl><dt><b>$i</b></dt>\n");
                foreach my $p (@params) {
                    $values{$i} =~ s/\b($p)\b/<code>$1<\/code>/g; }
                print ("          <dd>$values{$i}</dd></dl></dd>\n");
            }
        }
        print ("    </dl>\n    <hr>\n");
        %values = ();
    }
}

END { close (STDOUT); }
