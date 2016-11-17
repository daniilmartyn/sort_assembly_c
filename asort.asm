SECTION	.text

global	sortA

sortA:	
		nop
		; setup stack frame and save state

		push 	ebp
		mov		ebp, esp
		push 	eax
		push 	ebx
		push	ecx
		push	edx
		push 	esi
		
		mov	esi, [ebp+8]		; move array address into esi,
		mov	ebx, 0			; move immediate value 0 into ebx, will serve as left array pointer
		mov	ecx, [ebp+12]		; move count into ecx
		sub	ecx, 1			; subtract one from count, will serve as right array pointer
		
		shl ecx, 2			; multiply right bound value by 4 to make address offset

		call qsort			; call quicksort function
		
		pop esi
		pop	edx
		pop ecx				; restore registers
		pop	ebx
		pop	eax
		mov	ebp, esp
		pop	ebp

		ret				; return to calling function

		
qsort:
		nop
		; setup stack frame and save state

		push	ebp
		mov	ebp, esp

		sub 	esp, 8			; make room for 3 int local variables
		mov [ebp-4], ebx		; move left bound value address offset into [ebp-4]
		mov [ebp-8], ecx		; move right bound value address offset into [ebp-8]
		
		push eax
		push edx
		push esi

		shr ebx, 2			; temporarily convert left address offset into index position
		shr ecx, 2			; temporarily convert right address offset into index position
		
		mov edx, ebx			; move value of ebx (left pointer) into edx
		add edx, ecx			; add value of ecx(right pointer) into edx
		shr edx, 1			; shift edx by 1 (divide value in edx by 2) is now pivot point
		shl edx, 2			; multiply edx by 4 to make into address offset		
		add edx, esi			; add base address to address offset
		mov edx, [edx]			; store string address in edx, not pointer to string

		shl ebx, 2			; restore left pointer to being address offset
		shl ecx, 2			; restore right pointer to being address offset
		
while1:		push edx			; push address of pivot string
		add ebx, esi			; add array base address to offset
		push dword [ebx]		; push address of string at left pointer
		call stringgt			; compare string at left pointer with pivot string
		pop dword [ebx]			; return stack to normal
		sub ebx, esi
		pop edx					
		cmp eax, 0			; compare result of stringgt with 0
		jge while2			; if greater or equal than 0, continue to second while loop
		add ebx, 4			; otherwise increment left pointer
		jmp while1			; and repeat loop
		
while2: 	push edx			; push address of pivot string
		add ecx, esi			; add array base address to offset
		push dword [ecx]		; push address of string at right pointer
		call stringgt			; compare string at right pointer with pivot string
		pop dword [ecx]			; return stack to normal
		sub ecx, esi
		pop edx					
		cmp eax, 0			; compare result of stringgt with 0
		jle swap			; if less than or equal to 0, swap strings at left and right pointers
		sub ecx, 4			; otherwise decrement right pointer
		jmp while2			; and repeat loop
		
swap:		cmp ebx, ecx			; compare left pointer to right pointer
		jg skipswap			; if left pointer > than right pointer, skip the swap
		push eax			; save eax temporarily
		push edx			; save edx temporarily
		mov eax, [esi+ebx]		; move string at left pointer into eax
		mov edx, [esi+ecx]		; move string at right pointer into edx
		mov [esi+ebx], edx		; move string from edx [right pointer] into left pointer spot
		mov [esi+ecx], eax		; move string from eax [left pointer] into right pointer spot
		pop edx				; restore edx
		pop eax				; restore eax
		add ebx, 4			; increment left pointer
		sub ecx, 4			; decrement right pointer
		
skipswap:	cmp ebx, ecx			; compare left pointer to right pointer again
		jle while1			; if left <= right pointer, then repeat the process of while-loops and swapping
			
		cmp [ebp-4], ecx		; compare left bound to right pointer
		jl leftsort			; if left < right pointer, sort left side of array

continue:	cmp ebx, [ebp-8]		; compare left pointer to right bound
		jl rightsort			; if left pointer < right bound, sort right side of array
		jmp done			; exit qsort

leftsort:	mov ebx, [ebp-4]		; set up left bound, left bound already set
		call qsort			; recursively sort the left side
		jmp continue			; go on to checking conditions for sorting the right side of the array
			
rightsort: 	mov ecx, [ebp-8]
		call qsort			; recursively sort the right side
		   
		
		; restore registers and stack frame
done:	pop esi
		pop edx
		pop ecx
		pop ebx
		pop eax
		mov esp, ebp
		pop ebp
		
		ret				; return to calling function


stringgt:
		nop
		; setup stack frame and save state
		push ebp
		mov	ebp, esp
		push edx
		push ecx
		push ebx
		
		mov	ebx, [ebp+8]	; move first string address into ebx
		mov	ecx, [ebp+12]	; move second string address into ecx
		
		mov edx, 0		; move immediate value 0 into the edx register, this will be the counter for the strings
again:		cld				; clear direction flags, just in case
		mov	ah, [ebx+edx]	; move character at index [ebx+edx] into subregister ah from first string
		mov	al, [ecx+edx]	; move character at index [ecx+edx] into subregister al from second string
		cmp	ah,	al		; compare the character from the first string to character from second string
		jg	greater		; if first string character is greater than second string character, jump to greater
		jl	less		; if first string character is less than the second string character jump to less
		cmp	ah,	0		; check if current character (either one) is the newline character
		je	equal		; if it is the newline character jump to equal
		add	edx, 1		; increase string character counter
		jmp	again		; jump to again because string1 and string2 characters are same, and not at the end of the string
		
		
less:	mov	eax, -1		; store false result (0) in eax
		jmp	exit		; unconditional jump to exit
		
greater:mov	eax, 1				; store true result (1) in eax
		jmp exit			; unconditional jump to exit

equal:	mov	eax, 0			; store 0 result in eax
		
exit:	pop	ebx			; restore state and stack frame
		pop	ecx
		pop	edx
		mov esp, ebp
		pop ebp
		ret				; return to caller

