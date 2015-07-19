
type t =
    | Empty
    | Single of int
    | Wide of int * int
    | Composed of t list

let rec flatten = function
    | Empty | Single _ | Wide _ as range -> range
    | Composed [] -> Empty
    | Composed ranges ->
            let ranges = List.fold_left (fun acc r ->
                match flatten r with
                | Empty -> acc
                | Single _ | Wide _ as range -> range :: acc
                | Composed composed -> composed @ acc
            ) [] ranges in
            (match ranges with
            | [] -> Empty
            | _ -> Composed ranges)

let normalize range =
    match flatten range with
        | Empty | Single _ | Wide _ as range -> range
        | Composed ranges ->
                let ranges = List.sort (fun a b ->
                    match a, b with
                    | Wide (a1, _a2), Wide (b1, _b2) -> a1 - b1
                    | Single s, Wide (b1, _b2) -> s - b1
                    | Wide (a1, _a2), Single s -> a1 - s
                    | _ -> assert false
                ) ranges in
                (* TODO: coalesce ranges *)
                Composed ranges

let union r1 r2 =
    (* TODO *)
    Empty

let intersect r1 r2 =
    (* TODO *)
    Empty

let rec first_of = function
    | Empty | Composed [] -> raise Not_found
    | Single s -> s
    | Wide (w1, w2) -> w1
    | Composed (range :: _) -> first_of range

let next_of range n = match range with
    | Empty | Composed [] -> assert false
    | Single s ->
            assert (n = s);
            raise Not_found
    | Wide (w1, w2) ->
            assert (n >= w1 && n <= w2);
            if n < w2 then n + 1 else raise Not_found
    | Composed ranges ->
            (* TODO: binary search tree? *)
            let rec aux = function
            | [] -> raise Not_found
            | hd :: tl ->
                    match hd with
                    (* the range should be normalized so Empty or Composed
                     * must be impossible *)
                    | Empty | Composed _ -> assert false
                    | Single s -> if n < s then s else aux tl
                    | Wide (w1, w2) ->
                            if n < w2 then
                                if n >= w1 then n + 1 else w1
                            else aux tl
            in
            aux ranges

