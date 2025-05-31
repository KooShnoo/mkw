import sys

with open("./config/RMCP01/symbols.txt") as f:
    symbols = f.readlines()
with open("./config/RMCP01/module/symbols.txt") as f:
    symbols += f.readlines()
    
symbol_dict = {}

# these symbols can be duplicated across main.dol and StaticR.rel
duplicatable = [
    "Type",
    "BootInfo",
    "__register_global_object",
    "__destroy_global_chain",
    "__register_atexit",
    "_ctors",
    "_dtors",
    "__destroy_global_chain_reference",
    "__global_destructor_chain",
]

dups=0
for i, line in enumerate(symbols):
    tokens = line.split()
    
    symbol_name = tokens[0]
    addr = tokens[2].split(':')[-1][:-1]
    
    if symbol_name in duplicatable:
        continue
    
    if symbol_name in symbol_dict:
        print(f"!!WARNING!! Duplicate symbol `{symbol_name}` found originally at {symbol_dict[symbol_name]}, then again at {addr}", file=sys.stderr)
        dups += 1

    if symbol_name[0] != "@" and symbol_name[:3] != "...":
        symbol_dict[symbol_name] = addr
    
if dups > 0:
    print(f"symbols.txt contains {dups} duplicate symbols. these may cause linker errors, remove duplicates.", file=sys.stderr)
    print(f"NOTE this script is kinda stupid and has a lot of false positives, use ur brain.", file=sys.stderr)
    print(f"like if the dups are in diff sections or executables or smth, its fine probably", file=sys.stderr)
    exit(1)
else:
    print(f"no duplicates detected, all OK! :)")
    
