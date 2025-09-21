[bits 32]


global task_switch
task_switch:
    push ebp
    mov ebp, esp

    push ebx
    push esi
    push edi

    mov eax, esp
    and eax, 0xfffff000

    mov [eax], esp

    ; mov ecx, 50000000 ; 500ms
    ; cly:
    ;     jmp $+2
    ;     jmp $+2
    ;     jmp $+2
    ;     loop cly

    ; [eax] value = 0x1fec or 0x2fec
    mov eax, [ebp + 8]
    mov esp, [eax]

    pop edi
    pop esi
    pop ebx
    pop ebp

    ret