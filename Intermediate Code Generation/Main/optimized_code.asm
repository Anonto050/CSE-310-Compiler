.MODEL SMALL
.STACK 1000H
.DATA
	CR EQU 0DH
	LF EQU 0AH
	w DW 10 DUP (0000H)
.Code
main PROC
	MOV AX, @DATA
	MOV DS, AX
	PUSH BP
	MOV BP, SP
; 	PUSH 0
; 	POP BX	;array index
	PUSH BX
	SUB SP, 20
	MOV BX, 0
	SHL BX, 1
	MOV AX, w[BX]	;global array element
; 	PUSH 2
; 	POP AX	; 2 poped from stack
	PUSH AX	;array element
	PUSH BX	;array index
; 	PUSH AX	; -2 pushed to stack
; 	POP AX	;- 2 popped
	MOV AX, 2
	NEG AX
	POP BX	;index of the array element popped 
; 	PUSH 0
; 	POP BX	;array index
	MOV w[BX], AX	; assigning value of - 2 to w[BX]
	POP AX	 ;Popping out w
	MOV BX, 0
	SHL BX, 1
	NEG BX
	ADD BX, -4
	PUSH BP
	ADD BP, BX
	MOV BX, BP
	MOV AX, [BP]
	POP BP
; 	PUSH 0
; 	POP BX	;array index
	PUSH AX	;array element
	PUSH BX	;array index
	MOV BX, 0
	SHL BX, 1
; 	PUSH BX	;array index
; POP BX	 ;Pop array index from stack
; 	PUSH AX	;array element
; 	POP AX	;w popped
	MOV AX, w[BX]	;global array element
	POP BX	;index of the array element popped 
	PUSH BP	;storing BP
	MOV BP, BX
	MOV [BP], AX	; assigning value of w to x
	POP BP
; 	PUSH 0
; 	POP BX	;array index
	POP AX	 ;Popping out x
	PUSH [BP+-2]
	MOV BX, 0
	SHL BX, 1
	NEG BX
	ADD BX, -4
	PUSH BP
	ADD BP, BX
	MOV BX, BP
	MOV AX, [BP]
; 	PUSH BX	;array index
; POP BX	 ;Pop array index from stack
; 	PUSH AX	;array element
; 	POP AX	;x popped
	POP BP
	MOV [BP+-2], AX	; assigning value of x to i
	POP AX	 ;Popping out i
; 	PUSH 1
; 	POP BX	;array index
	PUSH [BP+-2]
	CALL print_output
	MOV BX, 1
	SHL BX, 1
	NEG BX
	ADD BX, -4
	PUSH BP
	ADD BP, BX
	MOV BX, BP
	MOV AX, [BP]
	POP BP
; 	PUSH 0
; 	POP BX	;array index
	PUSH AX	;array element
	PUSH BX	;array index
	MOV BX, 0
	SHL BX, 1
; 	PUSH BX	;array index
; 	POP BX	 ;Pop array index from stack
	MOV AX, w[BX]	;global array element
	PUSH AX	;array element
	MOV AX, [BX]	 ;set AX to the value of w[BX]
	INC AX	 ;Increment AX
	MOV w[BX], AX	 ;set w[BX] to AX
	POP AX	;w++ popped
	POP BX	;index of the array element popped 
	PUSH BP	;storing BP
	MOV BP, BX
	MOV [BP], AX	; assigning value of w++ to x
	POP BP
; 	PUSH 1
; 	POP BX	;array index
	POP AX	 ;Popping out x
	PUSH [BP+-2]
	MOV BX, 1
	SHL BX, 1
	NEG BX
	ADD BX, -4
	PUSH BP
	ADD BP, BX
	MOV BX, BP
	MOV AX, [BP]
; 	PUSH BX	;array index
; POP BX	 ;Pop array index from stack
; 	PUSH AX	;array element
; 	POP AX	;x popped
	POP BP
	MOV [BP+-2], AX	; assigning value of x to i
	POP AX	 ;Popping out i
	PUSH [BP+-2]
