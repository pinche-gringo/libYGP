#!/usr/bin/perl -w

# $Id: mgeni.pl,v 1.3 2005/01/10 23:21:37 markus Exp $

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

mgeni - Generates metainfo for (certain marked) attributes in a class.

=head1 SYNOPSIS

  mgeni.pl [OPTIONS]

=head1 OPTIONS

  --with-get, -g ... Generate get-methods for the attributes
  --with-set, -s ... Generate set-methods for the attributes
  --verbose, -v .... Be verbose
  --version, -V .... Print version and exit
  --help, -?, -h ... Show this help and exit

=head1 DESCRIPTION

Script to generate metainfo out of (specially marked) attributes in an
YGP::Entity-class.

The information is used to add the names of those attributes into the
attribute-list of YGP::Entry and for initializing those attributes with
default values.

Additionally get- and/or set-methods for those attributes can be genereated.

=cut


package main;
use strict;
use Pod::Usage;
use Getopt::Long;

my $verbose=0;
my $help=0;
my $version=0;
my $with_get=0;
my $with_set=0;

Getopt::Long::Configure ("bundling", "gnu_getopt", "no_ignore_case");
GetOptions ('verbose|v+' => \$verbose, 'version|V' => \$version,
            'help|h|?' => \$help, 'with-get|g' => \$with_get,
	    'with-set|s' => \$with_set) or pod2usage(2);

pod2usage (1) if ($help);

if ($version) {
    $0 =~ s!.*/(.*)!$1!;
    my $rev = '$Revision: 1.3 $';
    $rev =~ s/\$(\w+:\s+\d+\.\d+).*\$.*/$1/;
    print "$0 - V0.1.00     ($rev)\n";
    print "Author: Markus Schwab; e-mail: g17m0\@lycos.com\n\n",
          "Distributed under the terms of the GNU General Public License\n" if ($verbose);
    exit (1);
}

my $inComment = 0;
my $foundClass = 0;
$, = '';
$\ = '';
my $firstVal = 1;
my $pos = 0;
my $end;

my $FS = ' ';		# set field separator
my @Fld;
my $endPos;
my $i;
my $className='';
my $type;
my $attr;
my $following='';
my $name;
my $value='';
my $getline_ok;
my $fncs="\n";
my $nextValue='';
my $lastChar='';

line: while (<>) {
    chomp;	# strip record separator
    @Fld = split($FS, $_, 9999);
    if ($foundClass == 0) {
	if ($inComment == 1) {
	    $pos = index($_, '*/');
	    if ($pos == -1) {
		next line;
	    }
	    else {
		$inComment = 0;
		$_ = substr($_, $pos + 1);
	    }
	}

	$pos = index($_, '/*');
	if ($pos != -1) {
	    $endPos = index($_, '*/');
	    if ($endPos > $pos) {	#???
		$end = substr($_, $endPos + 1);
		$_ = substr($_, 0, $pos - 1);
		$_ = $_ . $Fld[$end];
	    }
	    else {
		$inComment = 1;
		next line;
	    }
	}

	$pos = index($_, '//');
	if ($pos != -1) {
	    $_ = substr($_, 0, $pos);
	}
	if ($_ =~ /class/) {
	    for ($i = 0; $i < $#Fld; $i++) {
		if ($Fld[$i] =~ /class/) {
		    $className = $Fld[$i + 1];
		    $nextValue = ($#Fld > ($i + 1)) ? $Fld[$i + 2] : '';
		    last;
		}
	    }
	    if ($className eq '') {
		do {
		    $_ = &Getline0 ();
		} while ($_ eq '');
		$className = $Fld[1];
		$nextValue = ($#Fld > 1) ? $Fld[2] : '';
	    }
	    while ($nextValue eq '') {
		$nextValue = &Getline0 ();
	    }

	    $lastChar = substr ($className, -1);
	    if (($lastChar ne '{') && ($lastChar ne ':') && ($lastChar ne ';')) {
		$lastChar = substr ($nextValue, 0, 1);
	    }

	    if (($lastChar eq '{') || ($lastChar eq ':')) {
		$foundClass = 1;

		print $className, '::', $className, " ()\n";
		$FS = ";[ \t]*";
	    }
	}
	else {
	    next line;
	}
    }
    next line if ($#Fld == -1);

    if ($_ =~ /(\/\/|\/*)[[:blank:]]*%attrib%/) {
	$type = $Fld[0];
	$name = ($#Fld > 1) ? $Fld[2] : "";
	$value = ($#Fld > 2) ? $Fld[3] : "";
	$type =~ s/^[[:blank:]]*//;
	$attr = $type;
	$attr =~ s/.*[[:blank:]]//;
	$type =~ s/[[:blank:]]+[^[:blank:]]*$//;

	if ($value ne '') {
	    print '   ', (($firstVal == 1) ? ':' : ','), ' ', $attr, ' (',
		   $value, ")\n";
	    $firstVal = 0;
	}
	if ($name ne '') {
	    $following = $following . '   addAttribute (*new YGP::Attribute<'
	      . $type . "> (\"" . $name . "\", " . $attr . "));\n";

	    my $upperAttr = uc (substr ($attr, 0, 1)) . substr ($attr, 1);
	    if ($with_get == 1) {
		$fncs = "$fncs\n$type " . $className . "::get$upperAttr () const {\n"
		    . "   return $attr; }\n"
	    }

	    if ($with_set == 1) {
		$fncs = "$fncs\nvoid " . $className . "::set$upperAttr (const $type& value) {\n"
		    . "   $attr = value; }\n"
	    }
	}
    }
}

if ($foundClass == 1) {
    print "{\n", $following, "}\n\n";
    print $className, '::~', $className, " () {\n}\n";
    print $fncs;
}

sub Getline0 {
    if ($getline_ok = (($_ = <>) ne '')) {
	chomp;	# strip record separator
	@Fld = split($FS, $_, 9999);
    }
    $_;
}
