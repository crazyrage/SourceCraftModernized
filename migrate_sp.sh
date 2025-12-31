#!/bin/bash
# SourcePawn 1.13 migration script

for file in /home/user/SourceCraftModernized/scripting/*.sp; do
    echo "Migrating: $file"

    # 1. public Plugin:myinfo → public Plugin myinfo
    sed -i 's/public Plugin:myinfo/public Plugin myinfo/g' "$file"

    # 2. new bool:var → bool var, new Float:var → float var
    sed -i 's/\bnew bool:/bool /g' "$file"
    sed -i 's/\bnew Float:/float /g' "$file"

    # 3. new String:var[N] → char var[N], decl String:var[N] → char var[N]
    sed -i 's/\bnew String:/char /g' "$file"
    sed -i 's/\bdecl String:/char /g' "$file"

    # 4. new Handle:var → Handle var
    sed -i 's/\bnew Handle:/Handle /g' "$file"

    # 5. new const String:name[] → static const char name[]
    sed -i 's/\bnew const String:/static const char /g' "$file"

    # 6. const String:param[] → const char[] param
    sed -i 's/const String:\([a-zA-Z_][a-zA-Z0-9_]*\)\[\]/const char[] \1/g' "$file"

    # 7. Float:param → float param, bool:param → bool param
    sed -i 's/\bFloat:\([a-zA-Z_][a-zA-Z0-9_]*\)/float \1/g' "$file"
    sed -i 's/\bbool:\([a-zA-Z_][a-zA-Z0-9_]*\)/bool \1/g' "$file"

    # 8. public Action:Func( → public Action Func(
    sed -i 's/public Action:/public Action /g' "$file"
    sed -i 's/public APLRes:/public APLRes /g' "$file"
    sed -i 's/public bool:/public bool /g' "$file"

    # Handle Handle: in function params
    sed -i 's/Handle:/Handle /g' "$file"

    # TopMenuAction: etc
    sed -i 's/TopMenuAction:/TopMenuAction /g' "$file"
    sed -i 's/TopMenuObject:/TopMenuObject /g' "$file"
    sed -i 's/MenuAction:/MenuAction /g' "$file"

    # 9. INVALID_HANDLE → null
    sed -i 's/INVALID_HANDLE/null/g' "$file"

    # 10. for(new i= → for(int i=
    sed -i 's/for\s*(\s*new\s\+\([a-zA-Z_][a-zA-Z0-9_]*\)\s*=/for(int \1=/g' "$file"

    # Additional: new var (standalone) → int var
    sed -i 's/^\(\s*\)new \([a-zA-Z_][a-zA-Z0-9_]*\)\s*;/\1int \2;/g' "$file"
    sed -i 's/^\(\s*\)new \([a-zA-Z_][a-zA-Z0-9_]*\)\s*=/\1int \2=/g' "$file"
    sed -i 's/{\s*new \([a-zA-Z_][a-zA-Z0-9_]*\)\s*;/{ int \1;/g' "$file"
    sed -i 's/{\s*new \([a-zA-Z_][a-zA-Z0-9_]*\)\s*=/{ int \1=/g' "$file"

    # decl var; → int var; (for simple declarations)
    sed -i 's/^\(\s*\)decl \([a-zA-Z_][a-zA-Z0-9_]*\)\s*;/\1int \2;/g' "$file"
    sed -i 's/^\(\s*\)decl \([a-zA-Z_][a-zA-Z0-9_]*\)\s*,/\1int \2,/g' "$file"

    echo "Completed: $file"
done

echo "Migration complete!"