; 	PUSH 0
; 	POP BX	;array index
	CALL print_output
	PUSH [BP+-2]
	MOV BX, 0
	SHL BX, 1
; 	PUSH BX	;array index
; POP BX	 ;Pop array index from stack
; 	PUSH AX	;array element
; 	POP AX	;w popped
	MOV AX, w[BX]	;global array element
	MOV [BP+-2], AX	; assigning value of w to i
	POP AX	 ;Popping out i
	PUSH [BP+-2]
	CALL print_output
; 	PUSH 0
; 	POP BX	; 0 poped from stack
	PUSH [BP+-2]
	PUSH [BP+-2]
	MOV BX, 0
; 	PUSH AX	; i + 0 pushed to stack
; 	POP AX	;i popped
	POP AX	; i poped from stack
	ADD AX, BX
	MOV [BP+-2], AX	; assigning value of i to i
	POP AX	 ;Popping out i
; 	PUSH 0
; 	POP BX	; 0 poped from stack
	PUSH [BP+-2]
	PUSH [BP+-2]
	MOV BX, 0
; 	PUSH AX	; i - 0 pushed to stack
; 	POP AX	;i popped
	POP AX	; i poped from stack
	SUB AX, BX
	MOV [BP+-2], AX	; assigning value of i to i
	POP AX	 ;Popping out i
; 	PUSH 1
; 	POP BX	; 1 poped from stack
	PUSH [BP+-2]
	PUSH [BP+-2]
	MOV BX, 1
; 	PUSH AX	; i * 1 pushed to stack
; 	POP AX	;i * 1 popped
	POP AX	; i poped from stack
	IMUL BX
	MOV [BP+-2], AX	; assigning value of i * 1 to i
	POP AX	 ;Popping out i
	PUSH [BP+-2]
	 ;i > 0
; 	PUSH 0
; 	POP BX	;0 popped
	CALL print_output
	PUSH [BP+-2]
	MOV BX, 0
	POP AX	;i popped
	CMP AX, BX	;i > 0
	JG L_1	;i > 0
	PUSH 0	;i > 0 is false
	JMP L_2	;i > 0 is false
L_1:
	PUSH 1	;i > 0 is true
	 ;i < 10
; 	PUSH 10
; 	POP BX	;10 popped
L_2:
	PUSH [BP+-2]
	MOV BX, 10
	POP AX	;i popped
	CMP AX, BX	;i < 10
	JL L_3	;i < 10
	PUSH 0	;i < 10 is false
	JMP L_4	;i < 10 is false
	 ;i > 0 && i < 10
L_3:
	PUSH 1	;i < 10 is true
L_4:
	POP BX	;i < 10 popped
	POP AX	;i > 0 popped
	CMP AX, 0	;i > 0
	JNE L_5	;i > 0 is true
	PUSH 0	;i > 0 is false
	JMP L_7	;i > 0 is false
L_5:
	CMP BX, 0	;i < 10
	JNE L_6	;i < 10 is true
	PUSH 0	;i < 10 is false
	JMP L_7	;i < 10 is false
L_6:
	PUSH 1	;i > 0 AND i < 10 is true
	 ;i < 0
; 	PUSH 0
; 	POP BX	;0 popped
L_7:
	PUSH [BP+-2]
	MOV BX, 0
	POP AX	;i popped
	CMP AX, BX	;i < 0
	JL L_8	;i < 0
	PUSH 0	;i < 0 is false
	JMP L_9	;i < 0 is false
L_8:
	PUSH 1	;i < 0 is true
; 	PUSH 10
; 	POP AX	; 10 poped from stack
L_9:
	PUSH [BP+-2]
	 ;i > - 10
; 	PUSH AX	; -10 pushed to stack
; 	POP BX	;- 10 popped
	MOV AX, 10
	NEG AX
	MOV BX, AX
	POP AX	;i popped
	CMP AX, BX	;i > - 10
	JG L_10	;i > - 10
	PUSH 0	;i > - 10 is false
	JMP L_11	;i > - 10 is false
	 ;i < 0 && i > - 10
