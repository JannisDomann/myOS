Checkliste: Der Weg zum dynamischen VMM
1. Vorbereitung (Datentypen & Helfer)

    vmm_map_at: Eine Version von vmm_map erstellen, die eine beliebige PML4 als Argument nimmt (statt cr3 zu lesen). Das brauchen wir, um die neue Tabelle aufzubauen, während die alte noch aktiv ist.
    Defines finalisieren: PAGE_FRAME_MASK, PHYS_OFFSET und die Index-Makros in vmm.h festlegen.

2. Das Bootstrap-Mapping (Der "Switch")

    vmm_create_kernel_context: Eine Funktion, die:
        Eine neue PML4-Seite vom PMM anfordert.
        Die ersten 1GB (oder den Kernel-Bereich) identisch mappt (damit der Code beim Umschalten nicht abstürzt).
        Den physischen RAM ab PHYS_OFFSET mappt.
    CR3-Update: Die neue PML4-Adresse in CR3 schreiben. Ab diesem Moment ist dein neuer VMM der "Chef".

3. Cleanup & Higher Half Migration

    Identity Mapping entfernen: Sobald der Kernel stabil in der Higher Half läuft, löschen wir die unteren Mappings (0x0 bis ...), damit der User-Space dort später Platz hat.
    Page Fault Handler: Eine Funktion in den IDT (Interrupt Descriptor Table) einhängen, die uns sagt, wenn etwas schiefgeht (#PF - Exception 14). Ohne das stochern wir im Dunkeln, falls ein Mapping fehlt.

4. Features (Dynamik)

    vmm_unmap: Seiten wieder freigeben.
    Kernel-Heap: Eine Funktion kmalloc, die den VMM nutzt, um dem Kernel dynamisch Speicher zu geben.