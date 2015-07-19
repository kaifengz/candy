
type constr =
    | Range of Range.t
    | AllDifferent
    | Sum of int
    | Func of (var : int -> value : int -> bool)
    | Func2 of (var1 : int -> value1 : int -> var2 : int -> value2 : int -> bool)
    | FuncAll of (vars : int list -> values : int list -> bool)

type problem =
{
    var_count : int;
    all_vars : int list;
    ranges : (int, Range.t) Hashtbl.t;
    constraints : (int, (int list * constr) list) Hashtbl.t;
}

let log = Printf.eprintf

let log_range ?(name="") range =
    let prefix =
        if name = "" then
            ""
        else
            Printf.sprintf "%s: " name
    in
    let rec string_of_range = function
        | Range.Empty -> "Empty"
        | Range.Single s -> Printf.sprintf "Single %d" s
        | Range.Wide (w1, w2) -> Printf.sprintf "Wide %d %d" w1 w2
        | Range.Composed ranges -> Printf.sprintf "Composed (%s)"
                (String.concat ") (" (List.map string_of_range ranges))
    in
    log "%s%s\n" prefix (string_of_range range)

let make (var_count : int) : problem =
    assert (var_count > 0);
    let rec vars n acc =
        if n = 0 then n :: acc else vars (n - 1) (n :: acc)
    in
    {
        var_count;
        all_vars = vars (var_count - 1) [];
        ranges = Hashtbl.create var_count;
        constraints = Hashtbl.create var_count;
    }

let hashtbl_find_opt hashtbl key =
    try Some (Hashtbl.find hashtbl key)
    with Not_found -> None

let add (p : problem) ?(variables : int list option) (c : constr) =
    let variables =
        match variables with
        | None -> p.all_vars
        | Some vars ->
            List.iter (fun var ->
                if var < 0 || var >= p.var_count then
                    raise (Invalid_argument (Printf.sprintf "Variable %d does not exist" var))
            ) vars;
            vars
    in
    let update_to_hashtbl hashtbl f =
        List.iter (fun var ->
            let item = hashtbl_find_opt hashtbl var in
            Hashtbl.replace hashtbl var (f item)
        ) variables
    in
    match c with
    | Range range ->
            let normalized = Range.normalize range in
            update_to_hashtbl p.ranges (function
                | None -> normalized
                | Some range' -> Range.intersect range' normalized
            )
    | _ ->
            let c' = match c with
            | Sum s -> FuncAll (fun ~vars ~values ->
                    s = (List.fold_left (+) 0 values))
            | _ -> c
            in
            update_to_hashtbl p.constraints (function
                | None -> [variables,c']
                | Some constraints -> (variables, c') :: constraints
            )

exception Invalid_problem of string
exception Unsolveable
exception Backtrack

let solve (p : problem) : (int * int) list =
    let validate_problem () =
        List.iter (fun var ->
            let range =
                try Hashtbl.find p.ranges var
                with Not_found ->
                    raise (Invalid_problem (Printf.sprintf "Variable %d is not ranged" var))
            in
            log_range ~name:(Printf.sprintf "Variable %d" var) range;
        ) p.all_vars
    in
    validate_problem ();

    let shortcuts () =
        Hashtbl.iter (fun _k v ->
            match v with | Range.Empty -> raise Unsolveable | _ -> ()
        ) p.ranges
    in
    shortcuts ();

    let searched = Hashtbl.create p.var_count in
    let find_variable_next_value var =
        let range = Hashtbl.find p.ranges var in
        try
            match hashtbl_find_opt searched var with
            | Some n -> Range.next_of range n
            | None -> Range.first_of range
        with Not_found ->
            raise Backtrack
    in

    let constrainted values idx =
        assert (idx >= 0 && idx < p.var_count);
        let get_var_values vars =
            try Some (List.map (Hashtbl.find values) vars)
            with Not_found -> None
        in
        try
            let constraints = Hashtbl.find p.constraints idx in
            List.iter (fun (vars, constr) ->
                match constr with
                | Range _ | Sum _ -> assert false
                | AllDifferent ->
                    let occupied = Hashtbl.create p.var_count in
                    List.iter (fun var ->
                        try
                            let n = Hashtbl.find values var in
                            if Hashtbl.mem occupied n then raise Backtrack;
                            Hashtbl.add occupied n ();
                        with Not_found ->
                            ()
                    ) vars
                | Func func ->
                    if not (func idx (Hashtbl.find values idx)) then raise Backtrack
                | Func2 func ->
                    let value1 = Hashtbl.find values idx in
                    List.iter (fun var ->
                        if var != idx then begin
                            let v2 = hashtbl_find_opt values var in
                            match v2 with
                            | None -> ()
                            | Some value2 ->
                                if not (func idx value1 var value2) then raise Backtrack
                        end
                    ) vars
                | FuncAll func ->
                    begin match get_var_values vars with
                    | None -> ()
                    | Some var_values ->
                        if not (func vars var_values) then raise Backtrack
                    end
            ) constraints
        with Not_found ->
            (* no constraints *)
            ()
    in

    let rec search idx =
        try
            let v = find_variable_next_value idx in
            log "Try #%d ==> %d\n" idx v;
            Hashtbl.replace searched idx v;
            try
                constrainted searched idx;
                let next = idx + 1 in
                if next < p.var_count then
                    search next
            with Backtrack ->
                search idx
        with Backtrack ->
            log "Backtrack #%d\n" idx;
            Hashtbl.remove searched idx;
            let prev = idx - 1 in
            if prev < 0 then raise Unsolveable;
            search prev
    in

    search 0;
    assert (Hashtbl.length searched = p.var_count);
    List.map (fun var ->
        (var, Hashtbl.find searched var)
    ) p.all_vars

let queens_problem n =
    let p = make n in
    add p (Range (Range.Wide (1, n)));
    add p (Func2 (fun ~var1 ~value1 ~var2 ~value2 ->
        let dvar = var1 - var2 and dvalue = value1 - value2 in
        dvalue != 0 && dvar != dvalue && dvar != -dvalue
        ));
    let solution = solve p in
    List.iter (fun (k, v) ->
        log "%d ==> %d\n" k v
    ) solution

let _ = queens_problem 8

