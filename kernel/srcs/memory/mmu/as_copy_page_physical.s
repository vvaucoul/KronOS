; [GLOBAL copy_page_physical]
; copy_page_physical:
;     ; Save registers
;     push ebx
;     push esi
;     push edi
;     pushf

;     ; Disable interrupts to prevent context switches during CR0 modification
;     cli

;     ; Load source and destination addresses
;     mov ebx, [esp+16] ; Source
;     mov ecx, [esp+20] ; Destination

;     ; Save original CR0 value
;     mov edx, cr0

;     ; Disable Write Protect bit in CR0
;     and edx, ~0x10000
;     mov cr0, edx

;     ; Determine the number of DWORDs to copy (assuming 4KB pages)
;     mov edx, 1024

;     ; Copy data using rep movsd for efficiency (if supported)
;     ; Note: This requires the source and destination to be aligned to 4-byte boundaries
;     ;       and the count (edx) to be the number of DWORDs.
;     ; rep movsd could be used here but it requires esi and edi to be set,
;     ; which is not the case in this function. Using the manual loop instead.

;     ; Manual loop for clarity and compatibility
; .loop:
;     mov eax, [ebx]
;     mov [ecx], eax
;     add ebx, 4
;     add ecx, 4
;     dec edx
;     jnz .loop

;     ; Restore original CR0 value
;     mov edx, [esp+12] ; Restored from the stack, not directly from edx to handle potential esp changes
;     mov cr0, edx

;     ; Restore flags and registers
;     popf
;     pop edi
;     pop esi
;     pop ebx
;     ret