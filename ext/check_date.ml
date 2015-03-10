
let handle () =
  let open Unix in

  if not @@ Sys.(file_exists argv.(1)) then exit 1;

  let stat_f1 = stat Sys.argv.(1) in
  let stat_f2 = stat Sys.argv.(2) in

  match stat_f1.st_mtime >= stat_f2.st_mtime with
  | true  -> exit 0
  | false -> exit 1



let _ =
  let usage = "ocaml check_date.ml <file1> <file2>" in
  match Array.length Sys.argv with
  | 3 ->
      handle ()

  | _ ->
    Printf.eprintf "%s\n" usage;
    exit 127

