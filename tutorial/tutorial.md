graph TD
    %% Globales Styling
    subgraph DISK ["DISK (Physikalisches Medium)"]
        S0["<b>Sektor 0: MBR</b><br/>Code + Partitionstabelle<br/>(512 Bytes)"]
        S1["Sektor 1: Stage 2..."]
        S2["Sektor 2..."]
        S0 --- S1 --- S2
    end

    subgraph RAM ["ARBEITSSPEICHER (RAM)"]
        IVT["0x0000: IVT (1KB)<br/>(BIOS Interrupts)"]
        BDA["0x0400: BDA (256B)<br/>(BIOS Data Area)"]
        FREE["0x0500: FREE / STACK<br/>(Wächst nach unten)"]
        BOOT["<b>0x7C00: GELADENER MBR</b><br/>(Stage 1)"]
        REMAIN["0x7E00: Freier RAM / Stage 2"]
        
        IVT --- BDA --- FREE --- BOOT --- REMAIN
    end

    %% Der Ladevorgang
    S0 -- "BIOS lädt Sektor 0" --> BOOT

    %% Styling
    style DISK fill:#001f3f,stroke:#00ff00,color:#ffffff
    style RAM fill:#001f3f,stroke:#00ff00,color:#ffffff
    style S0 fill:#004080,stroke:#00ff00,color:#ffffff,stroke-width:2px
    style BOOT fill:#004080,stroke:#00ff00,color:#ffffff,stroke-width:4px
    style IVT color:#cccccc
    style BDA color:#cccccc
    style FREE color:#00ff00


