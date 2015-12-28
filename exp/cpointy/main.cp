function void v_main(List<UniString> args) {
	int length = args.length;
	int i;
	$println("Args:");
	for (i = 0; i < length; ++i) {
		$print("    ");
		$println(args[i]);
	}
	$println("All done.");
	List<UniString> files = $io_list_dir(".");
	$println("Files in this directory:");
	for (i = 0; i < files.length; ++i) {
		$print("    > ");
		if ($io_is_dir(files[i])) $print("[DIR] ");
		$println(files[i]);
	}
}
