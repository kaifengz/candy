
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
                (* sorting *)
                let ranges = List.sort (fun a b ->
                    match a, b with
                    | Wide (w1, _w2), Wide (w1', _w2') -> w1 - w1'
                    | Single s, Single s' -> s - s'
                    | Single s, Wide (w1, w2) | Wide (w1, w2), Single s ->
                        (* Single goes first if the Single is contained by the Wide *)
                        if s >= w1 && s <= w2 then -1
                        else s - w1
                    | _ -> assert false
                ) ranges in
                (* coalescing *)
                let ranges = List.fold_right (fun range acc ->
                    match acc with
                    | [] -> [range]
                    | Single s :: tl ->
                        begin match range with
                        | Single s' ->
                            assert (s' <= s);
                            if s = s' then acc
                            else if s - s' = 1 then Wide (s', s) :: tl
                            else range :: acc
                        | Wide (w1', w2') ->
                            assert (w1' <= s);
                            if s <= w2' then acc
                            else if s = w2' + 1 then Wide (w1', w2' + 1) :: tl
                            else range :: acc
                        | _ -> assert false
                        end
                    | Wide (w1, w2) :: tl->
                        begin match range with
                        | Single s' ->
                            assert (s' <= w2);
                            if s' >= w1 && s' <= w2 then acc
                            else if s' + 1 = w1 then Wide (s', w2) :: tl
                            else range :: acc
                        | Wide (w1', w2') ->
                            assert (w1' <= w1);
                            if w2' + 1 >= w1 then Wide (w1', w2) :: tl
                            else range :: acc
                        | _ -> assert false
                        end
                    | _ -> assert false
                ) ranges [] in
                Composed ranges

let union r1 r2 =
    normalize (Composed [r1;r2])

let intersect r1 r2 =
    let rec intersect_aux r1 r2 acc =
        match r1, r2 with
        | [], _ | _, [] -> acc
        | Single s1 :: tl1, Single s2 :: tl2 ->
            if s1 = s2 then
                intersect_aux tl1 tl2 (Single s1 :: acc)
            else if s1 < s2 then
                intersect_aux tl1 r2 acc
            else
                intersect_aux r1 tl2 acc
        | (Wide (w1, w2) :: tl as r), (Wide (w1', w2') :: tl' as r') ->
            if w2 >= w2' then
                if w1 > w2' then intersect_aux r tl' acc
                else intersect_aux r tl' (
                        let max_w1 = if w1 >= w1' then w1 else w1' in
                        (if max_w1 = w2' then Single max_w1 else Wide (max_w1, w2')) :: acc
                    )
            else (* w2 < w2' *)
                if w1' > w2 then intersect_aux tl r' acc
                else intersect_aux tl r' (
                        let max_w1 = if w1 >= w1' then w1 else w1' in
                        (if max_w1 = w2 then Single max_w1 else Wide (max_w1, w2)) :: acc
                    )
        | (Single s :: tl as r), (Wide (w1, w2) :: tl' as r')
        | (Wide (w1, w2) :: tl' as r'), (Single s :: tl as r) ->
            if s >= w1 && s <= w2 then
                intersect_aux tl r' (Single s :: acc)
            else if s > w2 then
                intersect_aux r tl' acc
            else
                intersect_aux tl r' acc
        | _ -> assert false
    in
    let normalized_list range =
        match normalize range with
        | Composed ranges -> ranges
        | Empty -> []
        | _ as range -> [range]
    in
    let r1 = normalized_list r1 in
    let r2 = normalized_list r2 in
    let intersection = intersect_aux r1 r2 [] in
    match intersection with
    | [] -> Empty
    | [r] -> r
    | _ -> Composed (List.rev intersection)

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
            (* FIXME: binary search tree? *)
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

