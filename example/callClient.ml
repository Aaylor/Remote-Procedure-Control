
open Unix

let read_and_show fd buf =
  let length = Bytes.length buf in
  let rec aux () =
    match read fd buf 0 length with
    | -1 -> failwith "read_and_show"
    | 0  -> ()
    | n  -> ignore (write stdout buf 0 n); aux ()
  in aux ()

let _ =
  match Array.length Sys.argv with
  | 3 ->
    ()
  | _ ->
    Printf.eprintf "%s ARG1 ARG2\n" Sys.argv.(0);
    exit 1

let _ =
  let request = [|
    "../bin/client"; "-c"; "concat"; "-ret"; "-str"; 
    "-str"; Sys.argv.(1); "-str"; Sys.argv.(2)
  |] in
  let infd, outfd = pipe () in
  match fork () with
  | -1 ->
    failwith "fork error"
  | 0  ->
    close infd;
    dup2 outfd stdout;
    execv request.(0) request
  | n  ->
    close outfd;
    let buffer = Bytes.create 64 in
    read_and_show infd buffer;
    ignore (Unix.wait ())
