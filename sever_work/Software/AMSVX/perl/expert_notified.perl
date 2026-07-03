#!/usr/bin/perl -w
#  $Id: checkfs.cgi,v 1.12 2011/03/30 12:48:35 dmitrif Exp 
use strict;
use Carp;
use lib qw(../perl);
use lib::DBSQLServer;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";

# DESCRIBE DATAFILES
# RUN                                       NOT NULL NUMBER(38)
# VERSION                                            VARCHAR2(64)
# TYPE                                               VARCHAR2(64)
# FEVENT                                             NUMBER(38)
# LEVENT                                             NUMBER(38)
# NEVENTS                                            NUMBER(38)
# NEVENTSERR                                         NUMBER(38)
# TIMESTAMP                                          NUMBER(38)
# SIZEMB                                             NUMBER(38)
# STATUS                                             VARCHAR2(64)
# PATH                                               VARCHAR2(255)
# PATHB                                              VARCHAR2(255)
# CRC                                                NUMBER(38)
# CRCTIME                                            NUMBER(38)
# CASTORTIME                                         NUMBER(38)
# BACKUPTIME                                         NUMBER(38)
# TAG                                                NUMBER(38)
# FETIME                                             NUMBER(38)
# LETIME                                             NUMBER(38)
# PATHS                                              VARCHAR2(255)

    my $expert_name = 'A.Eline';
    foreach my $chop  (@ARGV){
    if ($chop =~/^-n/){
        $expert_name=unpack("x2 A*",$chop);
    }
    }

    my $o=new DBSQLServer();
     my $ok=$o->Connect();
if($ok){
    my $sql_notified = 'select expert_notified, expert_name from filesprocessing';
    my $ret = $o->Query($sql_notified);
    my $last_notified_time = $ret->[0]->[0];
    my $last_expert_name = $ret->[0]->[1];
    my $time=time();
    if ($time - $last_notified_time < 3600 and $last_expert_name ne '') {
        if ($last_expert_name ne '' and not $last_expert_name =~ /$expert_name/ and not $expert_name =~ /$last_expert_name/) {
            $expert_name = "$last_expert_name $expert_name";
        }
        elsif ($last_expert_name =~ /$expert_name/) {
             $expert_name = $last_expert_name;
        }
    }
    my $sql="update filesprocessing set expert_notified=$time, expert_name='$expert_name'";
    $o->Update($sql);
     $o->Commit(1);
    print "OK\n";
}
else{
    print "UnableToConnectDB \n";
}
