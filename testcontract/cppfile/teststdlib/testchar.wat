(module
  (type $t0 (func))
  (type $t1 (func (param i32 i32 i32) (result i32)))
  (type $t2 (func (param i32) (result i32)))
  (type $t3 (func (param i32)))
  (type $t4 (func (param i32 i32)))
  (import "env" "memcpy" (func $env.memcpy (type $t1)))
  (import "env" "abort" (func $env.abort (type $t0)))
  (import "env" "malloc" (func $env.malloc (type $t2)))
  (import "env" "free" (func $env.free (type $t3)))
  (import "env" "prints_l" (func $env.prints_l (type $t4)))
  (func $f5 (type $t0))
  (func $_Znwm (export "_Znwm") (type $t2) (param $p0 i32) (result i32)
    (local $l0 i32) (local $l1 i32)
    (set_local $l0
      (select
        (get_local $p0)
        (i32.const 1)
        (get_local $p0)))
    (block $B0
      (loop $L1
        (br_if $B0
          (tee_local $p0
            (call $env.malloc
              (get_local $l0))))
        (set_local $p0
          (i32.const 0))
        (br_if $B0
          (i32.eqz
            (tee_local $l1
              (i32.load offset=1024
                (i32.const 0)))))
        (call_indirect $t0
          (get_local $l1))
        (br $L1)))
    (get_local $p0))
  (func $_Znam (export "_Znam") (type $t2) (param $p0 i32) (result i32)
    (call $_Znwm
      (get_local $p0)))
  (func $_ZdlPv (export "_ZdlPv") (type $t3) (param $p0 i32)
    (block $B0
      (br_if $B0
        (i32.eqz
          (get_local $p0)))
      (call $env.free
        (get_local $p0))))
  (func $_ZdaPv (export "_ZdaPv") (type $t3) (param $p0 i32)
    (call $_ZdlPv
      (get_local $p0)))
  (func $f10 (type $t0)
    (call $env.abort)
    (unreachable))
  (func $init (export "init") (type $t0)
    (local $l0 i32) (local $l1 i32) (local $l2 i32) (local $l3 i32)
    (set_global $g0
      (tee_local $l0
        (i32.sub
          (get_global $g0)
          (i32.const 16))))
    (i32.store
      (i32.add
        (get_local $l0)
        (i32.const 8))
      (i32.const 0))
    (i64.store
      (get_local $l0)
      (i64.const 0))
    (set_local $l1
      (i32.const 1028))
    (block $B0
      (block $B1
        (br_if $B1
          (i32.eqz
            (i32.and
              (i32.const 1028)
              (i32.const 3))))
        (loop $L2
          (br_if $B0
            (i32.eqz
              (i32.load8_u
                (get_local $l1))))
          (br_if $L2
            (i32.and
              (tee_local $l1
                (i32.add
                  (get_local $l1)
                  (i32.const 1)))
              (i32.const 3)))))
      (set_local $l1
        (i32.add
          (get_local $l1)
          (i32.const -4)))
      (loop $L3
        (br_if $L3
          (i32.eqz
            (i32.and
              (i32.and
                (i32.xor
                  (tee_local $l2
                    (i32.load
                      (tee_local $l1
                        (i32.add
                          (get_local $l1)
                          (i32.const 4)))))
                  (i32.const -1))
                (i32.add
                  (get_local $l2)
                  (i32.const -16843009)))
              (i32.const -2139062144)))))
      (br_if $B0
        (i32.eqz
          (i32.and
            (get_local $l2)
            (i32.const 255))))
      (loop $L4
        (set_local $l2
          (i32.add
            (get_local $l1)
            (i32.const 1)))
        (set_local $l1
          (i32.add
            (get_local $l1)
            (i32.const 1)))
        (br_if $L4
          (i32.and
            (i32.load8_u
              (get_local $l2))
            (i32.const 255)))))
    (block $B5
      (br_if $B5
        (i32.ge_u
          (tee_local $l1
            (i32.sub
              (get_local $l1)
              (i32.const 1028)))
          (i32.const -16)))
      (block $B6
        (block $B7
          (block $B8
            (br_if $B8
              (i32.ge_u
                (get_local $l1)
                (i32.const 11)))
            (i32.store8
              (get_local $l0)
              (i32.shl
                (get_local $l1)
                (i32.const 1)))
            (set_local $l2
              (i32.or
                (get_local $l0)
                (i32.const 1)))
            (br_if $B7
              (get_local $l1))
            (br $B6))
          (set_local $l2
            (call $_Znwm
              (tee_local $l3
                (i32.and
                  (i32.add
                    (get_local $l1)
                    (i32.const 16))
                  (i32.const -16)))))
          (i32.store
            (get_local $l0)
            (i32.or
              (get_local $l3)
              (i32.const 1)))
          (i32.store offset=8
            (get_local $l0)
            (get_local $l2))
          (i32.store offset=4
            (get_local $l0)
            (get_local $l1)))
        (drop
          (call $env.memcpy
            (get_local $l2)
            (i32.const 1028)
            (get_local $l1))))
      (i32.store8
        (i32.add
          (get_local $l2)
          (get_local $l1))
        (i32.const 0))
      (block $B9
        (br_if $B9
          (i32.eqz
            (i32.and
              (i32.load8_u
                (get_local $l0))
              (i32.const 1))))
        (call $_ZdlPv
          (i32.load offset=8
            (get_local $l0))))
      (set_global $g0
        (i32.add
          (get_local $l0)
          (i32.const 16)))
      (return))
    (call $f10)
    (unreachable))
  (func $printString (export "printString") (type $t0)
    (local $l0 i32) (local $l1 i32) (local $l2 i32) (local $l3 i32)
    (set_global $g0
      (tee_local $l0
        (i32.sub
          (get_global $g0)
          (i32.const 16))))
    (i32.store
      (i32.add
        (get_local $l0)
        (i32.const 8))
      (i32.const 0))
    (i64.store
      (get_local $l0)
      (i64.const 0))
    (set_local $l1
      (i32.const 1028))
    (block $B0
      (block $B1
        (br_if $B1
          (i32.eqz
            (i32.and
              (i32.const 1028)
              (i32.const 3))))
        (loop $L2
          (br_if $B0
            (i32.eqz
              (i32.load8_u
                (get_local $l1))))
          (br_if $L2
            (i32.and
              (tee_local $l1
                (i32.add
                  (get_local $l1)
                  (i32.const 1)))
              (i32.const 3)))))
      (set_local $l1
        (i32.add
          (get_local $l1)
          (i32.const -4)))
      (loop $L3
        (br_if $L3
          (i32.eqz
            (i32.and
              (i32.and
                (i32.xor
                  (tee_local $l2
                    (i32.load
                      (tee_local $l1
                        (i32.add
                          (get_local $l1)
                          (i32.const 4)))))
                  (i32.const -1))
                (i32.add
                  (get_local $l2)
                  (i32.const -16843009)))
              (i32.const -2139062144)))))
      (br_if $B0
        (i32.eqz
          (i32.and
            (get_local $l2)
            (i32.const 255))))
      (loop $L4
        (set_local $l2
          (i32.add
            (get_local $l1)
            (i32.const 1)))
        (set_local $l1
          (i32.add
            (get_local $l1)
            (i32.const 1)))
        (br_if $L4
          (i32.and
            (i32.load8_u
              (get_local $l2))
            (i32.const 255)))))
    (block $B5
      (br_if $B5
        (i32.ge_u
          (tee_local $l1
            (i32.sub
              (get_local $l1)
              (i32.const 1028)))
          (i32.const -16)))
      (block $B6
        (block $B7
          (block $B8
            (br_if $B8
              (i32.ge_u
                (get_local $l1)
                (i32.const 11)))
            (i32.store8
              (get_local $l0)
              (i32.shl
                (get_local $l1)
                (i32.const 1)))
            (set_local $l2
              (tee_local $l3
                (i32.or
                  (get_local $l0)
                  (i32.const 1))))
            (br_if $B7
              (get_local $l1))
            (br $B6))
          (set_local $l2
            (call $_Znwm
              (tee_local $l3
                (i32.and
                  (i32.add
                    (get_local $l1)
                    (i32.const 16))
                  (i32.const -16)))))
          (i32.store
            (get_local $l0)
            (i32.or
              (get_local $l3)
              (i32.const 1)))
          (i32.store offset=8
            (get_local $l0)
            (get_local $l2))
          (i32.store offset=4
            (get_local $l0)
            (get_local $l1))
          (set_local $l3
            (i32.or
              (get_local $l0)
              (i32.const 1))))
        (drop
          (call $env.memcpy
            (get_local $l2)
            (i32.const 1028)
            (get_local $l1))))
      (i32.store8
        (i32.add
          (get_local $l2)
          (get_local $l1))
        (i32.const 0))
      (call $env.prints_l
        (select
          (i32.load offset=8
            (get_local $l0))
          (get_local $l3)
          (tee_local $l2
            (i32.and
              (tee_local $l1
                (i32.load8_u
                  (get_local $l0)))
              (i32.const 1))))
        (select
          (i32.load offset=4
            (get_local $l0))
          (i32.shr_u
            (get_local $l1)
            (i32.const 1))
          (get_local $l2)))
      (block $B9
        (br_if $B9
          (i32.eqz
            (i32.and
              (i32.load8_u
                (get_local $l0))
              (i32.const 1))))
        (call $_ZdlPv
          (i32.load
            (i32.add
              (get_local $l0)
              (i32.const 8)))))
      (set_global $g0
        (i32.add
          (get_local $l0)
          (i32.const 16)))
      (return))
    (call $f10)
    (unreachable))
  (table $T0 1 1 anyfunc)
  (memory $memory (export "memory") 2)
  (global $g0 (mut i32) (i32.const 66576))
  (global $__heap_base (export "__heap_base") i32 (i32.const 66576))
  (global $__data_end (export "__data_end") i32 (i32.const 1040))
  (data (i32.const 1024) "\00\00\00\00")
  (data (i32.const 1028) "hello world\00"))
