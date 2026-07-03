#!/usr/local/bin/perl -w
use strict;
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
# CTATIME                                            NUMBER(38)
# BACKUPTIME                                         NUMBER(38)
# TAG                                                NUMBER(38)
# FETIME                                             NUMBER(38)
# LETIME                                             NUMBER(38)
# PATHS                                              VARCHAR2(255)

my $eos=0;
my $ds="pass4";
my $runmin=1374267279;
my $run2p="";
my $path=0;
my $timei=0;
my $runmax=2337449847;
my $NEW;
my $OLD;
my $EOSSELECT='eos';
my $eosprefix="root://eosams.cern.ch//eos";
my $cta_prefix='/eos/ctapublic/archive/ams';
my $eos_xrd_server='eosams.cern.ch';
my $eos_prefix='/eos/ams';
my $cta_xrd_server='eosctapublic.cern.ch';

foreach my $chop  (@ARGV){
    if ($chop =~/^-r/) {
        $run2p=unpack("x2 A*",$chop);
        $run2p=" and run in (".$run2p.")";
    }
    if ($chop =~/^-ds/) {
        $ds=unpack("x3 A*",$chop);
    }
    if ($chop =~/^-old/) {
        $OLD=unpack("x4 A*",$chop);
    }
    if ($chop =~/^-new/) {
        $NEW=unpack("x4 A*",$chop);
    }
    if ($chop =~/^-path/) {
        $path=1;
    }
    if ($chop =~/^-eos/) {
        $eos=1;
    }
    if ($chop =~/^-copyeos/) {
        $eos=2;
    }
    if ($chop =~/^-deleos/) {
        $eos=4;
    }
    if ($chop =~/^-copychangeeos/) {
        $eos=3;
    }
    if ($chop =~/^-time/) {
        $timei=1;
    }
    if ($chop =~/^-RMAX/) {
        my $rmax=unpack("x5 A*",$chop);
        $runmax=int($rmax);
    }
    if ($chop =~/^-RMIN/) {
        my $rmin=unpack("x5 A*",$chop);
        $runmin=int($rmin);
    }
}

if (not defined $NEW or $NEW eq '' or not defined $OLD or $OLD eq '') {
    print "Please specify the names before and after renaming by -old and -new.\n";
    exit -1;
}

my $OLDREG = $OLD;
$OLDREG =~ s/\./\\./g;

