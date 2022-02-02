import csv
import sys
import os.path as op
import elftools as pyelf
from elftools.elf.elffile import ELFFile
from collections import defaultdict
path_to_elf = "fw/hello-world/hello"
def get_adress_line_map(dwarfinfo):
    allDict = {}
    for CU in dwarfinfo.iter_CUs():
        lineprog = dwarfinfo.line_program_for_CU(CU)
        for entry in lineprog.get_entries():
            if entry.state == None:
               continue
            elif not entry.state.end_sequence:
                final_path = op.normpath(op.join(op.abspath(op.dirname(path_to_elf)),lpe_filename(lineprog,entry.state.file)))
                if op.isfile(final_path):
                    if not final_path in allDict.keys():
                        allDict[final_path] = [[hex(entry.state.address),entry.state.line,0]]
                    else:
                        allDict[final_path].append([hex(entry.state.address),entry.state.line,0])
    return allDict
def lpe_filename(line_program, file_index):
    lp_header = line_program.header
    file_entries = lp_header["file_entry"]
    file_entry = file_entries[file_index - 1]
    dir_index = file_entry["dir_index"]
    if dir_index == 0:
        return file_entry.name.decode()
    directory = lp_header["include_directory"][dir_index - 1]
    return op.join(directory, file_entry.name).decode()

if __name__ == "__main__":
    map = {}         
    with open(path_to_elf, mode = "rb") as file:
        elffile = ELFFile(file)
        if not elffile.has_dwarf_info():
            print("No Debug information found, use -g as a g++ option")
            sys.exit(-1)
        map = get_adress_line_map(elffile.get_dwarf_info())
    with open("output.txt") as file:
        reader = csv.reader(file)
        for entry in reader:
            val = hex(int("0x"+entry[0],0))
            for tuplelist in map.values():
                for element in tuplelist:
                    if element[0] == val:
                        element[2] +=1
                        
    with open ("coverage.info", "w") as f:
        for key in map.keys():
            f.write(f"SF:{key}\n")
            for element in map[key]:
                f.write(f"DA:{element[1]},{element[2]}\n")
            f.write("end_of_record\n")

    
    
        