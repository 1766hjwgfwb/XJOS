[org 0x1000]


dw 0x55aa  ; assert magic number







mov si, string
call print


detect_memory:
    xor ebx, ebx

    mov ax, 0
    mov es, ax      ; ARDS buffer, return info es:di pointer
    mov edi, ards_buffer

    ; 16bits machine use 32bits, add 0x66
    mov edx, 0x534d4150  ; 'SMAP'  start accept

.next:
    ; mode
    mov eax, 0xe820
    mov ecx, 20       ; words

    int 0x15            ; BIOS interrupt

    jc error            ; if cf = 1

    add di, cx         ; move pointer to next entry
    inc word [ards_count]

    cmp ebx, 0          ; if ebx = 0, end of table
    jnz .next

    mov si, detecting
    call print

    xchg bx, bx ; bochs debug

    mov cx, [ards_count]     ; get count of entries
    mov si, 0

.show:
    mov eax, [ards_buffer + si] ; get entry
    mov ebx, [ards_buffer + si + 8]
    mov edx, [ards_buffer + si + 16]
    ; why dont use ecx?
    ; because ecx is used to count entries(cx)
    add si, 20
    xchg bx, bx
    loop .show

jmp $


print:
    mov ah, 0x0e    ; mdoe
.next:
    mov al, [ds:si]    ; mov al [ds:si]
    cmp al, 0
    jz .done
    int 0x10        ; load ax
    inc si
    jmp .next

.done:
    ret

string:
    db "Loading XJOS...", 10, 13, 0

detecting:
    db "Detecting Memory Loading Successfully...", 10, 13, 0


error:
    mov si, .msg
    call print
    hlt
    .msg db  "Loading failed.", 10, 13, 0

ards_count:
    dw 0
ards_buffer:    ; get ARDS structure, ARDS is a structure to describe memory map 
