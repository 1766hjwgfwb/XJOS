[org 0x7c00]

; 清屏（设置80x25文本模式）
mov ax, 3
int 0x10

; 设置段寄存器
mov ax, 0
mov ds, ax        ; 数据段为 0
mov ss, ax        ; 栈段为 0
mov sp, 0x7c00    ; 栈指针

; 设置显存段
mov ax, 0xb800
mov es, ax        ; 显存使用 es 段

; 打印字符串 "Hello, World!"
mov si, hello_string  ; 指向字符串
mov di, 0             ; 显存偏移
mov ah, 0x07          ; 属性：白色字符，黑色背景

print_loop:
    lodsb             ; 加载字符到 al，si 递增
    cmp al, 0         ; 检查字符串结束
    je done           ; 结束循环
    mov [es:di], al   ; 写入字符
    inc di            ; 移到属性字节
    mov [es:di], ah   ; 写入属性
    inc di            ; 移到下一个字符位置
    jmp print_loop    ; 继续循环

done:
    jmp $             ; 阻塞

hello_string:
    db 'Hello, World!', 0


times 510 - ($ - $$) db 0
db 0x55, 0xaa