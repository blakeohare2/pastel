function void v_main(List<UniString> args) {
	int length = args.length;
	int i;
	$println("Args:");
	for (i = 0; i < length; ++i) {
		$print("    ");
		$println(args[i]);
	}
	$println("All done.");
}