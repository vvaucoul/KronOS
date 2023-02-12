bits 32

extern scheduler

global scheduler_handler
scheduler_handler:
    cli
    pushad

    push esp
    push ebp

    call scheduler
    ; hlt ; should never reach here
    
    pop ebp
    pop esp

    popad

    sti

    iret