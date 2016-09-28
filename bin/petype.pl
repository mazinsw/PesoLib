use File::Basename;

$exe = $ARGV[0];

open(EXE, $exe) or die "can't open $exe: $!";
binmode(EXE);
if (read(EXE, $doshdr, 68)) {

	($magic,$skip,$offset)=unpack('a2a58l', $doshdr);
	die("Not an executable") if ($magic ne 'MZ');

	seek(EXE,$offset,SEEK_SET);
	if (read(EXE, $pehdr, 6)){
		($sig,$skip,$machine)=unpack('a2a2v', $pehdr);
		die("No a PE Executable") if ($sig ne 'PE');
		$name = basename($exe);
		if ($machine == 0x014c){
			printf("%20s = i386\n", $name);
		}
		elsif ($machine == 0x0200){
			printf("%20s = IA64\n", $name);
		}
		elsif ($machine == 0x8664){
			printf("%20s = AMD64\n", $name);
		}
		else{
			printf("Unknown machine type 0x%lx\n", $machine);
		}
	}
}

close(EXE);