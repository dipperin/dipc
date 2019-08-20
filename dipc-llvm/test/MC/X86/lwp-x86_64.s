# RUN: llvm-mc -triple x86_64-unknown-unknown --show-encoding %s | FileCheck %s --check-prefix=CHECK

llwpcb %rcx
# CHECK: llwpcb %rcx
# CHECK: encoding: [0x8f,0xe9,0xf8,0x12,0xc1]

slwpcb %rax
# CHECK: slwpcb %rax
# CHECK: encoding: [0x8f,0xe9,0xf8,0x12,0xc8]

lwpins $305419896, %ebx, %rax
# CHECK: lwpins $305419896, %ebx, %rax
# CHECK: encoding: [0x8f,0xea,0xf8,0x12,0xc3,0x78,0x56,0x34,0x12]

lwpins $591751049, (%rsp), %rdx
# CHECK: lwpins $591751049, (%rsp), %rdx
# CHECK: encoding: [0x8f,0xea,0xe8,0x12,0x04,0x24,0x89,0x67,0x45,0x23]

lwpval $1737075661, %ebx, %rax
# CHECK: lwpval $1737075661, %ebx, %rax
# CHECK: encoding: [0x8f,0xea,0xf8,0x12,0xcb,0xcd,0xab,0x89,0x67]

lwpval $2309737967, (%rsp), %rdx
# CHECK: lwpval $2309737967, (%rsp), %rdx
# CHECK: encoding: [0x8f,0xea,0xe8,0x12,0x0c,0x24,0xef,0xcd,0xab,0x89]
