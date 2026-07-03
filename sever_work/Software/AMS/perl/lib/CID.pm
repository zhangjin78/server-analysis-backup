#  $Id: CID.pm,v 1.9 2017/01/27 14:23:16 choutko Exp $
package CID;
@CID::EXPORT = qw(new);
sub new{
    my $class=shift;
    my $type=shift;
    my $self={
      HostName=>undef,
      uid=>0,
      pid=>$$,
      ppid=>0,
      threads=>1,
      threads_change=>0,
      Type=>"Monitor",
      Status=>"NOP",
      StatusType=>"Permanent",
      Interface=>"default",
      Mips=>1000,
      coid=>0,
    };
    if (defined $type){
        $self->{Type}=$type;
    }
    use Sys::Hostname;
    $self->{HostName}=hostname();
    return bless $self,$class;
}