L_10:
	PUSH 1	;i > - 10 is true
L_11:
	POP BX	;i > - 10 popped
	POP AX	;i < 0 popped
	CMP AX, 0	;i < 0
	JNE L_12	;i < 0 is true
	PUSH 0	;i < 0 is false
	JMP L_14	;i < 0 is false
L_12:
	CMP BX, 0	;i > - 10
	JNE L_13	;i > - 10 is true
	PUSH 0	;i > - 10 is false
	JMP L_14	;i > - 10 is false
	 ;(i > 0) || (i < 0)
L_13:
	PUSH 1	;i < 0 AND i > - 10 is true
L_14:
	POP BX	;(i < 0) popped
	POP AX	;(i > 0) popped
	CMP AX, 0	;(i > 0)
	JNE L_15	;(i > 0) is true
	CMP BX, 0	;(i < 0)
	JNE L_15	;(i < 0) is true
	PUSH 0	;(i > 0) OR (i < 0) is false
	JMP L_16	;(i > 0) OR (i < 0) is false
L_15:
	PUSH 1	;(i > 0) OR (i < 0) is true
L_16:
	POP AX
; 	JMP L_17
	CMP AX, 0
	JE L_18
; 	PUSH 100
; 	POP AX	;100 popped
L_17:
	PUSH [BP+-2]
	MOV AX, 100
	MOV [BP+-2], AX	; assigning value of 100 to i
	POP AX	 ;Popping out i
	JMP L_19
; 	PUSH 200
; 	POP AX	;200 popped
L_18:
	PUSH [BP+-2]
	MOV AX, 200
	MOV [BP+-2], AX	; assigning value of 200 to i
	POP AX	 ;Popping out i
L_19:
	PUSH [BP+-2]
	CALL print_output
	PUSH 0
	MOV AX,4CH
						 ; PRINTS A NEW LINE
	INT 21H
main ENDP
	NEW_LINE PROC
						 PUSH AX
						 PUSH DX
						 MOV AH, 2
						 MOV DL, cr
						 INT 21H
						 MOV AH, 2
						 MOV DL, lf
						 INT 21H
						 POP DX
						 POP AX
						 RET
						 NEW_LINE ENDP
	PRINT_OUTPUT PROC NEAR
        ;if(BX < -1) then the number is positive
        PUSH BP             ;Saving BP
        MOV BP, SP          ;BP points to the top of the stack
        ;else, the number is negative
        MOV BX, [BP+4]    ;The number to be printed
        CMP BX, 0
        JGE POSITIVE
        MOV AH, 2           
        MOV DL, '-'         ;Print a '-' sign
        INT 21H
        NEG BX              ;make BX positive
        POSITIVE:
        MOV AX, BX
        MOV CX, 0        ;Initialize character count
        PUSH_WHILE:
            ;quotient is in AX and remainder is in DX
            XOR DX, DX  ;clear DX
            MOV BX, 10  ;BX has the divisor //// AX has the dividend
            ;if(AX == 0) then break the loop
            DIV BX
            PUSH DX     ;Division by 10 will have a remainder less than 8 bits
            ;else continue
            INC CX       ;CX++
            CMP AX, 0
            JE END_PUSH_WHILE
            JMP PUSH_WHILE
        END_PUSH_WHILE:
        MOV AH, 2
        POP_WHILE:
            POP DX      ;Division by 10 will have a remaainder less than 8 bits
            ;if(CX <= 0) then end loop
            ADD DL, '0'
            INT 21H     ;So DL will have the desired character
            ;else continue
            DEC CX       ;CX--
            CMP CX, 0
        ;Print newline
            JLE END_POP_WHILE
            JMP POP_WHILE
        END_POP_WHILE:
        MOV DL, 0DH
        INT 21H
        MOV DL, 0AH
        INT 21H
        POP BP          ; Restore BP
