#!/usr/bin/perl -w
#  $Id: rc.o.cgi,v 1.5 2024/05/21 14:39:10 bshan Exp $
#use Gtk;
use strict;

use lib qw(.);
use lib::RemoteClient;
if($ENV{MOD_PERL}){
  $#ARGV=-1;
}


my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";
unshift @ARGV, "-N/cgi-bin/mon/rc.o.cgi";


#activate CGI

my $html=new RemoteClient();


#get all info from the db

 my $ok=$html->Connect();

 
#work
 
if (not $ok){
  $html->Warning();
}



