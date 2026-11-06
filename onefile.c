/*
 * OneFile OS
 * Creator: FelixProfi
 */

/* Указываем Clang'у что код снизу удобно ложится на 16 бит */
__asm__(".code16gcc\n");

void kernel_main(void);
__asm__ (
    ".section .boot, \"ax\"\n"
    ".global boot_entry\n"
    "boot_entry:\n"
    "cli\n"

    /* Инициализируем реальный режим */
    "xor %ax, %ax\n"
    "mov %ax, %ds\n"
    "mov %ax, %es\n"
    "mov %ax, %ss\n"
    "mov $0x7c00, %sp\n"
    "sti\n"

    "mov %dl, boot_drive\n"

    "mov $0x02, %ah\n"
    "mov $1, %al\n"
    "mov $0, %ch\n"
    "mov $2, %cl\n"
    "mov $0, %dh\n"
    "mov boot_drive, %dl\n"
    "mov $0x7e00, %bx\n"
    "int $0x13\n"
    "jc boot_error\n"

    /* Входим в защищённый режим */
    "cli\n"
    ".byte 0x0f, 0x01, 0x16\n"
    ".word gdt_descriptor\n"

    "mov %cr0, %eax\n"
    "or $1, %eax\n"
    "mov %eax, %cr0\n"

    "ljmpl $0x08, $protected_mode_entry\n"

    "boot_error:\n"
    "cli\n"
    "1: hlt\n"
    "jmp 1b\n"

    /* GDT */
    ".align 4\n"
    "gdt_start:\n"
    ".quad 0x0000000000000000\n"
    ".quad 0x00CF9A000000FFFF\n"
    ".quad 0x00CF92000000FFFF\n"
    "gdt_end:\n"

    "gdt_descriptor:\n"
    ".word gdt_end - gdt_start - 1\n"
    ".long gdt_start\n"

    "boot_drive:\n"
    ".byte 0\n"

    /* Устанавливаем удобную загрузочную сигнатуру */
    ".org 510\n"
    ".word 0xAA55\n"
);

/* ============================================================
 * STAGE 2: Защищённый режим
 * ============================================================ */

__attribute__((naked, noreturn, section(".kernel")))
void protected_mode_entry(void)
{
    __asm__ volatile (
        "mov $0x10, %ax\n"
        "mov %ax, %ds\n"
        "mov %ax, %es\n"
        "mov %ax, %ss\n"
        "mov %ax, %fs\n"
        "mov %ax, %gs\n"
        /* Кладём ядро на высокий адрес и сразу же прыгаем в него */
        "mov $0x90000, %esp\n"

        "call kernel_main\n"

        "1: cli\n"
        "hlt\n"
        "jmp 1b\n"
    );
}

/* ============================================================
 * Ядро
 * ============================================================ */

static const char message[]
__attribute__((used)) =
"WELCOME TO ONEFILE OS!";

        __attribute__((used, section(".kernel")))
        void kernel_main(void)
        {
            volatile unsigned char *vga =
            (volatile unsigned char *)0xB8000;

            /* Очищаем экран, потому что BIOS мог туда посрать */
            for (int i = 0; i < 80 * 25; i++) {
                vga[i * 2] = ' ';
                vga[i * 2 + 1] = 0x07;
            }

            /* Выводим сообщение простым циклом  */
            for (int i = 0; message[i] != '\0'; i++) {
                vga[i * 2] = message[i];
                vga[i * 2 + 1] = 0x0F;
            }

            /* Останавливаем процессор чтобы зависло наше сообщение*/
            for (;;) {
                __asm__ volatile (
                    "cli\n"
                    "hlt\n"
                );
            }
        }
