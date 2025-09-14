[org 0x1000]


dw 0x55aa  ; assert magic number







mov si, string
call print


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