#print "$OLD, $NEW, $OLDREG, $runmin, $runmax, $ds\n";
my $o=new DBSQLServer();
my $ok=$o->Connect();
if($ok){
    my $time=time();
    if($run2p eq ""){
        $run2p="and run>=$runmin and run<$runmax ";
    }
    my $sql = "select count(run) from amsdes.ntuples where  path like '\%$OLD/$ds%' and path not like '$cta_prefix%' and path not like '/eosams/%' and CTATIME <= 0 $run2p";
    my $ret=$o->Query($sql);
    if($ret->[0][0]>0){
        print "Not CTA file exists $ret->[0][0] \n";
        $sql = "select count(run) from amsdes.ntuples where  path like '\%$OLD/$ds%' and path not like '/cta_prefix%' and ctatime=0 $run2p";
        $ret=$o->Query($sql);
        if($ret->[0][0]>0){
            print "Not backuped to CTA file exists $ret->[0][0] \n";

        }
        exit;
    }
    my $sqldid = "select distinct did from datasetsdesc where dataset = '$NEW'";
    my $retdid = $o->Query($sqldid);
    my $newdid = 0;
    if (defined $retdid->[0][0] and $retdid->[0][0]> 0) {
        $newdid = $retdid->[0][0];
    }
    else {
        print "$sqldid returned nothing.\n";
        exit;
    }

    if($eos){
        $sql = "select count(run) from amsdes.ntuples where  path like '\%$OLD/$ds%' and eostime=0 $run2p";
        $ret=$o->Query($sql);

        if($ret->[0][0]>0){
            print "Not backuped to eos file exists $ret->[0][0] \n";

            if($eos>1){
                $sql = "select path from amsdes.ntuples where  path like '/$cta_prefix\%$OLD/$ds%' and eostime=0 $run2p";
                $ret=$o->Query($sql);
                foreach my $file (@{$ret}){
                    my $stage="stager_get -M $file->[0]";
                    system($stage);
                    $stage="stager_qry -M $file->[0]";
                    system($stage);
                    my $fileinput =$file->[0];
                    my $eospath = $fileinput;
                    $eospath=~s?$cta_prefix?$eos_prefix?;
                    my $fileoutput=$fileinput;
                    $fileinput="root://$cta_xrd_server:/$fileinput";
                    $fileoutput=~s?$cta_xrd_server?$eos_xrd_server?;
                    $fileoutput=~s?$cta_prefix?$eos_prefix?;
                    my $xrdcp="xrdcp ".'"'.$fileinput.'"'." $fileoutput";
                    my $i_xrdcp=system($xrdcp);
                    if($i_xrdcp){
                        print " xrdcp error $xrdcp \n";
                        if($eos>2){
                            my $tapesize=`xrd $cta_xrd_server stat $file->[0] | awk '{print \$4}'`;
                            chomp($tapesize);
                            my $eossize=`xrd $eos_xrd_server stat $eospath | awk '{print \$4}'`;
                            chomp($eossize);
                            if($tapesize eq $eossize) {
                                goto seteos;
                            }
                        }
                    }
                    else{
seteos:
                        my $eostime=time();
                        $sql="update ntuples set eostime=$eostime where path like '$file->[0]'";
                        $o->Update($sql);
                        $o->Commit();
                    }

                }
            }
            exit;
        }
    }
    $sql = "select count(path) from amsdes.ntuples where  path like '\%$NEW/$ds%' and run in (select run from   ntuples   where  path like '\%$OLD/$ds%' and path  like '$cta_prefix%') $run2p";
    $ret=$o->Query($sql);
    if($ret->[0][0]>0){
        print "duplicated runs  exist $ret->[0][0] \n";
        exit;
    }

    print "#processing...\n";
    #$sql="select path,jid,eostime,ctatime from ntuples   where  path like '\%$OLD/$ds%' and (path  like '$cta_prefix%' or path like '/eosams/%') $run2p";
    $sql="select path,jid,eostime,ctatime from ntuples   where  path like '\%$OLD/$ds%' $run2p";
    $ret=$o->Query($sql);
    foreach my $file (@{$ret}){
        my $origpath=$file->[0];
        my $cta_path;
        my $eos_path;
        my $cta_newpath;
        if ($origpath =~ /\/eos\/ctapublic\//) {
            $cta_path = $origpath;
            $eos_path = $cta_path;
            $eos_path =~ s#$cta_prefix#$eos_prefix#g
        }
        elsif ($origpath =~ /\/eosams\//) {
            $eos_path = $origpath;
            $eos_path =~ s#/eosams/#/eos/ams/#g;
            $cta_path = $eos_path;
            $cta_path =~ s#$eos_prefix#$cta_prefix#g;
        }
        else {
            my $dest_path = $origpath;
            $dest_path =~ s/$OLDREG/$NEW/;
            my $filemove="mv -v $origpath $dest_path ";
            my $i_filemove = system($filemove);
            if ($i_filemove) {
                print "unable to move $filemove $i_filemove \n";
            }
            $eos_path = $origpath;
            $eos_path =~ s#.*/MC/#/eos/ams/MC/#;
            $eos_path =~ s#.*/Data/#/eos/ams/Data/#;
            $cta_path = $eos_path;
            $cta_path =~ s#$eos_prefix#$cta_prefix#g;
        }
        my $eos_filein=$eos_path;
        my $eos_fileout=$eos_path;
        if($file->[2]>0){
            $eos_fileout=~s/$OLDREG/$NEW/;
            my $eoscopy="$EOSSELECT file rename $eos_filein $eos_fileout ";
            my $i_eoscopy=system($eoscopy);
            if($i_eoscopy){
                print "unable to eoscopy $eoscopy $i_eoscopy \n";

                if($eos>3){
                    my $tapesize=`xrd $cta_xrd_server stat $cta_path | awk '{print \$4}'`;
                    chomp($tapesize);
                    my $eossize=`xrd $eos_xrd_server stat $eos_path | awk '{print \$4}'`;
                    chomp($eossize);
                    if($tapesize ne $eossize) {
                        my $rm="$EOSSELECT rm $eos_path";
                        my $ret = system($rm);
                        if($ret){
                            print "rm failed $rm $ret \n";
                        }
                        else{
                            $sql="update ntuples set eostime=0 where path like '$file->[0]'";
                            $o->Update($sql);
                            $o->Commit(1); 
                        }
                    }
                }

                next;
            }
        }
        my $newpath = $origpath;
        $newpath =~ s/$OLDREG/$NEW/;
        $cta_newpath = $cta_path;
        $cta_newpath=~s/$OLDREG/$NEW/;
        my $cmd ="xrdfs $cta_xrd_server mv $cta_path $cta_newpath";
        my $i=0;
        if($file->[3]>0){
            $i = system($cmd);
        }
        if($i==0){
            $sql="update jobs set did=$newdid where jid=$file->[1]";      
            $o->Update($sql);
            $sql="update ntuples set path='$newpath' where path='$origpath'";      
            $o->Update($sql);
            $o->Commit(1); 
        }
        else{
            if($file->[2]>0){
                my $eoscopy="$EOSSELECT file rename $eos_fileout $eos_filein ";
                my $i_eoscopy=system($eoscopy);
                if($i_eoscopy){
                    print "fatal-unable to eoscopy back $eoscopy $i_eoscopy \n";
                    exit;
                }
            }
        }
    }
}
