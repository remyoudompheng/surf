#!perl
print ("// Do not edit this file...\n");
print ("#include <addDefaultSymbols.h>\n");
print ("void addDefaultSymbols (SymbolTable &st)\n{\n");
while (<>) {
    chomp;
    /\s*(\S*)\s*=\s*(\S*)\s*;/ || die "fail";
    print ("\tst.insert(\"$1\",\"$2\");\n");
}
print ("};\n");
