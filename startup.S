/* startup.S — Minimal BL602 startup code */
.section .vectors
.global _start
_start:
    /* Initialize stack pointer */
    la sp, _estack

    /* Jump to main (app_main) */
    la t0, app_main
    jr t0

    /* Halt if we ever return */
1:  j 1b

.section .text
.global app_main_wrapper
app_main_wrapper:
    /* Minimal wrapper — app_main is called from _start */
    ret
