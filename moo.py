# this is the RAM address where StaticR.rel is always loadad in mario kart wii
# this is the RAM address where StaticR.rel is always loadad in mario kart wii
STATICR_LOAD_ADDRESS = {
    'text': 0x805103B4,
    'ctors': 0x8088f400,
    'dtors': 0x8088f704,
    'rodata': 0x8088F720,
    'data': 0x808B2BD0,
    'bss': 0x809BD6E0,
}

with open("./config/RMCP01/module/symbols.txt") as f:
    symbols = f.readlines()

for i, line in enumerate(symbols):
    tokens = line.split()
    
    sym = tokens[0]
    addr = tokens[2].split(':')[-1][:-1]
    addr = int(addr, 16)
    section = tokens[2].split(':')[0][1:]
    
    rel_offset = STATICR_LOAD_ADDRESS.get(section)
    if rel_offset is not None:
        vaddr = addr + rel_offset
        tokens[3] = f"// vaddr=0x{vaddr:x}"
    symbols[i] = " ".join(tokens) + "\n"

with open(f"./symbols.txt", "w") as f:
    f.writelines(symbols)
