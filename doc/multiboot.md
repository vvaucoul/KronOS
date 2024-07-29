# Multiboot et Gestion de la Mémoire

## Introduction à Multiboot

Le standard Multiboot est une spécification conçue pour permettre aux chargeurs de démarrage de démarrer différents systèmes d'exploitation sans avoir besoin de connaître les détails spécifiques de chacun. Multiboot 1, en particulier, fournit une structure de données qui contient des informations importantes sur le système, comme la mémoire disponible, les modules chargés, et d'autres configurations nécessaires pour l'initialisation du noyau.

## Structure Multiboot

La structure `multiboot_info_t` fournit des informations sur l'environnement du système lors du démarrage. Voici les champs principaux :

```c
typedef struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint32_t vbe_mode;
    uint32_t vbe_interface_seg;
    uint32_t vbe_interface_off;
    uint32_t vbe_interface_len;
} __attribute__((packed)) multiboot_info_t;
```

## Description des Champs Principaux

### Mem Lower et Mem Upper

- Mem Lower : Indique la quantité de mémoire en kilo-octets disponible en dessous de 1 Mo.
- Mem Upper : Indique la quantité de mémoire en kilo-octets disponible au-dessus de 1 Mo.

### Boot Device

- Boot Device : Spécifie le périphérique de démarrage utilisé par le chargeur de démarrage.

### Command Line

- Command Line : La ligne de commande passée au noyau par le chargeur de démarrage.

### Mods Count et Mods Addr

- Mods Count : Le nombre de modules chargés par le chargeur de démarrage.
- Mods Addr : L'adresse où les informations sur les modules chargés sont stockées.


### Memory Map

- Memory Map Length : La longueur totale de la carte mémoire fournie par le chargeur de démarrage.
- Memory Map Addr : L'adresse où la carte mémoire est stockée.


## Entrées de la Carte Mémoire

- Chaque entrée de la carte mémoire fournit des informations sur un segment de mémoire disponible :


```c
typedef struct multiboot_mmap_entry {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed)) multiboot_mmap_entry_t;
```

- Size : La taille de cette entrée.
- Addr : L'adresse de début du segment de mémoire.
- Len : La longueur du segment de mémoire.
- Type : Le type de mémoire (1 pour mémoire utilisable, 2 pour mémoire réservée, etc.).

### Drives Length et Drives Addr

- Drives Length : La longueur des informations sur les lecteurs fournies par le chargeur de démarrage.
- Drives Addr : L'adresse où les informations sur les lecteurs sont stockées.

### Config Table

- Config Table : L'adresse d'une table de configuration fournie par le chargeur de démarrage.

### Boot Loader Name

- Boot Loader Name : Le nom du chargeur de démarrage utilisé.

### APM Table

- APM Table : L'adresse d'une table APM (Advanced Power Management).

### VBE Fields

- VBE Control Info et VBE Mode Info : Adresses vers les structures d'information de contrôle et de mode VBE.
- VBE Mode : Le mode VBE actuel utilisé.
- VBE Interface Seg et VBE Interface Off : Segment et offset de l'interface VBE.
- VBE Interface Len : Longueur de l'interface VBE.
Exemple de Lecture de la Carte Mémoire

## Exemple de Lecture de la Carte Mémoire

```c
if (mb_info->flags & MULTIBOOT_FLAG_MMAP) {
    printk("Memory Map Length: %u\n", mb_info->mmap_length);
    printk("Memory Map Addr: 0x%x\n", mb_info->mmap_addr);

    multiboot_mmap_entry_t* mmap = (multiboot_mmap_entry_t*) mb_info->mmap_addr;
    uint32_t mmap_end = mb_info->mmap_addr + mb_info->mmap_length;
    while ((uint32_t)mmap < mmap_end) {
        printk("Size: 0x%x, Addr: 0x%llx, Len: 0x%llx, Type: 0x%x\n",
               mmap->size, mmap->addr, mmap->len, mmap->type);
        mmap = (multiboot_mmap_entry_t*)((uint32_t)mmap + mmap->size + sizeof(mmap->size));
    }
}
```

## Explication des Types de Mémoire

- Type 0x1 : Mémoire utilisable. Cette mémoire peut être utilisée par le système d'exploitation pour allouer des processus, des buffers, etc.
- Type 0x2 : Mémoire réservée. Cette mémoire ne doit pas être utilisée par le système d'exploitation car elle peut être utilisée par le BIOS, les périphériques, ou d'autres composants du système.

## Distinction entre Mémoire Basse et Haute

### Mémoire Basse

La mémoire en dessous de 1 Mo est souvent appelée la mémoire basse :

- Mémoire Conventionnelle : Les premiers 640 KB (0 à 640 KB) de mémoire sont appelés mémoire conventionnelle.
- Mémoire Résiduelle : La mémoire entre 640 KB et 1 Mo (640 KB à 1024 KB) est appelée mémoire haute, utilisée pour la mémoire vidéo, le BIOS, et les périphériques.

### Mémoire Haute

La mémoire au-dessus de 1 Mo est souvent appelée la mémoire haute :

- Mémoire Étendue : Toute la mémoire au-dessus de 1 Mo est appelée mémoire étendue, utilisée par les systèmes d'exploitation modernes.
Illustration de la Mémoire

## Illustration de la Mémoire

```bash
+----------------------+ 0x00000000
| Mémoire Conventionnelle (640 KB)    |
+----------------------+ 0x000A0000
| Mémoire Haute (640 KB - 1 MB)       |
+----------------------+ 0x00100000
| Mémoire Étendue (1 MB et plus)      |
+----------------------+ ...
```

## Application Pratique

Lors de l'initialisation d'un système d'exploitation, ces informations sont cruciales pour :

- Éviter les conflits : Ne pas utiliser les zones de mémoire réservées pour le BIOS et les périphériques.
- Gestion Efficace de la Mémoire : Allouer correctement la mémoire aux processus et aux données du système d'exploitation.