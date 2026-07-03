#!/usr/bin/perl -w
#  $Id: testperl.cgi,v 1.3 2024/05/21 14:39:10 bshan Exp $
#use Gtk;
# test any perl text in conn with db
use strict;


use lib::RemoteClient;


my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";


#activate CGI

my $html=new RemoteClient(1);


#get number of used hosts for each site
my $ronly=1;
# my $ok=$html->ConnectOnlyDB($ronly);
 my $ok=$html->Connect($ronly);
 $html->CheckFS(1);
#  $html->TestPerl(18921);  
 



