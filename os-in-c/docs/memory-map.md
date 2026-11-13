# CSE103-OS Memory Map

## Physical Memory Layout

| Physical Address Range   | Size      | Description / Usage |
|--------------------------|-----------|---------------------|
| `0x00000000` - `0x000003FF` | 1 KB      | Real Mode Interrupt Vector Table (IVT) (Reserved) |
| `0x00000400` - `0x000004FF` | 256 B     | BIOS Data Area (BDA) (Reserved) |
| `0x00000500` - `0x0007FFFF` | ~510 KB   | Conventional Low Memory |
| `0x00080000` - `0x0009FFFF` | 128 KB    | Extended BIOS Data Area (EBDA) (Reserved) |
| `0x000A0000` - `0x000B7FFF` | 96 KB     | VGA Graphics Display Memory (Reserved) |
| `0x000B8000` - `0x000BFFFF` | 32 KB     | VGA Text Frame Buffer (80x25 color console) |
| `0x000C0000` - `0x000FFFFF` | 256 KB    | BIOS ROM & Option ROMs (Reserved) |
| `0x00100000` - `kernel_end`| Dynamic   | Kernel Executable (`.text`, `.rodata`, `.data`, `.bss`, stack) |
| `kernel_end` - `0x007FFFFF`| Dynamic   | Physical Memory Manager (PMM) Bitmap and Free Frames |
| `0x00800000` - `0x00FFFFFF`| 8 MB      | Kernel Dynamic Heap (`kmalloc` / `kfree` pool) |
| `0x01000000` - `Top of RAM`| Dynamic   | Available User / High Physical Memory Frames |

---

## Virtual Memory Paging (Identity Map)

| Virtual Address Range    | Page Directory Entry | Mapping Target | Flags |
|--------------------------|----------------------|----------------|-------|
| `0x00000000` - `0x003FFFFF` | PDE 0                | Physical `0x00000000` - `0x003FFFFF` (4 MB) | Present, Read/Write, Supervisor |
| `0x00400000` - `0x007FFFFF` | PDE 1                | Physical `0x00400000` - `0x007FFFFF` (4 MB) | Present, Read/Write, Supervisor |
| `0x00800000` - `0x00BFFFFF` | PDE 2                | Physical `0x00800000` - `0x00BFFFFF` (4 MB) | Present, Read/Write, Supervisor |
| `0x00C00000` - `0x00FFFFFF` | PDE 3                | Physical `0x00C00000` - `0x00FFFFFF` (4 MB) | Present, Read/Write, Supervisor |
